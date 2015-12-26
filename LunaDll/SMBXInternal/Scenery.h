#ifndef Scenery_hhhhh
#define Scenery_hhhhh

#include "../Defines.h"

#pragma pack(push, 1)
struct SMBXScenery
{
    Momentum momentum;
    short id;
    short field_32;

    // Note, 0-base indexed in SMBX code, and keep 0-base indexed here
    static inline SMBXScenery* Get(unsigned short index) {
        if (index >= GM_SCENERY_COUNT) return NULL;
        return &((SMBXScenery*)GM_SCENERY_PTR)[index];
    }

    static inline unsigned short Count() {
        return GM_SCENERY_COUNT;
    }

    static const short MAX_ID = 65;

};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(SMBXScenery) == 0x34, "sizeof(SMBXScenery) must be 0x34");
#endif

#endif