#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

#define DIRECTORY "../data/"
#define AGE 25

struct HeartRateZone
{
	// zone parameter
	int zoneWeight;
	int minHR;				// (bpm)
	int maxHR;				// (bpm)
	double duration;		// (min)
};

struct WorkoutList
{
	// data parameter
	std::string fileName;
	std::string sport;		// in CAPITAL
	std::string date;		// format: dd/mm/yy
};

struct WorkoutSummary
{
	// heart rate
	std::vector<int> hRData;
	std::vector<double> timeStamps;

	// data parameter
	std::string fileName;
	std::string sport;		// in CAPITAL
	std::string date;		// format: dd/mm/yy
	std::string startTime;	// format: h:min:s
	std::string duration;	// format: h:min:s
	int calories;			// (kcal)
	int height;				// (cm)
	double weight;			// (kg)
	std::string notes;

	// calculated data parameter
	int minHR = 999;		// (bpm)
	int maxHR = 0;			// (bpm)
	int avarageHR = 0;		// (bpm)
	double trimp = 0.0;
	//...
};

class DataManager
{
private:
	std::vector<WorkoutList> workoutList;
	WorkoutSummary workoutSummary;

	// file parameter
	std::string filePath;
	int fileCount = 0;
	bool isDirEmpty = false;

	int hRMax = 220 - AGE;

public:
	DataManager();

	bool LoadDirectory();
	bool LoadFromCSV(const std::string& fileName);
	void CalculateTRIMP();
	void WorkoutSummaryClear();

	const std::vector<WorkoutList> GetList() const { return workoutList; }
	const WorkoutSummary& GetSummary() const { return workoutSummary; }
	bool GetIsDirEmpty() const { return isDirEmpty; }
};