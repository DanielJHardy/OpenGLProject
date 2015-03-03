#include "AdvancedTexturing.h"

#include "gl_core_4_4.h"

#include "Gizmos.h"
#include "Utility.h"

#include "glm_header.h"

#include "stb_image.h"

#include <GLFW\glfw3.h>
#include <cstdio>

AdvancedTexturing::AdvancedTexturing(){}
AdvancedTexturing::~AdvancedTexturing(){}

bool AdvancedTexturing::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	m_backColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);

	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	//Camera
	m_sceneCam = FlyCamera(glm::radians(60.0f));
	m_sceneCam.setLookAt(vec3(-1, 0, -1), vec3(0, 0, 0), vec3(0, 1, 0));

	//GUI
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	m_bar = TwNewBar("Awesome Bar");
	TwAddVarRW(m_bar, "Clear Color", TW_TYPE_COLOR4F, &m_backColor, "");
	TwAddSeparator(m_bar, "sep1", "");
	TwAddVarRW(m_bar, "Light Dir", TW_TYPE_DIR3F, &m_lightDir, "");
	/////////////////////////

	//Load textures
	//LoadTexture("./data/textures/rock_diffuse.tga", &m_texture);
	//LoadTexture("./data/textures/rock_normal.tga", &m_normalMap);

	LoadTextures("./data/textures/rock_diffuse.tga", "./data/textures/rock_normal.tga");

	//Load shaders
	LoadShaders("./data/shaders/textureNorm_vertex.glsl", "./data/shaders/textureNorm_fragment.glsl", nullptr, &m_programID);


	//Quad buffer creation
	VertexNormal vertexData[] = {
		{ -5, 0, 5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
		{ 5, 0, 5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1 },
		{ 5, 0, -5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0 },
		{ -5, 0, -5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
	};

	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3
	};

	//Vertex Array Object
	glGenVertexArrays(1,&m_vao);
	glBindVertexArray(m_vao);

	//Vertex Buffer Object
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexNormal) * 4, vertexData, GL_STATIC_DRAW);

	//Element Array Buffer / Index Buffer
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE, sizeof(VertexNormal), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), ((char*)0)+48);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), ((char*)0)+16);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), ((char*)0)+32);

	//unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//
	
	m_lightDir = vec3(0,1,0);


	//
	glfwSetTime(0.0);
	return true;
}
void AdvancedTexturing::Shutdown()
{
	TwDeleteAllBars();
	TwTerminate();

	Gizmos::destroy();
}

bool AdvancedTexturing::Update()
{
	m_dt = (float)glfwGetTime();
	glfwSetTime(0.0);


	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);

	if (Application::Update() == false)
	{
		return false;
	}


	m_sceneCam.Update(m_dt);

	//Draw grid. should put this in a function
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
void AdvancedTexturing::Draw()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//use our texture program
	glUseProgram(m_programID);

	//bind the camera
	int loc = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&m_sceneCam.getProjectionView());

	//set texture slots
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalMap);

	//tell the shader where it is
	loc = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(loc, 0);
	loc = glGetUniformLocation(m_programID, "normal");
	glUniform1i(loc, 1);

	//bind the light
	vec3 light = m_lightDir;
	loc = glGetUniformLocation(m_programID, "LightDir");
	glUniform3f(loc, light.x, light.y, light.z);

	//draw
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	Gizmos::draw(m_sceneCam.getProjectionView());
	TwDraw();

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();


}

void AdvancedTexturing::LoadTexture(const char* a_filename, unsigned int* a_texturePos)
{
	int imageHeight, imageWidth, imageFormat;
	imageHeight = imageWidth = imageFormat = 0;

	unsigned char* data;

	//load diffuse map
	data = stbi_load(a_filename, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, a_texturePos);
	glBindTexture(GL_TEXTURE_2D, *a_texturePos);
	glTexImage2D(GL_TEXTURE_2D, GL_RGB,0, imageWidth, imageHeight,0 ,GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //Why is there a second ?

	stbi_image_free(data);

}

void AdvancedTexturing::LoadTextures(const char* a_diff_file, const char* a_norm_file)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data;

	// load diffuse map
	data = stbi_load(a_diff_file,&imageWidth, &imageHeight, &imageFormat, STBI_default);  

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
	// load normal map       
	data = stbi_load(a_norm_file,&imageWidth, &imageHeight, &imageFormat, STBI_default); 

	glGenTextures(1, &m_normalMap);
	glBindTexture(GL_TEXTURE_2D, m_normalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

}