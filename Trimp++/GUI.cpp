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
    ImGui::Text("Sport:       ");
    ImGui::SameLine();
    if (!list.sport.empty()) ImGui::Text(list.sport.c_str());
    else ImGui::Text("-");

    ImGui::Text("Date:        ");
    ImGui::SameLine();
    if (!list.date.empty()) ImGui::Text(list.date.c_str());
    else ImGui::Text("-");

    ImGui::Text("Duration:    ");
    ImGui::SameLine();
    if (!list.duration.empty()) ImGui::Text(list.duration.c_str());
    else ImGui::Text("-");

    ImGui::Text("Day Time:    ");
    ImGui::SameLine();
    if (!list.startTime.empty()) ImGui::Text(list.startTime.c_str());
    else ImGui::Text("-");

    ImGui::Text("Calories:     %d kcal", list.calories);

    ImGui::Text("File:        ");
    ImGui::SameLine();
    if (!list.fileName.empty()) ImGui::TextWrapped(list.fileName.c_str());
    else ImGui::Text("-");

    ImGui::Text("Notes:       ");
    ImGui::SameLine();
    if (!list.notes.empty()) ImGui::TextWrapped(list.notes.c_str());
    else ImGui::Text("-");

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
        ImGui::Text("Time:          %s", list.minHRTime.c_str());
    }

    ImGui::Text("Max HR:       %d", list.maxHR);
    ImGui::SameLine(134.0f);
    ImGui::Text("bpm");

    if (!list.hRData.empty() && list.maxHRIndex >= 0 && list.maxHRIndex < list.hRData.size())
    {
        ImGui::SameLine(190.0f);
        ImGui::Text("Time:          %s", list.maxHRTime.c_str());
    }

    ImGui::Text("TRIMP:        %-7.1f", list.trimp);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * tooltipSize);
        ImGui::TextColored(green, "TRIMP (Training Impulse):");
        ImGui::Text("The accumulated training load calculated by weighting the time spent in each heart rate zone.");
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Formula: sum(time_in_zone_min * zone_multiplier)");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    ImGui::SameLine(190.0f);
    ImGui::Text("TRIMP norm:    %.4f", list.trimpNorm);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * tooltipSize);
        ImGui::TextColored(green, "Normalized TRIMP:");
        ImGui::Text("The TRIMP accumulated per hour, normalized against the maximum expected hourly TRIMP threshold.");
        ImGui::TextColored(lightGrey, "Clamped between 0.0 and 1.0 based on MAX_TRIMP_PER_HOUR = %.2f.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    ImGui::Text("Peaks:        %d", list.peaks);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * tooltipSize);
        ImGui::TextColored(green, "Heart Rate Peaks:");
        ImGui::Text("The total number of detected heart rate peaks where the pulse was higher than its immediate neighbors.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    ImGui::SameLine(190.0f);
    ImGui::Text("Peaks norm:    %.4f", list.peaksNorm);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * tooltipSize);
        ImGui::TextColored(green, "Normalized Peaks:");
        ImGui::Text("The frequency of heart rate peaks per hour, scaled against the maximum expected peaks threshold.");
        ImGui::TextColored(lightGrey, "Clamped between 0.0 and 1.0 based on MAX_PEAKS_PER_HOUR = %.2f.", MAX_PEAKS_PER_HOUR);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    ImGui::Text("Recovery:     %d", list.recovery);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * tooltipSize);
        ImGui::TextColored(green, "Heart Rate Recovery:");
        ImGui::Text("The sum of all heart rate drops measured exactly %d seconds after each detected peak.", SECONDS_AFTER_PEAK);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    ImGui::SameLine(190.0f);
    ImGui::Text("Recovery norm: %.4f", list.recoveryNorm);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * tooltipSize);
        ImGui::TextColored(green, "Normalized Recovery:");
        ImGui::Text("The average heart rate drop per peak, scaled against the target maximum recovery drop threshold.");
        ImGui::TextColored(lightGrey, "Clamped between 0.0 and 1.0 based on MAX_RECOVERY_DROP = %.2f.", MAX_RECOVERY_DROP);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    ImGui::Text("Performance:  %.4f (%.2f%%)", list.performance, list.performance * 100);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * tooltipSize);
        ImGui::TextColored(green, "Performance Score:");
        ImGui::Text("A weighted composite score reflecting overall workout intensity and cardiovascular efficiency.");
        ImGui::TextColored(lightGrey, "Combines normalized TRIMP, Peaks, and Recovery values.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

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
        if (data.zoneDurations[i] > 0)
        {
            activeCounts.push_back(data.zoneDurations[i] / 60.0); // convert to min
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

        ImPlot::PlotPieChart(activeLabels.data(), activeCounts.data(), static_cast<int>(activeCounts.size()), 0.5, 0.5, 0.44f, "%.1f min", 180.0);

        ImPlot::EndPlot();
    }

    ImPlot::PopColormap();

    ImGui::End();
}

void GUI::HeartRateWindow(Display& display, DataManager& dataManager)
{
    ImGui::SetNextWindowSize(ImVec2(display.GetWidth() * 0.75f, display.GetHeight() * 0.4f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(display.GetWidth() * 0.25f, 0), ImGuiCond_Once);

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
        ImPlot::SetupAxes("Time [min]", "Heart Rate [bpm]");

        ImPlot::SetupAxisLimits(ImAxis_X1, 0, data.hRData.back().time / 60.0, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, data.minHR - 10, data.maxHR + 15, ImGuiCond_Always);

        // avgHR line
        if (data.avarageHR > 0 && !data.hRData.empty())
        {
            double avgX[2] = { 0.0, static_cast<double>(data.hRData.back().time) / 60.0 };
            double avgY[2] = { static_cast<double>(data.avarageHR), static_cast<double>(data.avarageHR) };

            ImPlotSpec spec;
            spec.LineColor = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
            spec.LineWeight = 1.0f;

            ImPlot::HideNextItem(true, ImPlotCond_Once);
            ImPlot::PlotLine("Avg HR##AvgHRLine", avgX, avgY, 2, spec);
        }

        // HR
        if (data.hRData.size() >= 2)
        {
            bool legendShown = false;

            for (size_t i = 0; i < data.hRData.size() - 1; ++i)
            {
                const auto& p1 = data.hRData[i];
                const auto& p2 = data.hRData[i + 1];

                double segmentX[2] = { static_cast<double>(p1.time) / 60.0, static_cast<double>(p2.time) / 60.0 };
                double segmentY[2] = { static_cast<double>(p1.hR), static_cast<double>(p2.hR) };

                double intensity = (static_cast<double>(p1.hR) / dataManager.GetHRMax()) * 100.0;

                int zone = 0;
                if (intensity < data.zones[0]) zone = 0;
                else if (intensity >= data.zones[0] && intensity < data.zones[1]) zone = 1;
                else if (intensity >= data.zones[1] && intensity < data.zones[2]) zone = 2;
                else if (intensity >= data.zones[2] && intensity < data.zones[3]) zone = 3;
                else if (intensity >= data.zones[3] && intensity < data.zones[4]) zone = 4;
                else if (intensity >= data.zones[4]) zone = 5;

                ImVec4 zoneColor = ImGui::ColorConvertU32ToFloat4(customColors[zone]);

                ImPlotSpec spec;
                spec.LineColor = zoneColor;
                spec.LineWeight = 1.0f;

                if (!legendShown)
                {
                    ImPlot::PlotLine("##HR", segmentX, segmentY, 2, spec);
                    legendShown = true;
                }
                else
                {
                    ImPlot::PlotLine("##HR_Segment", segmentX, segmentY, 2, spec);
                }
            }
        }

        // peaks
        if (!data.peaksData.empty())
        {
            std::vector<double> peakTimes;
            std::vector<double> peakHRs;
            peakTimes.reserve(data.peaksData.size());
            peakHRs.reserve(data.peaksData.size());

            for (const auto& peak : data.peaksData)
            {
                peakTimes.push_back(static_cast<double>(peak.time) / 60.0);
                peakHRs.push_back(static_cast<double>(peak.hR));
            }

            ImPlotSpec spec;
            spec.Marker = ImPlotMarker_Circle;
            spec.MarkerSize = 1.5f;
            spec.MarkerLineColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            spec.MarkerFillColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

            ImPlot::HideNextItem(true, ImPlotCond_Once);
            ImPlot::PlotScatter("Peaks", peakTimes.data(), peakHRs.data(), static_cast<int>(peakTimes.size()), spec);
        }

        // maxHR
        if (data.maxHR > 0 && !data.hRData.empty())
        {
            double peakX = static_cast<double>(data.hRData[data.maxHRIndex].time) / 60.0;
            double peakY = data.maxHR;

            ImPlotSpec spec;
            spec.Marker = ImPlotMarker_Circle;
            spec.MarkerSize = 4.0f;
            spec.MarkerLineColor = ImVec4(1.0f, 0.25f, 0.25f, 1.0f);
            spec.MarkerFillColor = ImVec4(1.0f, 0.25f, 0.25f, 1.0f);

            ImPlot::HideNextItem(true, ImPlotCond_Once);
            ImPlot::PlotScatter("Max HR", &peakX, &peakY, 1, spec);
        }

        ImPlot::EndPlot();
    }

    ImGui::End();
}