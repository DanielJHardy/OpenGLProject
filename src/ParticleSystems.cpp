#include "ParticleSystems.h"

#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Utility.h"

#include <GLFW\glfw3.h>
#include <cstdio>

ParticleSystems::ParticleSystems() {}
ParticleSystems::~ParticleSystems() {}

bool ParticleSystems::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	//Gizmos
	Gizmos::create();

	//GUI
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);
#ifdef _CALLBACKS_H_
	SetCallbacks(m_window);
#endif // DEBUG

	m_bar = TwNewBar("Awesome Bar");
	TwAddSeparator(m_bar, "sep1", "");

	//Camera
	m_sceneCam = FlyCamera(glm::radians(60.0f));
	m_sceneCam.setLookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));

	//emitter
	LoadShaders("./data/shaders/particle_vertex.glsl", "./data/shaders/particle_fragment.glsl", nullptr, &m_programID);

	m_emitter = new Emitter();
	m_emitter->Init(1000,vec3(0,0,0), 10000, 4.f, 4.f, 100, 100, 0.1f, 0.3f, vec4(1,0,0,1), vec4(1,1,0,1) );


	//complete startup
	glfwSetTime(0.0);
	return true;

}


void ParticleSystems::Shutdown()
{
	TwDeleteAllBars();
	TwTerminate();
	Gizmos::destroy();
}
bool ParticleSystems::Update()
{
	// Get DeltaTime
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	//base update
	if (Application::Update() == false)
	{
		return false;
	}

	Gizmos::clear();

	//update Camera
	m_sceneCam.Update(dt);

	//emitter
	m_emitter->Update(dt);
	m_emitter->UpdateVertexData(vec3(m_sceneCam.getWorldTransform()[3]), vec3(m_sceneCam.getWorldTransform()[1]));

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



void ParticleSystems::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw Gizmos
	Gizmos::draw(m_sceneCam.getProjectionView());

	//GUI
	TwDraw();

	glUseProgram(m_programID);

	int proj_view_uniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(proj_view_uniform, 1, GL_FALSE, (float*)&m_sceneCam.getProjectionView());

	m_emitter->Render();

	//swap buffers and poll events
	glfwSwapBuffers(this->m_window);
	glfwPollEvents();
}