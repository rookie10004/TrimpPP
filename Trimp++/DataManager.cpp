#include "DataManager.h"

DataManager::DataManager()
{
    for (std::filesystem::directory_iterator::value_type entry : std::filesystem::directory_iterator("../data"))
    {
        fileCount++;
    }

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
    workoutSummary.fileName = fileName;
	std::ifstream file(DIRECTORY + fileName);
	if (!file.is_open())
	{
		std::cerr << "DataManager: Error on opening .csv file" << std::endl;
		return false;
	}

    WorkoutSummaryClear();

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
            workoutSummary.sport = columns[1];
            workoutSummary.date = columns[2];
            workoutSummary.startTime = columns[3];
            workoutSummary.duration = columns[4];

            workoutSummary.calories = columns[11].empty() ? 0 : static_cast<int>(std::stof(columns[11]));
            workoutSummary.notes = columns[23];

            workoutSummary.height = columns[24].empty() ? 0 : static_cast<int>(std::stof(columns[24]));
            workoutSummary.weight = columns[25].empty() ? 0 : std::stof(columns[25]);
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

                    double totalSeconds = (hours * 3600.0) + (minutes * 60.0) + seconds;

                    workoutSummary.hRData.push_back(hr);
                    workoutSummary.timeStamps.push_back(totalSeconds);
                }
                catch (...)
                {
                    continue;
                }
            }
        }
    }
    file.close();

    CalculateTrimp();

    return false;
}

void DataManager::CalculateTrimp()
{
/* max HR ~= 220 - age
          ~= 200 - 25 = 195
    zone
    z1: 50-60%
    z2: 60-70%
    z3: 70-80%
    z4: 80-90%
    z5: 90-100%
*/

    if (workoutSummary.hRData.empty())
    {
        std::cerr << "DataManager: no HR-Data for TRIMP-calulcation" << std::endl;
        return;
    }

    double totalTrimp = 0.0;
    int totalCount = 0;
    int totalHR = 0;

    for (int hr : workoutSummary.hRData)
    {
        if (hr > workoutSummary.maxHR)
        {
            workoutSummary.maxHR = hr;
            workoutSummary.maxHRIndex = totalCount;
        }
        if (hr < workoutSummary.minHR)
        {
            workoutSummary.minHR = hr;
            workoutSummary.minHRIndex = totalCount;

        }

        double intensity = (static_cast<double>(hr) / hRMax) * 100.0;
        double factor = 0.0;

        if (intensity >= 50.0 && intensity < 60.0)
        {
            factor = 1.0;
        }
        else if (intensity >= 60.0 && intensity < 70.0)
        {
            factor = 2.0;
        }
        else if (intensity >= 70.0 && intensity < 80.0)
        {
            factor = 3.0;
        }
        else if (intensity >= 80.0 && intensity < 90.0)
        {
            factor = 4.0;
        }
        else if (intensity >= 90.0 && intensity <= 100.0)
        {
            factor = 5.0;
        }
        else if (intensity < 50.0)
        {
            factor = 0.5;
        }

        totalTrimp += factor;
        totalHR += hr;
        totalCount++;
    }

    if (totalCount > 0)
    {
        workoutSummary.avarageHR = totalHR / totalCount;
    }

    workoutSummary.trimp = totalTrimp / 60.0;
}

void DataManager::WorkoutSummaryClear()
{
    workoutSummary.hRData.clear();
    workoutSummary.timeStamps.clear();
    workoutSummary.sport = "";
    workoutSummary.date = "";
    workoutSummary.duration = "";
    workoutSummary.fileName = "";
    workoutSummary.calories = -1;
    workoutSummary.height = -1;
    workoutSummary.weight = -1.0;
    workoutSummary.notes = "";
    workoutSummary.startTime = "";
    workoutSummary.minHR = 999;
    workoutSummary.minHRIndex = 0;
    workoutSummary.maxHR = -1;
    workoutSummary.maxHRIndex = 0;
    workoutSummary.avarageHR = -1;
    workoutSummary.trimp = -1.0;
    workoutSummary.trimpNorm = -1.0;
    workoutSummary.peaks = -1.0;
    workoutSummary.peaksNorm = -1.0;
    workoutSummary.recovery = -1.0;
    workoutSummary.recoveryNorm = -1.0;
}
