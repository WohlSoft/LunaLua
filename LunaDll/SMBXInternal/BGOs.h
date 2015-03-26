//*** BGOs.h - Definition of known background object structures ***
#ifndef BGOs_hhhh
#define BGOs_hhhh

#include "../Defines.h"
#include "../Globals.h"
#include "../Misc/VB6StrPtr.h"

#pragma pack(push, 4)
struct SMBX_BGO {				// Length should be 0x38
	VB6StrPtr	ptLayerName;	// +0x00
	short		isHidden;		// +0x04
	short		id;				// +0x06
	Momentum	momentum;		// +0x08

    // Note, 0-base indexed in SMBX code, and keep 0-base indexed here
    static inline SMBX_BGO* Get(unsigned short index) {
        if (index >= GM_BGO_COUNT) return NULL;
        return &((SMBX_BGO*)GM_BGOS_PTR)[index];
    }

    static inline unsigned short Count() {
        return GM_BGO_COUNT;
    }

    static const short MAX_ID = 190;
};
#pragma pack(pop)

/* Verify struct is correctly sized */
static_assert(sizeof(SMBX_BGO) == 0x38, "sizeof(SMBX_BGO) must be 0x38");

#endif
