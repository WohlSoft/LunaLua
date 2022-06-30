#ifndef Musicbox_hhhhh
#define Musicbox_hhhhh

#include "../Defines.h"
#include "BaseItemArray.h"

#pragma pack(push, 1)
struct SMBXMusicbox : SMBX_FullBaseItemArray<SMBXMusicbox, 16, GM_MUSICBOX_COUNT_ADDR, GM_MUSICBOX_PTR_ADDR>
{
    Momentum momentum;
    short id;
    short unkField2;
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#if !defined(__INTELLISENSE__) && !defined(__clang__)
static_assert(sizeof(SMBXMusicbox) == 0x34, "sizeof(SMBXMusicbox) must be 0x34");
#endif

#endif