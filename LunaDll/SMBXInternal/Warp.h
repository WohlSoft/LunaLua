//*** Warp.h - Definition of known warp object structures ***
#ifndef Warp_hhhh
#define Warp_hhhh

#include "../Defines.h"
#include "../Globals.h"
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
    wchar_t *ptLayerName;                   //0x08
    short isHidden;                         //0x0C
    short unknown_0E;                       //0x0E
    short unknown_10;                       //0x10
    short starsRequired;                    //0x12
    double entranceX;                       //0x14
    double entranceY;                       //0x1C
    float unknown_24;                       //0x24
    float unknown_28;                       //0x28  NOTE: loadLevel sets this value to 3.0
    float unknown_2C;                       //0x2C
    float unknown_30;                       //0x30  NOTE: loadLevel sets this value to 3.0
    short unknown_34;                       //0x34
    short unknown_36;                       //0x36
    short unknown_38;                       //0x38
    short unknown_3A;                       //0x3A
    short unknown_3C;                       //0x3C
    short unknown_3E;                       //0x3E
    short unknown_40;                       //0x40
    short unknown_42;                       //0x42
    double exitX;                           //0x44
    double exitY;                           //0x4C
    float unknown_54;                       //0x54
    float unknown_58;                       //0x58  NOTE: loadLevel sets this value to 3.0
    float unknown_5C;                       //0x5C
    float unknown_60;                       //0x60  NOTE: loadLevel sets this value to 3.0
    short unknown_64;                       //0x64
    short unknown_66;                       //0x66
    short unknown_68;                       //0x68
    short unknown_6A;                       //0x6A
    short unknown_6C;                       //0x6C
    short unknown_6E;                       //0x6E
    short unknown_70;                       //0x70
    short unknown_72;                       //0x72
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
};
#pragma pack(pop)

/* Verify struct is correctly sized */
static_assert(sizeof(SMBX_Warp) == 0x90, "sizeof(SMBX_Warp) must be 0x90");

#endif
