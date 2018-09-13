//*** SMBXEvents.h - Definition of known event structure ***
#ifndef SMBXEvents_hhhhhh
#define SMBXEvents_hhhhhh

#include "../Defines.h"

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

struct SMBXEvent {

    short		NoSmoke; //(0xFFFF == no smoke, 0 == smoke)
    short		SoundID;
    VB6StrPtr	pName;
    VB6StrPtr	pTextMsg;

    VB6StrPtr	pHideLayerTarg1;
    VB6StrPtr	pHideLayerTarg2;
    VB6StrPtr	pHideLayerTarg3;
    VB6StrPtr	pHideLayerTarg4;
    VB6StrPtr	pHideLayerTarg5;
    VB6StrPtr	pHideLayerTarg6;
    VB6StrPtr	pHideLayerTarg7;
    VB6StrPtr	pHideLayerTarg8;
    VB6StrPtr	pHideLayerTarg9;
    VB6StrPtr	pHideLayerTarg10;
    VB6StrPtr	pHideLayerTarg11;
    VB6StrPtr	pHideLayerTarg12;
    VB6StrPtr	pHideLayerTarg13;
    VB6StrPtr	pHideLayerTarg14;
    VB6StrPtr	pHideLayerTarg15;
    VB6StrPtr	pHideLayerTarg16;
    VB6StrPtr	pHideLayerTarg17;
    VB6StrPtr	pHideLayerTarg18;
    VB6StrPtr	pHideLayerTarg19;
    VB6StrPtr	pHideLayerTarg20;
    VB6StrPtr	pHideLayerTarg21;

    VB6StrPtr	pShowLayerTarg1;
    VB6StrPtr	pShowLayerTarg2;
    VB6StrPtr	pShowLayerTarg3;
    VB6StrPtr	pShowLayerTarg4;
    VB6StrPtr	pShowLayerTarg5;
    VB6StrPtr	pShowLayerTarg6;
    VB6StrPtr	pShowLayerTarg7;
    VB6StrPtr	pShowLayerTarg8;
    VB6StrPtr	pShowLayerTarg9;
    VB6StrPtr	pShowLayerTarg10;
    VB6StrPtr	pShowLayerTarg11;
    VB6StrPtr	pShowLayerTarg12;
    VB6StrPtr	pShowLayerTarg13;
    VB6StrPtr	pShowLayerTarg14;
    VB6StrPtr	pShowLayerTarg15;
    VB6StrPtr	pShowLayerTarg16;
    VB6StrPtr	pShowLayerTarg17;
    VB6StrPtr	pShowLayerTarg18;
    VB6StrPtr	pShowLayerTarg19;
    VB6StrPtr	pShowLayerTarg20;
    VB6StrPtr	pShowLayerTarg21;

    VB6StrPtr	pToggleLayerTarg1;
    VB6StrPtr	pToggleLayerTarg2;
    VB6StrPtr	pToggleLayerTarg3;
    VB6StrPtr	pToggleLayerTarg4;
    VB6StrPtr	pToggleLayerTarg5;
    VB6StrPtr	pToggleLayerTarg6;
    VB6StrPtr	pToggleLayerTarg7;
    VB6StrPtr	pToggleLayerTarg8;
    VB6StrPtr	pToggleLayerTarg9;
    VB6StrPtr	pToggleLayerTarg10;
    VB6StrPtr	pToggleLayerTarg11;
    VB6StrPtr	pToggleLayerTarg12;
    VB6StrPtr	pToggleLayerTarg13;
    VB6StrPtr	pToggleLayerTarg14;
    VB6StrPtr	pToggleLayerTarg15;
    VB6StrPtr	pToggleLayerTarg16;
    VB6StrPtr	pToggleLayerTarg17;
    VB6StrPtr	pToggleLayerTarg18;
    VB6StrPtr	pToggleLayerTarg19;
    VB6StrPtr	pToggleLayerTarg20;
    VB6StrPtr	pToggleLayerTarg21;

    short		Section1MusicID; //(FFFF == don't change)
    short		Section2MusicID; //(FFFF == don't change)
    short		Section3MusicID; //(FFFF == don't change)
    short		Section4MusicID; //(FFFF == don't change)
    short		Section5MusicID; //(FFFF == don't change)
    short		Section6MusicID; //(FFFF == don't change)
    short		Section7MusicID; //(FFFF == don't change)
    short		Section8MusicID; //(FFFF == don't change)
    short		Section9MusicID; //(FFFF == don't change)
    short		Section10MusicID; //(FFFF == don't change)
    short		Section11MusicID; //(FFFF == don't change)
    short		Section12MusicID; //(FFFF == don't change)
    short		Section13MusicID; //(FFFF == don't change)
    short		Section14MusicID; //(FFFF == don't change)
    short		Section15MusicID; //(FFFF == don't change)
    short		Section16MusicID; //(FFFF == don't change)
    short		Section17MusicID; //(FFFF == don't change)
    short		Section18MusicID; //(FFFF == don't change)
    short		Section19MusicID; //(FFFF == don't change)
    short		Section20MusicID; //(FFFF == don't change)
    short		Section21MusicID; //(FFFF == don't change)

    short		Section1BackgroundID; //(FFFF == don't change)
    short		Section2BackgroundID; //(FFFF == don't change)
    short		Section3BackgroundID; //(FFFF == don't change)
    short		Section4BackgroundID; //(FFFF == don't change)
    short		Section5BackgroundID; //(FFFF == don't change)
    short		Section6BackgroundID; //(FFFF == don't change)
    short		Section7BackgroundID; //(FFFF == don't change)
    short		Section8BackgroundID; //(FFFF == don't change)
    short		Section9BackgroundID; //(FFFF == don't change)
    short		Section10BackgroundID; //(FFFF == don't change)
    short		Section11BackgroundID; //(FFFF == don't change)
    short		Section12BackgroundID; //(FFFF == don't change)
    short		Section13BackgroundID; //(FFFF == don't change)
    short		Section14BackgroundID; //(FFFF == don't change)
    short		Section15BackgroundID; //(FFFF == don't change)
    short		Section16BackgroundID; //(FFFF == don't change)
    short		Section17BackgroundID; //(FFFF == don't change)
    short		Section18BackgroundID; //(FFFF == don't change)
    short		Section19BackgroundID; //(FFFF == don't change)
    short		Section20BackgroundID; //(FFFF == don't change)
    short		Section21BackgroundID; //(FFFF == don't change)

    int			Unknown15C;

    float		PositionSettings[251];

    int			EndGame; //(1 == bowser defeat, 0 = none)
    VB6StrPtr	EventToTrigger;
    float		Delay;

    short		ForceUp; //(0xFFFF = checked)
    short		ForceDown; //(0xFFFF = checked)
    short		ForceLeft; //(0xFFFF = checked)
    short		ForceRight; //(0xFFFF = checked)
    short		ForceJump; //(0xFFFF = checked)
    short		ForceAltJump; //(0xFFFF = checked)
    short		ForceRun; //(0xFFFF = checked)
    short		ForceAltRun; //(0xFFFF = checked)
    short		ForceDrop; //(0xFFFF = checked)
    short		ForceStart; //(0xFFFF = checked)

    VB6StrPtr	LayerToMoveName;
    float		LayerHSpeed;
    float		LayerVSpeed;

    float		AutoscrollHSpeed;
    float		AutoscrollVSpeed;
    short		AutoscrollSectionNum;

    short		AutoStart; //(0xFFFF = checked)
};

namespace SMBXEvents {

    SMBXEvent* Get(int index);
    SMBXEvent* GetByName(std::wstring Name);
    short Count();

    void TriggerEvent(int Index, int UnknownArg); // doesn't work yet
    void TriggerEvent(std::wstring eventName, short forceNoSmoke);

}

inline short SMBXEvents::Count() { return GM_EVENT_COUNT; }

#endif
