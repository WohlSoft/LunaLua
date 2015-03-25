//*** Water.h - Definition of known water/quicksand object structures ***
#ifndef Water_hhhh
#define Water_hhhh

#include "../Defines.h"
#include "../Globals.h"
#include "../Misc/VB6StrPtr.h"

#pragma pack(push, 4)
struct SMBX_Water {				    // Length should be 0x40
    VB6StrPtr ptLayerName;          // +0x00
    short isHidden;                 // +0x04
    short unknown_06;               // +0x06
    float unknown_08;               // +0x08
    short isQuicksand;              // +0x0C
    short unknown_0E;               // +0x0E
    Momentum momentum;              // +0x10

    // Note, 1-base indexed in SMBX code, but we translate to 0-base indexed
    // for consistency of C++ code
    static inline SMBX_Water* Get(unsigned short index) {
        if (index >= GM_WATER_AREA_COUNT) return NULL;
        return &((SMBX_Water*)GM_WATER_AREAS_PTR)[index+1];
    }

    static inline unsigned short Count() {
        return GM_WATER_AREA_COUNT;
    }
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(SMBX_Water) == 0x40, "sizeof(SMBX_Water) must be 0x40");
#endif

#endif
