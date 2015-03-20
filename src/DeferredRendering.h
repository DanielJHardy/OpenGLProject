#ifndef _DEFERREDRENDERING_H
#define _DEFERREDRENDERING_H

#include "Application.h"
#include "FlyCamera.h"

#include "AntTweakBar.h"

#include "Vertex.h"

class DeferredRendering : public Application
{
public:
	DeferredRendering();
	virtual ~DeferredRendering();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void buildMesh();
	void buildQuad();
	void renderDirectionalLight(vec3 light_dir, vec3 light_color);


	void buildGbuffer();
	void buildLightBuffer();

	//load mesh
	OpenGLInfo m_bunny;
	OpenGLInfo m_screenspace_quad;

	//generate g-buffer
	unsigned int m_gbuffer_fbo;
	unsigned int m_albedo_texture;
	unsigned int m_position_texture;
	unsigned int m_normals_texture;
	unsigned int m_gbuffer_depth;

	//render lights
	unsigned int m_light_fbo;
	unsigned int m_light_texture;

	//shaders
	unsigned int m_gbuffer_program;
	unsigned int m_directional_light_program;
	unsigned int m_point_light_program;
	unsigned int m_spot_light_program;
	unsigned int m_composite_program;

	//composite pass

private:

	FlyCamera m_sceneCam;

	vec4 m_backColor;
	float m_fps;
	TwBar* m_bar;

	vec3 m_lightDir;

};



#endif //_CAMERAANDPROJECTIONS_H_