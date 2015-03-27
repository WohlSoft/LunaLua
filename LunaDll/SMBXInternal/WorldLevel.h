#ifndef WorldLevel_hhh
#define WorldLevel_hhh

#include "../Misc/VB6StrPtr.h"
#include "../Defines.h"

#pragma pack(push, 1)
struct WorldLevel
{
    Momentum momentum;
    short id;
    short field_32;
    VB6StrPtr levelFileName;
    short topExitType;
    short leftExitType;
    short bottomExitType;
    short rightExitType;
    short field_40;
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

    // Note, 0-base indexed in SMBX code, and keep 0-base indexed here
    static inline WorldLevel* Get(unsigned short index) {
        if (index >= GM_LEVEL_COUNT) return NULL;
        return &((WorldLevel*)GM_LEVEL_BASE)[index];
    }

    static inline unsigned short Count() {
        return GM_LEVEL_COUNT;
    }

    static const short MAX_ID = 32;
};
#pragma pack(pop)

/* Verify struct is correctly sized */
static_assert(sizeof(WorldLevel) == 0x64, "sizeof(WorldLevel) must be 0x64");

namespace SMBXLevel{
	WorldLevel* get(int index);
}

#endif
