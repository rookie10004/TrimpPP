#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <array>

#define DIRECTORY "../data/"
#define AGE 25

struct HRData
{
	// HR parameter
	int hR;					// bpm
	double timeStamp;		// sec
};

struct WorkoutList
{
	// general parameter
	std::string fileName;
	std::string sport;		// in CAPITAL
	std::string date;		// format: dd/mm/yy
};

struct WorkoutData
{
	// HR data
	std::vector<HRData> hRData;
	std::vector<HRData> peaksData;
	std::array<double, 6> zoneDurations = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

	// constants
	std::array<int, 6> zones;

	// general parameter
	std::string fileName;
	std::string sport;		// in CAPITAL
	std::string date;		// format: dd/mm/yy
	std::string startTime;	// format: h:min:s
	std::string duration;	// format: h:min:s
	int calories;			// (kcal)
	int height;				// (cm)
	double weight;			// (kg)
	std::string notes;

	// calculated HR parameter
	int avarageHR;			// (bpm)
	int minHR;				// (bpm)
	int maxHR;				// (bpm)
	int minHRIndex;
	int maxHRIndex;

	// calculated Score parameter
	double trimp;
	double trimpNorm;
	int peaks;
	double peaksNorm;
	int recovery;
	int recoveryNorm;
	double performance;
};

class DataManager
{
private:
	std::vector<WorkoutList> workoutList;
	WorkoutData workoutData;

	// file parameter
	std::string filePath;
	int fileCount = 0;
	bool isDirEmpty = false;

	// constants
	/* max HR ~= 220 - age
		  ~= 200 - 25 = 195
	*/
	int hRMax = 220 - AGE;
	/*	prefactor parameter
		a + b + c = 1
		a, b, c ≥ 0
	*/
	double a = 0.7;
	double b = 0.2;
	double c = 0.1;
	/*
		zone
		z1: 50-60%
		z2: 60-70%
		z3: 70-80%
		z4: 80-90%
		z5: 90-100%
	*/
	int z0 = 50;
	int z1 = 60;
	int z2 = 70;
	int z3 = 80;
	int z4 = 90;
	int z5 = 100;

	void CalculateZoneDuration();
	void CalculateTrimp();
	void CalculatePeaks();
	void CalculateRecovery();
	void CalculatePerformance();
	void WorkoutDataClear();

public:
	DataManager();

	bool LoadDirectory();
	bool LoadFromCSV(const std::string& fileName);

	const std::vector<WorkoutList> GetList() const { return workoutList; }
	const WorkoutData& GetData() const { return workoutData; }
	bool GetIsDirEmpty() const { return isDirEmpty; }
	const int GetHRMax() const { return hRMax; }
	std::string ConvertTime(double time); // in min (missing...)
};