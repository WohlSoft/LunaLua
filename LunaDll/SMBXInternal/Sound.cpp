#include "../Defines.h"
#include "Sound.h"

// PLAY SFX
void SMBXSound::PlaySFX(short soundindex) {
    native_playSFX(&soundindex);
}

// SET MUSIC
void SMBXSound::SetMusic(int musicnum, int section) {
    if(musicnum >= 0 && musicnum <= 0x18 && section <= 20 && section >= 0) {
        GM_SEC_MUSIC_TBL[section] = musicnum;
    }
}

// SET MUSIC PATH
void SMBXSound::SetMusicPath(int section, std::wstring newpath) {
    char* dbg = "MUSIC PATH DBG";
    if(section <= 20 && section >= 0) {
        GM_MUSIC_PATHS_PTR[section] = newpath;
    }
}

// PLAY MUSIC
void SMBXSound::PlayMusic(short section) {
    native_playMusic(&section);
}