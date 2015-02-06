#include "IntroToOpenGL.h"

#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <GLFW\glfw3.h>
#include <cstdio>

using glm::vec2;
using glm::vec3;
using glm::vec4;


IntroToOpenGL::IntroToOpenGL()
{
}



IntroToOpenGL::~IntroToOpenGL()
{

}

bool IntroToOpenGL::Startup()
{

	if (Application::Startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	mat4 projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);


	m_projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	m_timer = 0;

	glfwSetTime(0.0);
}

void IntroToOpenGL::Shutdown()
{


}

bool IntroToOpenGL::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);
	m_timer += dt;

	mat4 view = glm::lookAt(vec3(-10, 5, -10), vec3(0, 1, 0), vec3(0, 1, 0));


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();

	Gizmos::addTransform(mat4(1), 4);

	vec4 white(1);
	vec4 black(0.1f, 0.1f, 0.1f, 1);
	vec4 red(1.0f, 0.1f, 0.1f, 1);
	vec4 blue(0.0f, 0.1f, 1.1f, 1);
	vec4 green(0.0f, 1.1f, 0.1f, 1);

	for (int i = 0; i <= 20; i++)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i == 10 ? white : black);
	}

	mat4 sun_transform = glm::rotate(m_timer, vec3(0, 1, 0));
	mat4 child_1_transform = sun_transform * glm::translate(vec3(3, 0, 0)) * glm::rotate(m_timer * 2.0f, vec3(0, 1, 0));
	mat4 child_2_transform = child_1_transform * glm::translate(vec3(1, 0, 0)) * glm::rotate(m_timer * 3.0f, vec3(0, 1, 0));

	mat4 child_3_transform = -sun_transform * glm::translate(vec3(2, 0, 0)) * glm::rotate(m_timer * 4.0f, vec3(0, 1, 0));

	Gizmos::addSphere(sun_transform[3].xyz, 1.0f, 20, 20, red, &sun_transform);
	Gizmos::addSphere(child_1_transform[3].xyz, 0.6f, 20, 20, blue, &child_1_transform);
	Gizmos::addSphere(child_2_transform[3].xyz, 0.2f, 20, 20, red, &child_2_transform);
	Gizmos::addSphere(child_3_transform[3].xyz, 0.5f, 20, 20, green, &child_3_transform);

	Gizmos::draw(m_projection, view);


	return true;
}

void IntroToOpenGL::Draw()
{
	glfwSwapBuffers(this->m_window);
	glfwPollEvents();
}