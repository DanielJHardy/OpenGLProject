#ifndef _PARTICLESYSTEMS_H_
#define _PARTICLESYSTEMS_H_

#include "Application.h"
#include "FlyCamera.h"

#include "Emitter.h"



class ParticleSystems : Application
{
public:
	ParticleSystems();
	virtual ~ParticleSystems();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	unsigned int m_programID;

	Emitter* m_emitter;

private:

	FlyCamera m_sceneCam;

};


#endif // !_Quarternions_H_