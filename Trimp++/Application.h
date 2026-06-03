#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "imgui.h"
#include "Display.h"
#include "GUI.h"

#include <string>
#include <iostream>
#include <fstream>

class Application
{
	private:
		const char* title = "Trimp++";

		bool isRunning = true;

		Display display{ title };
		GUI gui;
		SDL_Renderer* renderer;

	public:
		void Init();
		void Setup();
		bool GetIsRunning() { return isRunning; }
		void InputHandle();
		void Render();
		void Update();

};

