#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#include "Application.h"
#include "FlyCamera.h"

#include "Vertex.h"
#include <vector>

class Textures : public Application
{
public:
	Textures();
	virtual ~Textures();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void loadTexture(const char* a_filename);

	//public variables
	unsigned int m_texture;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	unsigned int m_programID;

	std::vector<OpenGLInfo> m_gl_info;

private:

	FlyCamera m_sceneCam;
	vec4 m_backColor;




};



#endif 