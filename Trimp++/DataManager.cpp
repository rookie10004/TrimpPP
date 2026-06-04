#include "DataManager.h"

DataManager::DataManager()
{
	for (std::filesystem::directory_iterator::value_type entry : std::filesystem::directory_iterator("../data"))
	{
		fileCount++;
	}

	std::cout << "Application: file count: " << fileCount << std::endl;
}

bool DataManager::LoadFromCSV(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cerr << "DataManager: Error on opening .csv file" << std::endl;
		return false;
	}

	std::string line;

	if (!std::getline(file, line)) 
    {
		std::cerr << "DataManager: .csv file is empty" << std::endl;
		file.close();
		return false;
	}

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

        // mapping
        // index 2: date
        // index 3: startTime
        // index 4: duration
        // index 12: calories
        // index 23: notes
        // index 24: height
        // index 25: weight

        if (columns.size() > 25) 
        {
            workoutSummary.date = columns[2];
            workoutSummary.startTime = columns[3];
            workoutSummary.duration = columns[4];

            workoutSummary.calories = columns[12].empty() ? 0 : std::stoi(columns[12]);
            workoutSummary.notes = columns[23];

            workoutSummary.height = columns[24].empty() ? 0 : static_cast<int>(std::stof(columns[24]));
            workoutSummary.weight = columns[25].empty() ? 0 : std::stof(columns[25]);
        }

        return true;
    }

    return false;
}
