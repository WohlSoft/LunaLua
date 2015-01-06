#include "Globals.h"
#include "LevelCodes.h"
#include "DeathCounter.h"
#include <windows.h>
#include "Shlwapi.h"
#include <time.h>
#include "Logging.h"
#include "UserSaving.h"
#include "GlobalFuncs.h"
#include <cctype>


// Global settings
bool gLunaEnabled;
bool gShowDemoCounter;
bool gSkipSMBXHUD;			// Whether or not the SMBX HUD will be drawn
bool gPrintErrorsToScreen;
bool gIsOverworld;

// Global variables
int	gFrames;

int* gpFreeGlob;

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

int	gCurrentMainPlayer;

HDC	ghMemDC;			// General use screen-compatible DC
HBITMAP	ghGeneralDIB;	// General use screen-sized DIB
DWORD* gpScreenBits;	// Pointer to screen bits for general use DIB

short gNumpad4;
short gNumpad8;
short gNumpad2;
short gNumpad6;

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

/// INIT GLOBALS
void InitGlobals()
{
	char* dbg = "GLOBAL INIT DBG";
	gLunaEnabled = true;
	gShowDemoCounter = false;
	gSkipSMBXHUD = false;
	gPrintErrorsToScreen = true;
	gLogger.m_Enabled = false;
	gIsOverworld = false;
	gCellMan.Reset();

	gFrames = 0;

	gLastDownPress = 0;
	gDownTapped = 0;
	gLastUpPress = 0;
	gUpTapped = 0;
	gLastLeftPress = 0;
	gLeftTapped = 0;
	gLastRightPress = 0;
	gRightTapped = 0;

	gLastJumpPress = 0;
	gJumpTapped = 0;
	gLastRunPress = 0;
	gRunTapped = 0;

	gCurrentMainPlayer = 1;

	gNumpad4 = 0;
	gNumpad8 = 0;
	gNumpad2 = 0;
	gNumpad6 = 0;

	gLevelEnum = Invalid;

	//startup settings default
	gStartupSettings.patch = false;
	gStartupSettings.noSound = false;
	gStartupSettings.frameskip = true;
	gStartupSettings.game = false;
	gStartupSettings.lvlEditor = false;
	gStartupSettings.debugger = false;
	gStartupSettings.logger = false;
	gStartupSettings.newLauncher = false;

	srand((int)time(NULL));

	// Get compatible handle for screen
	ghMemDC = CreateCompatibleDC(NULL);

	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 800;
	bmi.bmiHeader.biHeight = -600;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	gpScreenBits = 0;

	ghGeneralDIB = CreateDIBSection(ghMemDC, &bmi, DIB_RGB_COLORS, (void**)&gpScreenBits, 0, 0);

	// Allocate 40k bytes of free mem
	gpFreeGlob = NULL;
	gpFreeGlob = new int[10000];
	ResetFreeGlob();

	// Try to activate death counter, and disable it if there's some problem
	gDeathCounter.TryLoadStats();
	if(gDeathCounter.mStatFileOK == false)
		gDeathCounter.mEnabled = false;

	/// Init autocode manager	
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	int count = GetModuleFileNameW(hModule, path, MAX_PATH);
	//PathRemoveFileSpec(path);	
	for(int i = count; i > 3; i--) {
		if(path[i] == L'\\') {
			path[i] = 0;
			break;
		}
	}
	gAutoMan.Clear(true);
	gAutoMan.ReadGlobals(std::wstring(path));
	gAutoMan.m_GlobalEnabled = true;
}

/// RESET FREE GLOB
void ResetFreeGlob() {
	if(gpFreeGlob)
		ZeroMemory(gpFreeGlob, 10000 * sizeof(int));
}

/// CLEAN UP
void CleanUp() {
	if(gpFreeGlob)
		delete[] gpFreeGlob;
	if(ghMemDC)
		DeleteObject(ghMemDC);
		DeleteObject(ghGeneralDIB);
}

std::vector<std::wstring> wsplit( std::wstring str, wchar_t delimiter )
{
	vector<std::wstring> ret;
	while ( true )
	{
		size_t pos = str.find_first_of( delimiter );
		std::wstring cur = str.substr( 0, pos );
		ret.push_back( cur );
		if ( pos == std::wstring::npos )
			break;
		str = str.substr( pos + 1 );
	}
	return ret;
}

std::vector<std::string> split(std::string str, char delimiter)
{
	vector<std::string> ret;
	while ( true )
	{
		size_t pos = str.find_first_of( delimiter );
		std::string cur = str.substr( 0, pos );
		ret.push_back( cur );
		if ( pos == std::string::npos )
			break;
		str = str.substr( pos + 1 );
	}
	return ret;
}

bool vecStrFind(const std::vector<std::string>& vecStr, const std::string& find)
{
	for(int i = 0; i < (int)vecStr.size(); ++i){
		if(vecStr[i] == find)
			return true;
	}
	return false;
}


using namespace std;
#include <sstream>
void resetDefines(){
	VASM_END_ANIM = 11;
	VASM_END_COINSOUND = 14;
	VASM_END_COINVAL = 1;

	GM_GRAVITY = 12;
	GM_JUMPHIGHT = 20;
	GM_JUMPHIGHT_BOUNCE = 20;


	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	int count = GetModuleFileNameW(hModule, path, MAX_PATH);
	for(int i = count; i > 3; i--) {
		if(path[i] == L'\\') {
			path[i] = 0;
			break;
		}
	}

	wstring resetDefinies = path;
	resetDefinies = resetDefinies.append(L"\\resetdefines.txt");
	wifstream rdef(resetDefinies, ios::binary|ios::in);
	if(!rdef.is_open()){
		return;
	}

	std::wstring wrdefCode((std::istreambuf_iterator<wchar_t>(rdef)), std::istreambuf_iterator<wchar_t>());
	rdef.close();

	vector<wstring> lines = wsplit(wrdefCode, L'\n');
	for(int i = 0; i < (int)lines.size(); ++i){
		wstring rdefLine = lines[i];
		vector<wstring> reDef = wsplit(rdefLine, L'\t');
		vector<wstring> clReDef;
		for(int j = 0; j < (int)reDef.size(); ++j){
			if(reDef[j].length()){
				clReDef.push_back(reDef[j]);
			}
		}
		if(clReDef.size() < 3)
			continue;


		DWORD addr;
		wstring addrType;
		double val;

		addr = wcstoul(clReDef[0].c_str(), NULL, 16);
		addrType = clReDef[1];
		val = wcstod(clReDef[2].c_str(), NULL);

		if(addrType == L"b"){
			*(BYTE*)addr = (BYTE)val;
		}else if(addrType == L"w"){
			*(WORD*)addr = (WORD)val;
		}else if(addrType == L"dw"){
			*(DWORD*)addr = (DWORD)val;
		}else if(addrType == L"f"){
			*(float*)addr = (float)val;
		}else if(addrType == L"df"){
			*(double*)addr = val;
		}
	}

}

bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

HMODULE getModule(std::string moduleName)
{
	HMODULE ret = 0;
	if( !(ret = GetModuleHandleA(moduleName.c_str())) ){
		ret = LoadLibraryA(moduleName.c_str());
	}
	return ret;
}

