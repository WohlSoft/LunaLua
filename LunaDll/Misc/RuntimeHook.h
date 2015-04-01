#ifndef RuntimeHook_hhh
#define RuntimeHook_hhh


#include "../Defines.h"
#include "../Globals.h"


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
/* Runtime Patch Public Functions                                       */
/************************************************************************/
void emulateVB6Error(int errorCode);
void showSMBXMessageBox(std::string message);
extern inline void patchWholeNativeFunction(void* native_func, int sizeOfNativeFunc, void* newFunc);

/************************************************************************/
/* Hooks                                                                */
/************************************************************************/
//The Init Hook will be run when "--patch" is executed.
extern void __stdcall InitHook();
//Force Termination when SMBX runs the end code.
//Without this code, there is a chance of "zombie"-processes.
extern void __stdcall forceTermination();
extern int __stdcall LoadWorld();     //The World Load Code
extern DWORD __stdcall WorldLoop();       //The World Loop Code
extern void* __stdcall WorldRender();     //The World Render Code
extern int __stdcall printLunaLuaVersion(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, unsigned int dwRop);       //The Main Menu Render Code
extern MCIERROR __stdcall mciSendStringHookA(__in LPCSTR lpstrCommand, __out_ecount_opt(uReturnLength) LPSTR lpstrReturnString, __in UINT uReturnLength, __in_opt HWND hwndCallback);       //The MCI Emulator Code
extern float __stdcall vbaR4VarHook(VARIANTARG* variant);       //The Converter Code
extern int __stdcall rtcMsgBoxHook(VARIANTARG* msgText, DWORD, DWORD, DWORD, DWORD);        //The Messagebox Hook Code
extern void __stdcall doEventsLevelEditorHook();        //Reset Level Editor
//Keyboard Hook
//extern short __stdcall GetKeyState(int vKey);
//TriggerSMBXEvent cmp function
extern int __stdcall __vbaStrCmp_TriggerSMBXEventHook(BSTR cmp1, BSTR cmp2);
extern void __stdcall checkLevelShutdown();       //Level Shutdown Check Function
extern void __stdcall handleError(int errCode);       //Error Handler
extern void __stdcall LoadLocalGfxHook();
extern BOOL __stdcall BitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);

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
void fixup_WarpLimit();
void fixup_Credits();
void fixup_NativeFuncs();


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

