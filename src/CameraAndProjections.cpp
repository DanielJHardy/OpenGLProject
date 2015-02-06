#include "CameraAndProjections.h"

#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <GLFW\glfw3.h>
#include <cstdio>

CameraAndProjections::CameraAndProjections()
{

}
CameraAndProjections::~CameraAndProjections()
{
}

bool CameraAndProjections::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	m_sceneCam = FlyCamera(glm::radians(60.0f));
	m_sceneCam.setLookAt(vec3(-1, 1, 0), vec3(0,0,0), vec3(0, 1, 0));

	glfwSetTime(0.0);
	return true;
}
void CameraAndProjections::Shutdown()
{
	Gizmos::destroy();
}

bool CameraAndProjections::Update()
{
	float dt = glfwGetTime();
	glfwSetTime(0.0);

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
void CameraAndProjections::Draw()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_sceneCam.getProjectionView());

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();


}