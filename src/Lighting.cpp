#include "Lighting.h"

#include "gl_core_4_4.h"

#include "Gizmos.h"
#include "Utility.h"

#include "glm_header.h"


#include <GLFW\glfw3.h>
#include <cstdio>


Lighting::Lighting()
{

}
Lighting::~Lighting()
{
}

bool Lighting::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	m_backColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	////////////////////

	//Load shaders
	LoadShaders("./data/shaders/lighting_vertex.glsl", "./data/shaders/lighting_fragment.glsl", nullptr, &m_programID);

	//Load FBX scene
	m_fbx = new FBXFile();
	m_fbx->load("./data/models/Lucy.fbx");
	createOpenGLBuffers(m_fbx);


	Gizmos::create();

	//Camera
	m_sceneCam = FlyCamera(glm::radians(60.0f));
	m_sceneCam.setLookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));


	glfwSetTime(0.0);
	return true;
}
void Lighting::Shutdown()
{
	cleanupOpenGLBuffers(m_fbx);
	glDeleteProgram(m_programID);

	Gizmos::destroy();
}

bool Lighting::Update()
{
	//Get deltatime
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	//call base class' update
	if (Application::Update() == false)
		return false;


	//Update camera
	m_sceneCam.Update(dt);

	//Create Graph and transform
	vec4 white(1);
	vec4 black(0.1f, 0.1f, 0.1f, 1);

	for (int i = 0; i <= 20; i++)
	{
		Gizmos::addLine(vec3(-10 + i, -1, -10), vec3(-10 + i, -1, 10), i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -1, -10 + i), vec3(10, -1, -10 + i), i == 10 ? white : black);
	}
	Gizmos::addTransform(mat4(1), 1);

	return true;
}
void Lighting::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glUseProgram(m_programID);

	//bind the camera
	int loc = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&m_sceneCam.getProjectionView());

	//bind the vertex array and draw mesh
	for (unsigned int  i = 0; i < m_fbx->getMeshCount(); i++)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}


	Gizmos::draw(m_sceneCam.getProjectionView());

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();
}

void Lighting::createOpenGLBuffers(FBXFile* fbx)
{
	//Create the Gl VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		//storage for the opengl data in 3 unsigned int
		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER, 
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;

	}
}

void Lighting::cleanupOpenGLBuffers(FBXFile* fbx)
{
	//clean up the vertex data attached to each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}