//*** BGOs.h - Definition of known background object structures ***
#ifndef BGOs_hhhh
#define BGOs_hhhh

#include "../Defines.h"
#include "../Misc/VB6StrPtr.h"
#include "BaseItemArray.h"

#pragma pack(push, 4)
struct SMBX_BGO : SMBX_FullBaseItemArray<SMBX_BGO, 1000, GM_BGO_COUNT_ADDR, GM_BGOS_PTR_ADDR> {				// Length should be 0x38
    VB6StrPtr	ptLayerName;	// +0x00
    short		isHidden;		// +0x04
    short		id;				// +0x06
    Momentum	momentum;		// +0x08
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(SMBX_BGO) == 0x38, "sizeof(SMBX_BGO) must be 0x38");
#endif

#endif
