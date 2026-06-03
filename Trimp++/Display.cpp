#include "Display.h"

Display::~Display()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::Init()
{
    //init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Display: SDL could not be initialized: " << SDL_GetError() << std::endl;
        return;
    }

    //create window
    window = SDL_CreateWindow(title, width, height, NULL);
    if (!window)
    {
        std::cerr << "Display: Window could not be created: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    std::cout << "Display: Init successfull" << std::endl;
}