#include "Camera.h"

Camera::Camera(){}
Camera::Camera(float a_fov)
{
	m_viewTransform = glm::lookAt(vec3(-1, 2, -1), vec3(0, 0, 0), vec3(0, 1, 0));
	m_worldTransform = glm::inverse(m_viewTransform);

	m_projectionTransform = glm::perspective(a_fov, 1280.0f / 720.0f, 0.1f, 100.0f);
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
}

Camera::~Camera(){}

void Camera::setPerspective(float a_fov, float a_aspectRatio, float a_near, float a_far)
{
	m_projectionTransform = glm::perspective(a_fov, a_aspectRatio, a_near, a_far);
	updateProjectionViewTransform();
}

void Camera::setLookAt(vec3 a_from, vec3 a_to, vec3 a_up)
{
	m_viewTransform = glm::lookAt(a_from, a_to, a_up);
	m_worldTransform = glm::inverse(m_viewTransform);
	updateProjectionViewTransform();
}

void Camera::setPosition(vec3 a_position)
{
	m_worldTransform[3].xyz = a_position;
	m_viewTransform = glm::inverse(m_worldTransform);
	updateProjectionViewTransform();
}

mat4 Camera::getWorldTransform()
{
	return m_worldTransform;
}

mat4 Camera::getView()
{
	return m_viewTransform;
}

mat4 Camera::getProjection()
{
	return m_projectionTransform;
}

mat4 Camera::getProjectionView()
{
	return m_projectionViewTransform;
}

void Camera::updateProjectionViewTransform()
{
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
}