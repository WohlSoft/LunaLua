#ifndef Scenery_hhhhh
#define Scenery_hhhhh

#include "../Defines.h"
#include "BaseItemArray.h"

#pragma pack(push, 1)
struct SMBXScenery : SMBX_FullBaseItemArray<SMBXScenery, 100, GM_SCENERY_COUNT_ADDR, GM_SCENERY_PTR_ADDR>
{
    Momentum momentum;
    short id;
    short field_32;
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#if !defined(__INTELLISENSE__) && !defined(__clang__)
static_assert(sizeof(SMBXScenery) == 0x34, "sizeof(SMBXScenery) must be 0x34");
#endif

#endif