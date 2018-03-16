#ifndef Path_hhhhh
#define Path_hhhhh

#include "../Defines.h"

#pragma pack(push, 1)
struct SMBXPath : SMBX_FullBaseItemArray<SMBXPath, 100, GM_PATH_COUNT_CONSTPTR, GM_PATH_PTR_CONSTPTR>
{
    Momentum momentum;
    short visible;
    short id;
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(SMBXPath) == 0x34, "sizeof(SMBXPath) must be 0x34");
#endif

#endif

