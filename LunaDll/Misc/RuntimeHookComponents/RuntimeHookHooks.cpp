#include "../RuntimeHook.h"
#include "../../LuaMain/LunaLuaMain.h"
#include "../../LuaMain/LuaEvents.h"
#include <comutil.h>
#include "../../Input/Input.h"
#include "../../GlobalFuncs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../SdlMusic/MusicManager.h"
#include "../../HardcodedGraphics/HardcodedGraphicsManager.h"
#include "../ErrorReporter.h"

#include "../SHMemServer.h"

#include "../../Rendering/RenderOverrides.h"
#include "../../Rendering/GLEngine.h"
#include "../../Main.h"
#include "../../libs/ini-reader/INIReader.h"

#include "../RunningStat.h"
#include "../../Rendering/RenderOverrideManager.h"

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
#ifndef NO_SDL
    if (!episodeStarted)
    {
        std::string wldPath = wstr2str(GM_FULLDIR);
        MusicManager::loadCustomSounds(wldPath + "\\");
        episodeStarted = true;
    }
#endif

    gSkipSMBXHUD = false;
    gIsOverworld = true;
    gOverworldHudControlFlag = WHUD_ALL;
    gLunaRender.ClearAll();
    gSpriteMan.ResetSpriteManager();
    gCellMan.Reset();
    gSavedVarBank.ClearBank();
    Input::ResetAll();

    gLunaRender.ReloadScreenHDC();
    g_GLEngine.ClearSMBXSprites();

    // Init var bank
    gSavedVarBank.TryLoadWorldVars();
    gSavedVarBank.CheckSaveDeletion();
    gSavedVarBank.CopyBank(&gAutoMan.m_UserVars);

    gLunaLua = CLunaLua();
    gLunaLua.init(CLunaLua::LUNALUA_WORLD, (std::wstring)GM_FULLDIR);

    // Recount deaths
    gDeathCounter.Recount();

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
    /* TEST */
    /*if (!gIntroRun){
        //Load world list
        native_loadWorldList();
        //Slot selection
        GM_CUR_MENUTYPE = 10;
        //DEMO
        GM_CUR_MENUPLAYER1 = 1;
        //Second level
        GM_CUR_MENULEVEL = 2;
        //Load save states
        native_loadSaveStates();

        *(WORD*)0xB2D6D4 = -1;
    }*/
    /* TEST END */
    std::string autostartFile = utf8_encode(getModulePath()) + "\\autostart.ini";

    if (!file_existsX(autostartFile))
        return GetTickCount();

    INIReader autostartConfig(autostartFile);
    if (autostartConfig.GetBoolean("autostart", "do-autostart", false)){
        if (!gAutostartRan){
            GameAutostart autostarter = GameAutostart::createGameAutostartByIniConfig(autostartConfig);
            autostarter.applyAutostart();
            gAutostartRan = true;
        }
        
    }

    if (GameAutostartConfig::nextAutostartConfig) {
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

    gLunaLua.doEvents();

    gSavedVarBank.SaveIfNeeded();

#pragma warning(suppress: 28159)
    return GetTickCount();
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
    Render::Print(utf8_decode(LUNALUA_VERSION), 3, 5, 5);
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
    if (gShowDemoCounter)
        gDeathCounter.Draw();

    if (gLunaLua.isValid()) {
        Event inputEvent("onHUDDraw", false);
        inputEvent.setDirectEventName("onHUDDraw");
        inputEvent.setLoopable(false);
        gLunaLua.callEvent(&inputEvent);
    }

    gSpriteMan.RunSprites();
    gLunaRender.RenderAll();

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
        stringstream q;
        q << variant->vt << " ";
        if (variant->vt == VT_R8)
        {
            q << "src:" << variant->dblVal << " dst:" << static_cast<float>(variant->dblVal);
        }
        string rr("vbaR4VarHook type:" + q.str() + ";");
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


extern int __stdcall __vbaStrCmp_TriggerSMBXEventHook(BSTR nullStr, BSTR eventName)
{

    int(__stdcall *origCmp)(BSTR, BSTR) = (int(__stdcall *)(BSTR, BSTR))IMP_vbaStrCmp;
    Event TriggerEventData("onEvent", true);
    gLunaLua.callEvent(&TriggerEventData, utf8_encode(eventName));
    if (TriggerEventData.native_cancelled())
        return 0;
    return origCmp(nullStr, eventName);

}

extern void __stdcall checkLevelShutdown()
{
    if (GM_EPISODE_MODE || GM_LEVEL_MODE){
        if (gLunaLua.isValid()){
            Event shutdownEvent("onExitLevel", false);
            shutdownEvent.setDirectEventName("onExitLevel");
            shutdownEvent.setLoopable(false);
            gLunaLua.callEvent(&shutdownEvent);
            gLunaLua.shutdown();

            //Clean & stop all user started sounds and musics
            PGE_MusPlayer::MUS_stopMusic();
            PGE_Sounds::clearSoundBuffer();
        }
        g_GLEngine.ClearSMBXSprites();
    }

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
    native_loadLocalGfx();

    // Load render override graphics
    gRenderOverride.loadLevelGFX();
}


extern BOOL __stdcall BitBltHook(
    HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop
    )
{
    // Only override if the BitBlt is for the screen
    bool skipRendering = false;
    if (hdcDest == (HDC)GM_SCRN_HDC)
    {
        skipRendering = gRenderOverride.renderOverrideBitBlt(nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc);

        if (!skipRendering){
            if (g_GLEngine.IsEnabled()) {
                g_GLEngine.EmulatedBitBlt(nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
                return -1;
            }
        }
    }

    if (skipRendering)
        return -1;

    return BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}

extern BOOL __stdcall StretchBltHook(
    HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
    HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
    DWORD dwRop
    )
{
    static uint8_t callCount = 0;

    // If we're copying from our rendering screen, we're done with the frame
    if (hdcSrc == (HDC)GM_SCRN_HDC && g_GLEngine.IsEnabled())
    {
        g_GLEngine.EmulatedStretchBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, dwRop);

        // Heuristic for the last StretchBlt of the frame
        if ((nWidthSrc == 800 && nHeightSrc == 600) || (callCount == 1))
        {
            g_GLEngine.EndFrame(hdcDest);
            callCount = 0;
        }
        else
        {
            callCount++;
        }

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
    if (gLunaLua.isValid()){
        Event inputEvent("onInputUpdate", false);
        inputEvent.setDirectEventName("onInputUpdate");
        inputEvent.setLoopable(false);
        gLunaLua.callEvent(&inputEvent);
    }
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

extern void __stdcall FrameTimingHookQPC()
{
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
    }

    // Get the desired duration for this frame
    frameDuration = FRAME_TIMING_MS - frameError * 0.5;

    QueryPerformanceCounter(&currentTime);
    frameTime = (currentTime.QuadPart - lastFrameTime) * qpcFactor;
    if (lastFrameTime == 0 || frameTime > 100.0) {
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
    frameError = frameError * 0.5 + frameTime - frameDuration;

#if defined(ENABLE_FRAME_TIMING_BENCHMARK)
    static RunningStat sFrameTime;
    if (sFrameTime.Count() > 65 * 10) sFrameTime.Clear();
    if (frameTime < 200.0)
    {
        sFrameTime.Push(frameTime);
    }
    gLunaRender.SafePrint(utf8_decode(sFrameTime.Str()), 3, 5, 5);
#endif

    if (frameError > 5.0) frameError = 5.0;
    if (frameError < -5.0) frameError = -5.0;
    lastFrameTime = currentTime.QuadPart;
}


extern void __stdcall FrameTimingMaxFPSHookQPC()
{
    // If we're in "max FPS" mode (either via cheat code or editor menu), bypass frame timing
    if (GM_MAX_FPS_MODE) return;

    // If we're not in "max FPS" mode, run the frame timing as normal
    FrameTimingHookQPC();
}

extern void __stdcall FrameTimingHook()
{
    static double lastFrameTime = 0.0;
    double nextFrameTime = lastFrameTime;
    static double frameError = 0.0;

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
    gLunaRender.SafePrint(utf8_decode(sFrameTime.Str()), 3, 5, 5);
#endif
}

extern void __stdcall FrameTimingMaxFPSHook()
{
    // If we're in "max FPS" mode (either via cheat code or editor menu), bypass frame timing
    if (GM_MAX_FPS_MODE) return;

    // If we're not in "max FPS" mode, run the frame timing as normal
    FrameTimingHook();
}

extern short __stdcall MessageBoxOpenHook()
{
    // A note here: If the message is set, then the message box will called
    // However, if a message is not set, then this function is called when the menu opens.
    if (GM_STR_MSGBOX){
        if (GM_STR_MSGBOX.length() > 0){
            if (gLunaLua.isValid()){
                Event messageBoxEvent("onMessageBox", false);
                messageBoxEvent.setDirectEventName("onMessageBox");
                messageBoxEvent.setLoopable(false);
                gLunaLua.callEvent(&messageBoxEvent, (std::string)GM_STR_MSGBOX);
            }
        }
    }
    
    return (short)GM_PLAYERS_COUNT;
}


extern void __stdcall WorldHUDPrintTextController(VB6StrPtr* Text, short* fonttype, float* x, float* y)
{
    if (gOverworldHudControlFlag == WHUD_ALL){
        native_print(Text, fonttype, x, y);
    }
}

extern BOOL __stdcall WorldOverlayHUDBitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
    if (gOverworldHudControlFlag == WHUD_NONE)
        return -1;

    return BitBltHook(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}


extern BOOL __stdcall WorldIconsHUDBitBltHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
    if (gOverworldHudControlFlag == WHUD_NONE || gOverworldHudControlFlag == WHUD_ONLY_OVERLAY)
        return -1;

    return BitBltHook(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}


extern short __stdcall WorldHUDIsOnCameraHook(unsigned int* camIndex, Momentum* momentumObj)
{
    if (gOverworldHudControlFlag == WHUD_NONE)
        return native_isOnCamera(camIndex, momentumObj);
    return native_isOnWCamera(camIndex, momentumObj);
}

extern HHOOK KeyHookWnd;
LRESULT CALLBACK KeyHOOKProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0){
        return CallNextHookEx(KeyHookWnd, nCode, wParam, lParam);
    }

    // Hook print screen key
    if (wParam == VK_SNAPSHOT && g_GLEngine.IsEnabled())
    {
        g_GLEngine.TriggerScreenshot();
        return 1;
    }

    return CallNextHookEx(KeyHookWnd, nCode, wParam, lParam);
}
