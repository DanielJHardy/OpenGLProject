#include "RenderingGeometry.h"

#include "gl_core_4_4.h"

#include "Gizmos.h"
#include "Utility.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <GLFW\glfw3.h>
#include <cstdio>


RenderingGeometry::RenderingGeometry()
{

}
RenderingGeometry::~RenderingGeometry()
{
}

bool RenderingGeometry::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	LoadShaders("./src/basic_vertex.glsl", "./src/basic_fragment.glsl", nullptr,  &m_programID);

	m_backColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Load object
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, "./data/models/dragon.obj");
	if (err.size() != 0)
		return false;

	createOpenGLBuffers(shapes);


	Gizmos::create();

	//Camera
	m_sceneCam = FlyCamera(glm::radians(60.0f));
	m_sceneCam.setLookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
	

	glfwSetTime(0.0);
	return true;
}
void RenderingGeometry::Shutdown()
{

	Gizmos::destroy();
}

bool RenderingGeometry::Update()
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
void RenderingGeometry::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	int view_proj_uniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&m_sceneCam.getProjectionView());
	for (unsigned int i = 0; i < m_gl_info.size(); ++i) 
	{ 
		glBindVertexArray(m_gl_info[i].m_VAO);     
		glDrawElements(GL_TRIANGLES, m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0); 
	}


	Gizmos::draw(m_sceneCam.getProjectionView());

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();
}

void RenderingGeometry::GenerateShaders()
{
	LoadShaders("./src/basic_vertex.glsl", "./src/basic_fragmant.glsl", nullptr, &m_programID);
}

void RenderingGeometry::createOpenGLBuffers(std::vector<tinyobj::shape_t>&shapes)
{
	m_gl_info.resize(shapes.size());

	for (unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index) 
	{
		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
		glBindVertexArray(m_gl_info[mesh_index].m_VAO);

		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
		float_count += shapes[mesh_index].mesh.normals.size();
		float_count += shapes[mesh_index].mesh.texcoords.size();

		std::vector<float> vertex_data;
		vertex_data.reserve(float_count);

		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.positions.begin(), shapes[mesh_index].mesh.positions.end());
		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.normals.begin(), shapes[mesh_index].mesh.normals.end());
		m_gl_info[mesh_index].m_index_count = shapes[mesh_index].mesh.indices.size();

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int), shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0); //position  
		glEnableVertexAttribArray(1); //normal data  

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0); 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}