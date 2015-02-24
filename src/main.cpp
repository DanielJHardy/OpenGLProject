
#include "Lighting.h"

int main()
{
	Lighting app;

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