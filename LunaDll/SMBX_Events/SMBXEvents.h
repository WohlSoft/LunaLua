//*** SMBXEvents.h - Definition of known event structure ***
#ifndef SMBXEvents_hhhhhh
#define SMBXEvents_hhhhhh

#include "../Defines.h"
#include "../Globals.h"

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
	wchar_t*	pName;
	wchar_t*	pTextMsg;

	wchar_t*	pHideLayerTarg1;
	wchar_t*	pHideLayerTarg2;
	wchar_t*	pHideLayerTarg3;
	wchar_t*	pHideLayerTarg4;
	wchar_t*	pHideLayerTarg5;
	wchar_t*	pHideLayerTarg6;
	wchar_t*	pHideLayerTarg7;
	wchar_t*	pHideLayerTarg8;
	wchar_t*	pHideLayerTarg9;
	wchar_t*	pHideLayerTarg10;
	wchar_t*	pHideLayerTarg11;
	wchar_t*	pHideLayerTarg12;
	wchar_t*	pHideLayerTarg13;
	wchar_t*	pHideLayerTarg14;
	wchar_t*	pHideLayerTarg15;
	wchar_t*	pHideLayerTarg16;
	wchar_t*	pHideLayerTarg17;
	wchar_t*	pHideLayerTarg18;
	wchar_t*	pHideLayerTarg19;
	wchar_t*	pHideLayerTarg20;
	wchar_t*	pHideLayerTarg21;

	wchar_t*	pShowLayerTarg1;
	wchar_t*	pShowLayerTarg2;
	wchar_t*	pShowLayerTarg3;
	wchar_t*	pShowLayerTarg4;
	wchar_t*	pShowLayerTarg5;
	wchar_t*	pShowLayerTarg6;
	wchar_t*	pShowLayerTarg7;
	wchar_t*	pShowLayerTarg8;
	wchar_t*	pShowLayerTarg9;
	wchar_t*	pShowLayerTarg10;
	wchar_t*	pShowLayerTarg11;
	wchar_t*	pShowLayerTarg12;
	wchar_t*	pShowLayerTarg13;
	wchar_t*	pShowLayerTarg14;
	wchar_t*	pShowLayerTarg15;
	wchar_t*	pShowLayerTarg16;
	wchar_t*	pShowLayerTarg17;
	wchar_t*	pShowLayerTarg18;
	wchar_t*	pShowLayerTarg19;
	wchar_t*	pShowLayerTarg20;
	wchar_t*	pShowLayerTarg21;

	wchar_t*	pToggleLayerTarg1;
	wchar_t*	pToggleLayerTarg2;
	wchar_t*	pToggleLayerTarg3;
	wchar_t*	pToggleLayerTarg4;
	wchar_t*	pToggleLayerTarg5;
	wchar_t*	pToggleLayerTarg6;
	wchar_t*	pToggleLayerTarg7;
	wchar_t*	pToggleLayerTarg8;
	wchar_t*	pToggleLayerTarg9;
	wchar_t*	pToggleLayerTarg10;
	wchar_t*	pToggleLayerTarg11;
	wchar_t*	pToggleLayerTarg12;
	wchar_t*	pToggleLayerTarg13;
	wchar_t*	pToggleLayerTarg14;
	wchar_t*	pToggleLayerTarg15;
	wchar_t*	pToggleLayerTarg16;
	wchar_t*	pToggleLayerTarg17;
	wchar_t*	pToggleLayerTarg18;
	wchar_t*	pToggleLayerTarg19;
	wchar_t*	pToggleLayerTarg20;
	wchar_t*	pToggleLayerTarg21;

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
	wchar_t*	EventToTrigger;
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

	wchar_t*	LayerToMoveName;
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
	void TriggerEvent(std::wstring EventName, int UnknownArg);

}

inline short SMBXEvents::Count() { return GM_EVENT_COUNT; }

#endif
