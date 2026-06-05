#pragma once

#include <imgui.h>
#include <implot.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>

#include "Display.h"
#include "DataManager.h"

class GUI
{
private:
	// color vec
	ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 grey = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	ImVec4 lightGrey = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 superLightGrey = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	ImVec4 darkGrey = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

public:

	void Init(SDL_Window* window, SDL_Renderer* renderer);

	void Draw(Display& display, DataManager& dataManager, bool isDirEmpty);
	void SetStyle();

	void SelectionWindow(Display& display, DataManager& dataManager, bool isDirEmpty);
	void TrainingOverviewWindow(Display& display, DataManager& dataManager);
	void ZoneWindow(Display& display, DataManager& dataManager);
	void HeartRateWindow(Display& display, DataManager& dataManager);
};

