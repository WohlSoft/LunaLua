#include "DeathCounter.h"
#include "PlayerMOB.h"
#include <Windows.h>
#include "MiscFuncs.h"
#include "Level.h"
using namespace std;

// CTOR
DeathCounter::DeathCounter() {
	mStatFileOK = false;
	mEnabled = true;
	mCurTotalDeaths = 0;
	mCurLevelDeaths = 0;
}

// TRY LOAD STATS - Attempts to load stats from stats file. Creates and inits the file if it doesn't exist.
bool DeathCounter::TryLoadStats() {

	// Try to open the file
	int tempint = 0;
	fstream statsfile(DEATHCT_FNAME, ios::binary|ios::in|ios::out);

	// If open failed, try to create empty file
	if(statsfile.is_open() == false) {
		statsfile.open(DEATHCT_FNAME, ios::out);
		statsfile.write((char*)&tempint, 4);
		statsfile.flush();	
		statsfile.close();
		statsfile.open(DEATHCT_FNAME, ios::binary|ios::in|ios::out);;
	}

	// If create failed, disable death counter
	if(statsfile.is_open() == false) {
		mStatFileOK = false;
		mEnabled = false;
		return false;
	}

	// If size less than 100, init new file
	statsfile.seekg(0, fstream::end);
	int cursize = (int)statsfile.tellg();
	statsfile.seekg(0, fstream::beg);

	if(cursize < 50) {
		InitStatsFile(&statsfile);
		statsfile.flush();		
		mStatFileOK = true;
		statsfile.seekg(0, fstream::beg);
	}

	if(statsfile.good() == false) {
		mStatFileOK = false;
		mEnabled = false;
		return false;
	}

	// Check version
	statsfile.read((char*)&tempint, sizeof(int));
	if(tempint < 5) {
		mStatFileOK = false;
		mEnabled = false;
		return false;
	} else {
		mStatFileOK = true;
		mEnabled = true;
	}

	ClearRecords();
	ReadRecords(&statsfile);

	statsfile.close();
	return true;
}

// UPDATE DEATHS - Determine if the main player has died and update death counter state if so
void DeathCounter::UpdateDeaths(bool write_save) {
	std::wstring debuginfo(L"UpdateDeaths");

	PlayerMOB* demo = Player::Get(1);
	if(demo == 0) return;
	
	// For now, we'll assume the player died if player 1's death timer is at exactly 50 frames
	if(demo->DeathTimer == 50) {
		std::wstring curlvl = Level::GetName();
		AddDeath(curlvl, 1);
		if(write_save) {
			TrySave();
			Recount();
		}
	}
}

// ADD DEATH
void DeathCounter::AddDeath(std::wstring lvlname, int amount) {
	std::wstring debuginfo(L"AddDeath");

	if(mEnabled == false) return;

		for (std::list<DeathRecord*>::iterator iter = mDeathRecords.begin(), end = mDeathRecords.end();	iter != end; ++iter) {						
			if((*iter)->LevelName.compare(lvlname) == 0) { // On first name match...
				(*iter)->Deaths += amount;		// Inc death count
				return;							// and exit
			}
		}

		// if no match, create new death record and add it to list
		DeathRecord* newrec = new DeathRecord;
		newrec->LevelName = lvlname;
		newrec->Deaths = amount;
		mDeathRecords.push_back(newrec);
}

// INIT STATS FILE
void DeathCounter::InitStatsFile(fstream* statsfile) {	
	WriteHeader(statsfile);
}


// WRITE HEADER - Write the death counter file header at beginning of file
void DeathCounter::WriteHeader(fstream* statsfile) {
	int writeint = LUNA_VERSION;

	// Write dll version
	statsfile->seekg(0, fstream::beg);
	statsfile->write((char*)&writeint, sizeof(writeint));

	// Init reserved
	char writebyte = 0;
	while(statsfile->tellg() < 100) {
		statsfile->write((char*)&writebyte, sizeof(writebyte));
	}

	// Write record count at 100 bytes (0 record count)
	writeint = 0;
	statsfile->write((char*)&writeint, sizeof(writeint));
}


// READ RECORDS - Add death records from file into death record list
void DeathCounter::ReadRecords(std::fstream* statsfile) {	

	// Read the record count at 100 bytes
	statsfile->seekg(100);
	int tempint = 0;
	statsfile->read((char*)&tempint, sizeof(tempint));

	if(tempint == 0)
		return;

	for(int i = 0; i < tempint; i++) {
		DeathRecord* newrec = new DeathRecord;
		newrec->Load(statsfile);
		mDeathRecords.push_back(newrec);
	}
}


// WRITE RECORDS - Writes death record count at pos 100 in the file followed by each record
void DeathCounter::WriteRecords(std::fstream* statsfile) {
	int reccount = mDeathRecords.size();
	statsfile->seekg(100);
	statsfile->write((char*)&reccount, sizeof(reccount));

	// Write each record, if any exist
	if(mDeathRecords.size() >= 1) {
		for (std::list<DeathRecord*>::iterator iter = mDeathRecords.begin(), end = mDeathRecords.end();	iter != end; ++iter) {				
			(*iter)->Save(statsfile);
		}
	}

}

// TRY SAVE - Externally callable, safe auto-save function
void DeathCounter::TrySave() {
	if(mStatFileOK && mEnabled) {
		fstream statsfile(DEATHCT_FNAME, ios::binary|ios::in|ios::out);
		if(statsfile.good())
			Save(&statsfile);
		statsfile.flush();
		statsfile.close();
	}
}

// SAVE
void DeathCounter::Save(std::fstream* statsfile) {
	if(mStatFileOK && mEnabled) {
		WriteHeader(statsfile);
		WriteRecords(statsfile);
	}
}


// CLEAR RECORDS - Clear the death record list and dealloc its records
void DeathCounter::ClearRecords() {
	while(mDeathRecords.empty() == false) {
		delete mDeathRecords.front();
		mDeathRecords.pop_front();
	}
}

// RECOUNT - Recount and relist the death count for the current level and the total deathcount
void DeathCounter::Recount() {
	if(!mEnabled) return;

	int total = 0;
	mCurLevelDeaths = 0;

	for (std::list<DeathRecord*>::iterator iter = mDeathRecords.begin(), end = mDeathRecords.end();	iter != end; ++iter) {
		total += (*iter)->Deaths;
		if((*iter)->LevelName.compare(Level::GetName()) == 0) {
			mCurLevelDeaths = (*iter)->Deaths;
		}
	}

	mCurTotalDeaths = total;

}

// DRAW - Print the death counter in its current state
void DeathCounter::Draw() {
	if(!mEnabled) return;

	// Format string to print
	std::wstring printstr(std::to_wstring((long long)mCurLevelDeaths) + L" / " + std::to_wstring((long long)mCurTotalDeaths));
	float minusoffset = (float)(123 - (printstr.size() * 8));

	// Print to screen in upper left
	Render::Print(std::wstring(L"DEMOS"), 3, 80, 27);
	Render::Print(std::wstring(printstr), 3, minusoffset, 48);
}

// PRINT DEBUG - Prints all death records to the screen
void DeathCounter::PrintDebug() {
	if(mDeathRecords.size() >= 1) {		
		float y = 300;
		for (std::list<DeathRecord*>::iterator iter = mDeathRecords.begin(), end = mDeathRecords.end();	iter != end; ++iter) {
			gLunaRender.SafePrint(std::to_wstring((long long)(*iter)->Deaths), 2, 50, y);
			gLunaRender.SafePrint((*iter)->LevelName, 2, 80, y);
			y+=30;
		}		
	}
	
}