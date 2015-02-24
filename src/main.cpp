
#include "Textures.h"

int main()
{
	Textures  app;

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