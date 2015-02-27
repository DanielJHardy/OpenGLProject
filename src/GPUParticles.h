#ifndef _GPUPARTICLES_H_
#define _GPUPARTICLES_H_

#include "Application.h"
#include "FlyCamera.h"

#include "AntTweakBar.h"
#include "GPUEmitter.h"

class GPUParticles : public Application
{
public:
	GPUParticles();
	virtual ~GPUParticles();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	GPUPointEmitter m_emitter;
	float m_time;

private:

	FlyCamera m_sceneCam;
	vec4 m_backColor;

	float m_fps;
	TwBar* m_bar;

};



#endif 