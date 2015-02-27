#include "Textures.h"

#include "gl_core_4_4.h"

#include "Gizmos.h"
#include "Utility.h"

#include "glm_header.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GLFW\glfw3.h>
#include <cstdio>


Textures::Textures()
{

}
Textures::~Textures()
{
}

bool Textures::Startup()
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

	//Texture
	loadTexture("./data/textures/crate.png");

	LoadShaders("./data/shaders/texture_vertex.glsl","./data/shaders/texture_fragment.glsl", nullptr,&m_programID);

	//Quad
	float vertexData[] = {
		-5, 0, 5, 1, 0, 1,
		 5, 0, 5, 1, 1, 1,
		5, 0, -5, 1, 1, 0,
		-5, 0, -5, 1, 0, 0, 
	};

	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	Gizmos::create();

	//Camera
	m_sceneCam = FlyCamera(glm::radians(60.0f));
	m_sceneCam.setLookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));


	glfwSetTime(0.0);
	return true;
}
void Textures::Shutdown()
{

	Gizmos::destroy();
}

bool Textures::Update()
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
void Textures::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//use shaders
	glUseProgram(m_programID);

	//bind camera
	int location = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, (float*)&m_sceneCam.getProjectionView());

	//set texture slot
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	//tell the shader where it is
	location = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(location, 0);

	//draw
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	


	Gizmos::draw(m_sceneCam.getProjectionView());

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();
}

void Textures::loadTexture(const char* a_filename)
{
	int width, height;
	
	
	int channels;
	unsigned char* data = stbi_load(a_filename, &width, &height, &channels, STBI_default);
	
	
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	
	stbi_image_free(data);

}