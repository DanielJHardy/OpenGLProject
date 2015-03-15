#include "RenderTargets.h"

#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

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

	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);

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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	Gizmos::draw(m_sceneCam.getProjectionView());
	TwDraw();

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();


}