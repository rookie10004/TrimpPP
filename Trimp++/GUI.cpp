#include "GUI.h"

void GUI::Init(SDL_Window* window, SDL_Renderer* renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    //SetStyle();

    // SDL3 backend Init
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    std::cout << "GUI: Init successfull" << std::endl;
}

void GUI::Draw(Display& display, DataManager& dataManager, bool isDirEmpty)
{
    // ImGui frame start
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // UI
    SelectionWindow(display, dataManager, isDirEmpty);
    TrainingOverviewWindow(display, dataManager);
    ZoneWindow(display, dataManager);
    HeartRateWindow( display, dataManager);

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

void GUI::SelectionWindow(Display& display, DataManager& dataManager, bool isDirEmpty)
{
    ImGui::SetNextWindowSize(ImVec2(display.GetWidth() * 0.25, display.GetHeight()), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);

    ImGui::Begin((std::string("Directory: ") + DIRECTORY).c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse);
    
    ImGui::SeparatorText("Select Workout");

    const auto& list = dataManager.GetList();
    static int selectedIndex = -1;

    ImGui::Text("Date          Sport");
    if (!isDirEmpty)
    {
        if (ImGui::BeginListBox("##WorkoutListBox", ImVec2(-FLT_MIN, 520)))
        {
            for (int i = 0; i < list.size(); i++)
            {
                std::string label = list[i].date + "    " + list[i].sport;

                bool isSelected = (selectedIndex == i);

                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    selectedIndex = i;

                    std::cout << "DataManager: load file: " << list[i].fileName << std::endl;
                    dataManager.LoadFromCSV(list[i].fileName);
                }
            }
        }
        else
        {
            if (ImGui::BeginListBox("##Liste", ImVec2(-FLT_MIN, 520)))
            {
                ImGui::Text("N/A     N/A    N/A     N/A");
                ImGui::Text("Directory is empty...");
            }
        }
    }

    ImGui::EndListBox();

    ImGui::End();
}

void GUI::TrainingOverviewWindow(Display& display, DataManager& dataManager)
{
    ImGui::SetNextWindowSize(ImVec2(display.GetWidth() * 0.75 / 2, display.GetHeight() * 0.6), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(display.GetWidth() * 0.25, display.GetHeight() * 0.4), ImGuiCond_Once);

    ImGui::Begin("Overview", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse);

    WorkoutSummary list = dataManager.GetSummary();

    ImGui::SeparatorText("Basic Data");
    ImGui::Text("Sport:     %s", list.sport.c_str());
    ImGui::Text("Date:      %s", list.date.c_str());
    ImGui::Text("Duration:  %s", list.duration.c_str());
    ImGui::Text("Day Time:  %s", list.startTime.c_str());
    ImGui::Text("Calories:  %d", list.calories);
    ImGui::Text("File:      %s", list.fileName.c_str());
    ImGui::Text("Notes:     ");
    ImGui::TextWrapped(list.notes.c_str());

    ImGui::SeparatorText("Heart Rate Data");
    ImGui::Text("Min HR:    %d", list.minHR);
    ImGui::Text("Max HR:    %d", list.maxHR);
    ImGui::Text("Avg HR:    %d", list.avarageHR);
    ImGui::Text("TRIMP:     %.1f", list.trimp);
    //...

    ImGui::End();
}

void GUI::ZoneWindow(Display& display, DataManager& dataManager)
{
    ImGui::SetNextWindowSize(ImVec2(display.GetWidth() * 0.75 / 2, display.GetHeight() * 0.6), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(display.GetWidth() * 0.25 + display.GetWidth() * 0.75 / 2, display.GetHeight() * 0.4), ImGuiCond_Once);

    ImGui::Begin("Zones", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::End();
}

void GUI::HeartRateWindow(Display& display, DataManager& dataManager)
{
    ImGui::SetNextWindowSize(ImVec2(display.GetWidth() * 0.75, display.GetHeight() * 0.4), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(display.GetWidth() * 0.25, 0), ImGuiCond_Once);

    ImGui::Begin("Heart Rate", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::End();
}
