#include "GUI.h"

void GUI::Init(SDL_Window* window, SDL_Renderer* renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    SetStyle();

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
    /*ImGuiStyle& style = ImGui::GetStyle();

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

    style.Colors[ImGuiCol_PopupBg] = black;*/

    ImPlot::AddColormap("CustomZoneColors", customColors, 6);
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
                std::string label = list[i].date + "    " + list[i].sport + "##" + std::to_string(i);

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

    WorkoutData list = dataManager.GetData();

    ImGui::SeparatorText("Basic Data");
    ImGui::Text("Sport:        %s", list.sport.c_str());
    ImGui::Text("Date:         %s", list.date.c_str());
    ImGui::Text("Duration:     %s", list.duration.c_str());
    ImGui::Text("Day Time:     %s", list.startTime.c_str());
    ImGui::Text("Calories:     %d kcal", list.calories);
    ImGui::Text("File:         %s", list.fileName.c_str());
    ImGui::Text("Notes:       ");
    ImGui::SameLine();
    if (!list.notes.empty())
    {
        ImGui::TextWrapped(list.notes.c_str());
    }
    else
    {
        ImGui::Text("-");
    }

    ImGui::SeparatorText("Heart Rate Data");
    ImGui::Text("Avg HR:       %d", list.avarageHR);
    ImGui::SameLine(134.0f);
    ImGui::Text("bpm");
    ImGui::Text("Min HR:       %d", list.minHR);
    ImGui::SameLine(134.0f);
    ImGui::Text("bpm");
    if (!list.hRData.empty() && list.minHRIndex >= 0 && list.minHRIndex < list.hRData.size())
    {
        ImGui::SameLine(190.0f);
        ImGui::Text("Time:          %-6.0f", list.hRData[list.minHRIndex].timeStamp);
        ImGui::SameLine(340.0f);
        ImGui::Text("sec");
    }

    ImGui::Text("Max HR:       %d", list.maxHR);
    ImGui::SameLine(134.0f);
    ImGui::Text("bpm");

    if (!list.hRData.empty() && list.maxHRIndex >= 0 && list.maxHRIndex < list.hRData.size())
    {
        ImGui::SameLine(190.0f);
        ImGui::Text("Time:          %-6.0f", list.hRData[list.maxHRIndex].timeStamp);
        ImGui::SameLine(340.0f);
        ImGui::Text("sec");
    }

    ImGui::Text("TRIMP:        %-7.1f", list.trimp);
    ImGui::SameLine(190.0f);
    ImGui::Text("TRIMP norm:    %-7.1f", list.trimp);
    ImGui::Text("Peaks:        %d", list.peaks);
    ImGui::SameLine(190.0f);
    ImGui::Text("Peaks norm:    %-7.1f", list.peaksNorm);
    ImGui::Text("Recovery:     %d", list.recovery);
    ImGui::SameLine(190.0f);
    ImGui::Text("Recovery norm: %d", list.recoveryNorm);
    ImGui::Text("Performance:  %-7.1f", list.performance);

    ImGui::End();
}

void GUI::ZoneWindow(Display& display, DataManager& dataManager)
{
    ImGui::SetNextWindowSize(ImVec2(display.GetWidth() * 0.75 / 2, display.GetHeight() * 0.6), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(display.GetWidth() * 0.25 + display.GetWidth() * 0.75 / 2, display.GetHeight() * 0.4), ImGuiCond_Once);

    ImGui::Begin("Zones", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse);

    const auto& data = dataManager.GetData();

    if (data.hRData.empty())
    {
        ImGui::Text("N/A");
        ImGui::End();
        return;
    }

    std::string zoneLabels[6] = {
        "Z0 (<" + std::to_string(data.zones[0]) + "%)",
        "Z1 (" + std::to_string(data.zones[0]) + "-" + std::to_string(data.zones[1]) + "%)",
        "Z2 (" + std::to_string(data.zones[1]) + "-" + std::to_string(data.zones[2]) + "%)",
        "Z3 (" + std::to_string(data.zones[2]) + "-" + std::to_string(data.zones[3]) + "%)",
        "Z4 (" + std::to_string(data.zones[3]) + "-" + std::to_string(data.zones[4]) + "%)",
        "Z5 (" + std::to_string(data.zones[4]) + "-" + std::to_string(data.zones[5]) + "%)"
    };

    std::vector<double> activeCounts;
    std::vector<const char*> activeLabels;

    for (int i = 0; i < 6; ++i)
    {
        if (data.zoneDurations[i] > 0.0)
        {
            activeCounts.push_back(data.zoneDurations[i]);
            activeLabels.push_back(zoneLabels[i].c_str());
        }
    }

    ImVec2 plotSize = ImGui::GetContentRegionAvail();

    ImPlot::PushColormap("CustomZoneColors");

    if (ImPlot::BeginPlot("##ZonesPie", plotSize, ImPlotFlags_NoInputs | ImPlotFlags_NoMouseText | ImPlotFlags_NoTitle | ImPlotFlags_Equal))
    {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupLegend(ImPlotLocation_NorthEast, ImPlotLegendFlags_None);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 1.0, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 1.0, ImGuiCond_Always);

        ImPlot::PlotPieChart(activeLabels.data(), activeCounts.data(), static_cast<int>(activeCounts.size()), 0.5, 0.5, 0.44f, "%.0f sec", 180.0);

        ImPlot::EndPlot();
    }

    ImPlot::PopColormap();

    ImGui::End();
}

void GUI::HeartRateWindow(Display& display, DataManager& dataManager)
{
    ImGui::SetNextWindowSize(ImVec2(display.GetWidth() * 0.75, display.GetHeight() * 0.4), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(display.GetWidth() * 0.25, 0), ImGuiCond_Once);

    ImGui::Begin("Heart Rate", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse);

    const auto& data = dataManager.GetData();

    if (data.hRData.empty())
    {
        ImGui::Text("N/A");
        ImGui::End();
        return;
    }
    
    ImVec2 plotSize = ImGui::GetContentRegionAvail();

    if (ImPlot::BeginPlot("##HeartRatePlot", plotSize))
    {
        ImPlot::SetupAxes("Time [sec]", "Heart Rate [bpm]");

        ImPlot::SetupAxisLimits(ImAxis_X1, 0, data.hRData.back().timeStamp, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, data.minHR - 10, data.maxHR + 15, ImGuiCond_Always);

        std::vector<double> timeStamps, heartRates;
        timeStamps.reserve(data.hRData.size());
        heartRates.reserve(data.hRData.size());
        for (const auto& hr : data.hRData) {
            timeStamps.push_back(hr.timeStamp);
            heartRates.push_back(static_cast<double>(hr.hR));
        }

        ImPlot::PlotLine("HR##HeartRate", timeStamps.data(), heartRates.data(), static_cast<int>(timeStamps.size()));

        if (data.maxHR > 0 && !data.hRData.empty())
        {
            double peakX = data.hRData[data.maxHRIndex].timeStamp;
            double peakY = data.maxHR;

            ImPlotSpec spec;
            spec.Marker = ImPlotMarker_Circle;
            spec.MarkerSize = 3.0f;
            spec.MarkerLineColor = ImVec4(1.0f, 0.25f, 0.25f, 1.0f);
            spec.MarkerFillColor = ImVec4(1.0f, 0.25f, 0.25f, 1.0f);

            ImPlot::PlotScatter("Max HR", &peakX, &peakY, 1, spec);

            //std::string labelText = std::to_string(data.maxHR) + " bpm";
            //ImPlot::PlotText(labelText.c_str(), peakX, peakY, ImVec2(0, -10));
        }

        ImPlot::EndPlot();

        //logik für checkbox die jeden peak ab grenze makiert
    }

    ImGui::End();
}
