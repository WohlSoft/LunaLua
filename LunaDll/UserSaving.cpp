#include "UserSaving.h"
#include "MiscFuncs.h"


// INIT
void SavedVariableBank::Init() {

}

// TRY LOAD WORLD VARS
bool SavedVariableBank::TryLoadWorldVars() {
	wstring full_path = GetSaveFileFullPath(GetSaveFileName());

	this->m_VarBank.clear();

	wifstream var_file(full_path, ios::in);	
	if(var_file.is_open() == false) {
		var_file.close();
		return;
	}
	
	var_file.close();
	return false; //
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

