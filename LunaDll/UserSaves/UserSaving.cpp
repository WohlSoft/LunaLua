#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "UserSaving.h"
#include "../Globals.h"
#include "../Misc/MiscFuncs.h"
#include "../GlobalFuncs.h"

#define SPECIAL_SAVE_STR L"__LunaVarBankSpecialCounter"

// INIT
void SavedVariableBank::Init() {
}

// TRY LOAD WORLD VARS
bool SavedVariableBank::TryLoadWorldVars() {
	if(Saves::GetCurSaveSlot() > 3)
		return false;
    std::wstring full_path = GetSaveFileFullPath(GetSaveFileName());

	ClearBank();

	// Try to open the file
    std::wfstream var_file(WStr2Str(full_path).c_str(), std::ios::in | std::ios::out);

	// If open failed, try to create empty file
	if(var_file.is_open() == false) {
        var_file.open(WStr2Str(full_path).c_str(), std::ios::out);
		var_file.flush();	
		var_file.close();
        var_file.open(WStr2Str(full_path).c_str(), std::ios::in | std::ios::out);;
	}

	// If create failed, get out
	if(var_file.is_open() == false)
		return false;

	// If size < 2 bytes, init new save file
	var_file.seekg(0, std::fstream::end);
	int cursize = (int)var_file.tellg();
	var_file.seekg(0, std::fstream::beg);

	if(cursize < 2) {
		InitSaveFile(&var_file);
		var_file.flush();		
		var_file.seekg(0, std::fstream::beg);
	}

	ReadFile(&var_file);
	
	var_file.flush();
	var_file.close();
	return true; 
}

// GET SAVE FILE NAME
std::wstring SavedVariableBank::GetSaveFileName() {
	return L"LunaSavedVars" + std::to_wstring((long long)Saves::GetCurSaveSlot()) + L".txt";
}

// GET SAVE FILE FULL PATH
std::wstring SavedVariableBank::GetSaveFileFullPath(std::wstring save_file_name) {
    std::wstring full_path = (std::wstring)GM_FULLDIR;

	full_path = full_path.append(L"\\");
	full_path = full_path.append(save_file_name);
	return full_path;
}

// INIT SAVE FILE
void SavedVariableBank::InitSaveFile(std::wfstream* pFilestream) {
	if(pFilestream != NULL && pFilestream->is_open()) {
		*pFilestream << L"__Lunadll_Version" << std::endl;
		*pFilestream << LUNA_VERSION << std::endl;
	}
}

// READ FILE
// The format of the save file is simply keys and values separated by newlines
void SavedVariableBank::ReadFile(std::wfstream* pFilestream) {
	if(pFilestream != NULL && pFilestream->is_open()) {
        std::wstring line;
		double val = 0;
		while(getline(*pFilestream, line)) {
            std::wstring key = line;
            std::wstring val_str;
			getline(*pFilestream, val_str);
			val = _wtof(val_str.c_str());
			SetVar(key, val);
		}
	}
}

// SET VAR
void SavedVariableBank::SetVar(std::wstring k, double v) {
	m_VarBank[k] = v;
}

// GET VAR
double SavedVariableBank::GetVar(std::wstring key) {
	if(!VarExists(key))
		return 0;
	return m_VarBank[key];
}

// VAR EXISTS
bool SavedVariableBank::VarExists(std::wstring k) {
	if(m_VarBank.find(k) == m_VarBank.end())
		return false;
	return true;
}

// CLEAR BANK
void SavedVariableBank::ClearBank() {
	m_VarBank.clear();
}

// WRITE BANK
void SavedVariableBank::WriteBank() {
	if(Saves::GetCurSaveSlot() > 3)
		return;
    std::wstring full_path = GetSaveFileFullPath(GetSaveFileName());

    std::wfstream var_file(WStr2Str(full_path).c_str(), std::ios::out | std::ios::trunc);

	for(std::map<std::wstring, double>::iterator it = m_VarBank.begin(); it != m_VarBank.end(); ++it) {
		var_file << it->first << std::endl << it->second << std::endl;
	}

	var_file.flush();
	var_file.close();	
}

// CHECK SAVE DELETION
void SavedVariableBank::CheckSaveDeletion() {
	if(Saves::GetCurSaveSlot() > 3)
		return;
    std::wstring star_counter = SPECIAL_SAVE_STR;
	if(!VarExists(star_counter))
		SetVar(star_counter, GM_STAR_COUNT);		

	// Reset save slot detected? Reset the user save file too
	if(GM_STAR_COUNT < GetVar(SPECIAL_SAVE_STR)) {

		if(true) //DEBUG
			gLogger.Log(L"Deleting user save file - Star count: " + std::to_wstring((long long)GM_STAR_COUNT) + L" slot: " + std::to_wstring((long long)Saves::GetCurSaveSlot()), LOG_STD);

        std::wstring full_path = GetSaveFileFullPath(GetSaveFileName());
		ClearBank();
        std::wfstream var_file(WStr2Str(full_path).c_str(), std::ios::out);
		InitSaveFile(&var_file);
		var_file.flush();
		var_file.close();
	}
}

// SaveIfNeeded
void SavedVariableBank::SaveIfNeeded() {
	// Basically, force save if the player collected a star
	if(GM_STAR_COUNT > GetVar(SPECIAL_SAVE_STR)) {
		SetVar(SPECIAL_SAVE_STR, GM_STAR_COUNT);
		WriteBank();
	}
}

//COPY BANK
void SavedVariableBank::CopyBank(std::map<std::wstring, double>* target_map) {
	if(target_map != NULL) {
		for(std::map<std::wstring, double>::iterator it = m_VarBank.begin(); it != m_VarBank.end(); ++it) {
			(*target_map)[it->first] = it->second;
		}
	}
}
