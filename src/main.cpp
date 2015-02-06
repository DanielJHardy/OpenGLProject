#define GLM_SWIZZLE
#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <GLFW\glfw3.h>
#include <cstdio>

#include "IntroToOpenGL.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;



int main()
{
	IntroToOpenGL app;

	if (app.Startup() == false)
	{
		return -1;
	}

	while (app.Update() == true)
	{
		app.Draw();
	}

	app.Startup();

	return 0;

	///////////////////////////////////////////
	// Loads up glfw
	if (glfwInit() == 0)
		return -1;

	//creates a window
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);

	// CHeck for window creation failure
	if (window == nullptr)
		return -1;

	//does something
	glfwMakeContextCurrent(window);


	//load opengl
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();
	printf("Successfully loaded OpenGL version %d.%d\n", major_version, minor_version);

	//////////////

	glClearColor(0.3f, 0.3f, 0.3f,1.0f);
	glEnable(GL_DEPTH_TEST);



	Gizmos::create();

	mat4 projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);


	float camera_x = -10.0f;
	float camera_z = -10.0f;
	float rot = 0.0f;

	float timer = 0.0f;

	glfwSetTime(0.0);
	while (!glfwWindowShouldClose(window))
	{
		float dt = (float)glfwGetTime();
		glfwSetTime(0.0);
		timer += dt;

		// left/right
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera_x += 5 * dt;

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera_x -= 5 * dt;

		// forward/back
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera_z += 5 * dt;

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera_z -= 5 * dt;

		// rotation
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera_z += 5 * dt;

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			rot += 3 * dt;

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			rot -= 3 * dt;

		mat4 view = glm::lookAt(vec3(camera_x, 1, camera_z), vec3(camera_x + rot, 1, camera_z + 1), vec3(0, 1, 0));
		

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Gizmos::clear();

		Gizmos::addTransform(mat4(1),4);

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

		mat4 sun_transform = glm::rotate(timer, vec3(0,1,0));
		mat4 child_1_transform = sun_transform * glm::translate(vec3(3, 0, 0)) * glm::rotate(timer * 2.0f, vec3(0,1,0));
		mat4 child_2_transform = child_1_transform * glm::translate(vec3(1, 0, 0)) * glm::rotate(timer * 3.0f, vec3(0, 1, 0));

		mat4 child_3_transform = -sun_transform * glm::translate(vec3(2, 0, 0)) * glm::rotate(timer * 4.0f, vec3(0, 1, 0));

		Gizmos::addSphere(sun_transform[3].xyz, 1.0f, 20, 20, red, &sun_transform);
		Gizmos::addSphere(child_1_transform[3].xyz, 0.6f, 20, 20, blue , &child_1_transform);
		Gizmos::addSphere(child_2_transform[3].xyz, 0.2f, 20, 20, red, &child_2_transform);
		Gizmos::addSphere(child_3_transform[3].xyz, 0.5f, 20, 20, green, &child_3_transform);

		Gizmos::draw(projection, view);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	/////////////
	Gizmos::destroy();

	// cleanup
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}