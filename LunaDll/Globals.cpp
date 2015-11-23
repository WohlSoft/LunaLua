#include "Globals.h"
#include "LevelCodes/LevelCodes.h"
#include "DeathCounter/DeathCounter.h"
#include <windows.h>
#include "Shlwapi.h"
#include <time.h>
#include "Logging/Logging.h"
#include "UserSaves/UserSaving.h"
#include "GlobalFuncs.h"
#include <cctype>
#include "Rendering/RenderOverrideManager.h"

HINSTANCE gHInstance;

// Global settings
bool gLunaEnabled;
bool gShowDemoCounter;
bool gPrintErrorsToScreen;
bool gIsOverworld;
bool gHook_SkipTestMsgBox;
bool gAutostartRan = false;
SMBXHUDSettings gSMBXHUDSettings = {
    false,   // skip
    WHUD_ALL // overworldHudControlFlag
};

// Global variables
int	gFrames;

int gLastDownPress;
int gDownTapped;
int gLastUpPress;
int gUpTapped;
int gLastLeftPress;
int gLeftTapped;
int gLastRightPress;
int gRightTapped;

int gLastJumpPress;
int gJumpTapped;
int gLastRunPress;
int gRunTapped;

HDC	ghMemDC;			// General use screen-compatible DC
HBITMAP	ghGeneralDIB;	// General use screen-sized DIB
DWORD* gpScreenBits;	// Pointer to screen bits for general use DIB

LvlEnum gLevelEnum;

// Global objs
Renderer gLunaRender;

DeathCounter gDeathCounter;

AutocodeManager gAutoMan;

Logger gLogger;

CSpriteManager gSpriteMan;

CellManager gCellMan;

SavedVariableBank gSavedVarBank;

MciEmulator gMciEmulator;

startUpSettings gStartupSettings;

HardcodedGraphicsManager gGraphManager;

SHMemServer gShMemServer;

RenderOverrideManager gRenderOverride;

FreeImageInit        gFreeImgInit;

GeneralLunaConfig    gGeneralConfig;

CLunaLua gLunaLua;


