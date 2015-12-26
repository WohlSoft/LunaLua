#ifndef Musicbox_hhhhh
#define Musicbox_hhhhh

#include "../Defines.h"

#pragma pack(push, 1)
struct SMBXMusicbox
{
    Momentum momentum;
    short id;
    short unkField2;

    // Note, 0-base indexed in SMBX code, and keep 0-base indexed here
    static inline SMBXMusicbox* Get(unsigned short index) {
        if (index >= GM_MUSICBOX_COUNT) return NULL;
        return &((SMBXMusicbox*)GM_MUSICBOX_PTR)[index];
    }

    static inline unsigned short Count() {
        return GM_MUSICBOX_COUNT;
    }

    static const short MAX_ID = 16;

};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(SMBXMusicbox) == 0x34, "sizeof(SMBXMusicbox) must be 0x34");
#endif

#endif