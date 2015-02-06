#ifndef _INTROTOOPENGL_H_
#define _INTROTOOPENGL_H_

#include "Application.h"
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::mat4;

class IntroToOpenGL : public Application
{
public:
	IntroToOpenGL();
	virtual ~IntroToOpenGL();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	float m_timer;
	mat4 m_projection;

};



#endif //_INTROTOOPENGL_H_