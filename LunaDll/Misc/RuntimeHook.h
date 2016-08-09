#ifndef RuntimeHook_hhh
#define RuntimeHook_hhh

#include <vector>
#include <string>
#include "../Defines.h"

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/
typedef EXCEPTION_DISPOSITION __cdecl SEH_HANDLER(
    _In_ EXCEPTION_RECORD *_ExceptionRecord,
    _In_ void * _EstablisherFrame,
    _Inout_ CONTEXT *_ContextRecord,
    _Inout_ void * _DispatcherContext
    );



#ifndef NO_SDL
extern bool episodeStarted;
#endif

/************************************************************************/
/* Runtime Patch Main Functions                                         */
/************************************************************************/
void SetupThunRTMainHook();
void ParseArgs(const std::vector<std::wstring>& args);
void TrySkipPatch();

/************************************************************************/
/* Runtime Patch Public Functions                                       */
/************************************************************************/
void emulateVB6Error(int errorCode);
void showSMBXMessageBox(std::string message);

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
extern void __stdcall LevelHUDHook(int* cameraIdx, int* unknown0x4002);
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
extern BOOL __stdcall BitBltTraceHook(DWORD retAddr, HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
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
extern void __stdcall InitLevelEnvironmentHook();
extern short __stdcall MessageBoxOpenHook();
extern void __stdcall CameraUpdateHook_Wrapper();

extern void __stdcall WorldHUDPrintTextController(VB6StrPtr* Text, short* fonttype, float* x, float* y);
extern BOOL __stdcall WorldOverlayHUDBitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
extern BOOL __stdcall WorldIconsHUDBitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
extern short __stdcall WorldHUDIsOnCameraHook(unsigned int* camIndex, Momentum* momentumObj);

extern void __stdcall GenerateScreenshotHook();
extern LRESULT CALLBACK KeyHOOKProc(int nCode, WPARAM wParam, LPARAM lParam);

// Hooks which are helping fixups
extern void IsNPCCollidesWithVeggiHook_Wrapper();



/************************************************************************/
/* Hooks for analyze purpose                                            */
/************************************************************************/
extern void __stdcall collideNPCLoggingHook(DWORD retAddr, short* npcIndexToCollide, CollidersType* typeOfObject, short* objectIndex);
extern BOOL __stdcall HardcodedGraphicsBitBltHook(DWORD retAddr, HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
extern void __stdcall RenderLevelHook();
extern void __stdcall RenderWorldHook();

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
void fixup_Veggibug();
void fixup_NativeFuncs();
void fixup_BGODepletion();

/************************************************************************/
/* Render Priority Hooks                                                */
/************************************************************************/
template <int priority>
_declspec(naked) static void __stdcall _RenderBelowPriorityHookImpl() {
    __asm {
        pushf
        push eax
        push ecx
        push edx
    }
    gLunaRender.RenderBelowPriority((priority >= 100) ? DBL_MAX : priority);
    __asm {
        pop edx
        pop ecx
        pop eax
        popf
        ret
    }
}
template<int priority>
static inline constexpr void* GetRenderBelowPriorityHook(void) {
    return static_cast<void(__stdcall *)(void)>(&_RenderBelowPriorityHookImpl<priority>);
}

// Extended Character Id Support
void runtimeHookCharacterIdRegister(short id, const std::string& name, short base, short filterBlock, short switchBlock, short deathEffect);
void runtimeHookCharacterIdUnregister(short id);
void runtimeHookCharacterIdReset();

// Game Mode Handling
void __stdcall runtimeHookSmbxChangeModeHookRaw(void);
void __stdcall runtimeHookLoadLevel(VB6StrPtr* filename);
void __stdcall loadLevel_OrigFunc(VB6StrPtr* filename);
void __stdcall runtimeHookCloseWindow(void);

void __stdcall runtimeHookChangeResolution(void* arg1, void* arg2, void* arg3, void* arg4);
void __stdcall runtimeHookSmbxCheckWindowedRaw(void);

void __stdcall runtimeHookBlockBumpableRaw(void);
void __stdcall runtimeHookNPCVulnerabilityRaw(void);
void __stdcall runtimeHookNPCSpinjumpSafeRaw(void);

#endif

