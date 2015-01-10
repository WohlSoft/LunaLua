#ifndef AutocodeManager_Hhh
#define AutocodeManager_Hhh
#include <list>
#include <iostream>
#include <fstream>
#include <map>
#include "Defines.h"
#include "Autocode.h"

#define AUTOCODE_FNAME L"lunadll.txt"
#define WORLDCODE_FNAME L"lunaworld.txt"
#define GLOBALCODE_FNAME L"lunaglobal.txt"
#define PARSE_FMT_STR L" %[^,], %lf , %lf , %lf , %lf , %lf , %999[^\n]"
#define PARSE_FMT_STR_2 L" %[^,], %i , %i , %i , %i , %i , %999[^\n]"
//						 Cmd	Trg   P1    P2    P3    Len   String

struct AutocodeManager {

	AutocodeManager();

	// File funcs
	void ReadFile(std::wstring dir_path); // Load level codes from dir_path
	void ReadWorld(std::wstring dir_path); // Load worldwide codes from dir_path
	void ReadGlobals(std::wstring dir_path); // Load global codes from dir_path
	void Parse(std::wifstream* open_file, bool add_to_globals);

	// Management funcs
	void Clear(bool clear_global_codes);
	void ForceExpire(int section);
	void ClearExpired();	
	void DeleteEvent(std::wstring event_reference_name);		// Look up event with given name and expire it
	void DoEvents(bool init);
	void ActivateCustomEvents(int new_section, int eventcode);
	Autocode* GetEventByRef(std::wstring event_reference_name);	// Return ptr to event with the given ref, or NULL if it fails
	Autocode* FindMatching(int section, std::wstring string);

	// Variable bank funcs
	double GetVar(std::wstring var_name);		// returns 0 if var doesn't exist in bank
	bool VarExists(std::wstring var_name);
	bool VarOperation(std::wstring var_name, double value, OPTYPE operation_to_do);

	// Members
	bool					m_Enabled;			// Whether or not individual level scripts enabled
	bool					m_GlobalEnabled;	// Whether or not global game scripts enabled
	std::list<Autocode*>	m_Autocodes;
	std::list<Autocode*>	m_InitAutocodes;
	std::list<Autocode*>	m_CustomCodes;
	std::list<Autocode*>	m_GlobalCodes;

	std::map<std::wstring, double> m_UserVars;

	// Hearts manager stuff
	int m_Hearts;
};

#endif
