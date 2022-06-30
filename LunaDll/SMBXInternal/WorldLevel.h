#ifndef WorldLevel_hhh
#define WorldLevel_hhh

#include "../Misc/VB6StrPtr.h"
#include "../Defines.h"
#include "BaseItemArray.h"

#pragma pack(push, 1)
struct WorldLevel : SMBX_FullBaseItemArray<WorldLevel, 100, GM_LEVEL_COUNT_ADDR, GM_LEVEL_BASE_ADDR>
{
    Momentum momentum;
    short id;
    short field_32;
    VB6StrPtr levelFileName;
    ExitType topExitType;
    ExitType leftExitType;
    ExitType bottomExitType;
    ExitType rightExitType;
    short visible;
    short field_42;
    VB6StrPtr levelTitle;
    short levelWarpNumber;
    short field_4A;
    double goToX;
    double goToY;
    short isPathBackground;
    short isBigBackground;
    short isGameStartPoint;
    short isAlwaysVisible;
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#if !defined(__INTELLISENSE__) && !defined(__clang__)
static_assert(sizeof(WorldLevel) == 0x64, "sizeof(WorldLevel) must be 0x64");
#endif

namespace SMBXLevel{
    WorldLevel* get(int index);
}

#endif
