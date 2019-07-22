//*** SMBXEvents.h - Definition of known event structure ***
#ifndef SMBXEvents_hhhhhh
#define SMBXEvents_hhhhhh

#include "../Defines.h"
#include "BaseItemArray.h"

// - SMBX Event Struct. size = 0x588 (1416 bytes)
//
//+0x00		w	= No Smoke (0xFFFF == no smoke, 0 == smoke)
//+0x02		w	= Sound effect ID to play
//+0x04		dw	= ptr to event name
//+0x08		dw	= ptr to text message

//+0x0C		dw	= ptr to Hide Layer target 1
//+0x10		dw	= ptr to Hide Layer target 2
//+0x14		dw	= ptr to Hide Layer target 3
//+0x18		dw	= ptr to Hide Layer target 4
//+0x1C		dw	= ptr to Hide Layer target 5
//+0x20		dw	= ptr to Hide Layer target 6
//+0x24		dw	= ptr to Hide Layer target 7
//+0x28		dw	= ptr to Hide Layer target 8
//+0x2C		dw	= ptr to Hide Layer target 9
//+0x30		dw	= ptr to Hide Layer target 10
//+0x34		dw	= ptr to Hide Layer target 11
//+0x38		dw	= ptr to Hide Layer target 12
//+0x3C		dw	= ptr to Hide Layer target 13
//+0x40		dw	= ptr to Hide Layer target 14
//+0x44		dw	= ptr to Hide Layer target 15
//+0x48		dw	= ptr to Hide Layer target 16
//+0x4C		dw	= ptr to Hide Layer target 17
//+0x50		dw	= ptr to Hide Layer target 18
//+0x54		dw	= ptr to Hide Layer target 19
//+0x58		dw	= ptr to Hide Layer target 20
//+0x5C		dw	= ptr to Hide Layer target 21
//+0x60		dw	= ptr to Show Layer target 1
//+0x64		dw	= ptr to Show Layer target 2
//+0x68		dw	= ptr to Show Layer target 3
//+0x6C		dw	= ptr to Show Layer target 4
//+0x70		dw	= ptr to Show Layer target 5
//+0x74		dw	= ptr to Show Layer target 6
//+0x78		dw	= ptr to Show Layer target 7
//+0x7C		dw	= ptr to Show Layer target 8
//+0x80		dw	= ptr to Show Layer target 9
//+0x84		dw	= ptr to Show Layer target 10
//+0x88		dw	= ptr to Show Layer target 11
//+0x8C		dw	= ptr to Show Layer target 12
//+0x90		dw	= ptr to Show Layer target 13
//+0x94		dw	= ptr to Show Layer target 14
//+0x98		dw	= ptr to Show Layer target 15
//+0x9C		dw	= ptr to Show Layer target 16
//+0xA0		dw	= ptr to Show Layer target 17
//+0xA4		dw	= ptr to Show Layer target 18
//+0xA8		dw	= ptr to Show Layer target 19
//+0xAC		dw	= ptr to Show Layer target 20
//+0xB0		dw	= ptr to Show Layer target 21
//+0xB4		dw	= ptr to Toggle Layer target 1
//+0xB8		dw	= ptr to Toggle Layer target 2
//+0xBC		dw	= ptr to Toggle Layer target 3
//+0xC0		dw	= ptr to Toggle Layer target 4
//+0xC4		dw	= ptr to Toggle Layer target 5
//+0xC8		dw	= ptr to Toggle Layer target 6
//+0xCC		dw	= ptr to Toggle Layer target 7
//+0xD0		dw	= ptr to Toggle Layer target 8
//+0xD4		dw	= ptr to Toggle Layer target 9
//+0xD8		dw	= ptr to Toggle Layer target 10
//+0xDC		dw	= ptr to Toggle Layer target 11
//+0xE0		dw	= ptr to Toggle Layer target 12
//+0xE4		dw	= ptr to Toggle Layer target 13
//+0xE8		dw	= ptr to Toggle Layer target 14
//+0xEC		dw	= ptr to Toggle Layer target 15
//+0xF0		dw	= ptr to Toggle Layer target 16
//+0xF4		dw	= ptr to Toggle Layer target 17
//+0xF8		dw	= ptr to Toggle Layer target 18
//+0xFC		dw	= ptr to Toggle Layer target 19
//+0x100	dw	= ptr to Toggle Layer target 20
//+0x104	dw	= ptr to Toggle Layer target 21

//+0x108~	w	= Music index to change to per section (FFFF == dont change, FFFE = default)

//+0x132~	w	= Background to change to per section (FFFF = dont change, FFFE = default)

//+0x15C	dw	=
//+0x160~	f	= Position settings

//+0x54C	dw	= End Game (1 == bowser defeat, 0 = none)
//+0x550	dw	= Ptr to name of event to trigger

//+0x554	df	= Delay
//+0x55C	w	= Up (0xFFFF = checked)
//+0x55E	w	= Down
//+0x560	w	= Left
//+0x562	w	= Right
//+0x564	w	= Jump
//+0x566	w	= Alt jump
//+0x568	w	= Run
//+0x56A	w	= Alt run
//+0x56C	w	= Drop
//+0x56E	w	= Start

//+0x570	dw	= Ptr to name of Layer Movement layer
//+0x574	f	= Horizontal layer speed
//+0x578	f	= Vertical layer speed

//+0x57C	f	= Horizontal autoscroll
//+0x580	f	= Vertical autoscroll
//+0x584	w	= Autoscroll section

//+0x588	w	= Auto start

#pragma pack(push, 1)
struct SMBXEvent : SMBX_StaticBaseItemArray<SMBXEvent, 255, GM_EVENTS_PTR_CONSTPTR>
{
    short NoSmoke;                  // 0x00 (0xFFFF == no smoke, 0 == smoke)
    short SoundID;                  // 0x02
    VB6StrPtr pName;                // 0x04
    VB6StrPtr pTextMsg;             // 0x08

    VB6StrPtr pHideLayerTarg[21];   // 0x0C
    VB6StrPtr pShowLayerTarg[21];   // 0x60
    VB6StrPtr pToggleLayerTarg[21]; // 0xB4
    short SectionMusicID[21];       // 0x108 (FFFF == don't change)
    short SectionBackgroundID[21];  // 0x132 (FFFF == don't change)
    Bounds SectionBounds[21];       // 0x15C

    short EndGame;                  // 0x54C (1 == bowser defeat, 0 = none)
    short unknown_54E;              // 0x55E
    VB6StrPtr EventToTrigger;       // 0x550
    double Delay;                   // 0x554
    KeyMap ForceKeyboard;           // 0x55C
    VB6StrPtr LayerToMove;          // 0x570
    float LayerHSpeed;              // 0x574
    float LayerVSpeed;              // 0x578
    float AutoscrollHSpeed;         // 0x57C
    float AutoscrollVSpeed;         // 0x580
    short AutoscrollSecNum;         // 0x584
    short AutoStart;                // 0x586
};
#pragma pack(pop)

#ifndef __INTELLISENSE__
static_assert(sizeof(SMBXEvent) == 0x588, "sizeof(SMBXEvent) must be 0x588");
#endif

namespace SMBXEvents {

    SMBXEvent* Get(int index);
    SMBXEvent* GetByName(std::wstring Name);
    short Count();

    void TriggerEvent(int Index, int UnknownArg); // doesn't work yet
    void TriggerEvent(std::wstring eventName, short forceNoSmoke);

}

inline short SMBXEvents::Count() { return GM_EVENT_COUNT; }

#endif
