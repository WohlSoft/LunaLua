#include <windows.h>
#include <time.h>
#include <cctype>
#include <cstdarg>
#include "Globals.h"
#include "LevelCodes/LevelCodes.h"
#include "Shlwapi.h"
#include "UserSaves/UserSaving.h"
#include "GlobalFuncs.h"

HINSTANCE gHInstance;

HWND gMainWindowHwnd = NULL;
bool gMainWindowFocused = false;

// Global settings
bool gLunaEnabled;
bool gPrintErrorsToScreen;
bool gIsOverworld;
bool gHook_SkipTestMsgBox;
bool gAutostartRan = false;
bool gIsWindowsVistaOrNewer = false;
SMBXHUDSettings gSMBXHUDSettings = {
    false,    // skip
    WHUD_ALL, // overworldHudControlFlag
    false     // skipStarCount
};
bool gRenderBGOFlag = true;
bool gRenderSizableFlag = true;
std::atomic<uint32_t> gMainWindowSize = 0; // Low word is width, height word is height

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

BYTE gKeyState[256] = { 0 };

HDC	ghMemDC;			// General use screen-compatible DC
HBITMAP	ghGeneralDIB;	// General use screen-sized DIB
DWORD* gpScreenBits;	// Pointer to screen bits for general use DIB

LvlEnum gLevelEnum;

// First two array entries are players 1+2
// Last two array entries are copies of the above for last tick
KeyMap gRawKeymap[4] = {0};

bool gIsTestModePauseActive;

// Global obj

AutocodeManager gAutoMan;

CSpriteManager gSpriteMan;

CellManager gCellMan;

SavedVariableBank gSavedVarBank;

MciEmulator gMciEmulator;

startUpSettings gStartupSettings;

HardcodedGraphicsManager gGraphManager;

SHMemServer gShMemServer;

FreeImageInit        gFreeImgInit;

GeneralLunaConfig    gGeneralConfig;

CLunaLua gLunaLua;


// Paths
std::string  gAppPathANSI;

std::string  gAppPathUTF8;

std::wstring gAppPathWCHAR;

// Printf-like message boxes
void printBoxW(const wchar_t *fmt, ...)
{
    wchar_t buf[32768];
    va_list ap;
    va_start(ap, fmt);
    std::vswprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    dbgbox(buf);
}

void printBoxA(const char *fmt, ...)
{
    char buf[32768];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    dbgboxA(buf);
}
