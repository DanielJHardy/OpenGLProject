#ifndef _FLYCAMERA_H_
#define _FLYCAMERA_H_

#include "Camera.h"

class FlyCamera : public Camera
{
public:
	FlyCamera();
	FlyCamera(float a_fov);
	~FlyCamera();

	virtual void Update(float a_deltaTime);
	void setSpeed(float a_speed);

private:
	float m_speed;
	vec3 m_up;
};

#endif // !_FLYCAMERA_H_
