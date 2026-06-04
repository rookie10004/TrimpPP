#pragma once

#include <SDL3/SDL.h>
#include <iostream>

class Display
{
private:
	const char* title;

	//window size
	int width = 1000;
	int height = 600;

	SDL_Window* window;
public:
	Display(const char* title) : title(title) {}
	~Display();

	void Init();

	int GetWidth() { return width; }
	int GetHeight() { return height; }
	SDL_Window* GetWindow() { return window; }
};

