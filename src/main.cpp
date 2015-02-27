
#include "GPUParticles.h"

int main()
{
	GPUParticles app;

	if (app.Startup() == false)
	{
		return -1;
	}

	while (app.Update() == true)
	{
		app.Draw();
	}

	return 0;

	
}