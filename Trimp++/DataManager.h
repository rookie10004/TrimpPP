#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

struct HeartRateZone
{
	// zone parameter
	int zoneWeight;
	int minHR;				// (bpm)
	int maxHR;				// (bpm)
	double duration;		// (min)
};

struct WorkoutSummary
{
	// data parameter
	std::string sport;		// in CAPITAL
	std::string date;		// format: dd/mm/yy
	std::string startTime;	// format: h:min:s
	std::string duration;	// format: h:min:s
	int calories;			// (kcal)
	int height;				// (cm)
	double weight;			// (kg)
	std::string notes;
};

class DataManager
{
private:
	// heart rate
	std::vector<int> hRData;
	std::vector<double> timeStamps;
	int maxHR;				// (bpm)
	int restHR;				// (bpm)
	int avarageHR;			// (bpm)

	WorkoutSummary workoutSummary;

	// file parameter
	int fileCount = 0;
	

public:
	DataManager();

	bool LoadFromCSV(const std::string& filePath);

	int GetFileCount() const { return fileCount; }
	const WorkoutSummary& GetSummary() const { return workoutSummary; }
	int GetMaxHR() const;
	int GetRestHR() const;
	int GetAvarageHR() const;
};

