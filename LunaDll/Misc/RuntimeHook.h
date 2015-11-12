#ifndef RuntimeHook_hhh
#define RuntimeHook_hhh


#include "../Defines.h"
#include "../Globals.h"

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/
typedef EXCEPTION_DISPOSITION __cdecl SEH_HANDLER(
    _In_ EXCEPTION_RECORD *_ExceptionRecord,
    _In_ void * _EstablisherFrame,
    _Inout_ CONTEXT *_ContextRecord,
    _Inout_ void * _DispatcherContext
    );

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
static inline void PATCH_FUNC_CALL_SAFE(DWORD ptr, void* func) { // Takes 13 bytes, but tries to be very safe...
    BYTE* bPtr = (BYTE*)ptr;

    bPtr[0] = 0x9C; // pushf
    bPtr[1] = 0x50; // push eax
    bPtr[2] = 0x51; // push ecx
    bPtr[3] = 0x52; // push edx
    PATCH_FUNC((DWORD)&bPtr[4], func);
    bPtr[9] = 0x5A; // pop edx
    bPtr[10] = 0x59; // pop ecx
    bPtr[11] = 0x58; // pop eax
    bPtr[12] = 0x9D; // popf
}


#ifndef NO_SDL
extern bool episodeStarted;
#endif

/************************************************************************/
/* Runtime Patch Main Functions                                         */
/************************************************************************/
void SetupThunRTMainHook();
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
// Early initialization hook
void __stdcall ThunRTMainHook(void* arg1);
//The Init Hook will be run when "--patch" is executed.
extern void __stdcall InitHook();
//Force Termination when SMBX runs the end code.
//Without this code, there is a chance of "zombie"-processes.
extern void __stdcall forceTermination();
extern int __stdcall LoadWorld();     //The World Load Code
extern DWORD __stdcall WorldLoop();       //The World Loop Code
extern void* __stdcall WorldRender();     //The World Render Code
extern int __stdcall LoadIntro();       // Load Intro Code (Autostart)
extern int __stdcall printLunaLuaVersion(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, unsigned int dwRop);       //The Main Menu Render Code
extern MCIERROR __stdcall mciSendStringHookA(__in LPCSTR lpstrCommand, __out_ecount_opt(uReturnLength) LPSTR lpstrReturnString, __in UINT uReturnLength, __in_opt HWND hwndCallback);       //The MCI Emulator Code
extern float __stdcall vbaR4VarHook(VARIANTARG* variant);       //The Converter Code
extern int __stdcall rtcMsgBoxHook(VARIANTARG* msgText, DWORD, DWORD, DWORD, DWORD);        //The Messagebox Hook Code
extern void __stdcall doEventsLevelEditorHook();        //Reset Level Editor
extern void __stdcall NPCKillHook(short* npcIndex_ptr, short* unknownVal);
//Keyboard Hook
//extern short __stdcall GetKeyState(int vKey);
//TriggerSMBXEvent cmp function
extern int __stdcall __vbaStrCmp_TriggerSMBXEventHook(BSTR cmp1, BSTR cmp2);
extern void __stdcall checkLevelShutdown();       //Level Shutdown Check Function
extern void __stdcall recordVBErrCode(int errCode);       //Error Handler
extern SEH_HANDLER* LunaDLLOriginalExceptionHandler;
extern EXCEPTION_DISPOSITION __cdecl LunaDLLCustomExceptionHandler(
    EXCEPTION_RECORD *ExceptionRecord,
    void * EstablisherFrame,
    CONTEXT *ContextRecord,
    void * DispatcherContext);
extern void __stdcall LoadLocalGfxHook();
extern void __stdcall LoadLocalOverworldGfxHook();
extern BOOL __stdcall BitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
extern BOOL __stdcall StretchBltHook(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, DWORD dwRop);
extern int __stdcall replacement_VbaStrCmp(BSTR arg1, BSTR arg2);
extern void __stdcall UpdateInputHook();
extern void UpdateInputHook_Wrapper();
extern void __stdcall WindowInactiveHook();
extern void SetSMBXFrameTiming(double ms);
extern void __stdcall FrameTimingHook();
extern void __stdcall FrameTimingMaxFPSHook();
extern void __stdcall FrameTimingHookQPC();
extern void __stdcall FrameTimingMaxFPSHookQPC();
extern short __stdcall MessageBoxOpenHook();

extern void __stdcall WorldHUDPrintTextController(VB6StrPtr* Text, short* fonttype, float* x, float* y);
extern BOOL __stdcall WorldOverlayHUDBitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
extern BOOL __stdcall WorldIconsHUDBitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
extern short __stdcall WorldHUDIsOnCameraHook(unsigned int* camIndex, Momentum* momentumObj);

extern void __stdcall GenerateScreenshotHook();
extern LRESULT CALLBACK KeyHOOKProc(int nCode, WPARAM wParam, LPARAM lParam);

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
void fixup_Mushbug();
void fixup_NativeFuncs();
void fixup_BGODepletion();

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

