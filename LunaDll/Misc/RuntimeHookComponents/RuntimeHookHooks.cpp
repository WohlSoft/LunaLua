#include <comutil.h>
#include "../../Misc/CollisionMatrix.h"
#include "string.h"
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

#include "../AsmPatch.h"

#include "../../Rendering/GL/GLEngine.h"
#include "../../Main.h"
#include <IniProcessor/ini_processing.h>

#include "../RunningStat.h"
#include "../../Rendering/BitBltEmulation.h"
#include "../../Rendering/RenderUtils.h"
#include "../../Rendering/RenderOps/RenderStringOp.h"
#include "../../Rendering/WindowSizeHandler.h"

#include "../../SMBXInternal/NPCs.h"
#include "../../SMBXInternal/Blocks.h"
#include "../../SMBXInternal/Level.h"
#include "../../SMBXInternal/Sound.h"

#include "../PerfTracker.h"

#include "../../Misc/TestMode.h"
#include "../../Misc/WaitForTickEnd.h"
#include "../../Rendering/ImageLoader.h"
#include "../../Misc/LoadScreen.h"

#include "../../SMBXInternal/HardcodedGraphicsAccess.h"
#include "../../Rendering/LunaImage.h"

#include "../../libs/PGE_File_Formats/file_formats.h"

#include "../../Misc/VB6RNG.h"

void CheckIPCQuitRequest();

extern HHOOK HookWnd;
extern HHOOK KeyHookWnd;

// compared against between onWarpEnter and onWarp hooks -
// used to detect player changing sections
int warpHookRefSection;

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
        // Start deferring music
        g_EventHandler.loadTimeDeferMusic();

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

    if (!gAutostartRan && !gStartupSettings.waitForIPC && !TestModeIsEnabled())
    {
        gAutostartRan = true;

        if (gStartupSettings.epSettings.enabled)
        {
            GameAutostart autostarter = GameAutostart::createGameAutostartByStartupEpisodeSettings(gStartupSettings.epSettings);
            autostarter.applyAutostart();
        }
        else
        {
            std::string autostartFile = WStr2Str(getLatestConfigFile(L"autostart.ini"));
            if (file_existsX(autostartFile))
            {
                IniProcessing autostartConfig(autostartFile);
                if (autostartConfig.beginGroup("autostart"))
                {
                    bool doAutostart = autostartConfig.value("do-autostart", false).toBool();
                    autostartConfig.endGroup();
                    if (doAutostart)
                    {
                        // Note: Internally this uses beginGroup and endGroup, so the group won't be open after it
                        GameAutostart autostarter = GameAutostart::createGameAutostartByIniConfig(autostartConfig);
                        autostarter.applyAutostart();

                        autostartConfig.beginGroup("autostart");
                        if (autostartConfig.value("transient", false).toBool())
                        {
                            remove(autostartFile.c_str());
                        }
                        autostartConfig.endGroup();
                    }
                }
                autostartConfig.endGroup();
            }
        }
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
        short newIdx = npcIdx - 1;    // 0 based not including dummy
        short oldIdx = GM_NPCS_COUNT; // 0 based not including dummy

        // Decrement the reference count of the removed NPC's collision group
        gCollisionMatrix.decrementReferenceCount(NPC::GetRawExtended(newIdx+1)->collisionGroup);

        // Update extended NPC fields
        if (newIdx != oldIdx)
        {
            *NPC::GetRawExtended(newIdx+1) = *NPC::GetRawExtended(oldIdx+1);
        }
        NPC::GetRawExtended(oldIdx+1)->Reset();

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

    if (wcscmp(eventName, L"Level - Start") == 0)
    {
        // Trigger onStart here to ensure it happens just before the "Level - Start" event
        gLunaLua.triggerOnStart();

        // Mark next render frame as the 'first'
        g_GLEngine.SetFirstFramePending();
    }

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

        int cameraIdx = Renderer::Get().GetCameraIdx();
        SMBX_CameraInfo* cam = SMBX_CameraInfo::Get(cameraIdx);
        if (cam == NULL) return FALSE;

        int xOff = 0;
        int yOff = 0;
        if (GM_EARTHQUAKE)
        {
            xOff = static_cast<int>(VB6RNG::generateNumber() * GM_EARTHQUAKE * 4) - GM_EARTHQUAKE * 2;
            yOff = static_cast<int>(VB6RNG::generateNumber() * GM_EARTHQUAKE * 4) - GM_EARTHQUAKE * 2;
        }

        g_GLEngine.RenderCameraToScreen(cameraIdx, cam->renderX + xOff, cam->renderY + yOff, cam->width, cam->height);

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

static void __stdcall UpdateInputFinishHook()
{
    //https://github.com/smbx/smbx-legacy-source/blob/4a7ff946da8924d2268f6ee8d824034f3a7d7658/modPlayer.bas#L5959
    int playerCount = GM_PLAYERS_COUNT;
    if (playerCount > 2 && GM_LEVEL_MODE == 0 && GM_WINNING == 0 && GM_UNK_IS_CONNECTED == 0) {
        for (int playerIdx = 2; playerIdx <= playerCount; playerIdx++) {
            Player::Get(playerIdx)->keymap = Player::Get(1)->keymap;
        }
    }

    g_EventHandler.hookInputUpdate();
}

__declspec(naked) void UpdateInputFinishHook_Wrapper()
{
    __asm {
        MOV EBX, 1
        JMP UpdateInputFinishHook
    }
}

static __declspec(naked) void updateInput_Orig()
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP,8
        PUSH 0xA74916
        RET
    }
}

extern void __stdcall runtimeHookUpdateInput()
{
    gLunaGameControllerManager.pollInputs();
    gEscPressedRegistered = gEscPressed;
    gEscPressed = false;
    updateInput_Orig();
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
static const double DEFAULT_FRAME_TIMING_MS = 15.600;
static double FRAME_TIMING_MS = DEFAULT_FRAME_TIMING_MS;
extern void SetSMBXFrameTiming(double ms)
{
    FRAME_TIMING_MS = ms;
}

extern void SetSMBXFrameTimingDefault()
{
    SetSMBXFrameTiming(DEFAULT_FRAME_TIMING_MS);
}

extern double GetSMBXFrameTiming()
{
    return FRAME_TIMING_MS;
}

bool g_ResetFrameTiming = false;

extern void __stdcall FrameTimingHookQPC()
{
    CheckIPCQuitRequest();
    TestModeCheckPendingIPCRequest();
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
        CheckIPCQuitRequest();
        TestModeCheckPendingIPCRequest();
        return;
    }

    // If we're not in "max FPS" mode, run the frame timing as normal
    FrameTimingHookQPC();
}

extern void __stdcall FrameTimingHook()
{
    CheckIPCQuitRequest();
    TestModeCheckPendingIPCRequest();
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
        CheckIPCQuitRequest();
        TestModeCheckPendingIPCRequest();
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

static _declspec(naked) void __stdcall msgbox_OrigFunc(short* pPlayerIdx)
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x8E54C6
        RET
    }
}

void __stdcall runtimeHookMsgbox(short* pPlayerIdx)
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
        Renderer::QueueStateStacker renderStack;
        msgbox_OrigFunc(pPlayerIdx);
    }
}

static void __stdcall runtimeHookNpcMsgbox(unsigned int npcIdxWithOffset, short* pPlayerIdx)
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

_declspec(naked) void __stdcall runtimeHookNpcMsgbox_Wrapper(short* pPlayerIdx)
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
        short oldRenderDoneCameraUpdate = g_renderDoneCameraUpdate;
        std::shared_ptr<Event> cameraUpdateEvent = std::make_shared<Event>("onCameraUpdate", false);
        cameraUpdateEvent->setDirectEventName("onCameraUpdate");
        cameraUpdateEvent->setLoopable(false);
        gLunaLua.callEvent(cameraUpdateEvent, cameraIdx);
        g_renderDoneCameraUpdate = oldRenderDoneCameraUpdate;
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

        // Clear primary framebuffer
        if (g_GLEngine.IsEnabled())
        {
            std::shared_ptr<GLEngineCmd_SetCamera> cmd = std::make_shared<GLEngineCmd_SetCamera>();
            cmd->mIdx = 0;
            cmd->mX = 0;
            cmd->mY = 0;
            g_GLEngine.QueueCmd(cmd);
        }

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
        short oldRenderDoneCameraUpdate = g_renderDoneCameraUpdate;
        g_EventHandler.hookLevelRenderFirstCameraStart();
        g_renderDoneCameraUpdate = oldRenderDoneCameraUpdate;
    }

    // Send camera position to GLEngine
    SMBX_CameraInfo::setCameraX(cameraIdx, cameraPos[cameraIdx][0]);
    SMBX_CameraInfo::setCameraY(cameraIdx, cameraPos[cameraIdx][1]);
    if (g_GLEngine.IsEnabled())
    {
        std::shared_ptr<GLEngineCmd_SetCamera> cmd = std::make_shared<GLEngineCmd_SetCamera>();
        cmd->mIdx = cameraIdx;
        cmd->mX = SMBX_CameraInfo::getCameraX(cameraIdx);
        cmd->mY = SMBX_CameraInfo::getCameraY(cameraIdx);
        g_GLEngine.QueueCmd(cmd);
    }

    // Start camera render for this camera
    Renderer::Get().StartCameraRender(cameraIdx);

    if (gLunaLua.isValid()) {
        SMBX_CameraInfo cameraData;
        SMBX_CameraInfo *cameraPtr = SMBX_CameraInfo::Get(cameraIdx);
        memcpy(&cameraData, cameraPtr, sizeof(SMBX_CameraInfo));

        short oldRenderDoneCameraUpdate = g_renderDoneCameraUpdate;

        std::shared_ptr<Event> cameraDrawEvent = std::make_shared<Event>("onCameraDraw", false);
        cameraDrawEvent->setDirectEventName("onCameraDraw");
        cameraDrawEvent->setLoopable(false);
        gLunaLua.callEvent(cameraDrawEvent, cameraIdx, maxCameraIdx);

        g_renderDoneCameraUpdate = oldRenderDoneCameraUpdate;

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

extern WORD __stdcall IsNPCCollidesWithVeggiHook(WORD* npcIndex, WORD* objType) {
    NPCMOB* npcObj = ::NPC::Get(*npcIndex - 1);
    if (npcObj && (npcObj->id >= 0) && (npcObj->id <= NPC::MAX_ID) && isVegetableNPC_ptr[npcObj->id]) {
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
        PUSHFD
        PUSH EAX
        PUSH DWORD PTR DS : [EBP + 0xC] // objType
        PUSH DWORD PTR DS : [EBP + 0x8] // npcIndex
        CALL IsNPCCollidesWithVeggiHook
        MOV DX, AX
        XOR ECX, ECX
        POP EAX
        POPFD
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
    g_renderDoneCameraUpdate = 0;
    RenderLevelReal();
    g_renderDoneCameraUpdate = oldRenderDoneCameraUpdate;

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
        MOV g_ranOnDrawThisFrame, 0
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
    if (g_GLEngine.IsEnabled() && !Renderer::IsAltThreadActive())
    {
        // Set camera 0 (primary framebuffer)
        std::shared_ptr<GLEngineCmd_SetCamera> cmd = std::make_shared<GLEngineCmd_SetCamera>();
        cmd->mIdx = 0;
        cmd->mX = 0;
        cmd->mY = 0;
        g_GLEngine.QueueCmd(cmd);
    }
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
        Sleep(100);
        LunaDllWaitFrame(false);
    }

    // Handler for test mode if it's enabled
    testModeSmbxChangeModeHook();
}

__declspec(naked) void __stdcall runtimeHookSmbxChangeModeHookRaw(void)
{
    __asm {
        pushfd
        push eax
        push ecx
        push edx
        call runtimeHookSmbxChangeModeHook
        pop edx
        pop ecx
        pop eax
        popfd
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

Characters playerStoredCharacters[] = {CHARACTER_MARIO,CHARACTER_MARIO,CHARACTER_MARIO,CHARACTER_MARIO };

void __stdcall runtimeHookLoadLevel(VB6StrPtr* filename)
{
    if (!GM_CREDITS_MODE)
    {
        for (int i = 1; i <= min(GM_PLAYERS_COUNT, (WORD)4); i++) {
            // store player characters at the time of level load,
            // these are used to restore the character if the episode has to be reloaded
            playerStoredCharacters[i-1] = Player::Get(i)->Identity;
        }
    }
    
    // Shut down Lua stuff before level loading just in case
    gLunaLua.exitContext();
    gCachedFileMetadata.purge();

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
    native_applyFullscreen(arg1, arg2, arg3, arg4);
}

static void __stdcall runtimeHookSmbxCheckWindowed(void)
{
    GM_UNK_WINDOWED = COMBOOL(true);
}
__declspec(naked) void __stdcall runtimeHookSmbxCheckWindowedRaw(void)
{
    __asm {
        pushfd
        push eax
        push ecx
        push edx
        call runtimeHookSmbxCheckWindowed
        pop edx
        pop ecx
        pop eax
        popfd
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
        pushfd
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
        popfd
        mov eax, dword ptr ds : [0xB25D14]
        push 0xA28FE8
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popfd
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
        pushfd
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
        popfd
        push 0x9AA9EA
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popfd
        push 0x9AA365
        ret
    }
}

static int __stdcall runtimeHookNPCNoWaterPhysics(unsigned int npcIdx)
{
    NPCMOB* npc = NPC::GetRaw(npcIdx);
    ExtendedNPCFields* ext = NPC::GetRawExtended(npcIdx);
    if (npc != nullptr)
    {
        if (NPC::GetNoWaterPhysics(npc->id))
        {
            return -1;
        }

        if (ext->noblockcollision)
        {
            return -1;
        }
    }

    return 0;
}

__declspec(naked) void __stdcall runtimeHookNPCNoWaterPhysicsRaw(void)
{
    // 00A0A991 | 0F 8E 89 01 00 00        | jle foo3.A0AB20                         |
    // 00A0A997
    __asm {
        jle early_exit
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr ss:[ebp-0x180] // Args #1
        call runtimeHookNPCNoWaterPhysics
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xA0A997
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popfd
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

static void __stdcall runtimeHookNPCTerminalVelocity(NPCMOB* npc)
{
    // Reimplement the NPC terminal velocity behaviour
    double terminalVelocity = NPC::GetTerminalVelocity(npc->id);

    if ((terminalVelocity >= 0) && (npc->momentum.speedY > terminalVelocity))
    {
        npc->momentum.speedY = terminalVelocity;
    }
}

_declspec(naked) void __stdcall runtimeHookNPCTerminalVelocityRaw()
{
    __asm {
        push esi // Arg #1 (pointer to NPC)

        call runtimeHookNPCTerminalVelocity

        push 0xA10170
        ret
    }
}

static void __stdcall runtimeHookCheckInput(int playerNum, int playerIdx, KeyMap* keymap)
{
    // Test that player index is in range, and that it matches the true player number (ignore clones)
    if ((playerIdx >= 0 && playerIdx <= 1) && ((playerIdx + 1) == playerNum))
    {
        gRawKeymap[playerIdx+2] = gRawKeymap[playerIdx]; // Update prev values
        gRawKeymap[playerIdx] = *keymap; // Set new values
    }
}

__declspec(naked) void __stdcall runtimeHookCheckInputRaw(void)
{
    // TODO: The loop this hook runs inside seems to never use the true player number? Ths loop should probably be limited to player 1 and 2...
    __asm {
        pushfd
        push eax
        push ecx
        push edx
        push ebx                     // Args #3 (keymap ptr)
        push esi                     // Args #2 (player idx we're reading for)
        push dword ptr ss:[ebp-0x18] // Args #1 (true player number)
        call runtimeHookCheckInput
        pop edx
        pop ecx
        pop eax
        popfd
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
        pushfd
        push eax
        push ecx
        push edx
        push esi // Arg #1 (new HDC)
        call runtimeHookSetHDC
        pop edx
        pop ecx
        pop eax
        popfd
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
    // This is for changing fullscreen in the way that 1.3 handled it.
    // We don't want to be using this code, because it just causes problems.
    // Maximizing/restoring the window does plenty well for full-screen handling.
    //auto initGameWindow = (void(__stdcall *)(void)) (void*)0x96AD80;
    //initGameWindow();
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
    gCachedFileMetadata.purge();

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

    // Clear the autostart patch at this point
    GameAutostart::ClearAutostartPatch();

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
    gCachedFileMetadata.purge();

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
        JNE runtimeHookPiranahDivByZeroTriggerLabel
        RET
    runtimeHookPiranahDivByZeroTriggerLabel:
        JMP runtimeHookPiranahDivByZeroTrigger
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

static _declspec(naked) void __stdcall removeBlock_OrigFunc(unsigned short* blockIndex, short* makeEffects)
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x9E0D56
        RET
    }
}

void __stdcall runtimeHookRemoveBlock(unsigned short* blockIndex, short* makeEffects)
{
    bool isCancelled = false;

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> blockRemoveEvent = std::make_shared<Event>("onBlockRemove", true);
        blockRemoveEvent->setDirectEventName("onBlockRemove");
        blockRemoveEvent->setLoopable(false);
        gLunaLua.callEvent(blockRemoveEvent, *blockIndex, *makeEffects != 0, false);
        isCancelled = blockRemoveEvent->native_cancelled();
    }

    if (!isCancelled)
    {
        removeBlock_OrigFunc(blockIndex, makeEffects);
    }
}


static _declspec(naked) void __stdcall doPOW_OrigFunc()
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0x9E4606
        RET
    }
}

void __stdcall runtimeHookPOW()
{
    bool isCancelled = false;

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> POWEvent = std::make_shared<Event>("onPOW", true);
        POWEvent->setDirectEventName("onPOW");
        POWEvent->setLoopable(false);
        gLunaLua.callEvent(POWEvent);
        isCancelled = POWEvent->native_cancelled();
    }

    if (!isCancelled)
    {
        doPOW_OrigFunc();
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
        pushfd
        push eax
        push ecx
        push edx
        push 4 // RED
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xA32558
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchGreenNpc(void)
{
    __asm {
        pushfd
        push eax
        push ecx
        push edx
        push 3 // GREEN
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xA32558
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchBlueNpc(void)
{
    __asm {
        pushfd
        push eax
        push ecx
        push edx
        push 2 // BLUE
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xA32558
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchYellowNpc(void)
{
    __asm {
        pushfd
        push eax
        push ecx
        push edx
        push 1 // YELLOW
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xA32558
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchYellowBlock(void)
{
    __asm {
        pushfd
        push eax
        push ecx
        push edx
        push 1 // YELLOW
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        popfd
        push 0x9DB424
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchBlueBlock(void)
{
    __asm {
        pushfd
        push eax
        push ecx
        push edx
        push 2 // BLUE
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        popfd
        push 0x9DB5BF
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchGreenBlock(void)
{
    __asm {
        pushfd
        push eax
        push ecx
        push edx
        push 3 // GREEN
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        popfd
        push 0x9DB75F
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookColorSwitchRedBlock(void)
{
    __asm {
        pushfd
        push eax
        push ecx
        push edx
        push 4 // RED
        call runtimeHookColorSwitch
        pop edx
        pop ecx
        pop eax
        popfd
        push 0x9DB8FA
        ret
    }
}


static _declspec(naked) void __stdcall collectNPC_OrigFunc(short* playerIdx, short* npcIdx)
{
    __asm {
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x8
        PUSH 0xA24CD6
        RET
    }
}

void __stdcall runtimeHookCollectNPC(short* playerIdx, short* npcIdx)
{
    PlayerMOB* player = Player::Get(*playerIdx);
    NPCMOB* npc = NPC::GetRaw(*npcIdx);

    // Duplicate of logic in TouchBonus
    if (npc->cantHurtPlayerIndex == *playerIdx && !(isCoin_ptr[npc->id] && player->HeldNPCIndex != *npcIdx && npc->killFlag == 0))
        return;

    // Obscure case of touching a fairy pendant in a clown car.
    // This is the only case outside of what's above where the NPC won't die.
    if (npc->id == 254 && player->MountType == 2)
        return;

    // Call onNPCCollect
    bool isCancelled = false;

    if (gLunaLua.isValid())
    {
        std::shared_ptr<Event> npcCollectEvent = std::make_shared<Event>("onNPCCollect", true);
        npcCollectEvent->setDirectEventName("onNPCCollect");
        npcCollectEvent->setLoopable(false);
        gLunaLua.callEvent(npcCollectEvent, *npcIdx, *playerIdx);
        
        if (npcCollectEvent->native_cancelled())
            return;
    }

    collectNPC_OrigFunc(playerIdx, npcIdx);
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

    // Get window size
    auto windowSize = gWindowSizeHandler.getWindowSize();

    // Clear HDC...
    BitBlt(frmHDC, 0, 0, windowSize.x, windowSize.y, frmHDC, 0, 0, WHITENESS);

    // Draw with respecting alpha channel
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.AlphaFormat = AC_SRC_ALPHA;
    bf.SourceConstantAlpha = 0xff;
    AlphaBlend(frmHDC, 0, 0, windowSize.x, windowSize.y, splashHDC, 0, 0, 800, 600, bf);

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

// checks if the block EDI is a semisolid BUT not a slope
// this patch is right above runtimeHookSemisolidInteractionHook in the assmebly
_declspec(naked) void __stdcall runtimeHookNPCSemisolidSlopeCollisionHook()
{
    __asm {
        xor ecx, ecx

        // check if slope
        mov edx, dword ptr ds:[0x00B2B94C] // blockdef_floorslope_ADDR
        cmp word ptr ds:[edx+edi*2], 0
        // exit early if the block IS a slope
        jne __runtimeHookNPCSemisolidSlopeCollisionHook_exit

        // check semisolid
        mov edx, dword ptr ds:[0x00B2C048] // blockdef_semisolid_ADDR
        cmp word ptr ds:[edx+edi*2], 0xFFFF
        // exit to original return addr
        push 0x00A12038
        ret

    __runtimeHookNPCSemisolidSlopeCollisionHook_exit:
        // early exit for slope
        xor edx, edx // done to make cpu state consistent, probably not really necessary
        cmp edx, 1
        push 0x00A12038
        ret

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

void __stdcall runtimeHookUpdateJoystick()
{
    static unsigned short runtimeHookJoystick[15 + 32] = { 0 };
    *(unsigned short**)0xB2DBD4 = runtimeHookJoystick;

    JOYINFOEX ji = { 0 };
    ji.dwSize = sizeof(JOYINFOEX);

    UINT uJoyID = *(unsigned int*)0xB2D878;

    gLunaGameControllerManager.emulatedJoyGetPosEx(uJoyID, &ji);

    for (int i = 0; i < 32; i++)
    {
        runtimeHookJoystick[i + 15] = COMBOOL(ji.dwButtons & (1UL << i));
    }

    *(int*)0xB2DBE0 = ji.dwXpos;
    *(int*)0xB2DBE4 = ji.dwYpos;
    *(int*)0xB2DBE8 = ji.dwPOV;
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

static unsigned int __stdcall runtimeHookStaticDirection(unsigned int npcId)
{
    if (NPC::GetStaticDirection(npcId))
    {
        return 0xA0F18C; // Skip setting direction
    }
    return 0xA0F150; // Allow setting direction
}

_declspec(naked) void __stdcall runtimeHookStaticDirectionWrapper(void)
{
    __asm {
        push ecx
        push edx

        movzx eax, ax
        push eax // eax = NPC ID
        call runtimeHookStaticDirection

        pop edx
        pop ecx

        push eax
        ret
    }
}

static void __stdcall runtimeHookStoreCustomMusicPath(unsigned int section)
{
    SMBXSound::StoreActiveCustomMusicPath(section);
}

_declspec(naked) void __stdcall runtimeHookStoreCustomMusicPathWrapper(void)
{
    // JMP from 00A61EDA
    __asm {
        push eax
        push ecx
        push edx

        push ebx
        call runtimeHookStoreCustomMusicPath

        pop edx
        pop ecx
        pop eax

        mov dword ptr ss : [ebp - 0xE8], eax
        push 0xA61EE0
        ret
    }
}

void __stdcall runtimeHookCheckWindowFocus()
{
    if (!gMainWindowFocused && !LunaLoadScreenIsActive())
    {
        // During this block of code, pause music if it was playing
        PGE_MusPlayer::DeferralLock musicPauseLock(true);

        // Wait for focus
        TestModeSendNotification("suspendWhileUnfocusedNotification");
        while (!gMainWindowFocused && !LunaLoadScreenIsActive())
        {
            Sleep(100);
            LunaDllWaitFrame(false);
        }
        TestModeSendNotification("resumeAfterUnfocusedNotification");
    }
}

extern std::string g_SecDefaultMusicPaths[21];
static void __stdcall runtimeHookResetSectionMusic(unsigned int sec1, unsigned int sec2)
{
    // Note: sec1 and sec2 are always expected to be the same
    if ((sec1 >= 21) || (sec2 >= 21)) return;

    GM_SEC_MUSIC_TBL[sec2] = GM_SEC_DEFAULT_MUSIC_TBL[sec1];
    GM_MUSIC_PATHS_PTR[sec2] = g_SecDefaultMusicPaths[sec1];
}

_declspec(naked) void __stdcall runtimeHookResetSectionMusicWrapper8EB370(void)
{
    __asm {
        push eax
        push ecx
        push edx

        push esi
        push edi
        call runtimeHookResetSectionMusic
        pop edx
        pop ecx
        pop eax
        push 0x8EB383
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookResetSectionMusicWrapper8EBA87(void)
{
    __asm {
        push eax
        push ecx
        push edx

        push esi
        push edi
        call runtimeHookResetSectionMusic
        pop edx
        pop ecx
        pop eax
        push 0x8EBA9A
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookResetSectionMusicWrapperA3576F(void)
{
    __asm {
        push eax
        push ecx
        push edx

        push dword ptr ss : [ebp - 0x324]
        push dword ptr ss : [ebp - 0x328]
        call runtimeHookResetSectionMusic
        pop edx
        pop ecx
        pop eax
        push 0xA3578F
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookResetSectionMusicWrapperA35E9B(void)
{
    __asm {
        push eax
        push ecx
        push edx

        push ebx
        push dword ptr ss : [ebp - 0x328]
        call runtimeHookResetSectionMusic
        pop edx
        pop ecx
        pop eax
        push 0xA35EB5
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookResetSectionMusicWrapperAA4486(void)
{
    __asm {
        push eax
        push ecx
        push edx

        push ebx
        push dword ptr ss : [ebp - 0x100]
        call runtimeHookResetSectionMusic
        pop edx
        pop ecx
        pop eax
        push 0xAA44A0
        ret
    }
}

static int __stdcall runtimeHookPlayerBouncePushCheck(unsigned int blockId, PlayerMOB* player)
{
    if (gDisablePlayerFilterBounceFix) return 0; // Allow collision

    short characterFilter = Blocks::GetBlockPlayerFilter(blockId);

    // -1 means allow all characters
    if (characterFilter == -1)
    {
        return -1; // No collision
    }

    short characterId = (short)player->Identity;
    if (characterFilter == characterId)
    {
        return -1; // No collision
    }

    return 0; // Allow collision
}

_declspec(naked) void __stdcall runtimeHookPlayerBouncePushCheckWrapper(void)
{
    // Overwrites 009C0B3E jne smbx.9C0C19
    __asm {
        jne disallowCollide
        push edx
        push ebx // player
        push edi // blockId
        call runtimeHookPlayerBouncePushCheck
        pop edx
        cmp eax, 0
        jne disallowCollide
        push 0x9C0B44
        ret
    disallowCollide :
        push 0x9C0C19
        ret
    }
}

static inline double blockGetTopYTouching(Block& block, Momentum& loc)
{
    // Get slope type
    short blockType = block.BlockType;
    short slopeDirection;
    if ((blockType >= 1) && (blockType <= Block::MAX_ID))
    {
        slopeDirection = blockdef_floorslope[blockType];
    }
    else
    {
        slopeDirection = 0;
    }

    // The simple case, no slope
    if (slopeDirection == 0)
    {
        return block.momentum.y;
    }

    // The degenerate case, no width
    if (block.momentum.width <= 0)
    {
        return block.momentum.y;
    }

    // The following uses a slope calculation like 1.3 does

    // Get right or left x coordinate as relevant for the slope direction
    double refX = loc.x;
    if (slopeDirection > 0) refX += loc.width;

    // Get how far along the slope we are in the x direction
    double slope = (refX - block.momentum.x) / block.momentum.width;
    if (slopeDirection > 0) slope = 1.0 - slope;
    if (slope < 0.0) slope = 0.0;
    if (slope > 1.0) slope = 1.0;

    // Determine the y coordinate
    return block.momentum.y + block.momentum.height - (block.momentum.height * slope);
}

static int __stdcall runtimeHookCompareWalkBlock(unsigned int oldBlockIdx, unsigned int newBlockIdx, Momentum* referenceLoc)
{
    if (oldBlockIdx > GM_BLOCK_COUNT) return 0;
    if (newBlockIdx > GM_BLOCK_COUNT) return 0;
    Block& oldBlock = Blocks::GetBase()[oldBlockIdx];
    Block& newBlock = Blocks::GetBase()[newBlockIdx];

    double newBlockY = blockGetTopYTouching(newBlock, *referenceLoc);
    double oldBlockY = blockGetTopYTouching(oldBlock, *referenceLoc);

    if (newBlockY < oldBlockY)
    {
        // New block is higher, replace
        return -1;
    }
    else if (newBlockY > oldBlockY)
    {
        // New block is lower, don't replace
        return 0;
    }

    // Break tie based on if one is moving upward faster
    double newBlockSpeedY = newBlock.momentum.speedY;
    double oldBlockSpeedY = oldBlock.momentum.speedY;
    if (newBlockSpeedY < oldBlockSpeedY)
    {
        // New block is moving more upward, replace
        return -1;
    }
    else if (newBlockSpeedY > oldBlockSpeedY)
    {
        // New block is moving more downward, don't replace
        return 0;
    }

    // Break tie based on x-proximity
    double refX = referenceLoc->x + referenceLoc->width * 0.5;
    double newBlockDist = abs((newBlock.momentum.x + newBlock.momentum.width * 0.5) - refX);
    double oldBlockDist = abs((oldBlock.momentum.x + oldBlock.momentum.width * 0.5) - refX);
    if (newBlockDist < oldBlockDist)
    {
        // New block is closer, replace
        return -1;
    }
    if (newBlockDist > oldBlockDist)
    {
        // New block further, don't replace
        return 0;
    }

    // Break tie based on narrower width (more specific match)
    double newBlockWidth = newBlock.momentum.width;
    double oldBlockWidth = oldBlock.momentum.width;
    if (newBlockWidth < oldBlockWidth)
    {
        // New block is narrower, replace
        return -1;
    }
    if (newBlockWidth > oldBlockWidth)
    {
        // New block wider, don't replace
        return 0;
    }

    // Still tied? Let's just not replace
    return 0;
}

_declspec(naked) void __stdcall runtimeHookCompareWalkBlockForPlayerWrapper(void)
{
    // JMP from 009A3FD3
    __asm {
        // eax, ecx, edx are all free for use here

        lea   edx,word ptr ss:[ebx+0xC0]
        movsx ecx,word ptr ss:[ebp-0x120]
        movsx eax,word ptr ss:[ebp-0xF8]
        push edx
        push ecx
        push eax
        call runtimeHookCompareWalkBlock

        cmp eax, 0
        jne blockIsHigher

        push 0x9A4319 // Block is not higher
        ret
    blockIsHigher:
        push 0x9A42DC // Block is higher
        ret
    }
}

// Note, the following assume no recursion of the NPC physics processing loop, which is a safe assumption but worth noting
static unsigned int g_npcTempHitBlock;
static double g_npcTempHit;
static short g_npcTempHitBlockIsSlope;

_declspec(naked) void __stdcall runtimeHookPreserveNPCWalkBlock()
{
    // Patches over 00A14BA6 | jne 0xA15F7C
    // edx is free for use at this point
    __asm {
        pushfd
        movsx edx, word ptr ss : [ebp - 0x178]
        mov g_npcTempHitBlock, edx
        fld qword ptr ss : [ebp - 0x100]
        fstp g_npcTempHit
        popfd
        jne otherHitspot
        push 0xA14BAC // HitSpot 1
        ret
    otherHitspot :
        push 0xA15F7C // Not HitSpot 1
        ret
    }
}

static void __stdcall runtimeHookCompareNPCWalkBlockInternal(short& tempHitBlock, double& tempHit, NPCMOB& npc)
{
    unsigned int oldBlockIdx  = g_npcTempHitBlock;
    unsigned int newBlockIdx  = tempHitBlock;
    Momentum&    referenceLoc = npc.momentum;

    // If no temp block was already set, just exit
    if (g_npcTempHitBlock == 0)
    {
        // tempHitBlock is set, don't revert
        g_npcTempHitBlockIsSlope = 0;
        if (npc.unknown_22 && (npc.unknown_22 != newBlockIdx))
        {
            if (runtimeHookCompareWalkBlock(npc.unknown_22, newBlockIdx, &referenceLoc) != 0)
            {
                npc.unknown_22 = 0;
            }
        }
        return;
    }

    // Compare blocks
    int compareResult = runtimeHookCompareWalkBlock(oldBlockIdx, newBlockIdx, &referenceLoc);

    // Revert to the old block if the comparison says we shouldn't replace
    if (compareResult == 0)
    {
        // We shouldn't replace, so revert variables
        tempHitBlock = g_npcTempHitBlock;
        tempHit = g_npcTempHit;
    }
    else
    {
        // tempHitBlock is set, don't revert
        g_npcTempHitBlockIsSlope = 0;
        if (npc.unknown_22 && (npc.unknown_22 != newBlockIdx))
        {
            if (runtimeHookCompareWalkBlock(npc.unknown_22, newBlockIdx, &referenceLoc) != 0)
            {
                npc.unknown_22 = 0;
            }
        }
    }
}

_declspec(naked) void __stdcall runtimeHookCompareNPCWalkBlock()
{
    // Patches over 00A16B82 | cmp word ptr ds : [esi + 0x136], 0
    // eax, ecx, and edx are free for use at this point
    __asm {
        mov eax, g_npcTempHitBlock
        cmp word ptr ss:[ebp - 0x178], ax
        je tempHitBlockUnchanged

        // Pass to C++ code for behaviour
        lea ecx, ss:[ebp - 0x100]
        lea edx, ss:[ebp - 0x178]
        push esi // *referenceLoc
        push ecx // *tempHit
        push edx // *tempHitBlock
        call runtimeHookCompareNPCWalkBlockInternal

    tempHitBlockUnchanged:
        // Original code
        cmp word ptr ds : [esi + 0x136], 0
        push 0xA16B8A
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookNPCWalkFixClearTemp()
{
    // Patches over 00A0C8D4 | mov dword ptr ss:[ebp-0x100],edx
    //              00A0C8DA | mov dword ptr ss:[ebp-0xFC],edx
    // and similar
    __asm {
        pushfd
        push eax
        xor eax, eax
        mov dword ptr ss : [ebp - 0x100], eax
        mov dword ptr ss : [ebp - 0xFC], eax
        mov dword ptr ss : [ebp - 0x178], eax
        pop eax
        popfd
        ret
    }
}

_declspec(naked) void __stdcall runtimeHookNPCWalkFixTempHitConditional()
{
    // Patches over comparison of tempHit from 00A1BB3A to 00A1BB4B
    __asm {
        cmp dword ptr ss : [ebp - 0x178], 0
        je noWalk
        cmp g_npcTempHitBlockIsSlope, 0
        jne noWalk
        push 0xA1BB51
        ret
    noWalk:
        push 0xA1D38C
        ret
    }
}

static void __stdcall runtimeHookNPCWalkFixSlopeInternal(short& blockIdx, short& tempHitBlock, double& tempHit, Momentum& referenceLoc)
{
    // Do nothing for this case anymore, this is otherwise handled?
}

_declspec(naked) void __stdcall runtimeHookNPCWalkFixSlope()
{
    // Patches over slope walk invalidation from 0xA13188 through 0xA1322F
    // eax, ecx, and edx are free for use at this point
    __asm {
        // Pass to C++ code for behaviour
        lea ecx, ss : [ebp - 0x100]
        lea edx, ss : [ebp - 0x178]
        lea eax, ss : [ebp - 0x188]
        push esi // *referenceLoc
        push ecx // *tempHit
        push edx // *tempHitBlock
        push eax // *blockIdx
        call runtimeHookNPCWalkFixSlopeInternal

        push 0xA1322F
        ret
    }
}


static double findMomentumToBoundaryDistance(const Momentum& momentum, const Bounds& bounds)
{
    double distX = 0;
    double distY = 0;

    if ((momentum.x + momentum.width) < bounds.left)
    {
        distX = momentum.x + momentum.width - bounds.left;
    }
    else if (momentum.x > bounds.right)
    {
        distX = bounds.right - momentum.x;
    }

    if ((momentum.y + momentum.height) < bounds.top)
    {
        distY = momentum.y + momentum.height - bounds.top;
    }
    else if (momentum.y > bounds.bottom)
    {
        distY = bounds.bottom - momentum.y;
    }

    return sqrt(distX*distX + distY*distY);
}

void __stdcall runtimeHookNPCSectionFix(short* npcIdx)
{
    NPCMOB* npc = NPC::GetRaw(*npcIdx);
    ExtendedNPCFields* ext = NPC::GetRawExtended(*npcIdx);
    const Momentum& momentum = npc->momentum;

    // Skip if in the main menu
    if (GM_LEVEL_MODE == -1)
    {
        ext->fullyInsideSection = npc->currentSection;
        return;
    }

    // Held NPC behaviour
    if (npc->grabbingPlayerIndex > 0)
    {
        // Don't let it despawn
        if (npc->offscreenCountdownTimer < 10)
            npc->offscreenCountdownTimer = 10;
        
        // Match the player's section
        npc->currentSection = Player::Get(npc->grabbingPlayerIndex)->CurrentSection;
        return;
    }

    // Is it still within the bounds of the current section?
    const Bounds& bounds = GM_LVL_BOUNDARIES[npc->currentSection];

    if (momentum.x <= bounds.right && momentum.y <= bounds.bottom && momentum.x+momentum.width >= bounds.left && momentum.y+momentum.height >= bounds.top)
    {
        ext->fullyInsideSection = npc->currentSection;
        return;
    }
    else if (momentum.x-32 <= bounds.right && momentum.y-32 <= bounds.bottom && momentum.x+momentum.width+32 >= bounds.left && momentum.y+momentum.height+32 >= bounds.top)
    {
        // If just barely outside, no need to check the section again
        // This is particularly important for older levels with very close sections
        return;
    }
    
    // Is it in the bounds of a section? If so, choose it
    for (short sectionIdx = 0; sectionIdx <= 20; sectionIdx++)
    {
        if (sectionIdx == npc->currentSection)
        {
            continue;
        }

        const Bounds& bounds = GM_LVL_BOUNDARIES[sectionIdx];

        if (momentum.x <= bounds.right && momentum.y <= bounds.bottom && momentum.x+momentum.width >= bounds.left && momentum.y+momentum.height >= bounds.top)
        {
            npc->currentSection = sectionIdx;
            ext->fullyInsideSection = sectionIdx;
            return;
        }
    }

    // Out of bounds, so find the closest section
    double closestSectionDist = 0;
    short closestSection = -1;

    for (short sectionIdx = 0; sectionIdx <= 20; sectionIdx++)
    {
        // Find the distance to the section's boundaries or original boundaries - whichever is closest
        double distToOriginalBounds = findMomentumToBoundaryDistance(momentum, GM_ORIG_LVL_BOUNDS[sectionIdx]);
        double distToBounds = findMomentumToBoundaryDistance(momentum, GM_LVL_BOUNDARIES[sectionIdx]);

        double dist = std::min(distToBounds,distToOriginalBounds);

        if (closestSection == -1 || dist < closestSectionDist)
        {
            closestSectionDist = dist;
            closestSection = sectionIdx;
        }
    }

    npc->currentSection = closestSection;
}


static void __stdcall runtimeHookNPCSectionWrapInternal(unsigned int npcIdx, unsigned int section)
{
    // At this point, we've passed the normal checks for if NPC level wraparound should apply
    // aside from checking the coordinates
    NPCMOB* npc = NPC::GetRaw(npcIdx);
    ExtendedNPCFields* ext = NPC::GetRawExtended(npcIdx);
    Momentum& momentum = npc->momentum;

    // If not _actually_ in section bounds beforehand, don't wrap
    if (npc->currentSection != ext->fullyInsideSection) return;

    // Perform the normal section wrap logic
    auto& bounds = GM_LVL_BOUNDARIES[section];
    if ((momentum.x + momentum.width) < bounds.left)
    {
        momentum.x = bounds.right - 1.0;
    }
    else if (momentum.x > bounds.right)
    {
        momentum.x = bounds.left - momentum.width + 1.0;
    }
}

__declspec(naked) void __stdcall runtimeHookNPCSectionWrap(void)
{
    // 00A0C931 | movsx edi,word ptr ds:[esi+146]
    // eax, ecx, edx and flags are free for use at this point
    __asm {
        push edi                          // section
        movsx eax, word ptr ss : [ebp - 0x180]
        push eax                          // npc index
        push 0xA0C9F3                     // return address
        jmp runtimeHookNPCSectionWrapInternal
    }
}


static void markBlocksUnsorted()
{
    // NOTE: We re-run this anyway even if GM_BLOCKS_SORTED is already cleared, to make sure the max
    //       lookup value is updated based on block count, since that for some reason seems
    //       necessary.
    WORD blockCount = GM_BLOCK_COUNT;
    GM_BLOCKS_SORTED = 0;
    for (int i = 0; i <= 16000; i++)
    {
        GM_BLOCK_LOOKUP_MIN[i] = 1;
        GM_BLOCK_LOOKUP_MAX[i] = blockCount;
    }
}

static void __stdcall runtimeHookAfterPSwitchBlocksReordered(void)
{
    markBlocksUnsorted();
}

_declspec(naked) void __stdcall runtimeHookAfterPSwitchBlocksReorderedWrapper(void)
{
    __asm {
        push eax
        push ecx
        push edx
        call runtimeHookAfterPSwitchBlocksReordered
        pop edx
        pop ecx
        pop eax
        mov edi, 1
        push 0x009E450C
        ret
    }
}

static const std::wstring destroyedPSwitchBlockLayerName(L"Destroyed PSwitch Blocks");

static void __stdcall runtimeHookPSwitchStartRemoveBlock(unsigned int blockIdx)
{
    Block& b = *Block::GetRaw(blockIdx);

    b.BlockType = 0;
    b.IsHidden = -1;
    b.pLayerName = destroyedPSwitchBlockLayerName;
}

_declspec(naked) void __stdcall runtimeHookPSwitchStartRemoveBlockWrapper(void)
{
    __asm {
        push eax
        push ecx
        push edx
        push edi // Arg 1
        call runtimeHookPSwitchStartRemoveBlock
        pop edx
        pop ecx
        pop eax
        push 0x009E3D9E
        ret
    }
}

static unsigned int __stdcall runtimeHookPSwitchGetNewBlockAtEnd()
{
    unsigned int blockCount = GM_BLOCK_COUNT;

    // TODO: This could be faster if it mattered, and maybe share block reusing with Lua block spawning?
    for (unsigned int i = 0; i < blockCount; i++)
    {
        Block& b = *Block::GetRaw(i);
        if (b.pLayerName == destroyedPSwitchBlockLayerName)
        {
            // Reset extended properties
            Blocks::GetRawExtended(i)->Reset();

            if (gLunaLua.isValid()) {
                std::shared_ptr<Event> blockInvalidateEvent = std::make_shared<Event>("onBlockInvalidateForReuseInternal", false);
                blockInvalidateEvent->setDirectEventName("onBlockInvalidateForReuseInternal");
                blockInvalidateEvent->setLoopable(false);
                gLunaLua.callEvent(blockInvalidateEvent, i);
            }

            return i;
        }
    }

    blockCount++;
    GM_BLOCK_COUNT = blockCount;
    return blockCount;
}

_declspec(naked) void __stdcall runtimeHookPSwitchGetNewBlockAtEndWrapper(void)
{
    __asm {
        push ecx
        push edx
        call runtimeHookPSwitchGetNewBlockAtEnd
        pop edx
        pop ecx
        mov esi, eax
        push 0x009E3E71
        ret
    }
}

static unsigned int __stdcall runtimeHookNPCNoBlockCollisionTest(unsigned int npcIdx)
{
    ExtendedNPCFields* ext = NPC::GetRawExtended(npcIdx);
    if (ext != nullptr)
    {
        if (ext->noblockcollision)
        {
            return -1;
        }
    }

    return 0;
}

__declspec(naked) void __stdcall runtimeHookNPCNoBlockCollision9E2AD0(void)
{
    // Death by block bump
    // 009E2AD0 | jne smbx.9E2EB6
    __asm {
        jne early_exit
        pushfd
        push eax
        push ecx
        push edx
        movsx eax, word ptr ss : [esp + 0x28]
        push eax // Args #1
        call runtimeHookNPCNoBlockCollisionTest
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        popfd
        push 0x9E2AD6
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popfd
    early_exit :
        push 0x9E2EB6
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNPCNoBlockCollisionA089C3(void)
{
    // ???
    // 00A089C3 | jne smbx.A08CA5
    __asm {
        jne early_exit
        pushfd
        push eax
        push ecx
        push edx
        movsx eax, word ptr ss : [ebp - 0x188]
        push eax // Args #1
        call runtimeHookNPCNoBlockCollisionTest
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xA089C9
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popfd
    early_exit :
        push 0xA08CA5
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNPCNoBlockCollisionA10EAA(void)
{
    // Main Block Collision
    // 00A10EAA | cmp word ptr ds:[ecx+edi*2],0
    __asm {
        cmp word ptr ds:[ecx+edi*2],0
        jne early_exit
        pushfd
        push eax
        push ecx
        push edx
        movsx eax, word ptr ss : [ebp - 0x180]
        push eax // Args #1
        call runtimeHookNPCNoBlockCollisionTest
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xA10EBB
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popfd
    early_exit :
        push 0xA10EB1
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNPCNoBlockCollisionA113B0(void)
{
    // ???
    // 00A113B0 | cmp word ptr ds:[ecx+edi*2],0
    __asm {
        cmp word ptr ds:[ecx+edi*2],0
        jne early_exit
        pushfd
        push eax
        push ecx
        push edx
        movsx eax, word ptr ss : [ebp - 0x180]
        push eax // Args #1
        call runtimeHookNPCNoBlockCollisionTest
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xA113B7
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popfd
    early_exit :
        push 0xA11414
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNPCNoBlockCollisionA1760E(void)
{
    // Beltspeed
    // 00A1760E | cmp word ptr ds:[edx+edi*2],0
    __asm {
        cmp word ptr ds:[edx+edi*2],0
        jne early_exit
        pushfd
        push eax
        push ecx
        push edx
        movsx eax, word ptr ss : [ebp - 0x180]
        push eax // Args #1
        call runtimeHookNPCNoBlockCollisionTest
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xA17619
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popfd
    early_exit :
        push 0xA17BDA
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNPCNoBlockCollisionA1B33F(void)
{
    // Bounce off NPCs
    // 00A1B33F | cmp word ptr ds:[eax+edi*2],0
    __asm {
        cmp word ptr ds:[eax+edi*2],0
        jne early_exit
        pushfd
        push eax
        push ecx
        push edx
        movsx eax, word ptr ss : [ebp - 0x180]
        push eax // Args #1
        call runtimeHookNPCNoBlockCollisionTest
        cmp eax, 0
        jne alternate_exit
        movsx eax, word ptr ss : [ebp - 0x188]
        push eax // Args #1
        call runtimeHookNPCNoBlockCollisionTest
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xA1B346
        ret
    alternate_exit :
        pop edx
        pop ecx
        pop eax
        popfd
    early_exit :
        push 0xA1B386
        ret
    }
}

static unsigned int __stdcall runtimeHookBlockNPCFilterInternal(unsigned int hitSpot, NPCMOB* npc, unsigned int blockIdx, unsigned int npcIdx, unsigned int oldSlope)
{
    // If already not hitting, ignore
    if (hitSpot == 0) return 0;

    Block* block = Block::GetRaw(blockIdx);
    if (block)
    {
        short npcFilter = Blocks::GetBlockNPCFilter(block->BlockType);
        if ((npcFilter != 0) && ((npcFilter == -1) || (npcFilter == npc->id)))
        {
            // The filter was a non-zero and matched, so no collision
            return 0;
        }

        // if this is a semisolid slope
        if (blockdef_semisolid[block->BlockType] && blockdef_floorslope[block->BlockType] != 0)
        {
            // stop appropriate flying npcs from colliding with semisolid slopes
            if (npc_isflying[npc->id] != 0) {
                if (!NPC::CheckSemisolidCollidingFlyType((unsigned int)npc->ai1)) {
                    return 0;
                }
            }
            // check semisolid slope collision
            int collidesBelow = npc->collidesBelow;
            if (oldSlope != 0) {
                // unlike players, npcs set their bottom collision timer when standing on slopes
                // because of this, we have to check if a slope was stood on previously and cancel the bottom timer out
                collidesBelow = 0;
            }
            if (!Blocks::FilterSemisolidSlopeCollision(&npc->momentum, blockIdx, collidesBelow)) {
                return 0;
            } else {
                // change hitSpot to always act as top collision
                hitSpot = 1;
            }
        }

        ExtendedNPCFields* ext = NPC::GetRawExtended(npcIdx);

        if (block->OwnerNPCID != 0) // Belongs to an NPC
        {
            ExtendedNPCFields* ownerExt = NPC::GetRawExtended(block->OwnerNPCIdx);

            if (!gCollisionMatrix.getIndicesCollide(ext->collisionGroup,ownerExt->collisionGroup)) // Check collision matrix
                return 0;
        }
        else
        {
            ExtendedBlockFields* blockExt = Blocks::GetRawExtended(blockIdx);

            if (!gCollisionMatrix.getIndicesCollide(ext->collisionGroup,blockExt->collisionGroup)) // Check collision matrix
                return 0;
        }


    }

    return hitSpot;
}

__declspec(naked) void __stdcall runtimeHookBlockNPCFilter(void)
{
    // 00A11B76 | mov ax, word ptr ds : [esi + 0xE2]
    // eax, ecx, edx and flags are free for use at this point
    __asm {
        movsx eax, word ptr ss:[ebp-0x104] // oldSlope (for semisolid slope check)
        push eax
        movsx eax, word ptr ss:[ebp-0x180]   // npcIdx
        push eax
        movsx eax, word ptr ss:[ebp-0x188]   // blockIdx
        push eax
        push esi                             // npc pointer
        push dword ptr ss:[ebp-0x14]         // hitSpot
        call runtimeHookBlockNPCFilterInternal
        mov dword ptr ss : [ebp - 0x14], eax // store return back to hitSpot

        mov ax, word ptr ds : [esi + 0xE2] // The code we're replacing
        push 0xA11B7D
        ret
    }
}

static unsigned int __stdcall runtimeHookNPCCollisionGroupInternal(int npcAIdx, int npcBIdx)
{
    if (npcAIdx == npcBIdx) // Don't collide if it's the same NPC - this is what the code we're replacing does!
        return 0; // Collision cancelled
    
    ExtendedNPCFields* extA = NPC::GetRawExtended(npcAIdx);
    ExtendedNPCFields* extB = NPC::GetRawExtended(npcBIdx);

    if (!gCollisionMatrix.getIndicesCollide(extA->collisionGroup,extB->collisionGroup)) // Check collision matrix
        return 0; // Collision cancelled

    return -1; // Collision goes ahead
}

__declspec(naked) void __stdcall runtimeHookNPCCollisionGroup(void)
{
    __asm {
        push eax
        push ecx
        push edx
        mov eax, dword ptr ss:[ebp-0x188]   // npcBIdx
        push eax
        movsx eax, word ptr ss:[ebp-0x180]   // npcAIdx
        push eax
        call runtimeHookNPCCollisionGroupInternal
        cmp eax, 0                           // return value
        jne continue_collision
        jmp cancel_collision
    continue_collision:
        pop edx
        pop ecx
        pop eax
        push 0xA181B3
        ret
    cancel_collision:
        pop edx
        pop ecx
        pop eax
        push 0xA1BAD5
        ret
    }
}

static unsigned int __stdcall runtimeHookBlockPlayerFilterInternal(short playerIdx, int blockIdx)
{
    PlayerMOB* player = Player::Get(playerIdx);
    Block* block = Block::GetRaw(blockIdx);

    // IsHidden flag, which is what the code we're replacing checks for
    if (block->IsHidden)
    {
        return 0;
    }

    short characterFilter = Blocks::GetBlockPlayerFilter(block->BlockType);

    // -1 means allow all characters
    if (characterFilter == -1)
    {
        return 0;
    }

    // Matching characters, cancel collision
    short characterId = (short)player->Identity;
    if (characterFilter == characterId)
    {
        return 0;
    }
    
    // if this is a semisolid slope
    if (blockdef_semisolid[block->BlockType] && blockdef_floorslope[block->BlockType] != 0)
    {
        // check semisolid slope collision
        if (!Blocks::FilterSemisolidSlopeCollision(&player->momentum, blockIdx, player->LayerStateStanding)) {
            return 0;
        }
    }

    // Player's noblockcollision flag
    ExtendedPlayerFields* playerExt = Player::GetExtended(playerIdx);

    if (playerExt->noblockcollision)
    {
        return 0;
    }
    
    // Collision groups
    ExtendedBlockFields* blockExt = Blocks::GetRawExtended(blockIdx);

    if (!gCollisionMatrix.getIndicesCollide(playerExt->collisionGroup,blockExt->collisionGroup)) // Check collision matrix
    {
        return 0;
    }

    // No filter needed, carry on
    return -1;
}

__declspec(naked) void __stdcall runtimeHookBlockPlayerFilter(void)
{
    __asm {
        push eax                // push these to make sure they're safe after the function call
        push ecx
        push edx
        push esi

        movsx ecx, word ptr ss:[ebp-0x120]
        push ecx                // Block index
        movsx ecx, word ptr ss:[ebp-0x114]
        push ecx                // Player index
        call runtimeHookBlockPlayerFilterInternal

        cmp eax, 0 // return value
        jne continue_collision
        jmp cancel_collision
    continue_collision:
        pop esi                 // we can pop these again
        pop edx
        pop ecx
        pop eax
        push 0x9A16F4
        ret
    cancel_collision:
        pop esi                 // we can pop these again
        pop edx
        pop ecx
        pop eax
        push 0x9A4FE9
        ret
    }
}

static unsigned int __stdcall runtimeHookPlayerNPCInteractionCheckInternal(short playerIdx, short npcIdx)
{
    // Player's nonpcinteraction flag
    ExtendedPlayerFields* playerExt = Player::GetExtended(playerIdx);

    if (playerExt->nonpcinteraction)
    {
        return 0;
    }

    // Collision groups
    ExtendedNPCFields* npcExt = NPC::GetRawExtended(npcIdx);

    if (!gCollisionMatrix.getIndicesCollide(playerExt->collisionGroup,npcExt->collisionGroup)) // Check collision matrix
    {
        return 0;
    }

    return -1;
}

__declspec(naked) void __stdcall runtimeHookPlayerNPCInteractionCheck(void)
{
    __asm {
        push eax

        push eax                // NPC index
        movsx eax, word ptr ss:[ebp-0x114]
        push eax                // Player index

        call runtimeHookPlayerNPCInteractionCheckInternal

        cmp eax, 0 // return value
        jne continue_collision
        jmp cancel_collision
    continue_collision:
        pop eax

        movsx edi,ax
        add edi,0x80

        push 0x9A79F8
        ret
    cancel_collision:
        pop eax

        push 0x9ADCDB
        ret
    }
}

static unsigned int __stdcall runtimeHookPlayerNPCCollisionCheckInternal(short playerIdx)
{
    // Player's noblockcollision flag
    ExtendedPlayerFields* playerExt = Player::GetExtended(playerIdx);

    if (playerExt->noblockcollision)
    {
        return 0;
    }

    return -1;
}

__declspec(naked) void __stdcall runtimeHookPlayerNPCCollisionCheck9AE8FA(void)
{
    // Handles collision from the sides/bottom
    __asm {
        cmp word ptr ds:[edx+ecx*0x8 + 0x136], 0   // check the projectile flag (this is what the code that this replaces does)
        jne cancel_collision

        movsx eax, word ptr ss:[ebp-0x114]
        push eax                // Player index

        call runtimeHookPlayerNPCCollisionCheckInternal

        cmp eax, 0 // return value
        jne continue_collision
        jmp cancel_collision
    continue_collision:
        push 0x9AE909
        ret
    cancel_collision:
        push 0x9ADCDB
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookPlayerNPCCollisionCheck9ABC0B(void)
{
    // Handles collision from the top
    __asm {
        movsx eax, word ptr ss:[ebp-0x114]
        push eax                // Player index

        call runtimeHookPlayerNPCCollisionCheckInternal

        cmp eax, 0 // return value
        jne continue_collision
        jmp cancel_collision
    continue_collision:
        mov ecx, dword ptr ss:[ebp-0xB4] // original code

        push 0x9ABC11
        ret
    cancel_collision:
        push 0x9AD246 // jumps to code for handling other hit spots
        ret
    }
}

static unsigned int __stdcall runtimeHookPlayerPlayerInteractionInternal(short* playerAIdxPtr, short playerBIdx)
{
    // Don't interact if it's the same player
    // This hook replaces the code that would normally handle this
    short playerAIdx = *playerAIdxPtr;

    if (playerAIdx == playerBIdx)
    {
        return 0;
    }

    // noplayerinteraction flag
    ExtendedPlayerFields* extA = Player::GetExtended(playerAIdx);
    ExtendedPlayerFields* extB = Player::GetExtended(playerBIdx);

    if (extA->noplayerinteraction || extB->noplayerinteraction)
    {
        return 0;
    }

    // Collision groups
    if (!gCollisionMatrix.getIndicesCollide(extA->collisionGroup,extB->collisionGroup)) // Check collision matrix
    {
        return 0;
    }

    return -1;
}

__declspec(naked) void __stdcall runtimeHookPlayerPlayerInteraction(void)
{
    __asm {
        push eax

        push eax                // Player B's index
        mov eax, dword ptr ss:[ebp+8]
        push eax                // (Pointer to) player A's index

        call runtimeHookPlayerPlayerInteractionInternal

        cmp eax, 0 // return value
        jne continue_collision
        jmp cancel_collision
    continue_collision:
        pop eax

        push 0x9CAFD0
        ret
    cancel_collision:
        pop eax

        push 0x9CC25D
        ret
    }
}

static unsigned int __stdcall runtimeHookLevelPauseCheckInternal(void)
{
    if (gEscPressedRegistered)
    {
        // ESC key pressed before more recent updateInput
        return 0x8CA40B; // Pause button pressed
    }

    if ((gRawKeymap[0].pauseKeyState != 0) && ((gRawKeymap[2].pauseKeyState == 0)) && (Player::Get(1)->keymap.pauseKeyState != 0))
    {
        // Pause key from player 1
        return 0x8CA40B; // Pause button pressed
    }

    return 0x8CA5A6; // Pause button not pressed
}

__declspec(naked) void __stdcall runtimeHookLevelPauseCheck(void)
{
    // 008CA405 | 0F84 9B010000              | je smbx.8CA5A6
    __asm {
        call runtimeHookLevelPauseCheckInternal
        push eax
        ret
    }
}

static int __stdcall runtimeHookPlayerHarmInternal(short* playerIdxPtr)
{
    bool playerHarmCancelled = false;

    if (gLunaLua.isValid() && (GM_WINNING == 0)) {
        std::shared_ptr<Event> playerHarmEvent = std::make_shared<Event>("onPlayerHarm", true);
        playerHarmEvent->setDirectEventName("onPlayerHarm");
        playerHarmEvent->setLoopable(false);
        gLunaLua.callEvent(playerHarmEvent, *playerIdxPtr);

        playerHarmCancelled = playerHarmEvent->native_cancelled();
    }

    return playerHarmCancelled ? -1 : 0;
}

__declspec(naked) void __stdcall runtimeHookPlayerHarm(void)
{
    // 009B52FC | 0F8F 550B0000              | jg smbx.9B5E57
    __asm {
        jg wasntPlayerHarm
        push eax
        push ecx
        push edx
        push dword ptr ss:[ebp+8]
        call runtimeHookPlayerHarmInternal
        cmp eax, 0
        jne cancelPlayerHarm
        pop edx
        pop ecx
        pop eax
        push 0x9B5302
        ret
    cancelPlayerHarm:
        pop edx
        pop ecx
        pop eax
    wasntPlayerHarm:
        push 0x9B5E57
        ret
    }
}

__declspec(naked) void __stdcall killPlayer_OrigFunc(short* playerIdxPtr)
{
    __asm {
        push ebp
        mov ebp, esp
        sub esp, 0x8
        push 0x9B66D6
        ret
    }
}

void __stdcall runtimeHookPlayerKill(short* playerIdxPtr)
{
    bool playerKillCancelled = false;

    if (gLunaLua.isValid() && (GM_WINNING == 0)) {
        std::shared_ptr<Event> playerKillEvent = std::make_shared<Event>("onPlayerKill", true);
        playerKillEvent->setDirectEventName("onPlayerKill");
        playerKillEvent->setLoopable(false);
        gLunaLua.callEvent(playerKillEvent, *playerIdxPtr);

        playerKillCancelled = playerKillEvent->native_cancelled();
    }

    if (!playerKillCancelled)
    {
        killPlayer_OrigFunc(playerIdxPtr);
    }
}

static int __stdcall runtimeHookWarpEnterInternal(short* playerIdx, int warpIdx)
{
    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> warpEvent = std::make_shared<Event>("onWarpEnter", true);
        warpEvent->setDirectEventName("onWarpEnter");
        warpEvent->setLoopable(false);
        gLunaLua.callEvent(warpEvent, warpIdx, *playerIdx);

        if (warpEvent->native_cancelled())
        {
            // Cancel warp
            return 0;
        }

        // Store section to detect if player has changed sections in onWarp
        warpHookRefSection = Player::Get(*playerIdx)->CurrentSection;
    }

    // Continue warp
    return -1;
}

__declspec(naked) void __stdcall runtimeHookWarpEnter(void)
{
    // 009CA0D5 | 66:837D EC FF                  | cmp word ptr ss:[ebp-14],FFFF                        |
    __asm {
        push ebp
        push edi
        push eax

        cmp word ptr ss:[ebp-0x14],0xFFFF // if 'canWarp' isn't true, cancel immediately...
        jne cancel_warp                   // that's what the code this replaces does!

        movsx eax, word ptr ss:[ebp-0x18] // warp index
        push eax
        mov eax, dword ptr ss:[ebp+0x8]   // player index
        push eax

        call runtimeHookWarpEnterInternal // call function to decide if we cancel

        cmp eax,0                         // if 0 was returned, cancel
        jne continue_warp
        jmp cancel_warp
    continue_warp:
        pop eax
        pop edi
        pop ebp
        push 0x9CA0E0
        ret
    cancel_warp:
        pop eax
        pop edi
        pop ebp
        push 0x9CA62A
        ret
    }
}

void __stdcall runtimeHookWarpInstantInternal(short* playerIdx, int warpIdx)
{
    if (gLunaLua.isValid()) {
        // Detect if player changed sections
        if (warpHookRefSection != Player::Get(*playerIdx)->CurrentSection) {
            // Queue onSectionChange event to execute
            gLunaLua.queuePlayerSectionChangeEvent(*playerIdx);
        }

        std::shared_ptr<Event> warpEvent = std::make_shared<Event>("onWarp", false);
        warpEvent->setDirectEventName("onWarp");
        warpEvent->setLoopable(false);
        gLunaLua.callEvent(warpEvent, warpIdx, *playerIdx);
    }
}

__declspec(naked) void __stdcall runtimeHookWarpInstant(void)
{
    // 009CAE34 | 66:C781 5C010000 3200        | mov word ptr ds:[ecx+15C],32                         | 32:'2'
    // Note: the code this returns to just immediately leaves the sub,
    //       so we don't need to push and pop any registers.
    __asm {
        mov word ptr ds:[ecx+0x15C],0x32    // set warp cooldown to 50 (what the original code does)

        movsx eax, word ptr ss:[ebp-0x18]   // warp index
        push eax
        mov eax, dword ptr ss:[ebp+0x8]     // (pointer to) player index
        push eax

        call runtimeHookWarpInstantInternal // call function to run lunalua event

        push 0x9CAE9E                       // return to original code
        ret
    }
}

void __stdcall runtimeHookWarpPipeDoorInternal(short* playerIdx)
{
    // (Shared by both pipe and door hooks)
    if (gLunaLua.isValid()) {
        // Detect if player changed sections
        if (warpHookRefSection != Player::Get(*playerIdx)->CurrentSection) {
            // Queue onSectionChange event to execute
            gLunaLua.queuePlayerSectionChangeEvent(*playerIdx);
        }

        std::shared_ptr<Event> warpEvent = std::make_shared<Event>("onWarp", false);
        warpEvent->setDirectEventName("onWarp");
        warpEvent->setLoopable(false);
        gLunaLua.callEvent(warpEvent, Player::Get(*playerIdx)->TargetWarpIndex, *playerIdx);
    }
}

__declspec(naked) void __stdcall runtimeHookWarpPipe(void)
{
    // Placed in 3 spots:
    // 009D55CD | E9 AADFFFFF                  | jmp smbx.9D357C                                      |
    // 009D55F1 | 0F85 85DFFFFF                | jne smbx.9D357C                                      |
    // 009D5614 | E9 63DFFFFF                  | jmp smbx.9D357C                                      |
    __asm {
        push esi
        push ebx
        push ebp

        mov esi,dword ptr ss:[ebp+0x8]       // (pointer to) player index
        push esi

        call runtimeHookWarpPipeDoorInternal // call function to run lunalua event

        pop ebp
        pop ebx
        pop esi

        push 0x9D357C
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookWarpDoor(void)
{
    // 009D7037 | 66:A1 5E59B200               | mov ax,word ptr ds:[B2595E]                          |
    __asm {
        push esi
        push ebx
        push ebp

        mov esi,dword ptr ss:[ebp+0x8]       // (pointer to) player index
        push esi

        call runtimeHookWarpPipeDoorInternal // call function to run lunalua event

        pop ebp
        pop ebx
        pop esi

        mov ax, word ptr ds:[0xB2595E]       // load player count (this is what the code we're replacing does)

        push 0x9D703D
        ret
    }
}

void __stdcall runtimeHookDrawBackground(short* section, short* camera)
{
    if (gRenderBackgroundFlag)
    {
        native_drawBackground(section, camera);
    }
}

void __stdcall runtimeHookLoadWorldList(void)
{
    SMBXWorldFileBase::PopulateEpisodeList();
}

static unsigned int __stdcall runtimeHookSpeedOverrideCheck(unsigned int id)
{
    return NPC::GetLuaHandlesSpeed(id) ? -1 : 0;
}

__declspec(naked) void __stdcall runtimeHookSpeedOverride(void)
{
    // 00A0A383 | mov edx,dword ptr ds:[<npc_speed>]
    __asm {
        pushfd
        push eax
        push ecx

        push edi
        call runtimeHookSpeedOverrideCheck
        cmp eax, 0
        jne ignorespeed

        pop ecx
        pop eax
        popfd
        mov edx, dword ptr ds:[0xB25C18]
        push 0xA0A389
        ret
    ignorespeed:
        pop ecx
        pop eax
        popfd
        push 0xA0A39C
        ret
    }
}

// void __stdcall runtimeHookSpeedOverrideBelt(void);
__declspec(naked) void __stdcall runtimeHookSpeedOverrideBelt(void)
{
    // 00A15613 | fmul st(0),dword ptr ds:[eax+edi*4]
    // 00A15616 | jmp smbx.A1563E
    __asm {
        pushfd
        push eax
        push ecx
        push edx

        push edi
        call runtimeHookSpeedOverrideCheck
        cmp eax, 0
        jne ignorespeed

        pop edx
        pop ecx
        pop eax
        popfd
        fmul dword ptr ds : [eax + edi * 4]
        push 0xA1563E
        ret
    ignorespeed :
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xA1563E
        ret
    }
}

static void __stdcall runtimeHookBlockSpeedSet(int blockIdx)
{
    // Store layer-assigned block speed, and add extra speed
    Block&               block = *Block::GetRaw(blockIdx);
    ExtendedBlockFields& ext   = *Blocks::GetRawExtended(blockIdx);
    ext.layerSpeedX = block.momentum.speedX;
    ext.layerSpeedY = block.momentum.speedY;
    block.momentum.speedX = ext.layerSpeedX + ext.extraSpeedX;
    block.momentum.speedY = ext.layerSpeedY + ext.extraSpeedY;
}

_declspec(naked) void __stdcall runtimeHookBlockSpeedSet_FSTP_ECX_EAX_ESI(void)
{
    // 00AA5897 | 8D0496 | lea eax, dword ptr ds : [esi + edx * 4] |
    // 00AA589A | DD5CC1 48 | fstp qword ptr ds : [ecx + eax * 8 + 48], st(0) |
    __asm {
        lea eax, dword ptr ds : [esi + edx * 4]
        fstp qword ptr ds : [ecx + eax * 8 + 0x48]

        push eax
        push ecx
        push edx

        push esi
        call runtimeHookBlockSpeedSet

        pop edx
        pop ecx
        pop eax

        ret
    }
}

_declspec(naked) void __stdcall runtimeHookBlockSpeedSet_MOV_ECX_EDX_ESI(void)
{
    // 00AA6944 | 8B0D 045AB200 | mov ecx, dword ptr ds : [<GM_BLOCKS_PTR>] | (CAN IGNORE)
    // 00AA694A | 8944D1 4C | mov dword ptr ds : [ecx + edx * 8 + 4C], eax |
    // or
    // 00AA6AF7 | 8B0D 045AB200 | mov ecx, dword ptr ds : [<GM_BLOCKS_PTR>] | (CAN IGNORE)
    // 00AA6AFD | 8944D1 4C | mov dword ptr ds : [ecx + edx * 8 + 4C], eax |
    __asm {
        mov dword ptr ds : [ecx + edx * 8 + 0x4C], eax

        push eax
        push ecx
        push edx

        push esi
        call runtimeHookBlockSpeedSet

        pop edx
        pop ecx
        pop eax

        ret
    }
}

_declspec(naked) void __stdcall runtimeHookBlockSpeedSet_FSTP_EAX_EDX_ESI(void)
{
    // 00AA6DD7 | 8D148E | lea edx, dword ptr ds : [esi + ecx * 4] |
    // 00AA6DDA | DD5CD0 48 | fstp qword ptr ds : [eax + edx * 8 + 48], st(0) |
    // or
    // 009D1221 | 8D148E | lea edx, dword ptr ds : [esi + ecx * 4] |
    // 009D1224 | DD5CD0 48 | fstp qword ptr ds : [eax + edx * 8 + 48], st(0) |
    __asm {
        lea edx, dword ptr ds : [esi + ecx * 4]
        fstp qword ptr ds : [eax + edx * 8 + 0x48]

        push eax
        push ecx
        push edx

        push esi
        call runtimeHookBlockSpeedSet

        pop edx
        pop ecx
        pop eax

        ret
    }
}

_declspec(naked) void __stdcall runtimeHookBlockSpeedSet_FSTP_EAX_EDX_EDI(void)
{
    // 00A22E69 | 8D148F | lea edx, dword ptr ds : [edi + ecx * 4] |
    // 00A22E6C | DD5CD0 48 | fstp qword ptr ds : [eax + edx * 8 + 48], st(0) |
    __asm {
        lea edx, dword ptr ds : [edi + ecx * 4]
        fstp qword ptr ds : [eax + edx * 8 + 0x48]

        push eax
        push ecx
        push edx

        push edi
        call runtimeHookBlockSpeedSet

        pop edx
        pop ecx
        pop eax

        ret
    }
}

bool __stdcall saveFileExists() {
    std::wstring saveFilePath = GM_FULLDIR;
    saveFilePath += L"save";
    saveFilePath += std::to_wstring(GM_CUR_SAVE_SLOT);
    saveFilePath += L".sav";

    return fileExists(saveFilePath);
}

void __stdcall runtimeHookSetPlayerFenceSpeed(PlayerMOB *player) {
    int climbingNPC = (int) *((double*) (((char*) player) + 0x2C));

    if (climbingNPC >= 0) {
        if (climbingNPC > 5000) {
            emulateVB6Error(9);
        }

        player->momentum.speedX += NPC::GetRaw(climbingNPC)->momentum.speedX;
        player->momentum.speedY += NPC::GetRaw(climbingNPC)->momentum.speedY;
    } else {
        int climbingBGO = -climbingNPC-1;

        if (climbingBGO > 8000) {
            emulateVB6Error(9);
        }

        player->momentum.speedX += SMBX_BGO::GetRaw(climbingBGO)->momentum.speedX;
        player->momentum.speedY += SMBX_BGO::GetRaw(climbingBGO)->momentum.speedY;
    }
}

bool __stdcall runtimeHookIncreaseFenceFrameCondition(PlayerMOB *player) {
    int climbingNPC = (int) *((double*) (((char*) player) + 0x2C));

    if (climbingNPC >= 0) {
        if (climbingNPC > 5000) {
            emulateVB6Error(9);
        }

        return player->momentum.speedX != NPC::GetRaw(climbingNPC)->momentum.speedX || player->momentum.speedY < NPC::GetRaw(climbingNPC)->momentum.speedY - 0.1;
    } else {
        int climbingBGO = -climbingNPC-1;

        if (climbingBGO > 8000) {
            emulateVB6Error(9);
        }

        return player->momentum.speedX != SMBX_BGO::GetRaw(climbingBGO)->momentum.speedX || player->momentum.speedY < SMBX_BGO::GetRaw(climbingBGO)->momentum.speedY - 0.1;
    }
}

void __stdcall runtimeHookUpdateBGOMomentum(int bgoId, int layerId) {
    SMBX_BGO::GetRaw(bgoId)->momentum.speedX = Layer::Get(layerId)->xSpeed;
    SMBX_BGO::GetRaw(bgoId)->momentum.speedY = Layer::Get(layerId)->ySpeed;
}

void __stdcall runtimeHookPlayerKillLava(short* playerIdxPtr)
{
    if (gLavaIsWeak)
    {
        native_harmPlayer(playerIdxPtr);
    }
    else
    {
        native_killPlayer(playerIdxPtr);
    }
}

// variables used for counting collisions for weak_lava harm detection
int weakLavaTotalCollisions = 0; // total number of hitspot = 1 collisions
int weakLavaLavaCollisions = 0; // total nunmber of lava hitspot = 1 collisions

static bool __stdcall runtimeHookPlayerKillLavaSolidExitImpl(int hitSpot, int blockIdx, short* playerIdxPtr)
{
    auto& player = *Player::Get(*playerIdxPtr);
    auto& block = *Block::GetRaw(blockIdx);
    if (!gLavaIsWeak)
    {
        if ((!((player.MountType == 1) && (player.MountColor == 2))) &&
            ((block.BlockType > Block::MAX_ID) || (blockdef_floorslope[block.BlockType] == 0))
            )
        {
            // Conditions are met for the check that makes lava 'generous'
            if ((player.momentum.y + player.momentum.height) < (block.momentum.y + 6))
            {
                return false;
            }
        }
        native_killPlayer(playerIdxPtr);
        return false;
    }
    else
    {
        // Weak lava
        if (hitSpot == 1)
        {
            // Count top collisions,
            // we only hurt the player if the number of lava collisions equals the total number of top collisions
            // otherwise, the player would be harmed in cases where they are also standing on another block, which is not desirable!
            weakLavaLavaCollisions += 1;
        }
        else
        {
            native_harmPlayer(playerIdxPtr);
        }
        return true; // Treat weak lava as solid
    }
}

_declspec(naked) void __stdcall runtimeHookPlayerKillLavaSolidExit(short* playerIdxPtr)
{
    __asm {
        movsx esi, word ptr ss : [ebp - 0x120]
        push esi // Block index
        movsx esi, word ptr ss : [ebp - 0x54]
        push esi // HitSpot
        call runtimeHookPlayerKillLavaSolidExitImpl
        cmp al, 0 // Check if return value is false
        je runtimeHookPlayerKillLavaSolidExit_IsFalse
        push 0x9A3BDD // Return address for a execution path that makes it so the lava is treated as solid
        ret
    runtimeHookPlayerKillLavaSolidExit_IsFalse:
        push 0x9A5015 // Normal return address, treats lava that would harm as passthrough
        ret
    }
}


_declspec(naked) void __stdcall runtimeHookPlayerCountCollisionsForWeakLava(short* playerIdxPtr)
{
    __asm {
        push eax
        movsx eax, word ptr ss : [ebp - 0x54] // Get hitspot
        cmp ax, 1 // Check if hit from above
        jne runtimeHookPlayerCountCollisionsForWeakLava_NotHitSpot1 // skip if not hitspot 1
        // if it's hitspot 1, increment our total collision counter
        inc weakLavaTotalCollisions
        // clean up
    runtimeHookPlayerCountCollisionsForWeakLava_NotHitSpot1:
        pop eax
        ret
    }
}


int __stdcall runtimeHookCharacterIdTranslateHook(short* idPtr);
static void __stdcall runtimeHookPlayerBlockCollisionEndInternal(PlayerMOB* player)
{
    // if weak lava is enabled
    if (gLavaIsWeak)
    {
        // detect if the player is ONLY standing on lava, by counting all blocks stood on
        if (weakLavaLavaCollisions > 0 && weakLavaLavaCollisions >= weakLavaTotalCollisions)
        {
            // UGLY!!!!!!!! NEVER DO THIS!
            // convert the player back to its array index
            short playerIdx = ((int)(player - (PlayerMOB*)GM_PLAYERS_PTR));
            // harm player
            native_harmPlayer(&playerIdx);
        }
    }
    // reset counters for next collision loop
    weakLavaLavaCollisions = 0;
    weakLavaTotalCollisions = 0;
}
// ran post-block-collision-loop
// ALSO HANDLES CHARACTER ID TRANSLATION for link ducking check.
_declspec(naked) void __stdcall runtimeHookPlayerBlockCollisionEnd()
{
    // most of the anatomy of this hook is copied from RuntimeHookCharacterId.cpp
    __asm {
        // ASM_ARG(ebx + 0xF0)
        pushfd // set up ...
        push eax
        push ecx
        push edx

        // CODE FOR WEAK LAVA CHECK ////////////////////////////////
        push ebx // pointer to player object
        call runtimeHookPlayerBlockCollisionEndInternal
        ///////////////////////////////////////////////////////////
        
        // call the normal characterIdTranslateHook
        lea eax, dword ptr ds : [ebx + 0xF0]
        push eax
        call runtimeHookCharacterIdTranslateHook
        
        // ASM_TAIL_CMP_5
        pop edx // clean up...
        pop ecx
        cmp ax, 5
        pop eax
        lea esp, dword ptr ds : [esp + 4] 
        ret
    }
}

// fixes a crash when holding duck and releasing a veggie into a ? block whose idx exceeded the range of the npc array
_declspec(naked) void __stdcall runtimeHookFixVeggieBlockCrash()
{
    __asm {
        // instructions this hook overwrites:
        cmp word ptr ds : [ecx], ax
        je __runtimeHookFixVeggieBlockCrash_skipNPCIDCheck

        // move harm reason into ax register
        push eax // store eax register temporarily
        mov eax, dword ptr ss : [ebp + 0xC]
        mov ax, word ptr ds : [eax]
        // check if harm reason 4..
        cmp ax, 4
        pop eax // restore eax
        jne __runtimeHookFixVeggieBlockCrash_skipNPCIDCheck // jump to exit early if not reason 4

        // otherwise continue execution as normal if reason = 4
        push 0xA2B22E
        ret

        // short-circuit the check
    __runtimeHookFixVeggieBlockCrash_skipNPCIDCheck:
        push 0xA2B26E
        ret
    }
}


// fix link being allowed to turn into a fairy while in clowncar, killing him instantly - for leaf powerup
_declspec(naked) void __stdcall runtimeHookFixLinkFairyClowncar1()
{
    __asm {
        // this check is overwritten by this hook
        cmp word ptr ds : [ebx + 0x34], si
        jne __runtimeHookFixLinkFairyClowncar1_checkFailedRet
        // check mount == 0
        cmp word ptr ds : [ebx + 0x108], si
        jne __runtimeHookFixLinkFairyClowncar1_checkFailedRet

        // if both checks succeed, resume code execution at normal address
        push 0x99F6F0
        ret

    __runtimeHookFixLinkFairyClowncar1_checkFailedRet:
        // resume code execution where the check would fail to
        push 0x99F7B1
        ret
    }
}
// fix link being allowed to turn into a fairy while in clowncar, killing him instantly - for climbing npcs
_declspec(naked) void __stdcall runtimeHookFixLinkFairyClowncar2()
{
    __asm {
        // this check is overwritten by this hook
        cmp word ptr ds : [ecx + 0x140], 0
        jne __runtimeHookFixLinkFairyClowncar2_checkFailedRet
        // check mount == 0
        cmp word ptr ds : [ecx + 0x108], 0
        jne __runtimeHookFixLinkFairyClowncar2_checkFailedRet

        // if both checks succeed, resume code execution at normal address
        push 0x9AAFA8
        ret

        __runtimeHookFixLinkFairyClowncar2_checkFailedRet :
        // resume code execution where the check would fail to
        push 0x9AB2FF
            ret
    }
}
// ALSO climbing npc related
_declspec(naked) void __stdcall runtimeHookFixLinkFairyClowncar3()
{
    __asm {
        // this check is overwritten by this hook
        cmp word ptr ds : [ebx + 0xF2], ax
        jne __runtimeHookFixLinkFairyClowncar3_checkFailedRet
        // check mount == 0
        cmp word ptr ds : [ebx + 0x108], 0
        jne __runtimeHookFixLinkFairyClowncar3_checkFailedRet

        // if both checks succeed, resume code execution at normal address
        push 0x9A75D2
        ret

        __runtimeHookFixLinkFairyClowncar3_checkFailedRet :
        // resume code execution where the check would fail to
        push 0x9A78B6
            ret
    }
}


// close the game
// don't bother with preserving cpu state or anything, since we'll never return from here...
void __stdcall runtimeHookCloseGame()
{
    _exit(0);
}

extern PlayerMOB* getTemplateForCharacter(int id);
extern "C" void __cdecl LunaLuaSetGameData(const char* dataPtr, int dataLen);
static void LunaLuaResetEpisode() {
    // show loadscreen while re-loading everything
    LunaLoadScreenStart();
    // re-load world
    auto ep = EpisodeListItem::Get(GM_CUR_MENULEVEL - 1);
    VB6StrPtr pathVb6 = std::string(ep->episodePath) + std::string(ep->episodeWorldFile);
    native_loadWorld(&pathVb6);
    // re-load save
    if (saveFileExists())
    {
        native_loadGame();
    }
    // put the player back on the world map
    GM_EPISODE_MODE = COMBOOL(true);
    GM_TITLE_INTRO_MODE = COMBOOL(false);
    GM_LEVEL_MODE = COMBOOL(false);
    // reset checkpoints
    GM_STR_CHECKPOINT = "";
    // set flag for lua read
    gDidGameOver = true;
    // clear gamedata
    LunaLuaSetGameData(0, 0);
    // restore players' characters
    for (int i = 1; i <= GM_PLAYERS_COUNT; i++) {
        auto p = Player::Get(i);
        // restore this player's character
        p->Identity = playerStoredCharacters[min(i, 4)-1];
        // apply saved template
        auto t = getTemplateForCharacter(p->Identity);
        if (t != nullptr) {
            memcpy(p, t, sizeof(PlayerMOB));
        }
    }

    // hide loadscreen
    LunaLoadScreenKill();
}

// run standard lunadll loop stuff from credits
static const auto native_OutroLoop = (void(__stdcall *)())0x8F6D20;
void __stdcall runtimeHookCreditsLoop() {
    TestFunc();
    native_OutroLoop();
    if (GM_CREDITS_MODE == 0)
    {
        // finished credits!
        LunaLuaResetEpisode();
    }
}

// ran when gameover occurs
extern void LunaLuaGameoverScreenRun();
void __stdcall runtimeHookGameover() {
    LunaLuaGameoverScreenRun();
    LunaLuaResetEpisode();
}
