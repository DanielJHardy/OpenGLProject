#include "FlyCamera.h"

const double centerX = 1280 / 2;
const double centerY = 720 / 2;

FlyCamera::FlyCamera(){}
FlyCamera::FlyCamera(float a_fov) : Camera(a_fov)
{
	m_speed = 0;
	m_up = vec3(0, 1, 0);
	m_mouseSensitivity = 0.005f;
}

FlyCamera::~FlyCamera(){}

void FlyCamera::Update(float a_deltaTime)
{
	//Forward
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
	{
		m_worldTransform[3] -= m_viewTransform[0] * a_deltaTime;
	}
	//Backward
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
	{
		m_worldTransform[3] += m_viewTransform[0] * a_deltaTime;
	}

	//Left
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
	{
		m_worldTransform[3] += m_viewTransform[2] * a_deltaTime;
	}
	//Right
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
	{
		m_worldTransform[3] -= m_viewTransform[2] * a_deltaTime;
	}

	//Up
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		m_worldTransform[3] -= m_viewTransform[1] * a_deltaTime;
	}
	//Down
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_worldTransform[3] += m_viewTransform[1] * a_deltaTime;
	}

	double x, y;
	glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);

	//Rotation
	if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		if (x != centerX || y != centerY)
		{
			// Compute new orientation
			m_horizontalAngle += m_mouseSensitivity * a_deltaTime * float(centerX - x);
			m_verticalAngle += m_mouseSensitivity * a_deltaTime * float(centerY - y);

			vec3 direction(cos(m_verticalAngle) * sin(m_horizontalAngle), sin(m_verticalAngle), cos(m_verticalAngle) * cos(m_horizontalAngle));

			m_worldTransform[3] += vec4(direction, 0);

			//Reset mouse position to center screen
			glfwSetCursorPos(glfwGetCurrentContext(), centerX, centerY);
		}
	}



	m_viewTransform = glm::inverse(m_worldTransform);
	updateProjectionViewTransform();
}

void FlyCamera::setSpeed(float a_speed)
{
	m_speed = a_speed;
}