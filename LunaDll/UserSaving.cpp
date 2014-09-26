#include "UserSaving.h"
#include "MiscFuncs.h"
#include <iostream>
#include <string>
#include <stdlib.h>

#define SPECIAL_SAVE_STR L"__LunaVarBankSpecialCounter"

// INIT
void SavedVariableBank::Init() {
}

// TRY LOAD WORLD VARS
bool SavedVariableBank::TryLoadWorldVars() {
	if(Saves::GetCurSaveSlot() > 3)
		return false;
	wstring full_path = GetSaveFileFullPath(GetSaveFileName());

	ClearBank();

	// Try to open the file
	wfstream var_file(full_path, ios::in|ios::out);

	// If open failed, try to create empty file
	if(var_file.is_open() == false) {
		var_file.open(full_path, ios::out);
		var_file.flush();	
		var_file.close();
		var_file.open(full_path, ios::in|ios::out);;
	}

	// If create failed, get out
	if(var_file.is_open() == false)
		return false;

	// If size < 2 bytes, init new save file
	var_file.seekg(0, fstream::end);
	int cursize = (int)var_file.tellg();
	var_file.seekg(0, fstream::beg);

	if(cursize < 2) {
		InitSaveFile(&var_file);
		var_file.flush();		
		var_file.seekg(0, fstream::beg);
	}

	ReadFile(&var_file);
	
	var_file.flush();
	var_file.close();
	return true; 
}

// GET SAVE FILE NAME
wstring SavedVariableBank::GetSaveFileName() {
	return L"LunaSavedVars" + to_wstring((long long)Saves::GetCurSaveSlot()) + L".txt";
}

// GET SAVE FILE FULL PATH
wstring SavedVariableBank::GetSaveFileFullPath(wstring save_file_name) {
	wstring full_path = wstring((wchar_t*)GM_FULLDIR);	

	full_path = full_path.append(L"\\");
	full_path = full_path.append(save_file_name);
	return full_path;
}

// INIT SAVE FILE
void SavedVariableBank::InitSaveFile(wfstream* pFilestream) {
	if(pFilestream != NULL && pFilestream->is_open()) {
		*pFilestream << L"__Lunadll_Version" << endl;
		*pFilestream << LUNA_VERSION << endl;
	}
}

// READ FILE
// The format of the save file is simply keys and values separated by newlines
void SavedVariableBank::ReadFile(wfstream* pFilestream) {
	if(pFilestream != NULL && pFilestream->is_open()) {
		wstring line;		
		double val = 0;
		while(getline(*pFilestream, line)) {
			wstring key = line;
			wstring val_str;
			getline(*pFilestream, val_str);
			val = _wtof(val_str.c_str());
			SetVar(key, val);
		}
	}
}

// SET VAR
void SavedVariableBank::SetVar(wstring k, double v) {
	m_VarBank[k] = v;
}

// GET VAR
double SavedVariableBank::GetVar(wstring key) {
	if(!VarExists(key))
		return 0;
	return m_VarBank[key];
}

// VAR EXISTS
bool SavedVariableBank::VarExists(wstring k) {
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
	wstring full_path = GetSaveFileFullPath(GetSaveFileName());

	wfstream var_file(full_path, ios::out|ios::trunc);

	for(map<wstring, double>::iterator it = m_VarBank.begin(); it != m_VarBank.end(); ++it) {
		var_file << it->first << endl << it->second << endl;
	}

	var_file.flush();
	var_file.close();	
}

// CHECK SAVE DELETION
void SavedVariableBank::CheckSaveDeletion() {
	if(Saves::GetCurSaveSlot() > 3)
		return;
	wstring star_counter = SPECIAL_SAVE_STR;
	if(!VarExists(star_counter))
		SetVar(star_counter, GM_STAR_COUNT);		

	// Reset save slot detected? Reset the user save file too
	if(GM_STAR_COUNT < GetVar(SPECIAL_SAVE_STR)) {

		if(true) //DEBUG
			gLogger.Log(L"Deleting user save file - Star count: " + to_wstring((long long)GM_STAR_COUNT) + L" slot: " + to_wstring((long long)Saves::GetCurSaveSlot()), LOG_STD);

		wstring full_path = GetSaveFileFullPath(GetSaveFileName());
		ClearBank();
		wfstream var_file(full_path, ios::out);
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
void SavedVariableBank::CopyBank(map<wstring, double>* target_map) {
	if(target_map != NULL) {
		for(map<wstring, double>::iterator it = m_VarBank.begin(); it != m_VarBank.end(); ++it) {
			(*target_map)[it->first] = it->second;
		}
	}
}