//*** Water.h - Definition of known water/quicksand object structures ***
#ifndef Water_hhhh
#define Water_hhhh

#include "../Defines.h"
#include "../Globals.h"
#include "../Misc/VB6StrPtr.h"

#pragma pack(push, 4)
struct SMBX_Water {				// Length should be 0x40
	VB6StrPtr	ptLayerName;	// +0x00
	short 		isHidden;		// +0x04
	short		unknown_end[58]; // +0x06 to end
};
#pragma pack(pop)

#endif
