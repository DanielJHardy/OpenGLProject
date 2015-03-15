#ifndef _RENDERTARGETS_H_
#define _RENDERTARGETS_H_

#include "Application.h"
#include "FlyCamera.h"

#include "AntTweakBar.h"

class RenderTargets : public Application
{
public:
	RenderTargets();
	virtual ~RenderTargets();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	unsigned int m_fbo;

	unsigned int m_fboTexture;
	unsigned int m_fboDepth;

private:

	FlyCamera m_sceneCam;

	vec4 m_backColor;

	float m_fps;

	TwBar* m_bar;

};



#endif