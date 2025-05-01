#include <windows.h>
#include <time.h>
#include <cctype>
#include <cstdarg>
#include <mutex>
#include "Globals.h"
#include "LevelCodes/LevelCodes.h"
#include "Shlwapi.h"
#include "UserSaves/UserSaving.h"
#include "GlobalFuncs.h"

HINSTANCE gHInstance;

HWND gMainWindowHwnd = NULL;
bool gMainWindowFocused = false;
bool gMainWindowUnfocusPending = false;
bool gMainWindowUnfocusOverlay = false;

// Global settings
bool gLunaEnabled;
bool gPrintErrorsToScreen;
bool gIsOverworld;
bool gHook_SkipTestMsgBox;
bool gAutostartRan = false;
SMBXHUDSettings gSMBXHUDSettings = {
    false,    // skip
    WHUD_ALL, // overworldHudControlFlag
    false     // skipStarCount
};
bool gRenderBGOFlag = true;
bool gRenderSizableFlag = true;
bool gRenderBackgroundFlag = true;

// Settings for glitch fixes
bool gDisablePlayerFilterBounceFix = false;
bool gDisableNPCRespawnBugFix = false;

// Other gameplay settings
bool gLavaIsWeak = false;

// Flag for returning from gameover screen
bool gDidGameOver = false;

// Global variables
int	gFrames;

bool gCamerasInitialised = false;

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

// Mouse cursor
HCURSOR  gCustomCursor = nullptr;
bool     gCustomCursorHide = false;

LvlEnum gLevelEnum;

// First two array entries are players 1+2
// Last two array entries are copies of the above for last tick
KeyMap gRawKeymap[4] = {0};

bool gEscPressed = false;
bool gEscPressedRegistered = false;

bool gIsTestModePauseActive;

// Global obj

AutocodeManager gAutoMan;

CSpriteManager gSpriteMan;

CellManager gCellMan;

SavedVariableBank gSavedVarBank;

MciEmulator gMciEmulator;

StartupSettings gStartupSettings;

HardcodedGraphicsManager gGraphManager;

FreeImageInit        gFreeImgInit;

GeneralLunaConfig    gGeneralConfig;

LunaGameControllerManager gLunaGameControllerManager;

CLunaLua gLunaLua;

FILE* conout = nullptr;


// Paths
std::string  gAppPathANSI;
std::string  gAppPathUTF8;
std::wstring gAppPathWCHAR;
std::string  gCwdPathANSI;
std::string  gCwdPathUTF8;
std::wstring gCwdPathWCHAR;

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

std::string gEditorPlacedItem = "nil";
std::mutex g_editorIPCMutex;
