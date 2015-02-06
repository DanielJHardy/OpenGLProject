#ifndef _CAMERAANDPROJECTIONS_H_
#define _CAMERAANDPROJECTIONS_H_

#include "Application.h"
#include "FlyCamera.h"


class CameraAndProjections : public Application
{
public:
	CameraAndProjections();
	virtual ~CameraAndProjections();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

private:
	
	FlyCamera m_sceneCam;
	


};



#endif //_CAMERAANDPROJECTIONS_H_