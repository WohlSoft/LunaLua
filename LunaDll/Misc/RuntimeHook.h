#ifndef RuntimeHook_hhh
#define RuntimeHook_hhh

#include <vector>
#include <string>
#include "../Defines.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "AsmPatch.h"
#include "../GlobalFuncs.h"

struct SMBX_Warp;

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
/* Global Patch Variables                                               */
/************************************************************************/
extern AsmPatch<777> gDisablePlayerDownwardClipFix;
extern AsmPatch<8> gDisableNPCDownwardClipFix;
extern AsmPatch<167> gDisableNPCDownwardClipFixSlope;
extern Patchable& gNPCSectionFix;
extern Patchable& gFenceFixes;
extern Patchable& gLinkFairyClowncarFixes;

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
extern void __stdcall runtimeHookNPCRemovalConfirmHook();
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
extern void UpdateInputFinishHook_Wrapper();
extern void __stdcall runtimeHookUpdateInput();
extern void __stdcall WindowInactiveHook();
extern void SetSMBXFrameTiming(double ms);
extern void SetSMBXFrameTimingDefault();
extern double GetSMBXFrameTiming();
extern void __stdcall FrameTimingHook();
extern void __stdcall FrameTimingMaxFPSHook();
extern void __stdcall FrameTimingHookQPC();
extern void __stdcall FrameTimingMaxFPSHookQPC();
extern void __stdcall InitLevelEnvironmentHook();
extern void __stdcall runtimeHookMsgbox(short* pPlayerIdx);
extern void __stdcall runtimeHookNpcMsgbox_Wrapper(short* pPlayerIdx);
extern void __stdcall runtimeHookIgnoreThrownNPCs_Wrapper();
extern void __stdcall runtimeHookLinkShieldable_Wrapper();
extern void __stdcall runtimeHookNoShieldFireEffect_Wrapper();
extern void __stdcall CameraUpdateHook_Wrapper();
extern void __stdcall PostCameraUpdateHook_Wrapper();

extern void __stdcall WorldHUDPrintTextController(VB6StrPtr* Text, short* fonttype, float* x, float* y);
extern BOOL __stdcall WorldOverlayHUDBitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
extern BOOL __stdcall WorldIconsHUDBitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
extern short __stdcall WorldHUDIsOnCameraHook(unsigned int* camIndex, Momentum* momentumObj);

extern void __stdcall GenerateScreenshotHook();

// Hooks which are helping fixups
extern void IsNPCCollidesWithVeggiHook_Wrapper();



extern void __stdcall runtimeHookCreditsLoop();
extern void __stdcall runtimeHookGameover();

extern void __stdcall runtimeHookGameMenu();

/************************************************************************/
/* Hooks for some rendering purposes                                    */
/************************************************************************/
extern BOOL __stdcall HardcodedGraphicsBitBltHook(DWORD retAddr, HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
extern void __stdcall RenderLevelHook();
extern void __stdcall RenderWorldHook();
extern void __stdcall runtimeHookRestartCameraLoop();
extern void __stdcall runtimeHookSkipCamera();

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

/************************************************************************/
/* VB6 RNG Procs                                                        */
/************************************************************************/
extern void (__stdcall *rtcRandomize)(VARIANTARG const* arg);
extern float (__stdcall *rtcRandomNext)(VARIANTARG const* arg);


//Fixup errors
void fixup_TypeMismatch13();
void fixup_ErrorReporting();
void fixup_WarpLimit();
void fixup_EventLimit();
void fixup_LayerLimit();
void fixup_WebBox();
void fixup_Credits();
void fixup_Mushbug();
void fixup_Veggibug();
void fixup_NativeFuncs();
void fixup_BGODepletion();
void fixup_RenderPlayerJiterX();
void fixup_NPCSortedBlockArrayBoundsCrash();

/************************************************************************/
/* Render Priority Hooks                                                */
/************************************************************************/

template <int priority>
_declspec(naked) static void __stdcall _RenderBelowPriorityHookImpl() {
#ifdef __clang__
    // NB: I'm using %c modifiers for PriorityMostSignificantDWord and PriorityLeastSignificantDWord because of a clang bug: https://bugs.llvm.org/show_bug.cgi?id=24232
    __asm__ volatile (
        ".intel_syntax\n"
        "pushfd\n"
        "push eax\n"
        "push ecx\n"
        "push edx\n"

        "call %P[getRenderer]\n" // Pointer to the renderer is put in eax
        "mov ecx, eax\n" // The pointer to this is stored in ecx in the __thiscall convention

        "push %c[PriorityMostSignificantDWord]\n" // Push most significant dword of priority
        "push %c[PriorityLeastSignificantDWord]\n" // Push least significant dword of priority

        "call %P[RenderBelowPriority]\n"
        
        "pop edx\n"
        "pop ecx\n"
        "pop eax\n"
        "popfd\n"
        "ret\n"
        ".att_syntax\n"
        :
        : [PriorityMostSignificantDWord] "i" (DoubleMostSignificantDWord(priority >= 100 ? DBL_MAX : priority)),
          [PriorityLeastSignificantDWord] "i" (DoubleLeastSignificantDWord(priority >= 100 ? DBL_MAX : priority)),
          [getRenderer] "i" (&Renderer::Get),
          [RenderBelowPriority] "i" (&Renderer::RenderBelowPriority)

    );
#else
    __asm {
        pushfd
        push eax
        push ecx
        push edx
    }
    Renderer::Get().RenderBelowPriority((priority >= 100) ? DBL_MAX : priority);
    __asm {
        pop edx
        pop ecx
        pop eax
        popfd
        ret
    }
#endif
}
template<int priority>
static inline constexpr void* GetRenderBelowPriorityHook(void) {
    return static_cast<void(__stdcall *)(void)>(&_RenderBelowPriorityHookImpl<priority>);
}

template <int priority, unsigned int skipTargetAddr, bool* skipAddr>
_declspec(naked) static void __stdcall _RenderBelowPriorityHookWithSkipImpl() {
#ifdef __clang__
    // NB: I'm using %c modifiers for PriorityMostSignificantDWord, PriorityLeastSignificantDWord, skipTargetAddrValue and skipTargetAddrValue because of a clang bug: https://bugs.llvm.org/show_bug.cgi?id=24232
    __asm__ volatile (
        ".intel_syntax\n"
        "pushfd\n"
        "push eax\n"
        "push ecx\n"
        "push edx\n"

        "call %P[getRenderer]\n" // Pointer to the renderer is put in eax
        "mov ecx, eax\n" // The pointer to this is stored in ecx in the __thiscall convention

        "push %c[PriorityMostSignificantDWord]\n" // Push most significant dword of priority
        "push %c[PriorityLeastSignificantDWord]\n" // Push least significant dword of priority

        "call %P[RenderBelowPriority]\n"

        "mov al, byte ptr [%c[skipAddrValue]]\n"
        "test al, al\n"
        "jnz 1f\n"
        "mov dword ptr [esp + 16], %c[skipTargetAddrValue]\n"
    "1:\n"
        "pop edx\n"
        "pop ecx\n"
        "pop eax\n"
        "popfd\n"
        "ret\n"
        ".att_syntax\n"
        :
        : [PriorityMostSignificantDWord] "i" (DoubleMostSignificantDWord(priority >= 100 ? DBL_MAX : priority)),
          [PriorityLeastSignificantDWord] "i" (DoubleLeastSignificantDWord(priority >= 100 ? DBL_MAX : priority)),
          [skipTargetAddrValue] "i" (skipTargetAddr),
          [skipAddrValue] "i" (skipAddr),
          [getRenderer] "i" (&Renderer::Get),
          [RenderBelowPriority] "i" (&Renderer::RenderBelowPriority)
    );
#else
    __asm {
        pushfd
        push eax
        push ecx
        push edx
    }
    static unsigned int skipTargetAddrTmp = skipTargetAddr;
    Renderer::Get().RenderBelowPriority((priority >= 100) ? DBL_MAX : priority);
    if (*skipAddr)
    {
        __asm {
            pop edx
            pop ecx
            pop eax
            popfd
            ret
        }
    }
    else
    {
        __asm {
            pop edx
            pop ecx
            pop eax
            popfd
            add esp, 4
            push skipTargetAddrTmp
            ret
        }
    }
#endif
}
template<int priority, unsigned int skipTargetAddr, bool* skipAddr>
static inline constexpr void* GetRenderBelowPriorityHookWithSkip(void) {
    return static_cast<void(__stdcall *)(void)>(&_RenderBelowPriorityHookWithSkipImpl<priority, skipTargetAddr, skipAddr>);
}

// Extended Character Id Support
void runtimeHookCharacterIdApplyPatch();
void runtimeHookCharacterIdRegister(short id, const std::string& name, short base, short filterBlock, short switchBlock, short deathEffect);
void runtimeHookCharacterIdUnregister(short id);
void runtimeHookCharacterIdReset();
CharacterHitBoxData* runtimeHookGetExtCharacterHitBoxData(short characterId, short powerupId);

// Game Mode Handling
void __stdcall runtimeHookSmbxChangeModeHookRaw(void);
void __stdcall runtimeHookLoadLevel(VB6StrPtr* filename);
void __stdcall runtimeHookLoadLevelHeader(SMBX_Warp* warp, wchar_t* filename);
void __stdcall loadLevel_OrigFunc(VB6StrPtr* filename);
void __stdcall runtimeHookCloseWindow(void);

void __stdcall runtimeHookChangeResolution(void* arg1, void* arg2, void* arg3, void* arg4);
void __stdcall runtimeHookSmbxCheckWindowedRaw(void);

void __stdcall runtimeHookBlockBumpableRaw(void);
void __stdcall runtimeHookNPCVulnerabilityRaw(void);
void __stdcall runtimeHookNPCSpinjumpSafeRaw(void);
void __stdcall runtimeHookNPCNoWaterPhysicsRaw(void);
void __stdcall runtimeHookNPCWaterSplashAnimRaw(short* effectID, Momentum* coor, float* effectFrame, short* npcID, short* showOnlyMask);
void __stdcall runtimeHookNPCTerminalVelocityRaw(void);

void __stdcall runtimeHookNPCHarmlessGrabRaw(void);
void __stdcall runtimeHookNPCHarmlessThrownRaw(void);
void __stdcall runtimeHookCheckInputRaw(void);
void __stdcall runtimeHookSetHDCRaw(void);

void __stdcall runtimeHookInitGameHDC(void);
void __stdcall runtimeHookInitGameWindow(void);

void __stdcall runtimeHookLoadDefaultGraphics(void);

void __stdcall runtimeHookSaveGame(void);

void __stdcall runtimeHookCleanupLevel(void);

void __stdcall runtimeHookExitMainGame(void);

void __stdcall runtimeHookLoadWorld(VB6StrPtr* filename);

void __stdcall runtimeHookCleanupWorld(void);

void __stdcall runtimeHookPiranahDivByZero();
void __stdcall runtimeHookFixVeggieBlockCrash();

void __stdcall runtimeHookFixLinkFairyClowncar1();
void __stdcall runtimeHookFixLinkFairyClowncar2();
void __stdcall runtimeHookFixLinkFairyClowncar3();

// hooks for closing the game instaed of returning to titlescreen
void __stdcall runtimeHookCloseGame();

void __stdcall runtimeHookHitBlock(unsigned short* blockIndex, short* fromUpSide, unsigned short* playerIdx);
void __stdcall runtimeHookRemoveBlock(unsigned short* blockIndex, short* makeEffects);

void __stdcall runtimeHookPOW();

void __stdcall runtimeHookCollectNPC(short* playerIdx, short* npcIdx);

void __stdcall runtimeHookNPCTransformRandomVeggie(void);
void __stdcall runtimeHookNPCTransformSprout(void);
void __stdcall runtimeHookNPCTransformRandomBonus(void);
void __stdcall runtimeHookNPCTransformMushToHeart(void);
void __stdcall runtimeHookNPCTransformCoinToRupee(void);
void __stdcall runtimeHookNPCTransformSnifitBulletToSMB2Coin(void);
void __stdcall runtimeHookNPCTransformHeldYoshiToEgg(void);
void __stdcall runtimeHookNPCTransformBubblePopped(void);
void __stdcall runtimeHookNPCTransformKoopalingUnshell(void);
void __stdcall runtimeHookNPCTransformGaloombaUnflip(void);
void __stdcall runtimeHookNPCTransformPotionToDoor(void);
void __stdcall runtimeHookNPCTransformLudwigShell(void);
void __stdcall runtimeHookNPCTransformSMWSpinyEgg(void);
void __stdcall runtimeHookNPCTransformDespawned(void);
void __stdcall runtimeHookNPCTransformPSwitchResetRupeeCoins(void);
void __stdcall runtimeHookNPCTransformHeldSproutA(void);
void __stdcall runtimeHookNPCTransformHeldSproutB(void);
void __stdcall runtimeHookNPCTransformSMWKoopaEnterShell(void);
void __stdcall runtimeHookNPCTransformYoshiEatRandomVeggie(void);
void __stdcall runtimeHookYoshiEatPossibleNPCTransform(void);
void __stdcall runtimeHookYoshiEatExit(void);

void __stdcall runtimeHookLogCollideNpc(DWORD addr, short* pNpcIdx, CollidersType* pObjType, short* pObjIdx);
void __stdcall runtimeHookCollideNpc(short* pNpcIdx, CollidersType* pObjType, short* pObjIdx);
void __stdcall runtimeHookNpcHarmRaw_a291d8(void);
void __stdcall runtimeHookNpcHarmRaw_a29272(void);
void __stdcall runtimeHookNpcHarmRaw_a293ee(void);
void __stdcall runtimeHookNpcHarmRaw_a29442(void);
void __stdcall runtimeHookNpcHarmRaw_a2946b(void);
void __stdcall runtimeHookNpcHarmRaw_a294aa(void);
void __stdcall runtimeHookNpcHarmRaw_a29f8a(void);
void __stdcall runtimeHookNpcHarmRaw_a2a24c(void);
void __stdcall runtimeHookNpcHarmRaw_a2a304(void);
void __stdcall runtimeHookNpcHarmRaw_a2a36e(void);
void __stdcall runtimeHookNpcHarmRaw_a2a3e3(void);
void __stdcall runtimeHookNpcHarmRaw_a2a454(void);
void __stdcall runtimeHookNpcHarmRaw_a2a48c(void);
void __stdcall runtimeHookNpcHarmRaw_a2a542(void);
void __stdcall runtimeHookNpcHarmRaw_a2a55e(void);
void __stdcall runtimeHookNpcHarmRaw_a2a57f(void);
void __stdcall runtimeHookNpcHarmRaw_a2a5f4(void);
void __stdcall runtimeHookNpcHarmRaw_a2a627(void);
void __stdcall runtimeHookNpcHarmRaw_a2a662(void);
void __stdcall runtimeHookNpcHarmRaw_a2a695(void);
void __stdcall runtimeHookNpcHarmRaw_a2a6d9(void);
void __stdcall runtimeHookNpcHarmRaw_a2a704(void);
void __stdcall runtimeHookNpcHarmRaw_a2a782(void);
void __stdcall runtimeHookNpcHarmRaw_a2a7a3(void);
void __stdcall runtimeHookNpcHarmRaw_a2a7db(void);
void __stdcall runtimeHookNpcHarmRaw_a2a90e(void);
void __stdcall runtimeHookNpcHarmRaw_a2a92c(void);
void __stdcall runtimeHookNpcHarmRaw_a2aa5b(void);
void __stdcall runtimeHookNpcHarmRaw_a2aa7f(void);
void __stdcall runtimeHookNpcHarmRaw_a2aaa3(void);
void __stdcall runtimeHookNpcHarmRaw_a2ab3c(void);
void __stdcall runtimeHookNpcHarmRaw_a2ab7f(void);
void __stdcall runtimeHookNpcHarmRaw_a2ab99(void);
void __stdcall runtimeHookNpcHarmRaw_a2ac38(void);
void __stdcall runtimeHookNpcHarmRaw_a2acaf(void);
void __stdcall runtimeHookNpcHarmRaw_a2ad2a(void);
void __stdcall runtimeHookNpcHarmRaw_a2adca(void);
void __stdcall runtimeHookNpcHarmRaw_a2adef(void);
void __stdcall runtimeHookNpcHarmRaw_a2ae9e(void);
void __stdcall runtimeHookNpcHarmRaw_a2af22(void);
void __stdcall runtimeHookNpcHarmRaw_a2af72(void);
void __stdcall runtimeHookNpcHarmRaw_a2b01d(void);
void __stdcall runtimeHookNpcHarmRaw_a2b116(void);
void __stdcall runtimeHookNpcHarmRaw_a2b139(void);
void __stdcall runtimeHookNpcHarmRaw_a2b676(void);
void __stdcall runtimeHookNpcHarmRaw_a2b8b6(void);
void __stdcall runtimeHookNpcHarmRaw_a2b8c4(void);
void __stdcall runtimeHookNpcHarmRaw_a2b9c0(void);
void __stdcall runtimeHookNpcHarmRaw_a2ba11(void);
void __stdcall runtimeHookNpcHarmRaw_a2ba77(void);
void __stdcall runtimeHookNpcHarmRaw_a2badd(void);
void __stdcall runtimeHookNpcHarmRaw_a2bcdc(void);
void __stdcall runtimeHookNpcHarmRaw_a2bdeb(void);
void __stdcall runtimeHookNpcHarmRaw_a2be29(void);
void __stdcall runtimeHookNpcHarmRaw_a2bf45(void);
void __stdcall runtimeHookNpcHarmRaw_a2bfc9(void);
void __stdcall runtimeHookNpcHarmRaw_a2c130(void);
void __stdcall runtimeHookNpcHarmRaw_a2c13f(void);
void __stdcall runtimeHookNpcHarmRaw_a2c18b(void);
void __stdcall runtimeHookNpcHarmRaw_a2c300(void);
void __stdcall runtimeHookNpcHarmRaw_a2c404(void);
void __stdcall runtimeHookNpcHarmRaw_a2c437(void);
void __stdcall runtimeHookNpcHarmRaw_a2c4b1(void);
void __stdcall runtimeHookNpcHarmRaw_a2c50b(void);
void __stdcall runtimeHookNpcHarmRaw_a2c524(void);
void __stdcall runtimeHookNpcHarmRaw_a2c537(void);
void __stdcall runtimeHookNpcHarmRaw_a2c5ad(void);
void __stdcall runtimeHookNpcHarmRaw_a2c640(void);
void __stdcall runtimeHookNpcHarmRaw_a2c69f(void);
void __stdcall runtimeHookNpcHarmRaw_a2c6bf(void);
void __stdcall runtimeHookNpcHarmRaw_a2c712(void);
void __stdcall runtimeHookNpcHarmRaw_a2c7ab(void);
void __stdcall runtimeHookNpcHarmRaw_a2c80c(void);
void __stdcall runtimeHookNpcHarmRaw_a2c81e(void);
void __stdcall runtimeHookNpcHarmRaw_a2c88b(void);
void __stdcall runtimeHookNpcHarmRaw_a2c8a1(void);
void __stdcall runtimeHookNpcHarmRaw_a2c966(void);
void __stdcall runtimeHookNpcHarmRaw_a2cd3c(void);
void __stdcall runtimeHookNpcHarmRaw_a2cffe(void);
void __stdcall runtimeHookNpcHarmRaw_a2d1b9(void);
void __stdcall runtimeHookNpcHarmRaw_a2d2f5(void);
void __stdcall runtimeHookNpcHarmRaw_a2d311(void);
void __stdcall runtimeHookNpcHarmRaw_a2d79f(void);
void __stdcall runtimeHookNpcHarmRaw_a2d7ae(void);
void __stdcall runtimeHookNpcHarmRaw_a2d867(void);
void __stdcall runtimeHookNpcHarmRaw_a2d8ec(void);
void __stdcall runtimeHookNpcHarmRaw_a2d96c(void);
void __stdcall runtimeHookNpcHarmRaw_a2d977(void);
void __stdcall runtimeHookNpcHarmRaw_a2d9bf(void);
void __stdcall runtimeHookNpcHarmRaw_a2da36(void);
void __stdcall runtimeHookNpcHarmRaw_a2dac8(void);
void __stdcall runtimeHookNpcHarmRaw_a2dace(void);
void __stdcall runtimeHookNpcHarmRaw_a2dad4(void);
void __stdcall runtimeHookNpcHarmRaw_a2df6d(void);
void __stdcall runtimeHookNpcHarmRaw_a2df77(void);
void __stdcall runtimeHookNpcHarmRaw_a2e01d(void);
void __stdcall runtimeHookNpcHarmRaw_a2e058(void);
void __stdcall runtimeHookNpcHarmRaw_a2e0c2(void);
void __stdcall runtimeHookNpcHarmRaw_a2e155(void);
void __stdcall runtimeHookNpcHarmRaw_a2e278(void);
void __stdcall runtimeHookNpcHarmRaw_a2e2f5(void);
void __stdcall runtimeHookNpcHarmRaw_a2e556(void);
void __stdcall runtimeHookNpcHarmRaw_a2e641(void);
void __stdcall runtimeHookNpcHarmRaw_a2e64b(void);
void __stdcall runtimeHookNpcHarmRaw_a2e6c2(void);
void __stdcall runtimeHookNpcHarmRaw_a2e75c(void);
void __stdcall runtimeHookNpcHarmRaw_a2e795(void);
void __stdcall runtimeHookNpcHarmRaw_a2e800(void);
void __stdcall runtimeHookNpcHarmRaw_a2f26c(void);
void __stdcall runtimeHookNpcHarmRaw_a2f27a(void);
void __stdcall runtimeHookNpcHarmRaw_a2f2ed(void);
void __stdcall runtimeHookNpcHarmRaw_a2f376(void);
void __stdcall runtimeHookNpcHarmRaw_a2f792(void);
void __stdcall runtimeHookNpcHarmRaw_a2f79d(void);
void __stdcall runtimeHookNpcHarmRaw_a2f822(void);
void __stdcall runtimeHookNpcHarmRaw_a2f8fa(void);
void __stdcall runtimeHookNpcHarmRaw_a2f9bc(void);
void __stdcall runtimeHookNpcHarmRaw_a2f9d9(void);
void __stdcall runtimeHookNpcHarmRaw_a2fa5d(void);
void __stdcall runtimeHookNpcHarmRaw_a2fae5(void);
void __stdcall runtimeHookNpcHarmRaw_a2fb09(void);
void __stdcall runtimeHookNpcHarmRaw_a2fb13(void);
void __stdcall runtimeHookNpcHarmRaw_a2fb29(void);
void __stdcall runtimeHookNpcHarmRaw_a2fb34(void);
void __stdcall runtimeHookNpcHarmRaw_a2fbaf(void);
void __stdcall runtimeHookNpcHarmRaw_a2fde3(void);
void __stdcall runtimeHookNpcHarmRaw_a2fef9(void);
void __stdcall runtimeHookNpcHarmRaw_a2ff33(void);
void __stdcall runtimeHookNpcHarmRaw_a2ff4d(void);
void __stdcall runtimeHookNpcHarmRaw_a2ff9f(void);
void __stdcall runtimeHookNpcHarmRaw_a30011(void);
void __stdcall runtimeHookNpcHarmRaw_a3012c(void);
void __stdcall runtimeHookNpcHarmRaw_a30166(void);
void __stdcall runtimeHookNpcHarmRaw_a30180(void);
void __stdcall runtimeHookNpcHarmRaw_a30465(void);

void __stdcall runtimeHookColorSwitchRedNpc(void);
void __stdcall runtimeHookColorSwitchGreenNpc(void);
void __stdcall runtimeHookColorSwitchBlueNpc(void);
void __stdcall runtimeHookColorSwitchYellowNpc(void);
void __stdcall runtimeHookColorSwitchRedBlock(void);
void __stdcall runtimeHookColorSwitchGreenBlock(void);
void __stdcall runtimeHookColorSwitchBlueBlock(void);
void __stdcall runtimeHookColorSwitchYellowBlock(void);

void __stdcall runtimeHookLoadDefaultControls(void);

void __stdcall runtimeHookRunAnimInternal(short* effectID, Momentum* coor, float* effectFrame, short* npcID, short* showOnlyMask);

void __stdcall runtimeHookSemisolidInteractionHook_Raw();

MMRESULT __stdcall runtimeHookJoyGetPosEx(UINT uJoyID, LPJOYINFOEX pji);
MMRESULT __stdcall runtimeHookJoyGetPosExNull(UINT uJoyID, LPJOYINFOEX pji);
MMRESULT __stdcall runtimeHookJoyGetDevCapsA(UINT uJoyID, LPJOYCAPSA pjc, UINT cbjc);
void __stdcall runtimeHookUpdateJoystick();

void __stdcall runtimeHookDoExplosionInternal(Momentum* coor, short* bombType, short* playerIdx);

void __stdcall runtimeHookNpcGenerated(short* npcIdx);

void __stdcall runtimeHookStaticDirectionWrapper(void);

void __stdcall runtimeHookStoreCustomMusicPathWrapper(void);

void __stdcall runtimeHookCheckWindowFocus(void);

void __stdcall runtimeHookResetSectionMusicWrapper8EB370(void);
void __stdcall runtimeHookResetSectionMusicWrapper8EBA87(void);
void __stdcall runtimeHookResetSectionMusicWrapperA3576F(void);
void __stdcall runtimeHookResetSectionMusicWrapperA35E9B(void);
void __stdcall runtimeHookResetSectionMusicWrapperAA4486(void);

void __stdcall runtimeHookPlayerBouncePushCheckWrapper(void);

void __stdcall runtimeHookCompareWalkBlockForPlayerWrapper(void);

void __stdcall runtimeHookPreserveNPCWalkBlock();
void __stdcall runtimeHookCompareNPCWalkBlock();
void __stdcall runtimeHookNPCWalkFixClearTemp();
void __stdcall runtimeHookNPCWalkFixTempHitConditional();
void __stdcall runtimeHookNPCWalkFixSlope();

void __stdcall runtimeHookNPCSectionFix(short* npcIndex);
void __stdcall runtimeHookNPCSectionWrap(void);

void __stdcall runtimeHookAfterPSwitchBlocksReorderedWrapper(void);
void __stdcall runtimeHookPSwitchStartRemoveBlockWrapper(void);
void __stdcall runtimeHookPSwitchGetNewBlockAtEndWrapper(void);

void __stdcall runtimeHookNPCNoBlockCollision9E2AD0(void);
void __stdcall runtimeHookNPCNoBlockCollisionA089C3(void);
void __stdcall runtimeHookNPCNoBlockCollisionA10EAA(void);
void __stdcall runtimeHookNPCNoBlockCollisionA113B0(void);
void __stdcall runtimeHookNPCNoBlockCollisionA1760E(void);
void __stdcall runtimeHookNPCNoBlockCollisionA1B33F(void);

void __stdcall runtimeHookBlockPlayerFilter(void);
void __stdcall runtimeHookPlayerNPCInteractionCheck(void);
void __stdcall runtimeHookPlayerNPCCollisionCheck9AE8FA(void);
void __stdcall runtimeHookPlayerNPCCollisionCheck9ABC0B(void);
void __stdcall runtimeHookPlayerPlayerInteraction(void);

void __stdcall runtimeHookBlockNPCFilter(void);
void __stdcall runtimeHookNPCCollisionGroup(void);

void __stdcall runtimeHookLevelPauseCheck(void);

void __stdcall runtimeHookPlayerHarm(void);
void __stdcall runtimeHookPlayerKill(short* playerIdxPtr);

void __stdcall runtimeHookWarpEnter(void);
void __stdcall runtimeHookWarpInstant(void);
void __stdcall runtimeHookWarpPipe(void);
void __stdcall runtimeHookWarpDoor(void);

void __stdcall runtimeHookDrawBackground(short* section, short* camera);

void __stdcall runtimeHookLoadWorldList(void);

void __stdcall runtimeHookSpeedOverride(void);
void __stdcall runtimeHookSpeedOverrideBelt(void);

void __stdcall runtimeHookBlockSpeedSet_FSTP_ECX_EAX_ESI(void);
void __stdcall runtimeHookBlockSpeedSet_MOV_ECX_EDX_ESI(void);
void __stdcall runtimeHookBlockSpeedSet_FSTP_EAX_EDX_ESI(void);
void __stdcall runtimeHookBlockSpeedSet_FSTP_EAX_EDX_EDI(void);

bool __stdcall saveFileExists();

void __stdcall runtimeHookSetPlayerFenceSpeed(PlayerMOB *player);
bool __stdcall runtimeHookIncreaseFenceFrameCondition(PlayerMOB *player);
void __stdcall runtimeHookUpdateBGOMomentum(int bgoId, int layerId);

void __stdcall runtimeHookPlayerKillLava(short* playerIdxPtr);
void __stdcall runtimeHookPlayerKillLavaSolidExit(short* playerIdxPtr);
void __stdcall runtimeHookPlayerCountCollisionsForWeakLava(short* playerIdxPtr);
void __stdcall runtimeHookPlayerBlockCollisionEnd(void);

#endif
