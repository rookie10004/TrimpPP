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
		std::cerr << "DataManager: Error on opening .csv file" << std::endl;
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

    CalculateZoneDuration();
    CalculateTrimp();

    return false;
}

void DataManager::CalculateZoneDuration()
{
    if (workoutData.hRData.empty()) return;

    for (const auto& hrPoint : workoutData.hRData)
    {
        double intensity = (static_cast<double>(hrPoint.hR) / hRMax) * 100.0;

        if (intensity < z0) workoutData.zoneDurations[0]++;
        else if (intensity >= z0 && intensity < z1) workoutData.zoneDurations[1]++;
        else if (intensity >= z1 && intensity < z2) workoutData.zoneDurations[2]++;
        else if (intensity >= z2 && intensity < z3) workoutData.zoneDurations[3]++;
        else if (intensity >= z3 && intensity < z4) workoutData.zoneDurations[4]++;
        else if (intensity >= z4 && intensity <= z5) workoutData.zoneDurations[5]++;
    }
    workoutData.zoneDurations[0] /= 60;
    workoutData.zoneDurations[1] /= 60;
    workoutData.zoneDurations[2] /= 60;
    workoutData.zoneDurations[3] /= 60;
    workoutData.zoneDurations[4] /= 60;
    workoutData.zoneDurations[5] /= 60;
}

void DataManager::CalculateTrimp()
{
    if (workoutData.hRData.empty())
    {
        std::cerr << "DataManager: no HR-Data for TRIMP-calulcation" << std::endl;
        return;
    }

    double totalTrimp = 0.0;
    int totalCount = 0;
    int totalHR = 0;

    for (int i = 0; i < workoutData.hRData.size(); ++i)
    {
        int hr = workoutData.hRData[i].hR;

        if (hr > workoutData.maxHR)
        {
            workoutData.maxHR = hr;
            workoutData.maxHRIndex = i;
        }
        if (hr < workoutData.minHR)
        {
            workoutData.minHR = hr;
            workoutData.minHRIndex = i;
        }

        double intensity = (static_cast<double>(hr) / hRMax) * 100.0;
        double factor = 0.0;

        if (intensity >= z0 && intensity < z1)
        {
            factor = 1.0;
        }
        else if (intensity >= z1 && intensity < z2)
        {
            factor = 2.0;
        }
        else if (intensity >= z2 && intensity < z3)
        {
            factor = 3.0;
        }
        else if (intensity >= z3 && intensity < z4)
        {
            factor = 4.0;
        }
        else if (intensity >= z4 && intensity <= z5)
        {
            factor = 5.0;
        }
        else if (intensity < z0)
        {
            factor = 0.5;
        }

        totalTrimp += factor;
        totalHR += hr;
        totalCount++;
    }

    if (totalCount > 0)
    {
        workoutData.avarageHR = totalHR / totalCount;
    }

    workoutData.minHRTime = ConvertTimeToString(workoutData.hRData[workoutData.minHRIndex].time);
    workoutData.maxHRTime = ConvertTimeToString(workoutData.hRData[workoutData.maxHRIndex].time);

    workoutData.trimp = totalTrimp / 60.0;
}

void DataManager::CalculatePeaks()
{
}

void DataManager::CalculateRecovery()
{
}

void DataManager::CalculatePerformance()
{
    workoutData.performance = a * workoutData.trimpNorm + b * workoutData.peaksNorm + c * workoutData.recoveryNorm;
}

void DataManager::WorkoutDataClear()
{
    workoutData.hRData.clear();
    workoutData.peaksData.clear();
    workoutData.zoneDurations.fill(0.0);

    workoutData.sport = "";
    workoutData.date = "";
    workoutData.duration = "";
    workoutData.fileName = "";
    workoutData.calories = -1;
    workoutData.height = -1;
    workoutData.weight = -1.0;
    workoutData.notes = "";
    workoutData.startTime = "";

    workoutData.avarageHR = -1;
    workoutData.minHR = 999;
    workoutData.maxHR = -1;
    workoutData.minHRIndex = 0;
    workoutData.maxHRIndex = 0;

    workoutData.trimp = -1.0;
    workoutData.trimpNorm = -1.0;
    workoutData.peaks = -1;
    workoutData.peaksNorm = -1.0;
    workoutData.recovery = -1;
    workoutData.recoveryNorm = -1;
    workoutData.performance = -1.0;
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

