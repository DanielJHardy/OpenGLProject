#ifndef _ADVANCEDTEXTURING_H_
#define _ADVANCEDTEXTURING_H_

#include "Application.h"
#include "FlyCamera.h"

#include "AntTweakBar.h"

#include "Vertex.h"
#include <vector>

class AdvancedTexturing : public Application
{
public:
	AdvancedTexturing();
	virtual ~AdvancedTexturing();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void LoadTexture(const char* a_filename, unsigned int* a_texturePos);
	void LoadTextures(const char* a_diff_file, const char* a_norm_file);

	unsigned int m_programID;
	unsigned int m_texture, m_normalMap;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	vec3 m_lightDir;

private:

	float m_dt;
	FlyCamera m_sceneCam;

	vec4 m_backColor;
	TwBar* m_bar;

};



#endif 