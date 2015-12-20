#ifndef UserSaving_Hhh
#define UserSaving_Hhh

#include "../Defines.h"
#include <string>
#include <map>

// -- Custom user variables saving/loading manager --
// How it works: (On level load) Reads LunaSavedVars#.txt from world folder and reads all vars into object's local variable bank
// There are 3 LunaSavedVars files based on the current SMBX save slot
// Whole variable bank will be written back to world folder when calling WriteBank
// Object is basically a wrapper around a map with some file management functions
class SavedVariableBank {
public:
	SavedVariableBank() { Init(); }

	bool TryLoadWorldVars();		// Try to read in the saved variables for this world & current save slot

    std::wstring GetSaveFileName();						// Get name of current var save file
    std::wstring GetSaveFileFullPath(std::wstring file_name);	// Form the full path to current var save file

	void SetVar(std::wstring key, double val);			// Sets or adds a key/value pair to the bank
	bool VarExists(std::wstring key);					// Returns whether or not this var exists in the bank
	double GetVar(std::wstring key);						// Get value of a key, or 0 if key not found
	void CopyBank(std::map<std::wstring, double>* target);	// Copy all k,v pairs of variable bank to another map
	void ClearBank();

	void WriteBank();								// Save vars by writing the current bank back to world folder
	
	void SaveIfNeeded();							// Saves if it detects the player has collected a new star
	void CheckSaveDeletion();						// Tries to heuristically detect if the current user save should be deleted and deletes it

	/// Members ///
    std::map<std::wstring, double> m_VarBank;

private:
	void Init();					// Init the object
	void InitSaveFile(std::wfstream*);	// Init a new save file
	void ReadFile(std::wfstream*);		// Read all vars from open file into bank
};

namespace Saves {
	inline int GetCurSaveSlot() { return GM_CUR_SAVE_SLOT; }
}

#endif
