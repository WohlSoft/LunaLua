#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include "Defines.h"

struct DeathRecord {

	void Save(std::fstream* open_file);
	void Load(std::fstream* open_file);
	void WriteText(std::wofstream* open_file);		// Write death record as human readable

	std::wstring LevelName;
	int Deaths;
};

namespace DeathRecords {
	bool by_fewer_deaths(DeathRecord* lhs, DeathRecord* rhs);	// for automatic sorting
};