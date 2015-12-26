#ifndef Tile_hhhhh
#define Tile_hhhhh

#include "../Defines.h"

#pragma pack(push, 1)
struct SMBXTile
{
    Momentum momentum;
    short id;
    short field_32;

    // Note, 0-base indexed in SMBX code, and keep 0-base indexed here
    static inline SMBXTile* Get(unsigned short index) {
        if (index >= GM_TILE_COUNT) return NULL;
        return &((SMBXTile*)GM_TILE_PTR)[index];
    }

    static inline unsigned short Count() {
        return GM_TILE_COUNT;
    }

    static const short MAX_ID = 328;

};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(SMBXTile) == 0x34, "sizeof(SMBXTile) must be 0x34");
#endif

#endif

