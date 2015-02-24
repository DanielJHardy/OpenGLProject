#ifndef _LIGHTING_H_
#define _LIGHTING_H_

#include "Application.h"
#include "FlyCamera.h"

#include "FBXFile.h"

#include "Vertex.h"
#include <vector>

class Lighting : public Application
{
public:
	Lighting();
	virtual ~Lighting();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);

	FBXFile* m_fbx;
	unsigned int m_programID;

private:

	FlyCamera m_sceneCam;
	vec4 m_backColor;




};



#endif 