
#include "GPUParticles.h"
#include "Callbacks.h"

int main()
{
	GPUParticles app;

	if (app.Startup() == false)
	{
		return -1;
	}

	SetCallbacks(app.m_window);

	while (app.Update() == true)
	{
		app.Draw();
	}

	return 0;

	
}