//*** Warp.h - Definition of known warp object structures ***
#ifndef Warp_hhhh
#define Warp_hhhh

#include "../Defines.h"
#include "../Misc/VB6StrPtr.h"

enum SMBX_EntranceDir : short {
    ENTRANCE_UP = 1,
    ENTRANCE_LEFT = 2,
    ENTRANCE_DOWN = 3,
    ENTRANCE_RIGHT = 4
};

enum SMBX_ExitDir : short {
    EXIT_UP = 3,
    EXIT_LEFT = 4,
    EXIT_DOWN = 1,
    EXIT_RIGHT = 2
};

enum SMBX_WarpType : short {
    WARPTYPE_INSTANT = 0,
    WARPTYPE_PIPE = 1,
    WARPTYPE_DOOR = 2
};

#pragma pack(push, 1)
struct SMBX_Warp
{
    short isLocked;                         //0x00
    short allowCarryNPC;                    //0x02
    short noYoshi;                          //0x04
    short unknown_06;                       //0x06
    VB6StrPtr ptLayerName;                  //0x08
    short isHidden;                         //0x0C
    short unknown_0E;                       //0x0E
    short unknown_10;                       //0x10
    short starsRequired;                    //0x12
    Momentum entrance;                      //0x14
    Momentum exit;                          //0x44
    SMBX_WarpType warpType;                 //0x74
    short unknown_76;                       //0x76
    VB6StrPtr warpToLevelFileName;          //0x78
    short toWarpIndex;                      //0x7C  The Warp number
    short isLevelEntrance;                  //0x7E
    SMBX_EntranceDir entranceDirection;     //0x80
    SMBX_ExitDir exitDirection;             //0x82
    short isLevelExit;                      //0x84
    short warpToWorldmapX;                  //0x86
    short warpToWorldmapY;                  //0x88
    short unknown_8A;                       //0x8A
    short unknown_8C;                       //0x8C
    short unknown_8E;                       //0x8E

    // Note, 0-base indexed in SMBX code, and keep 0-base indexed here
    static inline SMBX_Warp* Get(unsigned short index) {
        if (index >= GM_WARP_COUNT) return NULL;
        return &((SMBX_Warp*)GM_WARPS_PTR)[index];
    }

    static inline unsigned short Count() {
        return GM_WARP_COUNT;
    }
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(SMBX_Warp) == 0x90, "sizeof(SMBX_Warp) must be 0x90");
#endif

#endif
