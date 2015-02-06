#include "FlyCamera.h"

FlyCamera::FlyCamera(){}
FlyCamera::FlyCamera(float a_fov) : Camera(a_fov)
{
	m_speed = 0;
	m_up = vec3(0, 1, 0);
}

FlyCamera::~FlyCamera(){}

void FlyCamera::Update(float a_deltaTime)
{
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
	{
		m_worldTransform[3] -= m_viewTransform[2] * a_deltaTime;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
	{
		m_worldTransform[3] += m_viewTransform[2] * a_deltaTime;
	}

	m_viewTransform = glm::inverse(m_worldTransform);
	updateProjectionViewTransform();
}

void FlyCamera::setSpeed(float a_speed)
{
	m_speed = a_speed;
}