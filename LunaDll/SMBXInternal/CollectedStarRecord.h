//*** CollectedStarRecord.h - Definition of known collected star record object structures ***
#ifndef CollectedStarRecord_hhhh
#define CollectedStarRecord_hhhh

#include "../Defines.h"
#include "../Misc/VB6StrPtr.h"
#include "BaseItemArray.h"

#pragma pack(push, 1)
struct SMBX_CollectedStarRecord : SMBX_FullBaseItemArray<SMBX_CollectedStarRecord, -1, GM_STAR_COUNT_ADDR, GM_STARS_PTR_ADDR>
{
    VB6StrPtr levelFileName; //0x00
    short     section;       //0x04
    short     unknown_06;    //0x06 padding
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(SMBX_CollectedStarRecord) == 0x08, "sizeof(SMBX_CollectedStarRecord) must be 0x08");
#endif

#endif
