#include "Quaternions.h"

#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <GLFW\glfw3.h>
#include <cstdio>

Quarternions::Quarternions() {}
Quarternions::~Quarternions() {}

bool Quarternions::Startup()
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
	m_sceneCam.setLookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));

	/////////////////////////////
	m_hip_frames[0].position = vec3(0, 5, 0);
	m_hip_frames[0].rotation = glm::quat(vec3(-1, 0, 0));

	m_knee_frames[0].position = vec3(0, -2.5, 0);
	m_knee_frames[0].rotation = glm::quat(vec3(-1, 0, 0));

	m_ankle_frames[0].position = vec3(0, -2.5, 0);
	m_ankle_frames[0].rotation = glm::quat(vec3(0, 0, 0));

	m_hip_frames[1].position = vec3(0, 5, 0);
	m_hip_frames[1].rotation = glm::quat(vec3(1, 0, 0));

	m_knee_frames[1].position = vec3(0, -2.5, 0);
	m_knee_frames[1].rotation = glm::quat(vec3(0, 0, 0));

	m_ankle_frames[1].position = vec3(0, -2.5, 0);
	m_ankle_frames[1].rotation = glm::quat(vec3(0, 0, 0));

	m_timer = 0;

	//complete startup
	glfwSetTime(0.0);
	return true;

}

mat4 EvaluateKeyFrames(Keyframe start, Keyframe end, float t)
{
	vec3 pos = glm::mix(start.position, end.position, t);
	glm::quat rot = glm::slerp(start.rotation, end.rotation, t);
	mat4 result = glm::translate(pos) * glm::toMat4(rot);

	return result;
}

void Quarternions::Shutdown()
{
	Gizmos::destroy();
}
bool Quarternions::Update()
{
	// Get DeltaTime
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);
	m_timer += dt / 4 ;

	if (m_timer > 1)
		m_timer = 0;

	//base update
	if (Application::Update() == false)
	{
		return false;
	}

	Gizmos::clear();

	////////////////////////////////
	hip_bone = EvaluateKeyFrames(m_hip_frames[0], m_hip_frames[1], m_timer);
	knee_bone = EvaluateKeyFrames(m_knee_frames[0], m_knee_frames[1], m_timer);
	ankle_bone = EvaluateKeyFrames(m_ankle_frames[0], m_ankle_frames[1], m_timer);

	mat4 global_hip = hip_bone;
	mat4 global_knee = hip_bone * knee_bone;
	mat4 global_ankle = global_knee * ankle_bone;

	vec3 hip_pos = global_hip[3].xyz;
	vec3 knee_pos = global_knee[3].xyz;
	vec3 ankle_pos = global_ankle[3].xyz;

	Gizmos::addAABBFilled(hip_pos, vec3(0.5f), vec4(1,1,0,1), &global_hip);
	Gizmos::addAABBFilled(knee_pos, vec3(0.5f), vec4(1, 0, 0, 1), &global_knee);
	Gizmos::addAABBFilled(ankle_pos, vec3(0.5f), vec4(1, 0, 1, 1), &global_ankle);

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



void Quarternions::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw Gizmos
	Gizmos::draw(m_sceneCam.getProjectionView());

	//swap buffers and poll events
	glfwSwapBuffers(this->m_window);
	glfwPollEvents();
}