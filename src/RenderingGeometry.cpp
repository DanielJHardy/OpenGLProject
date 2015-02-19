#include "RenderingGeometry.h"

#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <GLFW\glfw3.h>
#include <cstdio>

RenderingGeometry::RenderingGeometry() {}
RenderingGeometry::~RenderingGeometry() {}

bool RenderingGeometry::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	//Gizmos
	Gizmos::create();

	//Camera
	m_sceneCam = FlyCamera(glm::radians(60.0f));
	m_sceneCam.setLookAt(vec3(-1, 0, -1), vec3(0, 0, 0), vec3(0, 1, 0));

	//load object test
	//std::string err = tinyobj::LoadObj(m_shapes, m_materials, "./data/dragon.obj");

	//complete startup
	glfwSetTime(0.0);
	return true;

}
void RenderingGeometry::Shutdown()
{
	Gizmos::destroy();
}
bool RenderingGeometry::Update()
{
	// Get DeltaTime
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	//base update
	if (Application::Update() == false)
	{
		return false;
	}

	//update Camera
	m_sceneCam.Update(dt);


	//Draw Grid and transform
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

	//draw Gizmos
	Gizmos::draw(m_sceneCam.getProjectionView());

	//swap buffers and poll events
	glfwSwapBuffers(this->m_window);
	glfwPollEvents();
}