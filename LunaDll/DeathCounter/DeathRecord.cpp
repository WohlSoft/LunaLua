#include "DeathRecord.h"
#include <iomanip>

using namespace std;

void DeathRecord::Save(fstream* openfile) {

	//*(openfile) << (int)LevelName.size() * sizeof(wchar_t);
	//*(openfile) << LevelName.c_str();
	//*(openfile) << Deaths;

	// Write character count
	int tempint = (int)LevelName.size() * 2;
	openfile->write((char*)&tempint, sizeof(int));

	// Write string data
	int nullt = 0;
	openfile->write((char*)LevelName.data(), tempint);
	openfile->write((char*)&nullt, 2);

	// Write death count
	openfile->write((char*)&Deaths, sizeof(int));
}

void DeathRecord::Load(fstream* openfile) {
	char buf[150];
	ZeroMemory(buf, 150);

	// Read string length
	int length;
	openfile->read((char*)&length, sizeof(int));	

	// Read string data 
	for(int i = 0; i < length; i++) {
		buf[i] = openfile->get();
	}
	LevelName = wstring((wchar_t*)buf);
	openfile->get();
	openfile->get();

	// Read death count	
	openfile->read((char*)&Deaths, sizeof(int));	
}

// WRITE TEXT
void DeathRecord::WriteText(std::wofstream* open_file) {
	*open_file << left << setw(51) << this->LevelName << setw(3)  << L" -" << setw(7) << this->Deaths;
}

// BY FEWER DEATHS - For sorting based on fewest deaths
bool DeathRecords::by_fewer_deaths(DeathRecord* pLHS, DeathRecord* pRHS) {
	return pLHS->Deaths < pRHS->Deaths;
}