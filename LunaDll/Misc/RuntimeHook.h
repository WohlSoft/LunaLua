#ifndef RuntimeHook_hhh
#define RuntimeHook_hhh


#include "../Defines.h"
#include "../Globals.h"
#ifndef __MINGW32__
#pragma comment(lib, "comsuppw.lib")
#endif


/************************************************************************/
/* Macros                                                               */
/************************************************************************/
#define PATCH_FUNC(ptr, func) *(BYTE*)ptr = 0xE8;\
	*((DWORD*)(ptr+1)) = ((DWORD)(((DWORD)func) - ptr - 5))
#define PATCH_JMP(ptr, func) *(BYTE*)ptr = 0xE9;\
	*((DWORD*)(ptr+1)) = ((DWORD)(((DWORD)func) - ptr - 5))
#define PATCH_JMPOLD(ptr, func) *(BYTE*)source = 0xE9;\
	*((DWORD*)(source+1)) = ((DWORD)(((DWORD)dest) - source - 5))
#define PATCH_OFFSET(ptr, offset, type, value) *(type*)((DWORD)ptr + (DWORD)offset) = value
#define INSTR_NOP 0x90



#ifndef NO_SDL
extern bool episodeStarted;
#endif

/************************************************************************/
/* Runtime Patch Main Functions                                         */
/************************************************************************/
void ParseArgs(const std::vector<std::string>& args);
void TrySkipPatch();

/************************************************************************/
/* Hooks                                                                */
/************************************************************************/
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
//The Messagebox Hook Code
extern int __stdcall rtcMsgBoxHook(VARIANTARG* msgText, DWORD, DWORD, DWORD, DWORD);
//Reset Level Editor
extern void __stdcall doEventsLevelEditorHook();
//Keyboard Hook
//extern short __stdcall GetKeyState(int vKey);
//TriggerSMBXEvent cmp function
extern int __stdcall __vbaStrCmp_TriggerSMBXEventHook(BSTR cmp1, BSTR cmp2);
//Level Shutdown Check Function
extern void checkLevelShutdown();
//Error Handler
extern void handleError(int errCode);


/************************************************************************/
/* Libs                                                                 */
/************************************************************************/
extern HMODULE newLauncherLib;
extern HMODULE newDebugger;

/************************************************************************/
/* Debugg Procs                                                         */
/************************************************************************/
extern void (*runAsyncDebuggerProc)(void);
extern int (*asyncBitBltProc)(HDC, int, int, int, int, HDC, int, int, unsigned int);
extern void (*runAsyncLoggerProc)(void);
extern void (*asyncLogProc)(const char*);

struct resultStruct{
	int result;
	bool disableFrameskip;
	bool NoSound;
};

/************************************************************************/
/* Old (VB6) Procs                                                      */
/************************************************************************/
extern float (*__vbaR4Var)(VARIANTARG*);
extern int (__stdcall *rtcMsgBox)(VARIANTARG*, DWORD, DWORD, DWORD, DWORD);



//Fixup errors
void fixup_TypeMismatch13();
void fixup_ErrorReporting();



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

#endif

