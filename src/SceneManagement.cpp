#include "SceneManagement.h"

#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <GLFW\glfw3.h>
#include "BoundingObjects.h"

#include <cstdio>

SceneManagement::SceneManagement()
{

}
SceneManagement::~SceneManagement()
{
}

bool SceneManagement::Startup()
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
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fps, "");

	//
	glfwSetTime(0.0);
	return true;
}
void SceneManagement::Shutdown()
{
	TwDeleteAllBars();
	TwTerminate();

	Gizmos::destroy();
}

bool SceneManagement::Update()
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
	/////////////

	BoundingSphere sphere; sphere.centre = vec3(0, cosf((float)glfwGetTime()) + 1, 0); sphere.radius = 0.5f;
	vec4 plane(0, 1, 0, -1);
	float d = glm::dot(vec3(plane), sphere.centre) + plane.w;
	Gizmos::addSphere(sphere.centre, sphere.radius, 8, 8, vec4(1, 0, 1, 1));
	vec4 planeColour(1, 1, 0, 1); if (d > sphere.radius) planeColour = vec4(0, 1, 0, 1); else if (d < -sphere.radius) planeColour = vec4(1, 0, 0, 1);
	Gizmos::addTri(vec3(4, 1, 4), vec3(-4, 1, -4), vec3(-4, 1, 4), planeColour); Gizmos::addTri(vec3(4, 1, 4), vec3(4, 1, -4), vec3(-4, 1, -4), planeColour);

	vec4* planes = m_sceneCam.getFrustrumPlanes(m_sceneCam.getProjectionView());

	for (int i = 0; i < 6; i++)
	{
		float d = glm::dot(vec3(planes[i]), sphere.centre) + planes[i].w;
		if (d < -sphere.radius) 
		{ 
			printf("Behind, don't render it!\n"); 
			break; 
		}
		else if (d < sphere.radius) 
		{ 
			printf("Touching, we still need to render it!\n"); 
		}
		else 
		{
			printf("Front, fully visible so render it!\n"); 
		}
	}


	////////////////grid
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
void SceneManagement::Draw()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	Gizmos::draw(m_sceneCam.getProjectionView());
	TwDraw();

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();


}