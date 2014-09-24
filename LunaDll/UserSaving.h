#pragma once

#include "Defines.h"
#include <string>
#include <map>

using namespace std;

// -- Custom user variables saving/loading manager --
// How it works: (On level load) Reads LunaSavedVars#.txt from world folder and reads all vars into object's local variable bank
// There are 3 LunaSavedVars files based on the current SMBX save slot
// Whole variable bank will be written back to world folder when calling WriteBank
class SavedVariableBank {
public:
	SavedVariableBank() { Init(); }

	bool TryLoadWorldVars();	// Try to read in the saved variables for this world & current save slot

	wstring GetSaveFileName();						// Get name of current var save file
	wstring GetSaveFileFullPath(wstring file_name);	// Form the full path to current var save file

private:
	void Init();	// Initialize, create the base save files if they don't exist, etc

	map<std::wstring, double> m_VarBank;
};

namespace Saves {
	inline int GetCurSaveSlot() { return GM_CUR_SAVE_SLOT; }
}