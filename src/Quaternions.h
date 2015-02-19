#ifndef _Quarternions_H_
#define _Quarternions_H_

#include "Application.h"
#include "FlyCamera.h"

struct Keyframe
{
	vec3 position;
	glm::quat rotation;
};


class Quarternions : Application
{
public:
	Quarternions();
	virtual ~Quarternions();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

private:

	FlyCamera m_sceneCam;

	Keyframe m_hip_frames[2];
	Keyframe m_knee_frames[2];
	Keyframe m_ankle_frames[2];

	mat4 hip_bone;
	mat4 knee_bone;
	mat4 ankle_bone;

	float m_timer;
};


#endif // !_Quarternions_H_