#pragma once
#include "Defines.h"
#include "Globals.h"
#pragma comment(lib, "comsuppw.lib")

#define PATCH_FUNC(ptr, func) *(BYTE*)ptr = 0xE8;\
	*((DWORD*)(ptr+1)) = ((DWORD)(((DWORD)func) - ptr - 5))

#define PATCH_JMP(ptr, func) *(BYTE*)ptr = 0xE9;\
	*((DWORD*)(ptr+1)) = ((DWORD)(((DWORD)func) - ptr - 5))

#define PATCH_JMPOLD(ptr, func) *(BYTE*)source = 0xE9;\
	*((DWORD*)(source+1)) = ((DWORD)(((DWORD)dest) - source - 5))

#define COMBOOL(b) (b ? -1 : 0)



#ifndef NO_SDL
extern bool episodeStarted;
#endif


void ParseArgs(const std::vector<std::string>& args);

void TrySkipPatch();

//Hooks

//The Init Hook will be run when "--patch" is executed.
extern void InitHook();
//Force Termination when SMBX runs the end code.
//Without this code, there is a chance of "zombie"-processes.
extern void forceTermination();
//The World Load Code
extern int LoadWorld();
//The World Loop Code
extern DWORD WorldLoop();
//The World Render Code
extern void* WorldRender();
//The Main Menu Render Code
extern int __stdcall printLunaLuaVersion(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, unsigned int dwRop);
//The MCI Emulator Code
extern MCIERROR __stdcall mciSendStringHookA(__in LPCSTR lpstrCommand, __out_ecount_opt(uReturnLength) LPSTR lpstrReturnString, __in UINT uReturnLength, __in_opt HWND hwndCallback);
//The Converter Code
extern float __stdcall vbaR4VarHook(VARIANTARG* variant);


//DBG
extern long long dbglongTest;

//Libs
extern HMODULE newLauncherLib;


extern HMODULE newDebugger;

//DBG Procs
extern void (*runAsyncDebuggerProc)(void);
extern int (*asyncBitBltProc)(HDC, int, int, int, int, HDC, int, int, unsigned int);
extern void (*runAsyncLoggerProc)(void);
extern void (*asyncLogProc)(const char*);
//Old procs
extern float (*__vbaR4Var)(VARIANTARG*);

struct resultStruct{
	int result;
	bool disableFrameskip;
	bool NoSound;
};




//scraped for now..... D:
void RuntimePatch();

//Event Hooks
void record_SMBXTrigger(wchar_t* trigger, int unkVal, int type);

int getSMBXTriggerMain(BSTR* trigger, int* unkVal);
int getSMBXTriggerTalk(BSTR* trigger, int* unkVal);
int getSMBXTriggerUnknown(BSTR* trigger, int* unkVal);
int getSMBXTriggerPSwitch(BSTR* trigger, int* unkVal);
int getSMBXTriggerActivateEventLayer(BSTR* trigger, int* unkVal);
int getSMBXTriggerDeathEvent(BSTR* trigger, int* unkVal);
int getSMBXTriggerNoMoreObjEvent(BSTR* trigger, int* unkVal);
int getSMBXTriggerEventTrigger(BSTR* trigger, int* unkVal);