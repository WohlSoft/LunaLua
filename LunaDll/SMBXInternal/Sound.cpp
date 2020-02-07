#include "../Defines.h"
#include "Sound.h"
#include "../SMBXInternal/PlayerMOB.h"

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
void SMBXSound::PlayMusic(short section, bool forceRestart) {
    // If forcing a restart, just do that
    if (forceRestart)
    {
        native_playMusic(&section);
        return;
    }

    // Otherwise, check if we need to restart music. Try to act like section switch.
    short oldMusic = GM_SEC_CURRENT_MUSIC_ID;
    short newMusic = GM_SEC_MUSIC_TBL[section];
    bool isMusicDifferent = (oldMusic != newMusic);

    // If it's both custom music, check if the path has changed
    if (!isMusicDifferent && (oldMusic == 24) && (newMusic == 24))
    {
        isMusicDifferent = (std::wstring)GM_MUSIC_PATHS_PTR[section] != GetActiveCustomMusicPath();
    }
    
    // If the music is different, change music
    if (isMusicDifferent)
    {
        native_playMusic(&section);
    }
}

static std::wstring g_LastActiveCustomMusicPath = L"";

void SMBXSound::StoreActiveCustomMusicPath(unsigned int section)
{
    g_LastActiveCustomMusicPath = GM_MUSIC_PATHS_PTR[section];
}

std::wstring SMBXSound::GetActiveCustomMusicPath()
{
    return g_LastActiveCustomMusicPath;
}
