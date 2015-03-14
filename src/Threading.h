#ifndef _THREADING_H_
#define _THREADING_H_

#include "Application.h"

class Threading : public Application
{
public:
	Threading();
	virtual ~Threading();

	virtual bool Startup();
	virtual void Shutdown();

	virtual bool Update();
	virtual void Draw();

	void RunThreadingTut();

};



#endif 