#include "GUI.h"

void GUI::Init(SDL_Window* window, SDL_Renderer* renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    //SetStyle();

    //SDL3 Backend init
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    std::cout << "GUI: Init successfull" << std::endl;
}

void GUI::Draw(Display& display, const DataManager& dataManager)
{
    //ImGui Frame start
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    //UI
    SelectionWindow(display, dataManager);
    TrainingOverviewWindow(display);
    ZoneWindow(display);
    HeartRateWindow( display);


    ImGui::Render();
}

void GUI::SetStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_WindowBg] = black;

    style.Colors[ImGuiCol_Text] = white;

    style.Colors[ImGuiCol_TitleBg] = darkGrey;
    style.Colors[ImGuiCol_TitleBgActive] = lightGrey;

    style.Colors[ImGuiCol_Border] = grey;
    style.Colors[ImGuiCol_BorderShadow] = grey;

    style.Colors[ImGuiCol_Button] = grey;
    style.Colors[ImGuiCol_ButtonActive] = superLightGrey;
    style.Colors[ImGuiCol_ButtonHovered] = lightGrey;

    style.Colors[ImGuiCol_Separator] = grey;
    style.Colors[ImGuiCol_SeparatorActive] = superLightGrey;
    style.Colors[ImGuiCol_SeparatorHovered] = lightGrey;

    style.Colors[ImGuiCol_FrameBg] = grey;
    style.Colors[ImGuiCol_FrameBgActive] = superLightGrey;
    style.Colors[ImGuiCol_FrameBgHovered] = lightGrey;

    style.Colors[ImGuiCol_Header] = grey;
    style.Colors[ImGuiCol_HeaderActive] = superLightGrey;
    style.Colors[ImGuiCol_HeaderHovered] = lightGrey;

    style.Colors[ImGuiCol_PopupBg] = black;
}

void GUI::SelectionWindow(Display& display, const DataManager& dataManager)
{
    ImGui::SetNextWindowSize(ImVec2(display.GetWidth() * 0.25, display.GetHeight()), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);

    ImGui::Begin("Directory: Trimp++/data", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse);
    
    ImGui::SeparatorText("Select Workout");

    ImGui::Text("            Date   Time    File");
    if (ImGui::BeginListBox("##Liste", ImVec2(-FLT_MIN, 520)))
    {
        for (int i = 0; i < dataManager.GetFileCount(); i++)
        {
            // icon
            ImGui::Text("icon   19/01/2026  00:00:00    weawda");
        }
    }
    ImGui::EndListBox();

    ImGui::End();
}

void GUI::TrainingOverviewWindow(Display& display)
{
    ImGui::SetNextWindowSize(ImVec2(display.GetWidth() * 0.75 / 2, display.GetHeight() * 0.6), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(display.GetWidth() * 0.25, display.GetHeight() * 0.4), ImGuiCond_Once);

    ImGui::Begin("Overview", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::End();
}

void GUI::ZoneWindow(Display& display)
{
    ImGui::SetNextWindowSize(ImVec2(display.GetWidth() * 0.75 / 2, display.GetHeight() * 0.6), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(display.GetWidth() * 0.25 + display.GetWidth() * 0.75 / 2, display.GetHeight() * 0.4), ImGuiCond_Once);

    ImGui::Begin("Zones", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::End();
}

void GUI::HeartRateWindow(Display& display)
{
    ImGui::SetNextWindowSize(ImVec2(display.GetWidth() * 0.75, display.GetHeight() * 0.4), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(display.GetWidth() * 0.25, 0), ImGuiCond_Once);

    ImGui::Begin("Heart Rate", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::End();
}
