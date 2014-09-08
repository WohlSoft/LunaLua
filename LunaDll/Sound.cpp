#include "Defines.h"
#include "Sound.h"

// PLAY SFX
void SMBXSound::PlaySFX(int soundindex) {
	typedef void soundfunc(int*);
	soundfunc* f = (soundfunc*)GF_PLAY_SFX;	
	f(&soundindex);
}

// SET MUSIC
void SMBXSound::SetMusic(int musicnum, int section) {
	if(musicnum >= 0 && musicnum <= 0x18 && section <= 20 && section >= 0) {
		short* musictbl = (short*)GM_SEC_MUSIC_TBL_P;
		musictbl[section] = musicnum;
	}
}

// SET MUSIC PATH
void SMBXSound::SetMusicPath(int section, std::wstring newpath) {
	char* dbg = "MUSIC PATH DBG";
	if(section <= 20 && section >= 0) {

		wchar_t** ppPaths = (wchar_t**)GM_MUSIC_PATHS_PTR;
		wchar_t* pPath = ppPaths[section];
		std::wstring curpath = std::wstring(pPath);

		if(curpath.length() > newpath.length()) {
			memmove(pPath, newpath.c_str(), (newpath.length() + 2) * 2);
		}
	}
}

// PLAY MUSIC
void SMBXSound::PlayMusic(int section) {
	typedef void musicfunc(int*);
	musicfunc* f = (musicfunc*)GF_PLAY_MUSIC;	
	f(&section);
}