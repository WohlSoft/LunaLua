#include <comutil.h>
#include "../../Globals.h"
#include "../RuntimeHook.h"
#include "../../LuaMain/LunaLuaMain.h"
#include "../../LuaMain/LuaProxy.h"
#include "../../Input/Input.h"
#include "../../GlobalFuncs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../SdlMusic/MusicManager.h"
#include "../../HardcodedGraphics/HardcodedGraphicsManager.h"
#include "../ErrorReporter.h"

#include "../SHMemServer.h"
#include "../AsmPatch.h"

#include "../../Rendering/GL/GLEngine.h"
#include "../../Main.h"
#include <IniProcessor/ini_processing.h>

#include "../RunningStat.h"
#include "../../Rendering/BitBltEmulation.h"
#include "../../Rendering/RenderUtils.h"
#include "../../Rendering/RenderOps/RenderStringOp.h"

#include "../../SMBXInternal/NPCs.h"
#include "../../SMBXInternal/Blocks.h"
#include "../../SMBXInternal/Level.h"

#include "../PerfTracker.h"

#include "../../Misc/TestMode.h"
#include "../../Misc/WaitForTickEnd.h"
#include "../../Rendering/ImageLoader.h"
#include "../../Misc/LoadScreen.h"

#include "../../SMBXInternal/HardcodedGraphicsAccess.h"
#include "../../Rendering/LunaImage.h"

#include "../../libs/PGE_File_Formats/file_formats.h"

extern HHOOK HookWnd;
extern HHOOK KeyHookWnd;

// Simple init hook to run the main LunaDLL initialization
void __stdcall ThunRTMainHook(void* arg1)
{
    LunaDLLInit();

    native_ThunRTMain(arg1);
}

extern void __stdcall InitHook()
{
    if (gStartupSettings.newLauncher){
        typedef bool(*RunProc)(void);
        typedef void(*GetPromptResultProc)(void*);
        typedef void(*FreeVarsProc)(void);
        newLauncherLib = LoadLibraryA("LunadllNewLauncher.dll");
        if (!newLauncherLib){
            std::string errMsg = "Failed to load the new Launcher D:!\nLunadllNewLauncher.dll is missing?\nError Code: ";
            errMsg += std::to_string((long long)GetLastError());
            MessageBoxA(NULL, errMsg.c_str(), "Error", 0);
            return;
        }
        RunProc hRunProc = (RunProc)GetProcAddress(newLauncherLib, "run");
        GL_PROC_CHECK(hRunProc, run, newLauncherLib, LunadllNewLauncher.dll)
            GetPromptResultProc hPrompt = (GetPromptResultProc)GetProcAddress(newLauncherLib, "GetPromptResult");
        GL_PROC_CHECK(hPrompt, GetPromptResult, newLauncherLib, LunadllNewLauncher.dll)
            FreeVarsProc hFreeVarsProc = (FreeVarsProc)GetProcAddress(newLauncherLib, "FreeVars");
        GL_PROC_CHECK(hFreeVarsProc, FreeVars, newLauncherLib, LunadllNewLauncher.dll)
            hRunProc();
        resultStruct settings;
        hPrompt((void*)&settings);

        if (!settings.result){
            GM_ISLEVELEDITORMODE = 0; //set run to false
            _exit(0);
        }
        GM_ISGAME = -1;
        if (settings.result == 2){
            GM_ISLEVELEDITORMODE = -1;
        }
        GM_NOSOUND = COMBOOL(settings.NoSound);
        GM_FRAMESKIP = COMBOOL(settings.disableFrameskip);
    }
    else{
        GM_ISLEVELEDITORMODE = COMBOOL(gStartupSettings.lvlEditor);
        GM_ISGAME = COMBOOL(gStartupSettings.game);
        GM_FRAMESKIP = COMBOOL(gStartupSettings.frameskip);
        GM_NOSOUND = COMBOOL(gStartupSettings.noSound);
    }

    if (gStartupSettings.debugger){
        newDebugger = LoadLibraryA("LunadllNewLauncher.dll");
        if (!newDebugger){
            std::string errMsg = "Failed to load the new Launcher D:!\nLunadllNewLauncher.dll is missing?\nError Code: ";
            errMsg += std::to_string((long long)GetLastError());
            MessageBoxA(NULL, errMsg.c_str(), "Error", 0);
            newDebugger = NULL;
            return;
        }
        runAsyncDebuggerProc = (void(*)(void))GetProcAddress(newDebugger, "runAsyncDebugger");
        asyncBitBltProc = (int(*)(HDC, int, int, int, int, HDC, int, int, unsigned int))GetProcAddress(newDebugger, "asyncBitBlt@36");
        GL_PROC_CHECK(runAsyncDebuggerProc, runAsyncDebugger, newDebugger, LunadllNewLauncher.dll)
            GL_PROC_CHECK(asyncBitBltProc, asyncBitBlt, newDebugger, LunadllNewLauncher.dll)
            //PATCH_JMP(0x4242D0, &bitBltHook);

            *(void**)0xB2F1D8 = (void*)asyncBitBltProc;
        runAsyncDebuggerProc();
    }
    if (gStartupSettings.logger){
        if (!newDebugger)
            newDebugger = LoadLibraryA("LunadllNewLauncher.dll");

        runAsyncLoggerProc = (void(*)(void))GetProcAddress(newDebugger, "runAsyncLogger");
        asyncLogProc = (void(*)(const char*))GetProcAddress(newDebugger, "asyncLog");
        GL_PROC_CHECK(runAsyncLoggerProc, runAsyncLogger, newDebugger, LunadllNewLauncher.dll)
            GL_PROC_CHECK(asyncLogProc, asyncLog, newDebugger, LunadllNewLauncher.dll)
            runAsyncLoggerProc();
    }



    /*void (*exitCall)(void);
    exitCall = (void(*)(void))0x8D6BB0;
    exitCall();*/
}

extern void __stdcall forceTermination()
{
    _exit(0);
}

extern int __stdcall LoadWorld()
{
    // We want to make sure we init the renderer before we start LunaLua when
    // entering levels...
    GLEngineProxy::CheckRendererInit();

    LunaLoadScreenStart();

    ResetLunaModule();
    gIsOverworld = true;

#ifndef NO_SDL
    if (!episodeStarted)
    {
        std::string wldPath = WStr2Str(GM_FULLDIR);
        MusicManager::loadCustomSounds(wldPath + "\\");
        episodeStarted = true;
    }
#endif

    // Init var bank
    gSavedVarBank.TryLoadWorldVars();
    gSavedVarBank.CheckSaveDeletion();
    gSavedVarBank.CopyBank(&gAutoMan.m_UserVars);

    // Check that we're not just transiently going though having the world loaded...

    if (GM_NEXT_LEVEL_FILENAME.length() == 0)
    {
        gLunaLua.init(CLunaLua::LUNALUA_WORLD, (std::wstring)GM_FULLDIR);
        gLunaLua.setReady(true); // We assume that the SMBX engine is already ready when loading the world

        // Overworld is guaranteed to be loaded by this point, so trigger onStart
        gLunaLua.triggerOnStart();

        // Mark next render frame as the 'first'
        g_GLEngine.SetFirstFramePending();
    }

    short plValue = GM_PLAYERS_COUNT;
#ifndef __MINGW32__
    __asm {
        MOV EAX, 1
            MOV CX, plValue
    }
#else
    asm(".intel_syntax noprefix\n"
        "mov eax, 1\n"
        "mov cx, %0\n"
        ".att_syntax\n": "=r" (plValue));
    //".intel_syntax prefix" :: [plValue] "g" (plValue) : "edx");

#endif
}

extern int __stdcall LoadIntro()
{
    std::string autostartFile = WStr2Str(getLatestConfigFile(L"autostart.ini"));

    if (file_existsX(autostartFile))
    {
        IniProcessing autostartConfig(autostartFile);
        if(autostartConfig.beginGroup("autostart"))
        {
            if(autostartConfig.value("do-autostart", false).toBool())
            {
                if (!gAutostartRan)
                {
                    GameAutostart autostarter = GameAutostart::createGameAutostartByIniConfig(autostartConfig);
                    autostarter.applyAutostart();
                    gAutostartRan = true;
                    if(autostartConfig.value("transient", false).toBool())
                    {
                        remove(autostartFile.c_str());
                    }
                }
            }
        }
        autostartConfig.endGroup();
    }

    if (GameAutostartConfig::nextAutostartConfig)
    {
        GameAutostartConfig::nextAutostartConfig->doAutostart();
        GameAutostartConfig::nextAutostartConfig.reset();
    }


#pragma warning(suppress: 28159)
    return GetTickCount();
}


extern DWORD __stdcall WorldLoop()
{
    gSavedVarBank.CheckSaveDeletion();

    // Update inputs
    Input::CheckSpecialCheats();
    Input::UpdateInputTasks();

    g_EventHandler.hookWorldLoop();

    gSavedVarBank.SaveIfNeeded();

#pragma warning(suppress: 28159)
    return GetTickCount();
}

// HUD Drawing Patches
static auto skipStarCountPatch = PatchCollection(
    PATCH(0x973E85).CONDJMP_TO_NOPJMP(),
    PATCH(0x97ADBF).CONDJMP_TO_NOPJMP(),
    PATCH(0x9837A1).CONDJMP_TO_NOPJMP()
    );

// HUD Hook -- Runs each time the HUD is drawn.
extern void __stdcall LevelHUDHook(int* cameraIdx, int* unknown0x4002)
{
    if (gLunaEnabled) {
        OnLevelHUDDraw(*cameraIdx);
    }

    if (gSMBXHUDSettings.skipStarCount) {
        skipStarCountPatch.Apply();
    }
    else {
        skipStarCountPatch.Unapply();
    }

    Renderer::Get().RenderBelowPriority(5);
    if (!gSMBXHUDSettings.skip) {
        native_renderLevelHud(cameraIdx, unknown0x4002);
    }
}


extern int __stdcall printLunaLuaVersion(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, unsigned int dwRop)
{
#ifndef NO_SDL
    if (episodeStarted)
    {   //Reset sounds to default when main menu is loaded
        MusicManager::resetSoundsToDefault();
        episodeStarted = false;
    }
#endif
    static std::string vStr = LUNALUA_VERSION;
    std::transform(vStr.begin(), vStr.end(), vStr.begin(), ::toupper);
    RenderStringOp(Str2WStr(vStr), 3, 5, 5).Draw(&Renderer::Get());
    if (newDebugger)
    {
        if (asyncBitBltProc){
            return asyncBitBltProc(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
        }
    }

    return BitBltHook(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}

extern void* __stdcall WorldRender()
{
    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> inputEvent = std::make_shared<Event>("onHUDUpdate", false);
        inputEvent->setDirectEventName("onHUDUpdate");
        inputEvent->setLoopable(false);
        gLunaLua.callEvent(inputEvent);
    }

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> inputEvent = std::make_shared<Event>("onHUDDraw", false);
        inputEvent->setDirectEventName("onHUDDraw");
        inputEvent->setLoopable(false);
        gLunaLua.callEvent(inputEvent);
    }

    gSpriteMan.RunSprites();
    Renderer::Get().RenderBelowPriority(DBL_MAX);

    return (void*)0xB25010;
}

extern MCIERROR __stdcall mciSendStringHookA(__in LPCSTR lpstrCommand, __out_ecount_opt(uReturnLength) LPSTR lpstrReturnString, __in UINT uReturnLength, __in_opt HWND hwndCallback)
{
    bool doLogInput = true;
    bool doLogOutput = true;

    if (lpstrCommand == 0){
        doLogInput = false;
    }
    else{
        if (lpstrCommand[0] == 0){
            doLogInput = false;
        }
    }

    std::string inputStr = "";
    std::string outputStr = "";

    if (doLogInput){
        inputStr += "Input: ";
        inputStr += lpstrCommand;

        if (newDebugger){
            if (asyncLogProc){
                asyncLogProc(inputStr.c_str());
            }
        }
    }

    //Swap to restore old code or to use emulator
    //MCIERROR ret = mciSendStringA(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);
    MCIERROR ret = gMciEmulator.mciEmulate(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);

    if (lpstrReturnString == 0){
        doLogOutput = false;
    }
    else{
        if (lpstrReturnString[0] == 0){
            doLogOutput = false;
        }
    }
    if (doLogOutput){
        outputStr += "Output: ";
        outputStr += lpstrReturnString;

        if (newDebugger){
            if (asyncLogProc){
                asyncLogProc(outputStr.c_str());
            }
        }
    }
    return ret;
}

extern float __stdcall vbaR4VarHook(VARIANTARG* variant)
{
    if (asyncLogProc)
    {
        std::stringstream q;
        q << variant->vt << " ";
        if (variant->vt == VT_R8)
        {
            q << "src:" << variant->dblVal << " dst:" << static_cast<float>(variant->dblVal);
        }
        std::string rr("vbaR4VarHook type:" + q.str() + ";");
        asyncLogProc(rr.c_str());
    }

    switch (variant->vt)
    {
    case VT_BSTR:
    {
        wchar_t* str = variant->bstrVal;
        return (float)wcstod(str, NULL);
    }
        break;
    case (VT_BSTR | VT_BYREF) :
    {
        if (variant->pbstrVal == NULL) return 0.0;
        wchar_t* str = *(variant->pbstrVal);
        return (float)wcstod(str, NULL);
    }
                              break;
    case VT_I2:
    {
        short str = variant->iVal;
        return (float)str;
    }
        break;
    case (VT_I2 | VT_BYREF) :
    {
        if (variant->piVal == NULL) return 0.0;
        return (float)((int)(*(variant->piVal)));
    }
                            break;
    case VT_I4:
    {
        long str = variant->lVal;
        return (float)str;
    }
        break;
    case (VT_I4 | VT_BYREF) :
    {
        if (variant->plVal == NULL) return 0.0;
        return (float)(*variant->plVal);
    }
                            break;
    case VT_BOOL:
    {
        bool str = variant->boolVal;
        return (float)(int)str;
    }
        break;
    case (VT_BOOL | VT_BYREF) :
    {
        if (variant->pboolVal == NULL) return 0.0;
        bool str = *(variant->pboolVal);
        return (float)(int)str;
    }
                              break;
    case VT_R4:
    {
        return variant->fltVal;
    }
        break;
    case (VT_R4 | VT_BYREF) :
    {
        if (variant->pfltVal == NULL) return 0.0;
        return (*variant->pfltVal);
    }
                            break;
    case VT_R8:
    {
        return static_cast<float>(variant->dblVal);
    }
        break;
    case (VT_R8 | VT_BYREF) :
    {
        if (variant->pdblVal == NULL) return 0.0;
        return (float)(*variant->pdblVal);
    }
                            break;
    case VT_CY:
    {
        CY x = variant->cyVal;
        float y = (float)x.Hi;
        float z = (float)x.Lo;
        while (z>1.0)
        {
            z /= 10;
        }
        y += z;
        return y;
    }
        break;
    case (VT_CY | VT_BYREF) :
    {
        if (variant->pcyVal == 0) return 0.0;
        CY x = *(variant->pcyVal);
        float y = (float)x.Hi;
        float z = (float)x.Lo;
        while (z>1.0)
        {
            z /= 10;
        }
        y += z;
        return y;
    }
                            break;
    case  VT_UI1:
    {
        return (float)((int)((char)variant->bVal));
    }
        break;
    case (VT_UI1 | VT_BYREF) :
    {
        return (float)((int)((char)(*(variant->pbVal))));
    }
    default:
        break;
    }
    return 0.0;//__vbaR4Var(variant);
}

extern int __stdcall rtcMsgBoxHook(VARIANTARG* msgText, DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4)
{
    std::wstring msg((wchar_t*)msgText->bstrVal);
    if (gHook_SkipTestMsgBox){
        if (msg == std::wstring((wchar_t*)0x42BE28)){
            gHook_SkipTestMsgBox = false;
            return 7;
        }
    }
    gHook_SkipTestMsgBox = false;
    return rtcMsgBox(msgText, arg1, arg2, arg3, arg4);
}



extern void __stdcall doEventsLevelEditorHook()
{
    /*void* lvlEditForm = *(void**)(0x00B2D7E8);
    if(lvlEditForm){
    char* lvlEditFormVtbl = *(char**)lvlEditForm;
    ((HRESULT (__stdcall*)(void *, void *))*((char**)lvlEditFormVtbl + 58))(lvlEditForm, )

    }*/

    /*HMODULE vmVB6Lib = GetModuleHandleA("msvbvm60.dll");
    GetProcAddress(vmVB6Lib, "rtcDoEvents")();*/
}

static unsigned short npcRemovalConfirmed = 0;

extern void __stdcall NPCKillHook(short* npcIndex_ptr, short* killReason)
{
    short npcIdx = *npcIndex_ptr;
    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> npcKillEvent = std::make_shared<Event>("onNPCKill", true);
        npcKillEvent->setDirectEventName("onNPCKill");
        npcKillEvent->setLoopable(false);
        gLunaLua.callEvent(npcKillEvent, npcIdx, *killReason);
        if (npcKillEvent->native_cancelled())
        {
            ::NPC::Get(npcIdx - 1)->killFlag = 0;
            return;
        }
    }

    short oldNpcRemovalConfirmed = npcRemovalConfirmed;
    npcRemovalConfirmed = 0;

    native_cleanupKillNPC(npcIndex_ptr, killReason);

    if (npcRemovalConfirmed != 0)
    {
        short newIdx = npcIdx - 1;    // 0 based
        short oldIdx = GM_NPCS_COUNT; // 0 based

        // The NPC was indeed removed
        if (gLunaLua.isValid() && (newIdx >= 0) && (oldIdx >= 0)) {
            std::shared_ptr<Event> npcKillEvent = std::make_shared<Event>("onPostNPCRearrangeInternal", false);
            npcKillEvent->setDirectEventName("onPostNPCRearrangeInternal");
            npcKillEvent->setLoopable(false);
            gLunaLua.callEvent(npcKillEvent, newIdx, oldIdx);
        }
    }

    npcRemovalConfirmed = oldNpcRemovalConfirmed;
}

// 00A3A679
__declspec(naked) void __stdcall runtimeHookNPCRemovalConfirmHook()
{
    __asm {
        MOV npcRemovalConfirmed, -1
        RET
    }
}

extern int __stdcall __vbaStrCmp_TriggerSMBXEventHook(BSTR nullStr, BSTR eventName)
{
    int(__stdcall *origCmp)(BSTR, BSTR) = (int(__stdcall *)(BSTR, BSTR))IMP_vbaStrCmp;

    // Trigger onStart here to ensure it happens just before the "Level - Start" event
    gLunaLua.triggerOnStart();

    // Mark next render frame as the 'first'
    g_GLEngine.SetFirstFramePending();

    std::shared_ptr<Event> triggerEventData = std::make_shared<Event>("onEvent", true);
    triggerEventData->setDirectEventName("onEventDirect");
    gLunaLua.callEvent(triggerEventData, WStr2Str(eventName));
    if (triggerEventData->native_cancelled())
        return 0;
    return origCmp(nullStr, eventName);
}

extern void __stdcall checkLevelShutdown()
{
    // DEPRECATED HOOK

    __asm{
        CMP WORD PTR DS : [0x00B2C5B4], 0
    }
}

// Storage of original exception handler
SEH_HANDLER* LunaDLLOriginalExceptionHandler = NULL;

EXCEPTION_DISPOSITION __cdecl LunaDLLCustomExceptionHandler(
    EXCEPTION_RECORD *ExceptionRecord,
    void * EstablisherFrame,
    CONTEXT *ContextRecord,
    void * DispatcherContext)
{
    // For VB error code 40040, defer to the original handler
    bool isVB6Exception = (ExceptionRecord->ExceptionCode == 0xc000008f);
    if (isVB6Exception && lastVB6ErrCode == 40040) {
        return LunaDLLOriginalExceptionHandler(ExceptionRecord, EstablisherFrame, ContextRecord, DispatcherContext);
    }

    // Remove some hooks first
    if (HookWnd) UnhookWindowsHookEx(HookWnd);
    if (KeyHookWnd) UnhookWindowsHookEx(KeyHookWnd);

    ErrorReport::SnapshotError(ExceptionRecord, ContextRecord);
    ErrorReport::report();

    _exit(0);
    return ExceptionContinueSearch; // Never reached
}

extern void __stdcall recordVBErrCode(int errCode)
{
    // Running the whole stack trace now is *far* too slow and can make the
    // editor grind to a halt in some situations due to exceptions which are
    // caught in the VB code.

    // Instead... just capture a nice lightweight context. We can pass this
    // to StackWalker later.
    RtlCaptureContext(&lastVB6ErrContext);

    // Also record the VB6 error code, because this is simpler than fetching
    // VB6's "error" object that stores this internally (would involve calling
    // rtcErrObj)
    lastVB6ErrCode = (ErrorReport::VB6ErrorCode)errCode;

    //HERE NEED ESI CMP CODE (ORIGINAL CODE)
    __asm{
        CMP     ESI, 0x9C68
    }
}

extern void __stdcall LoadLocalGfxHook()
{
    // In the past, we would call native_loadLocalGfx() here, but that is now
    // being replaced.
    ImageLoader::Run();
}


extern void __stdcall LoadLocalOverworldGfxHook()
{
    // In the past, we would call native_loadWorldGfx() here, but that is now
    // being replaced.
    ImageLoader::Run();
}

extern BOOL __stdcall BitBltTraceHook(
    DWORD retAddr,
    HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop
    )
{
    /*
     Insert debug code based on return address, such as:
     if (retAddr >= 0x96C036 && retAddr <= 0x987C10) {
        return 0;
     }
     */

    /*
    Example code to log bitblts
    static std::ofstream f;
    if (hdcDest == (HDC)GM_SCRN_HDC) {
        if (!f.is_open()) {
            f.open("bitblt_log.txt", std::ios::out);
        }
        f << std::hex << (DWORD)retAddr << ": ";
        f << "hdc=0x" << std::hex << (DWORD)hdcSrc << " ";
        f << "pos=" << std::dec << nXDest << "," << nYDest << " ";
        f << "size=" << std::dec << nWidth << "," << nHeight << " ";
        f << "rop=0x" << std::hex << dwRop << " ";
        f << std::endl;
        f.flush();
    }
    */

    return BitBltHook(
        hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop
        );
}

extern BOOL __stdcall BitBltHook(
    HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop
    )
{
    // Only override if the BitBlt is for the screen
    if (hdcDest == (HDC)GM_SCRN_HDC)
    {
        // Make sure we kill the loadscreen before vanilla rendering
        if (!TestModeIsLoadPending())
        {
            LunaLoadScreenKill();
        }

        g_BitBltEmulation.onBitBlt(hdcSrc, nXDest, nYDest, nWidth, nHeight, nXSrc, nYSrc, dwRop);
        return -1;
    }

    return BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}

extern BOOL __stdcall StretchBltHook(
    HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
    HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
    DWORD dwRop
    )
{
    static uint8_t callCount = 0;


    if (hdcSrc == (HDC)GM_SCRN_HDC)
    {
        // Make sure any mask BitBlt's are flushed before the StretchBlt of the frame
        g_BitBltEmulation.flushPendingBlt();
    }

    // If we're copying from our rendering screen, we're done with the frame
    if (hdcSrc == (HDC)GM_SCRN_HDC && g_GLEngine.IsEnabled() && !Renderer::IsAltThreadActive())
    {
        g_GLEngine.RenderCameraToScreen(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, dwRop);

        return TRUE;
    }

    return StretchBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, dwRop);
}

// This is more optimized than __vbaStrCmp for the most typical case, but does the same thing
int __stdcall replacement_VbaStrCmp(BSTR arg1, BSTR arg2) {
    if (arg1 == NULL) arg1 = L"";
    if (arg2 == NULL) arg2 = L"";
    return wcscmp(arg1, arg2);
}



__declspec(naked) void UpdateInputHook_Wrapper()
{
    __asm {
        MOV EBX, 1
        JMP UpdateInputHook
    }
}


extern void __stdcall UpdateInputHook()
{
    gLunaGameControllerManager.pollInputs();
    g_EventHandler.hookInputUpdate();
}

extern void __stdcall WindowInactiveHook()
{
    // Replacement for what we hooked
    native_rtcDoEvents();

    // Don't hog the CPU when window is inactive!
    Sleep(1);
}

/**
 * 15.6ms is the normal vanilla SMBX frame time.
 * When using timeGetTime to time it, this introduces some frame jitter and we
 * could get lower frame jitter by going with 16ms...
 * however let's keep the gameplay speed faithful instead, and use
 * QueryPerformanceCounter for improved frame jitter when we're on an OS new
 * enough that we trust QueryPerformanceCounter.
 **/
//#define ENABLE_FRAME_TIMING_BENCHMARK
static double FRAME_TIMING_MS = 15.600;
extern void SetSMBXFrameTiming(double ms)
{
    FRAME_TIMING_MS = ms;
}

bool g_ResetFrameTiming = false;

extern void __stdcall FrameTimingHookQPC()
{
    WaitForTickEnd::RunQueued();
    g_PerfTracker.endFrame();
    static int64_t lastFrameTime = 0;
    static double frameError = 0.0;
    double frameDuration;
    LARGE_INTEGER currentTime;
    double frameTime;

    static double qpcFactor = 0.0;
    if (qpcFactor == 0.0) {
        LARGE_INTEGER sFreqStruct;
        QueryPerformanceFrequency(&sFreqStruct);
        qpcFactor = 1000.0 / sFreqStruct.QuadPart;
        if (qpcFactor > 1.0) {
            qpcFactor = 0.0;
        }
    }

    if (g_ResetFrameTiming)
    {
        g_ResetFrameTiming = false;
        lastFrameTime = 0;
        frameError = 0;
    }

    // Get the desired duration for this frame
    frameDuration = FRAME_TIMING_MS - frameError * 0.90;

    QueryPerformanceCounter(&currentTime);
    frameTime = (currentTime.QuadPart - lastFrameTime) * qpcFactor;
    if (qpcFactor == 0.0 || lastFrameTime == 0 || frameTime > 100.0) {
        // If we've lost track of time, synchronize with scheduler because it turns out if we
        // call Sleep at the wrong time Windows likes to eat too much CPU during the Sleep call.
        // Yeah. This is weird.
        DWORD firstTick = GetTickCount();
        DWORD tick;
        do {
            tick = GetTickCount();
        } while (tick == firstTick);
        QueryPerformanceCounter(&currentTime);
        lastFrameTime = currentTime.QuadPart;
        frameError = -2.0;

        g_PerfTracker.startFrame();
        return;
    }

    // Wait until time >= (nextFrameTime-1)
    // (We'll use a busy loop to finish off the rest of the timing, for sake of reduced jitter)
    while (frameDuration - frameTime >= 2.0) {
        Sleep((DWORD)(frameDuration - frameTime - 1.0));
        QueryPerformanceCounter(&currentTime);
        frameTime = (currentTime.QuadPart - lastFrameTime) * qpcFactor;
    }

    // Busy loop to finish off the timing
    while (frameDuration - frameTime > 0.0) {
        QueryPerformanceCounter(&currentTime);
        frameTime = (currentTime.QuadPart - lastFrameTime) * qpcFactor;
    }
    GM_CURRENT_TIME = GetTickCount();

    // Compensate for errors in frame timing
    frameError = frameError * 0.1 + frameTime - frameDuration;

#if defined(ENABLE_FRAME_TIMING_BENCHMARK)
    static RunningStat sFrameTime;
    if (sFrameTime.Count() > 65 * 10) sFrameTime.Clear();
    if (frameTime < 200.0)
    {
        sFrameTime.Push(frameTime);
    }
    Renderer::Get().SafePrint(utf8_decode(sFrameTime.Str()), 3, 5, 5);
#endif

    if (frameError > 10.0) frameError = 10.0;
    if (frameError < -10.0) frameError = -10.0;
    lastFrameTime = currentTime.QuadPart;

    g_PerfTracker.startFrame();
}


extern void __stdcall FrameTimingMaxFPSHookQPC()
{
    // If we're in "max FPS" mode (either via cheat code or editor menu), bypass frame timing
    if (GM_MAX_FPS_MODE)
    {
        WaitForTickEnd::RunQueued();
        return;
    }

    // If we're not in "max FPS" mode, run the frame timing as normal
    FrameTimingHookQPC();
}

extern void __stdcall FrameTimingHook()
{
    WaitForTickEnd::RunQueued();
    g_PerfTracker.endFrame();
    static double lastFrameTime = 0.0;
    double nextFrameTime = lastFrameTime;
    static double frameError = 0.0;

    if (g_ResetFrameTiming)
    {
        g_ResetFrameTiming = false;
        GM_CURRENT_TIME = GetTickCount();
        lastFrameTime = GM_CURRENT_TIME;
        nextFrameTime = lastFrameTime;
        frameError = 0;
    }

    // Compensate for error in the last frame's timing
    nextFrameTime += FRAME_TIMING_MS - frameError;

    // Wait until time >= (nextFrameTime-1)
    // (We'll use a busy loop to finish off the rest of the timing, for sake of reduced jitter)
    GM_CURRENT_TIME = GetTickCount();
    while ((nextFrameTime - 1) > GM_CURRENT_TIME && GM_CURRENT_TIME >= lastFrameTime) {
        Sleep(1);
        GM_CURRENT_TIME = GetTickCount();
    }

    // Busy loop to finish off the timing
    while (nextFrameTime > GM_CURRENT_TIME && GM_CURRENT_TIME >= lastFrameTime) {
        GM_CURRENT_TIME = GetTickCount();
    }
    lastFrameTime = GM_CURRENT_TIME;

    // Compensate for errors in frame timing
    frameError = GM_CURRENT_TIME - nextFrameTime;
    if (frameError > 5.0) frameError = 5.0;
    if (frameError < -5.0) frameError = -5.0;

#if defined(ENABLE_FRAME_TIMING_BENCHMARK)
    static RunningStat sFrameTime;
    static double dDivisor = 0.0;
    if (dDivisor == 0.0) {
        LARGE_INTEGER sFreqStruct;
        QueryPerformanceFrequency(&sFreqStruct);
        dDivisor = 1000.0 / sFreqStruct.QuadPart;
    }
    static LARGE_INTEGER sLastCount = {0};
    {
        LARGE_INTEGER sCount;
        QueryPerformanceCounter(&sCount);
        if (sLastCount.QuadPart != 0) {
            double milliCount = (sCount.QuadPart - sLastCount.QuadPart) * dDivisor;
            if (sFrameTime.Count() > 65*10) sFrameTime.Clear();
            sFrameTime.Push(milliCount);
            if (milliCount > 100) sFrameTime.Clear();
        }
        sLastCount.QuadPart = sCount.QuadPart;
    }
    Renderer::Get().SafePrint(utf8_decode(sFrameTime.Str()), 3, 5, 5);
#endif

    g_PerfTracker.startFrame();
}

extern void __stdcall FrameTimingMaxFPSHook()
{
    // If we're in "max FPS" mode (either via cheat code or editor menu), bypass frame timing
    if (GM_MAX_FPS_MODE)
    {
        WaitForTickEnd::RunQueued();
        return;
    }

    // If we're not in "max FPS" mode, run the frame timing as normal
    FrameTimingHook();
}

// Also know as "Player init" hook. This method is called when the player resets.
extern void __stdcall InitLevelEnvironmentHook()
{
    // This function will fail if the player is a custom character but Lua is not yet loaded
    // This failure condition would occur for cross level warp and test mode restart, because
    // that briefly transitions through overworld load state without actually loading Lua.
    // To work around this, just don't perform an excess unnecessary reset of the player during
    // this transition.
    if (GM_NEXT_LEVEL_FILENAME.length() == 0)
    {
        native_initLevelEnv();
    }
}

static _declspec(naked) void __stdcall msgbox_OrigFunc(unsigned int* pPlayerIdx)
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x8E54C6
        RET
    }
}

void __stdcall runtimeHookMsgbox(unsigned int* pPlayerIdx)
{
    bool isCancelled = false; // We want to be sure that it doesn't return on the normal menu
                              // A note here: If the message is set, then the message box will called
                              // However, if a message is not set, then this function is called when the menu opens.

    if ((GM_STR_MSGBOX) && (GM_STR_MSGBOX.length() > 0)) {
        if (gLunaLua.isValid()) {
            std::shared_ptr<Event> messageBoxEvent = std::make_shared<Event>("onMessageBox", true);
            messageBoxEvent->setDirectEventName("onMessageBox");
            messageBoxEvent->setLoopable(false);
            gLunaLua.callEvent(messageBoxEvent, (std::string)GM_STR_MSGBOX, *pPlayerIdx);
            isCancelled = messageBoxEvent->native_cancelled();
        }
    }
    else
    {
        if (gLunaLua.isValid()) {
            std::shared_ptr<Event> messageBoxEvent = std::make_shared<Event>("onPause", true);
            messageBoxEvent->setDirectEventName("onPause");
            messageBoxEvent->setLoopable(false);
            gLunaLua.callEvent(messageBoxEvent, *pPlayerIdx);
            isCancelled = messageBoxEvent->native_cancelled();
        }
    }

    if (!isCancelled)
    {
        msgbox_OrigFunc(pPlayerIdx);
    }
}

static void __stdcall runtimeHookNpcMsgbox(unsigned int npcIdxWithOffset, unsigned int* pPlayerIdx)
{
    unsigned int npcIdx = npcIdxWithOffset - 128;

    bool isCancelled = false;

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> messageBoxEvent = std::make_shared<Event>("onMessageBox", true);
        messageBoxEvent->setDirectEventName("onMessageBox");
        messageBoxEvent->setLoopable(false);
        gLunaLua.callEvent(messageBoxEvent, (std::string)GM_STR_MSGBOX, *pPlayerIdx, npcIdx);
        isCancelled = messageBoxEvent->native_cancelled();
    }

    if (!isCancelled)
    {
        msgbox_OrigFunc(pPlayerIdx);
    }
}

_declspec(naked) void __stdcall runtimeHookNpcMsgbox_Wrapper(unsigned int* pPlayerIdx)
{
    __asm {
        POP ECX
        PUSH EDI
        PUSH ECX
        JMP runtimeHookNpcMsgbox
    }
}

static int __stdcall runtimeHookIgnoreThrownNPCs(NPCMOB* npc)
{
    // Only takes the NPC which is being collided with, not the thrown NPC itself
    int npcID = npc->id;
    if (npcID >= 1 && npcID <= NPC::MAX_ID) {
        // Vanilla check overwritten by hook
        WORD* interactableArray = (WORD*)isInteractableNPC_ptr;
        if (interactableArray[npcID] != 0) {
            return 1;
        }
        // Custom flag check
        if (NPC::GetIgnoreThrownNPCs(npcID)) {
            return 1;
        }

    }
    return 0;
}

_declspec(naked) void __stdcall runtimeHookIgnoreThrownNPCs_Wrapper()
{
    __asm {
        LEA ECX, [ECX + EAX * 8] // Get NPC address
        POP EAX // Remove return address from stack
        PUSHFD // Save pre-call state
        PUSH ECX
        PUSH EDX
        PUSH ECX // Push NPC argument
        CALL runtimeHookIgnoreThrownNPCs // Call the target function
        POP EDX // Restore state
        POP ECX
        POPFD
        CMP EAX, 0 // Check if return value is false
        JE runtimeHookIgnoreThrownNPCs_IsFalse
        PUSH 0xA1BAD5 // Otherwise exits to path of code where killing is skipped
        RET
runtimeHookIgnoreThrownNPCs_IsFalse:
        PUSH 0xA1AA75 // Exits to path of code where NPC is killed
        RET
    }
}

static int __stdcall runtimeHookLinkShieldable(NPCMOB* npc)
{
    if (NPC::GetLinkShieldable(npc->id))
        return 1;
    return 0;
}

_declspec(naked) void __stdcall runtimeHookLinkShieldable_Wrapper()
{
    __asm {
        PUSH EAX // Store EAX if needed in false branch
        LEA EAX, [EAX + EDX * 8] // Get NPC address

        PUSHFD // Save pre-call state
        PUSH ECX
        PUSH EDX

        PUSH EAX // Push NPC argument
        CALL runtimeHookLinkShieldable // Call the target function

        POP EDX // Restore state
        POP ECX
        POPFD

        CMP EAX, 0 // Check if return value is false
        JE runtimeHookLinkShieldable_IsFalse

        POP EAX // Remove old eax from the stack
        POP EAX // Remove return address from stack
        PUSH 0xA52D11 // And skip checks
        RET

        runtimeHookLinkShieldable_IsFalse :
        POP EAX // Restore old value of EAX
        RET // Continue on with battle mode checks
    }
}

static int __stdcall runtimeHookNoShieldFireEffect(NPCMOB* npc)
{
    if (NPC::GetNoShieldFireEffect(npc->id))
        return 1;
    return 0;
}

_declspec(naked) void __stdcall runtimeHookNoShieldFireEffect_Wrapper()
{
    __asm {
        PUSHFD // Save pre-call state
        PUSH EAX
        PUSH EDX

        PUSH ESI // Push NPC argument
        CALL runtimeHookNoShieldFireEffect // Call the target function

        CMP EAX, 0 // Check if return value is false
        JNE runtimeHookNoShieldFireEffect_IsTrue

        POP EDX // Restore state
        POP EAX
        POPFD
        RET

        runtimeHookNoShieldFireEffect_IsTrue :
        POP EDX // Restore state
        POP EAX
        POPFD
        POP ECX // Remove return address from stack
        PUSH 0xA53384 // And skip effects
        RET
    }
}

static short g_renderDoneCameraUpdate = 0;
static bool g_ranOnDrawThisFrame = false;

static void __stdcall CameraUpdateHook(int cameraIdx)
{
    // Enforce rounded camera position
    SMBX_CameraInfo::setCameraX(cameraIdx, std::round(SMBX_CameraInfo::getCameraX(cameraIdx)));
    SMBX_CameraInfo::setCameraY(cameraIdx, std::round(SMBX_CameraInfo::getCameraY(cameraIdx)));

    Renderer::Get().StartCameraRender(cameraIdx);

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> cameraUpdateEvent = std::make_shared<Event>("onCameraUpdate", false);
        cameraUpdateEvent->setDirectEventName("onCameraUpdate");
        cameraUpdateEvent->setLoopable(false);
        gLunaLua.callEvent(cameraUpdateEvent, cameraIdx);
    }
}

void __declspec(naked) __stdcall CameraUpdateHook_Wrapper()
{
    __asm {
        POP EAX                        // POP the return address
        PUSH DWORD PTR DS:[EBP - 0x38] // Sneak a camera index argument in there
        PUSH EAX                       // PUSH the return address
        JMP CameraUpdateHook           // JMP to CameraUpdateHook
    };
}

static void __stdcall PostCameraUpdateHook(int cameraIdx, int maxCameraIdx)
{
    static double cameraPos[3][2] = {0};

    // Run onDraw at this point, before the first camera starts rendering
    if (!g_ranOnDrawThisFrame)
    {
        g_ranOnDrawThisFrame = true;

        // Store camera states
        for (int i=1; i<=maxCameraIdx; i++)
        {
            // Enforce rounded camera position
            SMBX_CameraInfo::setCameraX(i, std::round(SMBX_CameraInfo::getCameraX(i)));
            SMBX_CameraInfo::setCameraY(i, std::round(SMBX_CameraInfo::getCameraY(i)));

            cameraPos[i][0] = SMBX_CameraInfo::getCameraX(i);
            cameraPos[i][1] = SMBX_CameraInfo::getCameraY(i);
        }

        Renderer::Get().StartFrameRender();
        g_EventHandler.hookLevelRenderFirstCameraStart();
    }

    // Send camera position to GLEngine
    SMBX_CameraInfo::setCameraX(cameraIdx, cameraPos[cameraIdx][0]);
    SMBX_CameraInfo::setCameraY(cameraIdx, cameraPos[cameraIdx][1]);
    Renderer::Get().StoreCameraPosition(cameraIdx);

    // Start camera render for this camera
    Renderer::Get().StartCameraRender(cameraIdx);

    if (gLunaLua.isValid()) {
        SMBX_CameraInfo cameraData;
        SMBX_CameraInfo *cameraPtr = SMBX_CameraInfo::Get(cameraIdx);
        memcpy(&cameraData, cameraPtr, sizeof(SMBX_CameraInfo));

        std::shared_ptr<Event> cameraDrawEvent = std::make_shared<Event>("onCameraDraw", false);
        cameraDrawEvent->setDirectEventName("onCameraDraw");
        cameraDrawEvent->setLoopable(false);
        gLunaLua.callEvent(cameraDrawEvent, cameraIdx, maxCameraIdx);

        // Disallow changes to this camera's settings in onCameraDraw, for reasons.
        memcpy(cameraPtr, &cameraData, sizeof(SMBX_CameraInfo));
    }
}

void __declspec(naked) __stdcall PostCameraUpdateHook_Wrapper()
{
    __asm {
        POP EAX                           // POP the return address
        PUSH DWORD PTR DS : [EBP - 0x3E0] // Sneak max camera index argument in there
        PUSH DWORD PTR DS : [EBP - 0x38]  // Sneak a camera index argument in there
        PUSH EAX                          // PUSH the return address
        JMP PostCameraUpdateHook          // JMP to PostCameraUpdateHook
    };
}

#include "../../FileManager/SMBXFileManager.h"

extern void __stdcall WorldHUDPrintTextController(VB6StrPtr* Text, short* fonttype, float* x, float* y)
{
    if (gSMBXHUDSettings.overworldHudState == WHUD_ALL){
        native_print(Text, fonttype, x, y);
    }
}

extern BOOL __stdcall WorldOverlayHUDBitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
    if (gSMBXHUDSettings.overworldHudState == WHUD_NONE)
        return -1;

    return BitBltHook(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}


extern BOOL __stdcall WorldIconsHUDBitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
    if (gSMBXHUDSettings.overworldHudState == WHUD_NONE || gSMBXHUDSettings.overworldHudState == WHUD_ONLY_OVERLAY)
        return -1;

    return BitBltHook(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}


extern short __stdcall WorldHUDIsOnCameraHook(unsigned int* camIndex, Momentum* momentumObj)
{
    if (gSMBXHUDSettings.overworldHudState == WHUD_NONE)
        return native_isOnCamera(camIndex, momentumObj);
    return native_isOnWCamera(camIndex, momentumObj);
}


extern void __stdcall GenerateScreenshotHook()
{
    //dbgboxA("Screeny!");
    GM_DO_SCREENSHOT = COMBOOL(false);
    // Fails --> Renderer freezes!
    /*
    g_GLEngine.TriggerScreenshot([](const BITMAPINFOHEADER* header, void* pData, HWND curHwnd){
        //::GenerateScreenshot(L"Test.png", *header, pData);
        return true;
    });
    */
}


LRESULT CALLBACK KeyHOOKProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    static WCHAR unicodeData[32] = { 0 };

    if (nCode != 0){
        return CallNextHookEx(KeyHookWnd, nCode, wParam, lParam);
    }

    bool repeated = (lParam & 0x80000000) != (lParam & 0x40000000);
    bool altPressed = ((lParam & 0x20000000) == 0x20000000);
    bool keyDown = ((lParam & 0x80000000) == 0x0);
    bool keyUp = ((lParam & 0x80000000) == 0x80000000);
    unsigned int virtKey = wParam;
    unsigned int scanCode = (lParam >> 16) & 0xFF;

    if (virtKey < 256)
    {
        gKeyState[virtKey] = keyDown ? 0x80 : 0x00;
        if (virtKey == VK_CAPITAL)
        {
            gKeyState[virtKey] |= GetKeyState(VK_CAPITAL) & 0x1;
        }
    }

    bool ctrlPressed = ((gKeyState[VK_CONTROL] & 0x80) != 0) && (virtKey != VK_CONTROL);
    bool plainPress = (!repeated) && (!altPressed) && (!ctrlPressed);

    if (keyDown && gMainWindowFocused) {
        // Notify game controller manager
        if (!repeated)
        {
            gLunaGameControllerManager.notifyKeyboardPress(virtKey);
        }

        if (gLunaLua.isValid() && !ctrlPressed) {
            std::shared_ptr<Event> keyboardPressEvent = std::make_shared<Event>("onKeyboardPress", false);

            int unicodeRet = ToUnicode(virtKey, scanCode, gKeyState, unicodeData, 32, 0);
            if (unicodeRet > 0)
            {
                std::string charStr = WStr2Str(std::wstring(unicodeData, unicodeRet));
                gLunaLua.callEvent(keyboardPressEvent, static_cast<int>(virtKey), repeated, charStr);
            }
            else
            {
                gLunaLua.callEvent(keyboardPressEvent, static_cast<int>(virtKey), repeated);
            }
        }

        if (virtKey == VK_F12)
        {
            if (plainPress && g_GLEngine.IsEnabled())
            {
                short screenshotSoundID = 12;
                native_playSFX(&screenshotSoundID);
                g_GLEngine.TriggerScreenshot([](HGLOBAL globalMem, const BITMAPINFOHEADER* header, void* pData, HWND curHwnd) {
                    std::wstring screenshotPath = gAppPathWCHAR + std::wstring(L"\\screenshots");
                    if (GetFileAttributesW(screenshotPath.c_str()) & INVALID_FILE_ATTRIBUTES) {
                        CreateDirectoryW(screenshotPath.c_str(), NULL);
                    }
                    screenshotPath += L"\\";
                    screenshotPath += Str2WStr(generateTimestampForFilename()) + std::wstring(L".png");

                    ::GenerateScreenshot(screenshotPath, *header, pData);
                    GlobalFree(globalMem);
                    return true;
                });
            }
            return 1;
        }
        if (virtKey == VK_F11)
        {
            if (plainPress && g_GLEngine.IsEnabled())
            {
                short gifRecSoundID = (g_GLEngine.GifRecorderToggle() ? 24 : 12);
                native_playSFX(&gifRecSoundID);
            }
            return 1;
        }
        if ((virtKey == VK_F4) && !altPressed)
        {
            if (plainPress && g_GLEngine.IsEnabled())
            {
                gGeneralConfig.setRendererUseLetterbox(!gGeneralConfig.getRendererUseLetterbox());
                gGeneralConfig.save();
            }
            return 1;
        }
        if ((virtKey == 0x56) && ctrlPressed)
        {
            // Ctrl-V
            if (OpenClipboard(nullptr) == 0)
            {
                // Couldn't open clipboard
                return 1;
            }

            // Get unicode text handle
            bool textIsUnicode = true;
            HANDLE hData = GetClipboardData(CF_UNICODETEXT);
            if (hData == nullptr)
            {
                // Couldn't get text handle, try non-unicode
                hData = GetClipboardData(CF_TEXT);
                textIsUnicode = false;
                if (hData == nullptr)
                {
                    // Couldn't get any text
                    CloseClipboard();
                    return 1;
                }
            }

            // Lock data
            void* dataPtr = GlobalLock(hData);
            if (dataPtr == nullptr)
            {
                // Couldn't get pointer
                GlobalUnlock(hData);
                CloseClipboard();
            }

            // Convert to std::string
            std::string pastedText = textIsUnicode ? WStr2Str(static_cast<const wchar_t*>(dataPtr)) : std::string(static_cast<const char*>(dataPtr));

            // Unlock Data and close clipboard
            GlobalUnlock(hData);
            CloseClipboard();

            // Call event
            if ((pastedText.length() > 0) && gLunaLua.isValid()) {
                std::shared_ptr<Event> pasteTextEvent = std::make_shared<Event>("onPasteText", false);
                pasteTextEvent->setDirectEventName("onPasteText");
                pasteTextEvent->setLoopable(false);
                gLunaLua.callEvent(pasteTextEvent, pastedText);
            }
        }

        if ((virtKey == VK_RETURN) && altPressed)
        {
            if (gMainWindowHwnd != NULL)
            {
                WINDOWPLACEMENT wndpl;
                wndpl.length = sizeof(WINDOWPLACEMENT);
                if (GetWindowPlacement(gMainWindowHwnd, &wndpl))
                {
                    if (wndpl.showCmd == SW_MAXIMIZE)
                    {
                        ShowWindow(gMainWindowHwnd, SW_RESTORE);
                    }
                    else
                    {
                        ShowWindow(gMainWindowHwnd, SW_MAXIMIZE);
                    }
                }
            }
            return 1;
        }
    } // keyDown

    // Hook print screen key
    if ((virtKey == VK_SNAPSHOT) && gMainWindowFocused)
    {
        if (g_GLEngine.IsEnabled())
        {
            g_GLEngine.TriggerScreenshot([](HGLOBAL globalMem, const BITMAPINFOHEADER* header, void* pData, HWND curHwnd) {
                GlobalUnlock(&globalMem);
                // Write to clipboard
                OpenClipboard(curHwnd);
                EmptyClipboard();
                SetClipboardData(CF_DIB, globalMem);
                CloseClipboard();
                return false;
            });
        }
        return 1;
    }

    return CallNextHookEx(KeyHookWnd, nCode, wParam, lParam);
}

extern WORD __stdcall IsNPCCollidesWithVeggiHook(WORD* npcIndex, WORD* objType) {
    NPCMOB* npcObj = ::NPC::Get(*npcIndex - 1);
    if (isVegetableNPC_ptr[npcObj->id]) {
        if (*objType == 6) {
            npcObj->killFlag = 6;
            return 0xFFFF; // Don't handle extra code
        }
        return 0xFFFF; // Handle extra veggi code
    }
    return 0; // Don't handle extra code
}

_declspec(naked) extern void IsNPCCollidesWithVeggiHook_Wrapper()
{
    __asm {
        PUSHF
        SUB ESP, 2
        PUSH EAX
        PUSH DWORD PTR DS : [EBP + 0xC] // objType
        PUSH DWORD PTR DS : [EBP + 0x8] // npcIndex
        CALL IsNPCCollidesWithVeggiHook
        MOV DX, AX
        XOR ECX, ECX
        POP EAX
        ADD ESP, 2
        POPF
        CMP DX, CX
        RET
    }
}

extern BOOL __stdcall HardcodedGraphicsBitBltHook(DWORD retAddr, HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
    HWND destHwnd = WindowFromDC(hdcDest);
    std::cout << "HWND of DEST: " << (DWORD)destHwnd << std::endl;
    return BitBltHook(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}
static void __declspec(naked) __stdcall RenderLevelReal()
{
    __asm {
        // Copy of the code we're overwriting with the hook, plus jump back where we belong
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x18
        PUSH 0x909296
        RET
    };
}

extern void __stdcall RenderLevelHook()
{
    if (TestModeIsLoadPending()) return;
    LunaLoadScreenKill();
    PerfTrackerState state(PerfTracker::PERF_DRAWING);
    Renderer::Get().StartFrameRender();
    g_EventHandler.hookLevelRenderStart();

    short oldRenderDoneCameraUpdate = g_renderDoneCameraUpdate;
    bool oldRanOnDrawThisFrame = g_ranOnDrawThisFrame;
    g_renderDoneCameraUpdate = 0;
    g_ranOnDrawThisFrame = false;
    
    RenderLevelReal();

    g_renderDoneCameraUpdate = oldRenderDoneCameraUpdate;
    g_ranOnDrawThisFrame = oldRanOnDrawThisFrame;

    if (g_GLEngine.IsEnabled() && !Renderer::IsAltThreadActive())
    {
        g_GLEngine.EndFrame(g_GLEngine.GetHDC());
    }
    g_EventHandler.hookLevelRenderEnd();
    Renderer::Get().EndFrameRender();
}

// Hook to restart the camera loop in RenderLevel to allow updating all cameras
// Reroute 6 byte JG at 0x90C64E to this
void __declspec(naked) __stdcall runtimeHookRestartCameraLoop()
{
    __asm {
        MOV AX, g_renderDoneCameraUpdate
        TEST AX, AX
        JNE runtimeHookRestartCameraLoopExitLoop
        MOV g_renderDoneCameraUpdate, -1
        PUSH 0x90C61D // Restart loop
        RET
    runtimeHookRestartCameraLoopExitLoop:
        PUSH 0x94D5CF // Exit loop
        RET
    };
}

// Hook to skip camera updates during the second repetition of the camera loop
// Replace 7 bytes at 0x90C762 with JMP to this
void __declspec(naked) __stdcall runtimeHookSkipCamera()
{
    __asm {
        MOV AX, g_renderDoneCameraUpdate
        TEST AX, AX
        JNE runtimeHookSkipCameraExit
        PUSH 0x90C769 // Exit to camera update code
        RET
    runtimeHookSkipCameraExit:
        PUSH 0x90D6FE // Jump to rendering code
        RET
    };
}

static void __declspec(naked) __stdcall RenderWorldReal()
{
    __asm {
        // Copy of the code we're overwriting with the hook, plus jump back where we belong
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x8FEB16
        RET
    };
}

extern void __stdcall RenderWorldHook()
{
    if (TestModeIsLoadPending()) return;
    LunaLoadScreenKill();
    PerfTrackerState state(PerfTracker::PERF_DRAWING);
    Renderer::Get().StartFrameRender();
    g_EventHandler.hookWorldRenderStart();
    RenderWorldReal();
    if (g_GLEngine.IsEnabled() && !Renderer::IsAltThreadActive())
    {
        g_GLEngine.EndFrame(g_GLEngine.GetHDC());
    }
    g_EventHandler.hookWorldRenderEnd();
    Renderer::Get().EndFrameRender();
}

static void runtimeHookSmbxChangeModeHook(void)
{
    while (gStartupSettings.currentlyWaitingForIPC)
    {
        WaitMessage();
        LunaDllWaitFrame(false);
    }

    // Handler for test mode if it's enabled
    testModeSmbxChangeModeHook();
}

__declspec(naked) void __stdcall runtimeHookSmbxChangeModeHookRaw(void)
{
    __asm {
        pushf
            push eax
            push ecx
            push edx
    }
    runtimeHookSmbxChangeModeHook();
    __asm {
        pop edx
            pop ecx
            pop eax
            popf
            or ebx, 0xFFFFFFFF
            cmp word ptr ds : [0xB2C620], bx
            ret
    }
}

_declspec(naked) void __stdcall loadLevel_OrigFunc(VB6StrPtr* filename)
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x08D8F46
        RET
    }
}

void __stdcall runtimeHookLoadLevel(VB6StrPtr* filename)
{
    // Shut down Lua stuff before level loading just in case
    gLunaLua.exitContext();

    if (testModeLoadLevelHook(filename))
    {
        // If handled by testModeLoadLevelHook, skip
    }
    else
    {
        if (gStartupSettings.oldLvlLoader)
        {
            loadLevel_OrigFunc(filename);
        }
        else
        {
            SMBXLevelFileBase base;
            base.ReadFile(static_cast<std::wstring>(*filename), getCurrentLevelData());
        }
    }
}

void __stdcall runtimeHookLoadLevelHeader(SMBX_Warp* warp, wchar_t* filename)
{
    static LevelData levelData;

    if ((filename == nullptr) || (*filename == L'\0'))
    {
        return;
    }

    FileFormats::CreateLevelData(levelData);
    std::wstring filePath(filename);

    if (!fileExists(filePath))
    {
        return;
    }

    // Check if path has slash, if not then invalid
    size_t findLastSlash = filePath.find_last_of(L"/\\");
    if (findLastSlash == std::wstring::npos)
    {
        return;
    }

    // Append missing extension
    size_t findLastDot = filePath.find_last_of(L".", findLastSlash);
    if (findLastDot == std::wstring::npos)
    {
        if (!hasSuffix(filePath, L".lvl") && !hasSuffix(filePath, L".lvlx"))
            filePath.append(L".lvl");
    }

    if (!FileFormats::OpenLevelFileHeader(utf8_encode(filePath), levelData))
    {
        return;
    }

    warp->unknown_8C = levelData.stars;
}

void __stdcall runtimeHookCloseWindow(void)
{
    if (TestModeCheckHideWindow())
    {
        // If handled by TestModeCheckHideWindow, skip
        return;
    }

    native_exitMainGame();
}

void __stdcall runtimeHookChangeResolution(void* arg1, void* arg2, void* arg3, void* arg4)
{
    if (!gGeneralConfig.getForceDisableFullscreen())
    {
        native_applyFullscreen(arg1, arg2, arg3, arg4);
    }
    return;
}

static void __stdcall runtimeHookSmbxCheckWindowed(void)
{
    if (gGeneralConfig.getForceDisableFullscreen())
    {
        GM_UNK_WINDOWED = COMBOOL(true);
    }
}
__declspec(naked) void __stdcall runtimeHookSmbxCheckWindowedRaw(void)
{
    __asm {
        pushf
        push eax
        push ecx
        push edx
    }
    runtimeHookSmbxCheckWindowed();
    __asm {
        pop edx
        pop ecx
        pop eax
        popf
        cmp word ptr ds : [0xB250D8], 0xFFFFFFFF
        ret
    }
}

static int __stdcall runtimeHookBlockBumpable(int id)
{
    return Blocks::GetBlockBumpable(id) ? -1 : 0;
}

__declspec(naked) void __stdcall runtimeHookBlockBumpableRaw(void)
{
    __asm {
        push eax
        push ecx
        push edx
        movsx eax, ax
        push eax // Args #1
        call runtimeHookBlockBumpable
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        push 0x9DB25D
        ret
        alternate_exit :
        pop edx
        pop ecx
        pop eax
        push 0x9DB240
        ret
    }
}

static int __stdcall runtimeHookNPCVulnerability(NPCMOB* npc, CollidersType *harmType, short* otherIdx)
{
    if ((npc == nullptr) || (harmType == nullptr) || (otherIdx == nullptr))
    {
        return 0;
    }

    if (NPC::GetVulnerableHarmTypes(npc->id) & (1UL << *harmType))
    {
        // If damage type is HARM_TYPE_NPC, respect nofireball/etc
        if (*harmType == HARM_TYPE_NPC)
        {
            NPCMOB* otherNpc = NPC::GetRaw(*otherIdx);
            if ((otherNpc != nullptr) && (otherNpc->id == NPCID_PLAYERFIREBALL) && npc_nofireball[npc->id])
            {
                return 0;
            }

            if ((otherNpc != nullptr) && (otherNpc->id == NPCID_PLAYERICEBALL) && npc_noiceball[npc->id])
            {
                return 0;
            }
        }

        // If friendly, immune to everything but despawn
        if ((npc->friendly != 0) && (*harmType != HARM_TYPE_OFFSCREEN))
        {
            return 0;
        }

        return -1;
    }

    return 0;
}

__declspec(naked) void __stdcall runtimeHookNPCVulnerabilityRaw(void)
{
    __asm {
        pushf
        push eax
        push ecx
        push edx
        push [ebp + 0x10] // Args #3
        push [ebp + 0xC] // Args #2
        push esi // Args #1
        call runtimeHookNPCVulnerability
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        popf
        mov eax, dword ptr ds : [0xB25D14]
        push 0xA28FE8
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popf
        mov edi, dword ptr ds : [ebp + 0xC]
        mov ax, word ptr ds : [edi]
        movsx edi, word ptr ds : [esi + 0xE2]
        push 0xA2FA6F
        ret
    }
}

static int __stdcall runtimeHookNPCSpinjumpSafe(NPCMOB* npc)
{
    if ((npc != nullptr) && NPC::GetSpinjumpSafe(npc->id))
    {
        return -1;
    }

    return 0;
}

__declspec(naked) void __stdcall runtimeHookNPCSpinjumpSafeRaw(void)
{
    __asm {
        pushf
        push eax
        push ecx
        push edx
        lea edx, dword ptr ds : [eax + edx * 8]
        push edx // Args #1
        call runtimeHookNPCSpinjumpSafe
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        popf
        push 0x9AA9EA
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popf
        push 0x9AA365
        ret
    }
}

static int __stdcall runtimeHookNPCNoWaterPhysics(NPCMOB* npc)
{
    if ((npc != nullptr) && NPC::GetNoWaterPhysics(npc->id))
    {
        return -1;
    }

    return 0;
}

__declspec(naked) void __stdcall runtimeHookNPCNoWaterPhysicsRaw(void)
{
    // 00A0A991 | 0F 8E 89 01 00 00        | jle foo3.A0AB20                         |
    // 00A0A997
    __asm {
        jle early_exit
        pushf
        push eax
        push ecx
        push edx
        push esi // Args #1
        call runtimeHookNPCNoWaterPhysics
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        popf
        push 0xA0A997
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popf
    early_exit :
        push 0xA0AB20
        ret
    }
}

static unsigned int __stdcall runtimeHookNPCWaterSplashAnim(NPCMOB* npc, short* effectID, Momentum* coor, float* effectFrame, short* npcID, short* showOnlyMask)
{
    native_runEffect(effectID, coor, effectFrame, npcID, showOnlyMask);

    // Return non-zero for ignoring water physics
    if ((npc != nullptr) && NPC::GetNoWaterPhysics(npc->id))
    {
        return -1;
    }

    // Return 0 for behaving as-normal
    return 0;
}

__declspec(naked) void __stdcall runtimeHookNPCWaterSplashAnimRaw(short* effectID, Momentum* coor, float* effectFrame, short* npcID, short* showOnlyMask)
{
    __asm {
        POP EAX
        PUSH ESI
        CALL runtimeHookNPCWaterSplashAnim
        CMP EAX, 0
        JNE skip_water_physics_exit
        PUSH 0xA0A700
        RET
    skip_water_physics_exit:
        PUSH 0xA0A822
        RET
    }
}

static int __stdcall runtimeHookNPCHarmlessGrab(NPCMOB* npc)
{
    if ((npc != nullptr) && NPC::GetHarmlessGrab(npc->id))
    {
        return -1;
    }

    return 0;
}

__declspec(naked) void __stdcall runtimeHookNPCHarmlessGrabRaw(void)
{
    __asm {
        push esi // Args #1
        call runtimeHookNPCHarmlessGrab
        cmp eax, 0
        jne alternate_exit
        push 0xA0BA20
        ret
    alternate_exit :
        push 0xA0C5AA
        ret
    }
}

static int __stdcall runtimeHookNPCHarmlessThrown(unsigned int npcIdx)
{
    NPCMOB* npc = NPC::GetRaw(npcIdx);
    if (npc != nullptr)
    {
        return NPC::GetHarmlessThrown(npc->id) ? -1 : 0;
    }
    return 0;
}

_declspec(naked) void __stdcall runtimeHookNPCHarmlessThrownRaw()
{
    __asm {
        je harmlessRet

        push eax
        push ecx
        push edx

        movsx eax, word ptr ds:[ebp-0x180]
        push eax // Arg #1
        call runtimeHookNPCHarmlessThrown
        cmp eax, 0
        jne harmlessRestoreRet

        pop edx
        pop ecx
        pop eax
        push 0xA181B3
        ret
    harmlessRestoreRet:
        pop edx
        pop ecx
        pop eax
    harmlessRet:
        push 0xA1BAD5
        ret
    }
}

static void __stdcall runtimeHookCheckInput(int playerIdx, KeyMap* keymap)
{
    if (playerIdx >= 0 && playerIdx <= 1)
    {
        gRawKeymap[playerIdx+2] = gRawKeymap[playerIdx]; // Update prev values
        gRawKeymap[playerIdx] = *keymap; // Set new values
    }
}

__declspec(naked) void __stdcall runtimeHookCheckInputRaw(void)
{
    __asm {
        pushf
        push eax
        push ecx
        push edx
        push ebx // Args #2 (keymap ptr)
        push esi // Args #1 (player idx)
        call runtimeHookCheckInput
        pop edx
        pop ecx
        pop eax
        popf
        or edi, 0xFFFFFFFF
        cmp word ptr ds : [ebx + 0x4], di
        push 0xA75080
        ret
    }
}


static void __stdcall runtimeHookSetHDC(HDC newHDC)
{
    GM_SCRN_HDC = newHDC;
}

__declspec(naked) void __stdcall runtimeHookSetHDCRaw(void)
{
    __asm {
        pushf
        push eax
        push ecx
        push edx
        push esi // Arg #1 (new HDC)
        call runtimeHookSetHDC
        pop edx
        pop ecx
        pop eax
        popf
        ret
    }
}

void __stdcall runtimeHookInitGameHDC(void)
{
    auto initGameHDC = (void(__stdcall *)(void)) (void*)0x94F680;
    initGameHDC();
}

void __stdcall runtimeHookInitGameWindow(void)
{
    auto initGameWindow = (void(__stdcall *)(void)) (void*)0x96AD80;
    initGameWindow();
}

void __stdcall runtimeHookLoadDefaultGraphics(void)
{
    bool initDone = false;
    if (!initDone)
    {
        // Get initial HDCs set up...
        ImageLoader::Run(true);
        initDone = true;
    }
}

static _declspec(naked) void __stdcall saveGame_OrigFunc()
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x8E47D6
        RET
    }
}

void __stdcall runtimeHookSaveGame()
{
    // Hook for saving the game
    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> saveGameEvent = std::make_shared<Event>("onSaveGame", false);
        saveGameEvent->setDirectEventName("onSaveGame");
        saveGameEvent->setLoopable(false);
        gLunaLua.callEvent(saveGameEvent);
    }

    saveGame_OrigFunc();
}

static _declspec(naked) void __stdcall cleanupLevel_OrigFunc()
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x8DC6E6
        RET
    }
}

void __stdcall runtimeHookCleanupLevel()
{
    // Shut down Lua stuff before level cleanup
    gLunaLua.exitContext();

    cleanupLevel_OrigFunc();
}

static _declspec(naked) void __stdcall exitMainGame_OrigFunc()
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x8D6BB6
        RET
    }
}

void __stdcall runtimeHookExitMainGame()
{
    exitMainGame_OrigFunc();
}

static _declspec(naked) void __stdcall loadWorld_OrigFunc(VB6StrPtr* filename)
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x8DF5B6
        RET
    }
}

void __stdcall runtimeHookLoadWorld(VB6StrPtr* filename)
{
    // This only occurs when first loading the episode...
    // this isn't repeated later on

    loadWorld_OrigFunc(filename);
}

static _declspec(naked) void __stdcall cleanupWorld_OrigFunc()
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x8E2E46
        RET
    }
}

void __stdcall runtimeHookCleanupWorld()
{
    // Shut down Lua stuff before world cleanup
    gLunaLua.exitContext();

    cleanupWorld_OrigFunc();
}

static const float runtimeHookPiranahDivByZeroConst = 512.0f;
static const float* runtimeHookPiranahDivByZeroConstPtr = &runtimeHookPiranahDivByZeroConst;
static _declspec(naked) void __stdcall runtimeHookPiranahDivByZeroTrigger()
{
    __asm {
        PUSH EAX
        FSTP ST(0)
        FCLEX
        MOV EAX, runtimeHookPiranahDivByZeroConstPtr
        FLD dword ptr[eax]
        POP EAX
        RET
    }
}

_declspec(naked) void __stdcall runtimeHookPiranahDivByZero()
{
    __asm {
        JNE runtimeHookPiranahDivByZeroTrigger
        RET
    }
}

static _declspec(naked) void __stdcall hitBlock_OrigFunc(unsigned short* blockIndex, short* fromUpSide, unsigned short* playerIdx)
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x9DA626
        RET
    }
}

void __stdcall runtimeHookHitBlock(unsigned short* blockIndex, short* fromUpSide, unsigned short* playerIdx)
{
    bool isCancelled = false;

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> blockHitEvent = std::make_shared<Event>("onBlockHit", true);
        blockHitEvent->setDirectEventName("onBlockHit");
        blockHitEvent->setLoopable(false);
        gLunaLua.callEvent(blockHitEvent, *blockIndex, *fromUpSide != 0, *playerIdx);
        isCancelled = blockHitEvent->native_cancelled();
    }

    if (!isCancelled)
    {
        hitBlock_OrigFunc(blockIndex, fromUpSide, playerIdx);
    }
}

static void __stdcall runtimeHookColorSwitch(unsigned int color)
{
    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> blockHitEvent = std::make_shared<Event>("onColorSwitch", false);
        blockHitEvent->setDirectEventName("onColorSwitch");
        blockHitEvent->setLoopable(false);
        gLunaLua.callEvent(blockHitEvent, color);
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchRedNpc(void)
{
    __asm {
        pushf
        sub esp, 2
        push eax
        push ecx
        push edx
        push 4 // RED
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        add esp, 2
        popf
        push 0xA32558
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchGreenNpc(void)
{
    __asm {
        pushf
        sub esp, 2
        push eax
        push ecx
        push edx
        push 3 // GREEN
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        add esp, 2
        popf
        push 0xA32558
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchBlueNpc(void)
{
    __asm {
        pushf
        sub esp, 2
        push eax
        push ecx
        push edx
        push 2 // BLUE
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        add esp, 2
        popf
        push 0xA32558
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchYellowNpc(void)
{
    __asm {
        pushf
        sub esp, 2
        push eax
        push ecx
        push edx
        push 1 // YELLOW
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        add esp, 2
        popf
        push 0xA32558
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchYellowBlock(void)
{
    __asm {
        pushf
        sub esp, 2
        push eax
        push ecx
        push edx
        push 1 // YELLOW
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        add esp, 2
        popf
        push 0x9DB424
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchBlueBlock(void)
{
    __asm {
        pushf
        sub esp, 2
        push eax
        push ecx
        push edx
        push 2 // BLUE
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        add esp, 2
        popf
        push 0x9DB5BF
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchGreenBlock(void)
{
    __asm {
        pushf
        sub esp, 2
        push eax
        push ecx
        push edx
        push 3 // GREEN
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        add esp, 2
        popf
        push 0x9DB75F
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchRedBlock(void)
{
    __asm {
        pushf
        sub esp, 2
        push eax
        push ecx
        push edx
        push 4 // RED
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        add esp, 2
        popf
        push 0x9DB8FA
        ret
    }
}

static void drawReplacementSplashScreen(void)
{
    // Get form to draw on
    HDC frmHDC = nullptr;
    uintptr_t mainFrm = *reinterpret_cast<uintptr_t*>(0xB25010);
    uintptr_t mainFrmClass = *reinterpret_cast<uintptr_t*>(mainFrm);
    auto frmGetHDC = (HRESULT(__stdcall *)(uintptr_t, HDC*)) *(void**)(mainFrmClass + 0xD8);
    frmGetHDC(mainFrm, &frmHDC);
    if (!frmHDC) return;

    // Load splash image
    std::shared_ptr<LunaImage> splashReplacement = LunaImage::fromFile(L"graphics/hardcoded/hardcoded-30-4.png");
    if (!splashReplacement) return;

    // Get image as HBITMAP
    HBITMAP splashBMP = splashReplacement->asHBITMAP();
    if (!splashBMP) return;

    // Generate HDC for drawing...
    HDC splashHDC = CreateCompatibleDC(frmHDC);
    if (!splashHDC) return;
    SelectObject(splashHDC, splashBMP);

    // Clear HDC...
    BitBlt(frmHDC, 0, 0, 800, 600, frmHDC, 0, 0, WHITENESS);

    // Draw with respecting alpha channel
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.AlphaFormat = AC_SRC_ALPHA;
    bf.SourceConstantAlpha = 0xff;
    AlphaBlend(frmHDC, 0, 0, 800, 600, splashHDC, 0, 0, 800, 600, bf);

    // Cleanup
    DeleteDC(splashHDC);
}

void __stdcall runtimeHookLoadDefaultControls(void)
{
    // Draw replacement splash screen if we have one
    drawReplacementSplashScreen();

    // Run the regular load default controls...
    native_loadDefaultControls();
}

static _declspec(naked) void __stdcall runtimeHookRunAnim_OrigFunc(short* effectID, Momentum* coor, float* effectFrame, short* npcID, short* showOnlyMask)
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x9E7386
        RET
    }
}

void __stdcall runtimeHookRunAnimInternal(short* effectID, Momentum* coor, float* effectFrame, short* npcID, short* showOnlyMask)
{
    bool isCancelled = false;

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> runEffectInternalEvent = std::make_shared<Event>("onRunEffectInternal", true);
        runEffectInternalEvent->setDirectEventName("onRunEffectInternal");
        runEffectInternalEvent->setLoopable(false);

        luabind::object coorCopy = gLunaLua.newTable();
        coorCopy["x"] = coor->x;
        coorCopy["y"] = coor->y;
        coorCopy["speedX"] = coor->speedX;
        coorCopy["speedY"] = coor->speedY;
        coorCopy["width"] = coor->width;
        coorCopy["height"] = coor->height;

        // onRunEffectInternal(eventObj, id, coords, variant, npcID, drawOnlyMask)
        gLunaLua.callEvent(runEffectInternalEvent, *effectID, coorCopy, *effectFrame, *npcID, ((*showOnlyMask) != 0));
        isCancelled = runEffectInternalEvent->native_cancelled();
    }

    if (!isCancelled)
    {
        runtimeHookRunAnim_OrigFunc(effectID, coor, effectFrame, npcID, showOnlyMask);
    }
}

static unsigned int __stdcall runtimeHookSemisolidInteractionHook(NPCMOB* npc)
{
    unsigned short npcID = npc->id;
    if (npc_isflying[npcID] != 0) {
        // Flying NPCs generally don't collide with semisolids... unless it's just a hop?
        return NPC::CheckSemisolidCollidingFlyType((unsigned int)npc->ai1) ? -1 : 0;
    }
    return -1; // Collides
}

_declspec(naked) void __stdcall runtimeHookSemisolidInteractionHook_Raw()
{
    __asm {
        MOVSX EDI, word ptr ds:[ESI+0xE2]
        PUSH ESI
        CALL runtimeHookSemisolidInteractionHook
        XOR EDX, EDX
        MOV ECX, EAX
        TEST EAX, EAX
        PUSH 0xA12095
        RET
    }
}

MMRESULT __stdcall runtimeHookJoyGetPosEx(UINT uJoyID, LPJOYINFOEX pji)
{
    return gLunaGameControllerManager.emulatedJoyGetPosEx(uJoyID, pji);
}

MMRESULT __stdcall runtimeHookJoyGetPosExNull(UINT uJoyID, LPJOYINFOEX pji)
{
    pji->dwXpos = 0x7FFF;
    pji->dwYpos = 0x7FFF;
    pji->dwPOV = 0x7FFF;
    pji->dwButtons = 0;
    pji->dwButtonNumber = 0;
    return JOYERR_NOERROR;
}

MMRESULT __stdcall runtimeHookJoyGetDevCapsA(UINT uJoyID, LPJOYCAPSA pjc, UINT cbjc)
{
    if (uJoyID < 2)
    {
        memset(pjc, 0, sizeof(cbjc));
        pjc->wXmin = 0;
        pjc->wXmax = 0xFFFF;
        pjc->wYmin = 0;
        pjc->wYmax = 0xFFFF;
        pjc->wZmin = 0;
        pjc->wZmax = 0xFFFF;
        pjc->wNumButtons = 32;
        pjc->wRmin = 0;
        pjc->wRmax = 0xFFFF;
        pjc->wUmin = 0;
        pjc->wUmax = 0xFFFF;
        pjc->wVmin = 0;
        pjc->wVmax = 0xFFFF;
        pjc->wNumAxes = 1;
        pjc->wMaxButtons = 32;
        return JOYERR_NOERROR;
    }
    return MMSYSERR_NODRIVER;
}

static _declspec(naked) void __stdcall runtimeHookDoExplosion_OrigFunc(Momentum* coor, short* bombType, short* playerIdx)
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0xA3BA96
        RET
    }
}

void __stdcall runtimeHookDoExplosionInternal(Momentum* coor, short* bombType, short* playerIdx)
{
    bool isCancelled = false;

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> onExplosionInternalEvent = std::make_shared<Event>("onExplosionInternal", true);
        onExplosionInternalEvent->setDirectEventName("onExplosionInternal");
        onExplosionInternalEvent->setLoopable(false);

        luabind::object coorCopy = gLunaLua.newTable();
        coorCopy["x"] = coor->x;
        coorCopy["y"] = coor->y;
        coorCopy["speedX"] = coor->speedX;
        coorCopy["speedY"] = coor->speedY;
        coorCopy["width"] = coor->width;
        coorCopy["height"] = coor->height;

        gLunaLua.callEvent(onExplosionInternalEvent, coorCopy, *bombType, *playerIdx);
        isCancelled = onExplosionInternalEvent->native_cancelled();
    }

    if (!isCancelled)
    {
        runtimeHookDoExplosion_OrigFunc(coor, bombType, playerIdx);
    }
}

static void __stdcall runtimeHookNpcGenerated_Main(short genIdx, short* npcIdx)
{
    native_updateNPC(npcIdx); // This hook overwrites this call, but let this happen first

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> onNPCGeneratedEvent = std::make_shared<Event>("onNPCGenerated", false);
        onNPCGeneratedEvent->setDirectEventName("onNPCGenerated");
        onNPCGeneratedEvent->setLoopable(false);

        gLunaLua.callEvent(onNPCGeneratedEvent, (int)genIdx, (int)*npcIdx);
    }
}

void __declspec(naked) __stdcall runtimeHookNpcGenerated(short* npcIdx)
{
    __asm {
        POP EAX                           // POP the return address
        PUSH DWORD PTR DS : [EBP - 0x180] // Source NPC argument argument in there
        PUSH EAX                          // PUSH the return address
        JMP runtimeHookNpcGenerated_Main  // JMP to CameraUpdateHook
    };
}
