#ifndef _APPLICATION_H_
#define _APPLICATION_H_

struct GLFWwindow;

class Application
{
public:
	Application();
	virtual ~Application();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	GLFWwindow* m_window;

};



#endif //_APPLICATION_H_