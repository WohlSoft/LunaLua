//*** CollectedStarRecord.h - Definition of known collected star record object structures ***
#ifndef CollectedStarRecord_hhhh
#define CollectedStarRecord_hhhh

#include "../Defines.h"
#include "../Globals.h"
#include "../Misc/VB6StrPtr.h"

#pragma pack(push, 1)
struct SMBX_CollectedStarRecord
{
    VB6StrPtr levelFileName; //0x00
    short     section;       //0x04
    short     unknown_06;    //0x06

    // Note, 0-base indexed in SMBX code, and keep 0-base indexed here
    static inline SMBX_CollectedStarRecord* Get(unsigned short index) {
        if (index >= GM_STAR_COUNT) return NULL;
        return &((SMBX_CollectedStarRecord*)GM_STARS_PTR)[index];
    }

    static inline unsigned short Count() {
        return GM_STAR_COUNT;
    }
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(SMBX_CollectedStarRecord) == 0x08, "sizeof(SMBX_CollectedStarRecord) must be 0x08");
#endif

#endif
