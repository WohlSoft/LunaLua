//*** Globals.h - It has some globals in it ***

#pragma once
#pragma warning(disable: 4731)
#pragma warning(disable: 4800)
#pragma warning(disable: 4996) //localtime
#include "LevelCodes.h"
#include "Rendering.h"
#include "DeathCounter.h"
#include "AutocodeManager.h"
#include "Logging.h"
#include "CSprite.h"
#include "CSpriteManager.h"
#include "CellManager.h"

#pragma comment (lib, "Msimg32.lib")

/// Global settings
extern bool		gLunaEnabled;
extern bool		gShowDemoCounter;
extern bool		gSkipSMBXHUD;
extern bool		gPrintErrorsToScreen;

/// General use globals - These are all defined in Globals.cpp ///
extern int		gFrames;

extern int*		gpFreeGlob;

extern int		gLastDownPress;
extern int		gDownTapped;
extern int		gLastUpPress;
extern int		gUpTapped;
extern int		gLastLeftPress;
extern int		gLeftTapped;
extern int		gLastRightPress;
extern int		gRightTapped;

extern int		gLastJumpPress;
extern int		gJumpTapped;
extern int		gLastRunPress;
extern int		gRunTapped;

extern int		gCurrentMainPlayer;

extern HDC		ghMemDC;		// General-use screen-compatible DC
extern HBITMAP	ghGeneralDIB;	// General-use screen-sized DIB
extern DWORD*	gpScreenBits;	// Pointer to screen bits for general use DIB

// Key status
extern short	gNumpad4;
extern short	gNumpad8;
extern short	gNumpad2;
extern short	gNumpad6;

// Level init
extern LvlEnum	gLevelEnum;

// Misc
extern Renderer			gLunaRender;
extern DeathCounter		gDeathCounter;
extern AutocodeManager	gAutoMan;
extern CSpriteManager	gSpriteMan;
extern Logger			gLogger;
extern CellManager		gCellMan;

/// Functions ///
void InitGlobals();
void ResetFreeGlob();
void CleanUp();