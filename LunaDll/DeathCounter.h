#pragma once
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include "Defines.h"
#include "DeathRecord.h"

#define DEATHCT_FNAME L"demos.dmo"

struct DeathCounter {

	DeathCounter();

	bool TryLoadStats();
	void UpdateDeaths(bool write_save);
	void AddDeath(std::wstring, int amount);
	void TrySave();
	void Draw();
	void Recount();
	void ClearRecords();

	void PrintDebug();

private:
	void InitStatsFile(std::fstream* openfile);
	void WriteHeader(std::fstream* openfile);
	void WriteRecords(std::fstream* statsfile);
	void ReadRecords(std::fstream* openfile);	
	void Save(std::fstream* openfile);	

	// Members
public:
	bool mStatFileOK;
	bool mEnabled;

	int mCurTotalDeaths;
	int mCurLevelDeaths;

	std::list<DeathRecord*> mDeathRecords;
};