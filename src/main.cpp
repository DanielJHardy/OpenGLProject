#define GLM_SWIZZLE
#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <GLFW\glfw3.h>
#include <cstdio>

#include "Quaternions.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;



int main()
{
	Quarternions app;

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

	
}