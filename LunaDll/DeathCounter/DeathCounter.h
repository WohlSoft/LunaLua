#ifndef DeathCounter_hhhh
#define DeathCounter_hhhh

#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include "../Defines.h"
#include "DeathRecord.h"

#ifndef __MINGW32__
#define DEATHCT_FNAME L"demos.dmo"
#else
#define DEATHCT_FNAME "demos.dmo"
#endif

struct DeathCounter {

	DeathCounter();

	bool TryLoadStats();
	void UpdateDeaths(bool write_save);
	void AddDeath(std::wstring, int amount);
	void TrySave();
	void Draw();
	void Recount();
	void ClearRecords();
	void DumpAllToFile(std::wstring file_name);			// Dump all death counter records to a file in main directory

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
#endif
