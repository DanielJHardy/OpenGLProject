#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "Application.h"
#include "FlyCamera.h"
#include "AntTweakBar.h"

#include "FBXFile.h"

#include "Vertex.h"
#include <vector>

class Animation : public Application
{
public:
	Animation();
	virtual ~Animation();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);


	void EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton *skeleton, float timer);
	void UpdateBones(FBXSkeleton* skeleton);


	unsigned int m_programID;
	FBXFile* m_fbx;

private:

	FlyCamera m_sceneCam;

	vec4 m_backColor;

	float m_fps;
	float m_timer;

	TwBar* m_bar;

};



#endif 