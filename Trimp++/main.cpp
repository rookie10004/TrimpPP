#include "Application.h"

int main()
{

	Application app;
	app.Init();
	app.Setup();
	app.Update();

	while (app.GetIsRunning())
	{
		app.InputHandle();
		app.Render();
	}

	return 0;
}