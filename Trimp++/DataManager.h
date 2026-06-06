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
	int minHR;				// (bpm)
	int minHRIndex;
	int maxHR;				// (bpm)
	int maxHRIndex;
	int avarageHR;			// (bpm)
	double trimp;
	double trimpNorm;
	double peaks;
	double peaksNorm;
	double recovery;
	double recoveryNorm;
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

	void CalculateTrimp();
	void CalculatePeaks();
	void CalculateRecovery();
	void CalculatePerformance();
	void WorkoutSummaryClear();

public:
	DataManager();

	bool LoadDirectory();
	bool LoadFromCSV(const std::string& fileName);

	const std::vector<WorkoutList> GetList() const { return workoutList; }
	const WorkoutSummary& GetSummary() const { return workoutSummary; }
	bool GetIsDirEmpty() const { return isDirEmpty; }
	const int GetHRMax() const { return hRMax; }
};