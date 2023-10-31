#ifndef __loadfile_save_hhhhhhhhhhhh
#define __loadfile_save_hhhhhhhhhhhh
#include <string>

std::string GetSavesPath();
std::wstring GetSavesPathW();
void InitializeSavePath(); // makes sure the saves directory exists
bool EpisodeHasValidSavesPath(); // returns true if able to write to the episode folder

extern void LunaLua_writeSaveFile_savx();
extern bool LunaLua_loadSaveFile_savx();
extern void LunaLua_preLoadSaveFile();


#endif
