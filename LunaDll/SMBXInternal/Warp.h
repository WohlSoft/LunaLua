//*** Warp.h - Definition of known warp object structures ***
#ifndef Warp_hhhh
#define Warp_hhhh

#include "../Defines.h"
#include "../Globals.h"
#include "../Misc/VB6StrPtr.h"

#pragma pack(push, 4)
struct SMBX_Warp {				// Length should be 0x90
	short 		unknown_00;		// +0x00
	short 		unknown_02;		// +0x02
	short 		unknown_04;		// +0x04
	short 		unknown_06;		// +0x06
	VB6StrPtr	ptLayerName;	// +0x08
	short 		isHidden;		// +0x0C
	short		unknown_end[130]; // +0x0E to end
};
#pragma pack(pop)

#endif
