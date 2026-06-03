#include "Application.h"

void Application::Init()
{
    display.Init();

    //create renderer
    renderer = SDL_CreateRenderer(display.GetWindow(), NULL);
    if (!renderer)
    {
        std::cerr << "Application: Renderer could not be initialized: " << SDL_GetError() << std::endl;
        return;
    }

    std::cout << "Application: Init successfull" << std::endl;

    gui.Init(display.GetWindow(), renderer);
}

void Application::Setup()
{

}

void Application::InputHandle()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        if (event.type == SDL_EVENT_QUIT)
        {
            isRunning = false;
        }
    }

}

void Application::Render()
{
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    gui.Draw(display);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

    SDL_RenderPresent(renderer);
}

void Application::Update()
{
    
}

