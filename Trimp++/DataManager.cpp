#include "DataManager.h"

DataManager::DataManager()
{
    for (std::filesystem::directory_iterator::value_type entry : std::filesystem::directory_iterator("../data"))
    {
        fileCount++;
    }

    workoutData.zones = { z0, z1, z2, z3, z4, z5 };

    std::cout << "DataManager: files detected " << fileCount << std::endl;
}

bool DataManager::LoadDirectory()
{
    if (!std::filesystem::exists(DIRECTORY) || !std::filesystem::is_directory(DIRECTORY))
    {
        isDirEmpty = true;
        std::cerr << "DataManager: directory: " << DIRECTORY << " does not exist!" << std::endl;
        return false;
    }

    workoutList.clear();

    for (const auto& entry : std::filesystem::directory_iterator(DIRECTORY))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".CSV")
        {
            std::ifstream file(entry.path());
            
            if (!file.is_open()) continue;

            std::string headerLine;
            std::string dataLine;

            std::getline(file, headerLine);

            if (std::getline(file, dataLine))
            {
                std::vector<std::string> columns;
                std::string current_field = "";
                bool in_quotes = false;

                for (char c : dataLine)
                {
                    if (c == '"') {
                        in_quotes = !in_quotes;
                    }
                    else if (c == ',' && !in_quotes) {
                        columns.push_back(current_field);
                        current_field = "";
                    }
                    else {
                        current_field += c;
                    }
                }
                columns.push_back(current_field);

                /* mapping
                    Index 1: sport
                    Index 2: date
                */

                if (columns.size() > 4)
                {
                    WorkoutList item;
                    item.fileName = entry.path().filename().string();
                    item.sport = columns[1];
                    item.date = columns[2];

                    workoutList.push_back(item);
                }
            }
            file.close();
        }
    }

    if (workoutList.empty())
    {
        isDirEmpty = true;
        std::cerr << "DataManager: directory: " << DIRECTORY << " empty" << std::endl;
        return false;
    }

    isDirEmpty = false;
    return true;
}

bool DataManager::LoadFromCSV(const std::string& fileName)
{
    WorkoutDataClear();
    workoutData.fileName = fileName;
	std::ifstream file(DIRECTORY + fileName);
	if (!file.is_open())
	{
		std::cerr << "DataManager: error on opening .csv file" << std::endl;
		return false;
	}

	std::string line;

    // column 1
	if (!std::getline(file, line)) 
    {
		std::cerr << "DataManager: .csv file is empty" << std::endl;
		file.close();
		return false;
	}

    // column 2
    if (std::getline(file, line))
    {
        std::vector<std::string> columns;
        std::string current_field = "";
        bool in_quotes = false;

        for (char c : line)
        {
            if (c == '"')
            {
                in_quotes = !in_quotes;
            }
            else if (c == ',' && !in_quotes)
            {
                columns.push_back(current_field);
                current_field = "";
            }
            else
            {
                current_field += c;
            }
        }
        columns.push_back(current_field);

        /* mapping
            index 1: sport
            index 2: date
            index 3: startTime
            index 4: duration
            index 11: calories
            index 23: notes
            index 24: height
            index 25: weight 
         */


        if (columns.size() > 25) 
        {
            workoutData.sport = columns[1];
            workoutData.date = columns[2];
            workoutData.startTime = columns[3];
            workoutData.duration = columns[4];

            workoutData.calories = columns[11].empty() ? 0 : static_cast<int>(std::stof(columns[11]));
            workoutData.notes = columns[23];

            workoutData.height = columns[24].empty() ? 0 : static_cast<int>(std::stof(columns[24]));
            workoutData.weight = columns[25].empty() ? 0 : std::stof(columns[25]);
        }

    }

    // column 3
    if (!std::getline(file, line))
    {
        file.close();
        return true;
    }

    // column 4
    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string sampleRate;
        std::string timeStr;
        std::string hrStr;

        if (std::getline(ss, sampleRate, ',') &&
            std::getline(ss, timeStr, ',') &&
            std::getline(ss, hrStr, ','))
        {
            if (!hrStr.empty() && !timeStr.empty())
            {
                try
                {
                    int hr = std::stoi(hrStr);

                    int hours = 0, minutes = 0, seconds = 0;
                    std::stringstream timeSS(timeStr);
                    std::string chunk;

                    if (std::getline(timeSS, chunk, ':')) hours = std::stoi(chunk);
                    if (std::getline(timeSS, chunk, ':')) minutes = std::stoi(chunk);
                    if (std::getline(timeSS, chunk, ':')) seconds = std::stoi(chunk);

                    int totalSeconds = (hours * 3600) + (minutes * 60) + seconds;

                    workoutData.hRData.push_back({ hr, totalSeconds });
                }
                catch (...)
                {
                    continue;
                }
            }
        }
    }
    file.close();

    AnalyzeWorkoutData();

    return false;
}

void DataManager::AnalyzeWorkoutData()
{
    if (workoutData.hRData.empty())
    {
        std::cerr << "DataManager: no HR-data for calculation/analyzing" << std::endl;
        return;
    }

    long long totalHR = 0;

    for (int i = 0; i < workoutData.hRData.size() - 1; ++i)
    {
        totalHR += workoutData.hRData[i].hR;

        // minHR/maxHR
        if (workoutData.hRData[i].hR < workoutData.minHR)
        {
            workoutData.minHR = workoutData.hRData[i].hR;
            workoutData.minHRIndex = i;
        }
        if (workoutData.hRData[i].hR > workoutData.maxHR)
        {
            workoutData.maxHR = workoutData.hRData[i].hR;
            workoutData.maxHRIndex = i;
        }

        double intensity = (static_cast<double>(workoutData.hRData[i].hR) / hRMax) * 100.0;

        // peaks
        if (i > 0 && i < workoutData.hRData.size() - 1 && intensity >= z0)
        {
            int currentHR = workoutData.hRData[i].hR;
            int prevHR = workoutData.hRData[i - 1].hR;

            if (currentHR > prevHR)
            {
                int forwardIndex = i + 1;
                while (forwardIndex < workoutData.hRData.size() && workoutData.hRData[forwardIndex].hR == currentHR)
                {
                    forwardIndex++;
                }

                if (forwardIndex < workoutData.hRData.size())
                {
                    int nextHRAfterPlateau = workoutData.hRData[forwardIndex].hR;

                    if (currentHR > nextHRAfterPlateau)
                    {
                        workoutData.peaksData.push_back(workoutData.hRData[i]);
                        workoutData.peaks++;
                    }

                    i = forwardIndex - 1;
                }
                else
                {
                    i = forwardIndex - 1;
                }
            }
        }

        // zone duration
        if (intensity < z0) workoutData.zoneDurations[0]++;
        else if (intensity >= z0 && intensity < z1) workoutData.zoneDurations[1]++;
        else if (intensity >= z1 && intensity < z2) workoutData.zoneDurations[2]++;
        else if (intensity >= z2 && intensity < z3) workoutData.zoneDurations[3]++;
        else if (intensity >= z3 && intensity < z4) workoutData.zoneDurations[4]++;
        else if (intensity >= z4 && intensity <= z5) workoutData.zoneDurations[5]++;
    }

    // avgHR
    workoutData.avarageHR = static_cast<int>(totalHR / workoutData.hRData.size());

    // time convert in min
    workoutData.minHRTime = ConvertTimeToString(workoutData.hRData[workoutData.minHRIndex].time);
    workoutData.maxHRTime = ConvertTimeToString(workoutData.hRData[workoutData.maxHRIndex].time);

    CalculateTrimp();
    CalculatePeaks();
    CalculateRecovery();
    CalculatePerformance();
}

void DataManager::CalculateTrimp()
{
    // TRIMP = ∑ (time_in_zone_min * zone_multiplier)
    double totalTrimp = 0.0;

    totalTrimp += workoutData.zoneDurations[0] * 0.5;
    totalTrimp += workoutData.zoneDurations[1] * 1.0;
    totalTrimp += workoutData.zoneDurations[2] * 2.0;
    totalTrimp += workoutData.zoneDurations[3] * 3.0;
    totalTrimp += workoutData.zoneDurations[4] * 4.0;
    totalTrimp += workoutData.zoneDurations[5] * 5.0;

    workoutData.trimp = totalTrimp / 60; // convert in min

    // TRIMP_NORM = TRIMP per hour
    double totalHours = static_cast<double>(workoutData.hRData.back().time) / 3600.0;
    double trimpPerHour = workoutData.trimp / totalHours;

    workoutData.trimpNorm = std::clamp(trimpPerHour / MAX_TRIMP_PER_HOUR, 0.0, 1.0);
}

void DataManager::CalculatePeaks()
{
    if (workoutData.peaksData.empty())
    {
        std::cout << "DataManager: no peak-data for calculation/analyzing" << std::endl;
        return;
    }

    // PEAKS = Σ peak
    workoutData.peaks = static_cast<int>(workoutData.peaksData.size());

    // PEAKS_NORM = PEAKS per hour
    double totalSeconds = static_cast<double>(workoutData.hRData.back().time);
    
    if (totalSeconds > 0.0)
    {
        double totalHours = totalSeconds / 3600.0;
        double peaksPerHour = static_cast<double>(workoutData.peaks) / totalHours;

        workoutData.peaksNorm = std::clamp(peaksPerHour / MAX_PEAKS_PER_HOUR, 0.0, 1.0);
        return;
    }
}

void DataManager::CalculateRecovery()
{
    if (workoutData.peaksData.empty())
    {
        std::cout << "DataManager: no peak-data for calculation/analyzing" << std::endl;
        return;
    }

    // RECOVERY = Σ (hr_peak - hr_after_Xsec)
    int evaluablePeaksCount = 0; 

    for (size_t i = 0; i < workoutData.peaksData.size(); i++)
    {
        int currentPeakTime = workoutData.peaksData[i].time;
        int targetTime = currentPeakTime + SECONDS_AFTER_PEAK;

        bool nextPeakInterrupted = false;
        if (i + 1 < workoutData.peaksData.size())
        {
            if (workoutData.peaksData[i + 1].time < targetTime)
            {
                nextPeakInterrupted = true;
            }
        }

        if (nextPeakInterrupted) continue;

        bool validRecoveryPhase = true;
        int lastHR = workoutData.peaksData[i].hR;

        for (int t = currentPeakTime + 1; t <= targetTime; ++t)
        {
            if (t >= 0 && t < static_cast<int>(workoutData.hRData.size()))
            {
                int currentHR = workoutData.hRData[t].hR;

                if (currentHR > lastHR)
                {
                    validRecoveryPhase = false;
                    break;
                }
                lastHR = currentHR;
            }
            else
            {
                validRecoveryPhase = false;
                break;
            }
        }

        if (validRecoveryPhase)
        {
            int targetIndex = targetTime;
            if (targetIndex >= 0 && targetIndex < static_cast<int>(workoutData.hRData.size()))
            {
                double peakHR = workoutData.peaksData[i].hR;
                double afterHR = workoutData.hRData[targetIndex].hR;

                workoutData.recovery += (peakHR - afterHR);
                evaluablePeaksCount++;
            }
        }
    }

    // RECOVERY_NORM = RECOVERY / PEAKS
    if (evaluablePeaksCount > 0)
    {
        double avgRecoveryDrop = workoutData.recovery / evaluablePeaksCount;

        workoutData.recoveryNorm = std::clamp(avgRecoveryDrop / MAX_RECOVERY_DROP, 0.0, 1.0);
    }
    else
    {
        workoutData.recoveryNorm = 0.0;
        std::cout << "DataManager: Warning - No evaluable recovery phases found (All peaks clustered)" << std::endl;
    }
}

void DataManager::CalculatePerformance()
{
    workoutData.performance = a * workoutData.trimpNorm + b * workoutData.peaksNorm + c * workoutData.recoveryNorm;
}

void DataManager::WorkoutDataClear()
{
    workoutData.hRData.clear();
    workoutData.peaksData.clear();
    workoutData.zoneDurations.fill(0);

    workoutData.sport = "";
    workoutData.date = "";
    workoutData.duration = "";
    workoutData.fileName = "";
    workoutData.calories = 0;
    workoutData.height = 0;
    workoutData.weight = 0.0;
    workoutData.notes = "";
    workoutData.startTime = "";

    workoutData.avarageHR = 0;
    workoutData.minHR = 999;
    workoutData.maxHR = 0;
    workoutData.minHRIndex = 0;
    workoutData.maxHRIndex = 0;
    workoutData.minHRTime = "";
    workoutData.maxHRTime = "";

    workoutData.trimp = 0.0;
    workoutData.trimpNorm = 0.0;
    workoutData.peaks = 0;
    workoutData.peaksNorm = 0.0;
    workoutData.recovery = 0;
    workoutData.recoveryNorm = 0;
    workoutData.performance = 0.0;
}

std::string DataManager::ConvertTimeToString(int totalSeconds)
{
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds;

    return oss.str();
}

