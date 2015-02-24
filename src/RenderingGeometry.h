#ifndef _RENDERINGGEOMETRY_H_
#define _RENDERINGGEOMETRY_H_

#include "Application.h"
#include "FlyCamera.h"

#include "tiny_obj_loader.h"

#include "Vertex.h"

class RenderingGeometry : public Application
{
public:
	RenderingGeometry();
	virtual ~RenderingGeometry();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void createOpenGLBuffers(std::vector<tinyobj::shape_t>&shapes);

	void GenerateShaders();

	//public variables
	unsigned int m_programID;

	std::vector<OpenGLInfo> m_gl_info;

private:

	FlyCamera m_sceneCam;
	vec4 m_backColor;




};



#endif 