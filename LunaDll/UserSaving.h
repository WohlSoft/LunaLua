#pragma once

#include "Defines.h"
#include <string>
#include <map>

#define USER_SAVE_VARS_FNAME L"LunaSaveVars.txt"

using namespace std;

// Custom user variables saving/loading
class SavedVariableBank {
public:
	bool TryLoadWorldVars(wstring filename); // Try to read in the saved variables for this world

	inline int GetCurSaveSlot() { return GM_CUR_SAVE_SLOT; }

private:
	map<std::wstring, double> m_VarBank;
}