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
};
#pragma pack(pop)

/* Verify struct is correctly sized */
static_assert(sizeof(SMBX_BGO) == 0x38, "sizeof(SMBX_BGO) must be 0x38");

#endif
