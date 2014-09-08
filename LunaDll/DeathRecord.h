#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include "Defines.h"

struct DeathRecord {

	void Save(std::fstream* openfile);
	void Load(std::fstream* openfile);

	std::wstring LevelName;
	int Deaths;
};