#include "RenderTargets.h"

#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm_header.h"
#include "Utility.h"

#include <GLFW\glfw3.h>



#include <cstdio>

RenderTargets::RenderTargets()
{

}
RenderTargets::~RenderTargets()
{
}

bool RenderTargets::Startup()
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
	m_sceneCam.setLookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
	m_sceneCam.setSpeed(10);


	//GUI
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	m_bar = TwNewBar("Awesome Bar");
	TwAddVarRW(m_bar, "Clear Color", TW_TYPE_COLOR4F, &m_backColor, "");
	TwAddSeparator(m_bar, "sep1", "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fps, "");


	////Frame buffer
	//setup and bind  a framebuffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	/////attach render targets here

	//create a texture and bind it
	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);

	//specify texture format for storage
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//attach texture to the frame buffer as the first color attachment
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);

	//setup and bind a 24bit depth buffer as a render buffer
	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);

	//inform FBO about the attachments
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("FrameBuffer setup error!");

	//unbind the FBO so we can render to the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//create plane
	float vertexData[] = { -5, 0, -5, 1, 0, 0, 5, 0, -5, 1, 1, 0, 5, 10, -5, 1, 1, 1, -5, 10, -5, 1, 0, 1, };
	unsigned int indexData[] = { 0, 1, 2, 0, 2, 3, };

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_ibo);  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//load shaders
	LoadShaders("./data/shaders/texture_vertex.glsl", "./data/shaders/texture_fragment.glsl", nullptr, &m_programID);

	//create sphere
	Gizmos::addSphere(vec3(0, 5, 0), 0.5f, 8, 8, vec4(1, 1, 0, 1));


	//
	glfwSetTime(0.0);
	return true;
}
void RenderTargets::Shutdown()
{
	TwDeleteAllBars();
	TwTerminate();

	Gizmos::destroy();
}

bool RenderTargets::Update()
{
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	m_fps = 1 / dt;


	if (Application::Update() == false)
	{
		return false;
	}


	m_sceneCam.Update(dt);

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
void RenderTargets::Draw()
{


	//frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0,0,512,512);

	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_sceneCam.getProjectionView());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0,0, 1280, 720);

	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	int loc = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&m_sceneCam.getProjectionView());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glUniform1i(glGetUniformLocation(m_programID, "diffuse"), 0);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	Gizmos::draw(m_sceneCam.getProjectionView());

	TwDraw();

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();


}