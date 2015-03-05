#ifndef _CAMERAANDPROJECTIONS_H_
#define _CAMERAANDPROJECTIONS_H_

#include "Application.h"
#include "FlyCamera.h"

#include "AntTweakBar.h"

class SceneManagement : public Application
{
public:
	SceneManagement();
	virtual ~SceneManagement();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

private:

	FlyCamera m_sceneCam;

	vec4 m_backColor;

	float m_fps;

	TwBar* m_bar;

};



#endif //_CAMERAANDPROJECTIONS_H_