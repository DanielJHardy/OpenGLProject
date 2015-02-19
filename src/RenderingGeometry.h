#ifndef _RENDERINGGEOMETRY_H_
#define _RENDERINGGEOMETRY_H_

#include "Application.h"
#include "FlyCamera.h"



class RenderingGeometry : Application
{
public:
	RenderingGeometry();
	virtual ~RenderingGeometry();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

private:
	//std::vector<tinyobj::shape_t> m_shapes;
	//std::vector<tinyobj::material_t> m_materials;

	FlyCamera m_sceneCam;


};


#endif // !_RENDERINGGEOMETRY_H_