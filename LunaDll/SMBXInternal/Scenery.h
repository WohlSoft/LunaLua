#ifndef Scenery_hhhhh
#define Scenery_hhhhh

#include "../Defines.h"
#include "BaseItemArray.h"

#pragma pack(push, 1)
struct SMBXScenery : SMBX_FullBaseItemArray<SMBXScenery, 100, GM_SCENERY_COUNT_ADDR, GM_SCENERY_PTR_ADDR>
{
    Momentum momentum;
    short id;
    short visible;
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(SMBXScenery) == 0x34, "sizeof(SMBXScenery) must be 0x34");
#endif

#endif