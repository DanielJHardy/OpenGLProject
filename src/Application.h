#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

//#include "tiny_obj_loader.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

struct GLFWwindow;

struct OpenGLInfo
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_index_count;
};

class Application
{
public:
	Application();
	Application(char* a_title, int a_x, int a_y)
	{
		m_title = a_title;
		m_screenX = a_x;
		m_screenX = a_y;

	}
	virtual ~Application();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();


	//void createOpenGLBuffers(std::vector<tinyobj::shape_t> &shapes);

	GLFWwindow* m_window;

	char* m_title;
	int m_screenX;
	int m_screenY;

	//std::vector<OpenGLInfo> m_gl_info;
};



#endif //_APPLICATION_H_