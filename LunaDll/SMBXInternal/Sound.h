#ifndef sounds_hhhh
#define sounds_hhhh

#include <string>

namespace SMBXSound {
    void PlaySFX(short soundindex);

    void SetMusic(int musicnum, int section);
    void SetMusicPath(int section, std::wstring path);
    void PlayMusic(short section, bool forceRestart);
}
#endif
