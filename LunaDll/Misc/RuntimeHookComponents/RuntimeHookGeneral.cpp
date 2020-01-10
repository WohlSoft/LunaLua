#include "../../Main.h"
#include "../RuntimeHook.h"
#include <comutil.h>
#include "../../GlobalFuncs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../SdlMusic/MusicManager.h"
#include "../../HardcodedGraphics/HardcodedGraphicsManager.h"
#include "../ErrorReporter.h"
#include "../../GameConfig/GameConfiguration.h"
#include "../../Globals.h"
#include "../AsmPatch.h"

#include "../SHMemServer.h"

#include "../TestMode.h"
#include "../../IPC/IPCPipeServer.h"
#include "../../Rendering/ImageLoader.h"

#include "../NpcIdExtender.h"

#include "../../Misc/LoadScreen.h"

#ifndef NO_SDL
bool episodeStarted = false;
#endif

void(*runAsyncDebuggerProc)(void) = 0;
void(*runAsyncLoggerProc)(void) = 0;
int(*asyncBitBltProc)(HDC, int, int, int, int, HDC, int, int, unsigned int) = 0;
void(*asyncLogProc)(const char*) = 0;
float(*__vbaR4Var)(VARIANTARG*) = 0;
int(__stdcall *rtcMsgBox)(VARIANTARG*, DWORD, DWORD, DWORD, DWORD) = 0;

HMODULE newLauncherLib = 0;
HMODULE newDebugger = 0;
HHOOK HookWnd;
HHOOK KeyHookWnd;

#include <CommCtrl.h>

static void NpcBitbltPatch(void);
static unsigned int __stdcall LatePatch(void);

void SetupThunRTMainHook()
{
    // Remove protection on smbx.text section
    DWORD oldprotect;
    VirtualProtect((void*)0x401000, 0x724000, PAGE_EXECUTE_READWRITE, &oldprotect);

    // Set up hook that will launch LunaDLLInit
    PATCH(0x40BDDD).CALL(&ThunRTMainHook).Apply();
}

LRESULT CALLBACK MsgHOOKProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0){
        return CallNextHookEx(HookWnd, nCode, wParam, lParam);
    }

    LPCWPSTRUCT wData = (LPCWPSTRUCT)lParam;

    switch (wData->message)
    {
    case WM_COPYDATA:
        {
            PCOPYDATASTRUCT pcds = reinterpret_cast<PCOPYDATASTRUCT>(wData->lParam);
            if (pcds->cbData == 1) {
                if (pcds->dwData == 0xDEADC0DE) {
                    std::wstring lvlName = gShMemServer.read();
                    if (!lvlName.empty()) {
                        GM_FULLPATH = lvlName;
                    }
                    gHook_SkipTestMsgBox = true;
                    ((void(*)())0x00A02220)();
                }
            }
        }
        break;
    case WM_CREATE:
        if (wData->lParam != NULL)
        {
            CREATESTRUCTA* createData = reinterpret_cast<CREATESTRUCTA*>(wData->lParam);
            LPCSTR winName = createData->lpszName;
            if ((gMainWindowHwnd == NULL) && (winName != NULL) && (strncmp("- Version 1.2.2 -", &winName[20], 17) == 0))
            {
                // Store main window handle
                gMainWindowHwnd = wData->hwnd;
            }
        }
        break;
    case WM_DESTROY:
        if ((gMainWindowHwnd != NULL) && (gMainWindowHwnd == wData->hwnd))
        {
            // Our main window was destroyed? Clear hwnd and mark as unfocused
            gMainWindowHwnd = NULL;
            gMainWindowFocused = false;
        }
        break;
    case WM_SETFOCUS:
        if ((gMainWindowHwnd != NULL) && (gMainWindowHwnd == wData->hwnd))
        {
            // Our main window gained focus? Keep track of that.
            gMainWindowFocused = true;
        }
        break;
    case WM_KILLFOCUS:
        if ((gMainWindowHwnd != NULL) && (gMainWindowHwnd == wData->hwnd))
        {
            // Our main window lost focus? Keep track of that.
            gMainWindowFocused = false;
        }
        break;
    default:
        break;
    }
    
    return CallNextHookEx(HookWnd, nCode, wParam, lParam);
}

void ParseArgs(const std::vector<std::wstring>& args)
{

    if (vecStrFind(args, L"--patch"))
        gStartupSettings.patch = true;

    if (vecStrFind(args, L"--game"))
        gStartupSettings.game = true;

    if (vecStrFind(args, L"--leveleditor"))
        gStartupSettings.lvlEditor = true;

    if (vecStrFind(args, L"--noframeskip"))
        gStartupSettings.frameskip = false;

    if (vecStrFind(args, L"--nosound"))
        gStartupSettings.noSound = true;

    if (vecStrFind(args, L"--debugger"))
        gStartupSettings.debugger = true;

    if (vecStrFind(args, L"--logger"))
        gStartupSettings.logger = true;

    if (vecStrFind(args, L"--newlauncher"))
        gStartupSettings.newLauncher = true;

    if (vecStrFind(args, L"--console"))
        gStartupSettings.console = true;

    if (vecStrFind(args, L"--nogl"))
        gStartupSettings.noGL = true;

    for (unsigned int i = 0; i < args.size(); i++)
    {
        const std::wstring& arg = args[i];
        if (arg.find(L"--testLevel=") == 0)
        {
            STestModeSettings settings;
            settings.levelPath = arg.substr(12);
            testModeEnable(settings);
            gStartupSettings.patch = true;
            break;
        }
    }

    if (vecStrFind(args, L"--waitForIPC"))
    {
        gStartupSettings.waitForIPC = true;
        gStartupSettings.currentlyWaitingForIPC = true;
        gStartupSettings.patch = true;
    }

    if (vecStrFind(args, L"--hideOnCloseIPC"))
    {
        gStartupSettings.waitForIPC = true;
        gStartupSettings.patch = true;
    }

    if (vecStrFind(args, L"--oldLvlLoader"))
    {
        gStartupSettings.oldLvlLoader = true;
    }
}

static unsigned int __stdcall LatePatch(void)
{
    /************************************************************************/
    /* Engine Limit patches                                                 */
    /************************************************************************/

    // Runs this in LatePatch because we can only overwrite the pointer to the
    // warp array after it's been assigned to in the first place.
    fixup_WarpLimit();

    // Patch the event limit...
    fixup_EventLimit();

    // Patch the layer limit...
    fixup_LayerLimit();

    // Set new NPC ID limit...
    PatchNpcIdLimit();

    // Set new BGO ID limit...
    PatchBgoIdLimit();

    // Set new Block ID limit...
    PatchBlockIdLimit();

    // Run this in LatePatch because overwriting the SEH handler only works
    // after we have the VB runtime running.
    fixup_ErrorReporting();

    // Be sure that all values are init.
    ResetLunaModule();

    if (gStartupSettings.console)
        RedirectIOToConsole();

    /* Do what the place we patched this in is supposed to do: */
    /* 008BEC61 | mov eax,dword ptr ds:[B2D788] */
    return *((unsigned int*)(0xB2D788));
}

static IPCPipeServer ipcServer;
void TrySkipPatch()
{
    //Check for arguments and write them in gStartupSettings
    ParseArgs(splitCmdArgsW(std::wstring(GetCommandLineW())));

    // If we have stdin/stdout, attach to the IPC server
    ipcServer.AttachStdinStdout();

    if (gStartupSettings.patch){
        PATCH(0x8BECF2).NOP_PAD_TO_SIZE<0x1B5>().Apply(); //nop out the loader code
        *(WORD*)(0xB25046) = -1; //set run to true

        PATCH(0x8BED00).CALL(&InitHook).Apply();
    }

    // Init freeimage:
    gFreeImgInit.init();

    // Insert callback for patching which must occur after the runtime has started
    // (0x8BEC61 is not quite as early as would be ideal for this, but it's convenient)
    PATCH(0x8BEC61).CALL(&LatePatch).Apply();

    //Load graphics from the HardcodedGraphicsManager
    HardcodedGraphicsManager::loadGraphics();

    // Either in root or in config folder. The config folder is recommended however.
    gGeneralConfig.setFilename(getLatestConfigFile(L"luna.ini"));
    gGeneralConfig.loadOrDefault();
    //game.ini reader
    GameConfiguration::runPatchByIni(IniProcessing(WStr2Str(getLatestConfigFile(L"game.ini"))));

    /************************************************************************/
    /* Simple ASM Source Patches                                            */
    /************************************************************************/
    fixup_TypeMismatch13();
    fixup_WebBox();
    fixup_Credits();
    fixup_Mushbug();
    fixup_Veggibug();
    fixup_NativeFuncs();
    fixup_BGODepletion();
    fixup_RenderPlayerJiterX();

    /************************************************************************/
    /* Replaced Imports                                                     */
    /************************************************************************/
    IMP_vbaStrCmp = &replacement_VbaStrCmp;

    /************************************************************************/
    /* Set Hook                                                             */
    /************************************************************************/
    HookWnd = SetWindowsHookExA(WH_CALLWNDPROC, MsgHOOKProc, (HINSTANCE)NULL, GetCurrentThreadId());
    if (!HookWnd){
        DWORD errCode = GetLastError();
        std::string errCmd = "Failed to Hook";
        errCmd += "\nErr-Code: ";
        errCmd += std::to_string((long long)errCode);
        MessageBoxA(NULL, errCmd.c_str(), "Failed to Hook", NULL);
    }

    KeyHookWnd = SetWindowsHookExA(WH_KEYBOARD, KeyHOOKProc, (HINSTANCE)NULL, GetCurrentThreadId());
    if (!KeyHookWnd){
        DWORD errCode = GetLastError();
        std::string errCmd = "Failed to Hook";
        errCmd += "\nErr-Code: ";
        errCmd += std::to_string((long long)errCode);
        MessageBoxA(NULL, errCmd.c_str(), "Failed to Hook", NULL);
    }

    /************************************************************************/
    /* Source Code Function Patch                                           */
    /************************************************************************/
    PATCH(0x8D9446)
        .CALL(&OnLvlLoad)
        .NOP()
        .NOP()
        .Apply();

    PATCH(0x8CA23B)
        .CALL(&TestFunc)
        .NOP()
        .Apply();

    PATCH(0x92EC24)
        .CALL(&LevelHUDHook)
        .Apply();

    *(void**)0xB2F244 = (void*)&mciSendStringHookA;

    PATCH(0x8D6BB6).CALL(&forceTermination).Apply();

    PATCH(0x8C11D5).CALL(&LoadWorld).Apply();

    PATCH(0x8C16F7).CALL(&WorldLoop).Apply();

    PATCH(0x8C0E6D).CALL(&LoadIntro).Apply();

    PATCH(0x932353).CALL(&printLunaLuaVersion).Apply();

    PATCH(0x9090F5).CALL(&WorldRender).Apply();

    PATCH(0x9204E5).CALL(&NPCKillHook).Apply();
    PATCH(0x9B4E35).CALL(&NPCKillHook).Apply();
    PATCH(0xA0664E).CALL(&NPCKillHook).Apply();
    PATCH(0xA23278).CALL(&NPCKillHook).Apply();

    PATCH(0xA3A679).CALL(runtimeHookNPCRemovalConfirmHook).NOP_PAD_TO_SIZE<6>().Apply();

    PATCH(0xAA4352)
        .CALL(&__vbaStrCmp_TriggerSMBXEventHook)
        .NOP()
        .Apply();

    PATCH(0x8C23CB)
        .CALL(&checkLevelShutdown)
        .NOP()
        .NOP()
        .Apply();

    PATCH(0xA755D2).CALL(&UpdateInputHook_Wrapper).Apply();


    PATCH(0x902D3D).CALL(&WorldOverlayHUDBitBltHook).Apply();
    PATCH(0x902DFC).CALL(&WorldOverlayHUDBitBltHook).Apply();
    PATCH(0x902EBB).CALL(&WorldOverlayHUDBitBltHook).Apply();
    PATCH(0x902F80).CALL(&WorldOverlayHUDBitBltHook).Apply();

    PATCH(0x908995).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9087A8).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9085BB).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9083CE).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x908115).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x907F28).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x907D3B).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x907B4E).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9077FD).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x907537).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9072B2).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x90702D).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x906DB2).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9055CE).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x905304).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9051A7).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x905055).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x904F24).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x908995).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x904D4F).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9062E0).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x906183).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x906031).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x905F00).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x905D29).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x905990).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9065DE).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x906973).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x90499A).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9046D0).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x904573).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x904421).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9042F0).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x90411B).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x906B31).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x903D66).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9063FF).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x903A9C).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x90393F).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9037ED).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9036BC).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9034E7).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9032E9).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x90323D).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x90319F).CALL(&WorldIconsHUDBitBltHook).Apply();
    PATCH(0x9030F2).CALL(&WorldIconsHUDBitBltHook).Apply();


    PATCH(0x9000B2).CALL(&WorldHUDIsOnCameraHook).Apply();
    PATCH(0x900235).CALL(&WorldHUDIsOnCameraHook).Apply();
    PATCH(0x9004B7).CALL(&WorldHUDIsOnCameraHook).Apply();
    PATCH(0x90068F).CALL(&WorldHUDIsOnCameraHook).Apply();

    PATCH(0x901439).CALL(&WorldHUDPrintTextController).Apply();
    PATCH(0x90266A).CALL(&WorldHUDPrintTextController).Apply();
    PATCH(0x907611).CALL(&WorldHUDPrintTextController).Apply();
    PATCH(0x9081E7).CALL(&WorldHUDPrintTextController).Apply();
    PATCH(0x908B03).CALL(&WorldHUDPrintTextController).Apply();
    PATCH(0x908A67).CALL(&WorldHUDPrintTextController).Apply();

    PATCH(0x909217).CALL(&GenerateScreenshotHook).Apply();
    PATCH(0x94D5E7).CALL(&GenerateScreenshotHook).Apply();

    PATCH(0x8C03DC).CALL(&InitLevelEnvironmentHook).Apply();
    PATCH(0x8C0A1A).CALL(&InitLevelEnvironmentHook).Apply();
    PATCH(0x8C1383).CALL(&InitLevelEnvironmentHook).Apply();
    PATCH(0x8C1953).CALL(&InitLevelEnvironmentHook).Apply();
    PATCH(0x8CE292).CALL(&InitLevelEnvironmentHook).Apply();
    PATCH(0x8E61BD).CALL(&InitLevelEnvironmentHook).Apply();
    PATCH(0x8FE8D4).CALL(&InitLevelEnvironmentHook).Apply();
    PATCH(0x987E94).CALL(&InitLevelEnvironmentHook).Apply();
    PATCH(0x9B7B2C).CALL(&InitLevelEnvironmentHook).Apply();
    PATCH(0xA02AD3).CALL(&InitLevelEnvironmentHook).Apply();

    //PATCH(0x4242D0).JMP(GET_RETADDR_TRACE_HOOK<&BitBltTraceHook>()).Apply();
    PATCH(0x4242D0).JMP(&BitBltHook).Apply();
    PATCH(0x424314).JMP(&StretchBltHook).Apply();

    PATCH(0x8E54C0)
        .JMP(&runtimeHookMsgbox)
        .NOP_PAD_TO_SIZE<6>()
        .Apply();

    PATCH(0x9B22BF)
        .CALL(&runtimeHookNpcMsgbox_Wrapper)
        .NOP_PAD_TO_SIZE<5>()
        .Apply();

    // ignoreThrownNPCs check
    PATCH(0xA1AA52)
        .CALL(&runtimeHookIgnoreThrownNPCs_Wrapper)
        .NOP_PAD_TO_SIZE<35>()
        .Apply();

    // linkShieldable check
    PATCH(0xA52CB5)
        .CALL(&runtimeHookLinkShieldable_Wrapper)
        .NOP_PAD_TO_SIZE<56>()
        .Apply();

    // noShieldFireEffect check
    PATCH(0xA530B7)
        .CALL(&runtimeHookNoShieldFireEffect_Wrapper)
        .NOP_PAD_TO_SIZE<56>()
        .Apply();


    // Okay redigit, I know your debug values are in general pretty dumb, but right now they are awesome for easy patching! Thx mate!
    PATCH(0x90C856)
        .CALL(&CameraUpdateHook_Wrapper)
        .NOP()
        .NOP()
        .Apply();

    // Hook for after camera updates have finished, just before drawing the background
    PATCH(0x90D6FE)
        .CALL(&PostCameraUpdateHook_Wrapper)
        .NOP_PAD_TO_SIZE<7>()
        .Apply();

    // Hook to fix 100% CPU when window is inactive
    PATCH(0x8E6FE1)
        .NOP()
        .CALL(&WindowInactiveHook)
        .Apply();

    // PATCH(0x96CC61).TRACE_CALL<&HardcodedGraphicsBitBltHook>().Apply();

    // Don't trust QPC as much on WinXP
    void* frameTimingHookPtr;
    void* frameTimingMaxFPSHookPtr;
    if (gIsWindowsVistaOrNewer) {
        frameTimingHookPtr = (void*)&FrameTimingHookQPC;
        frameTimingMaxFPSHookPtr = (void*)&FrameTimingMaxFPSHookQPC;
    }
    else {
        frameTimingHookPtr = (void*)&FrameTimingHook;
        frameTimingMaxFPSHookPtr = (void*)&FrameTimingMaxFPSHook;
    }

    // Hooks to fix 100% CPU during operation
    // These ones are normally not sensitive to the "max FPS" setting
    PATCH(0x8BFD4A).SAFE_CALL(frameTimingHookPtr).NOP_PAD_TO_SIZE<0x40>().Apply();
    PATCH(0x8C0488).SAFE_CALL(frameTimingHookPtr).NOP_PAD_TO_SIZE<0x40>().Apply();
    PATCH(0x8C0EE6).SAFE_CALL(frameTimingHookPtr).NOP_PAD_TO_SIZE<0x40>().Apply();

    // These ones are normally sensitive to the "max FPS" setting
    PATCH(0x8C15A7).SAFE_CALL(frameTimingMaxFPSHookPtr).NOP_PAD_TO_SIZE<0x4A>().Apply();
    PATCH(0x8C20FC).SAFE_CALL(frameTimingMaxFPSHookPtr).NOP_PAD_TO_SIZE<0x4A>().Apply();
    PATCH(0x8E2AED).SAFE_CALL(frameTimingMaxFPSHookPtr).NOP_PAD_TO_SIZE<0x4A>().Apply();
    PATCH(0x8E56ED).SAFE_CALL(frameTimingMaxFPSHookPtr).NOP_PAD_TO_SIZE<0x4A>().Apply();

    // Level and world render hooks
    PATCH(0x909290).JMP(RenderLevelHook).NOP().Apply();
    PATCH(0x8FEB10).JMP(RenderWorldHook).NOP().Apply();

    // Disable some frivolous RenderLevel calls in vanilla code. Was causing excess onDraw compared to onTick
    PATCH(0x9BBC95).NOPS<5>().Apply(); // Tail hitting blocks case
    PATCH(0xA2C6C1).NOPS<5>().Apply(); // Unknown (birdo hit sound related)
    PATCH(0xA53053).NOPS<5>().Apply(); // Link shield case

    // Level rendering layering hooks

    //PATCH(0x90C856).NOP().NOP().CALL(GetRenderBelowPriorityHook<-95>()).Apply();
    //-100: Level Background
    PATCH(0x90F4FA).NOP().NOP().CALL(GetRenderBelowPriorityHookWithSkip<-95, 0x910433, &gRenderBGOFlag>()).Apply();
    // -95: Furthest back BGOs
    PATCH(0x910433).NOP().NOP().CALL(GetRenderBelowPriorityHookWithSkip<-90, 0x911F19, &gRenderSizableFlag>()).Apply();
    // -90: Sizable Blocks
    PATCH(0x910E5D).NOP().NOP().CALL(GetRenderBelowPriorityHookWithSkip<-85, 0x911F19, &gRenderBGOFlag>()).Apply();
    // -85: Some more BGOs
    PATCH(0x911F19).NOP().NOP().CALL(GetRenderBelowPriorityHookWithSkip<-80, 0x912748, &gRenderBGOFlag>()).Apply();
    // -80: Warp - Derived BGOs (locks on doors and stuff)
    PATCH(0x912748).NOP().NOP().CALL(GetRenderBelowPriorityHook<-75>()).Apply();
    // -75: Background NPCs (vines, piranah plants, diggable sand, mother brain, things in MB jars)
    PATCH(0x915316).NOP().NOP().CALL(GetRenderBelowPriorityHook<-70>()).Apply();
    // -70: Held NPCs
    PATCH(0x91D422).NOP().NOP().CALL(GetRenderBelowPriorityHook<-65>()).Apply();
    // -65: Normal Blocks
    PATCH(0x91DD44).NOP().NOP().CALL(GetRenderBelowPriorityHook<-60>()).Apply();
    // -60: Furthest Back Effects (doors, pressed p-switches, some other stuff)
    PATCH(0x91E1F2).NOP().NOP().CALL(GetRenderBelowPriorityHook<-55>()).Apply();
    // -55: Some NPCs (i.e. coins, clown car, chompy, herb, wood rocket, koopaling fire)
    PATCH(0x91F802).NOP().NOP().CALL(GetRenderBelowPriorityHook<-50>()).Apply();
    // -50: Some NPCs (ice blocks)
    PATCH(0x920040).NOP().NOP().CALL(GetRenderBelowPriorityHook<-45>()).Apply();
    // -45: Normal NPCs
    PATCH(0x922D00).NOP().NOP().CALL(GetRenderBelowPriorityHook<-40>()).Apply();
    // -40: Symbol above NPCs that want to chat (hardcoded-43/44)
    PATCH(0x923786).NOP().NOP().CALL(GetRenderBelowPriorityHook<-35>()).Apply();
    // -35: Player Mounts
    PATCH(0x927F21).NOP().NOP().CALL(GetRenderBelowPriorityHook<-30>()).Apply();
    // -30: Something else player mount related?
    PATCH(0x928EA5).NOP().NOP().CALL(GetRenderBelowPriorityHook<-25>()).Apply();
    // -25: Players
    PATCH(0x928F0A).NOP().NOP().CALL(GetRenderBelowPriorityHookWithSkip<-20, 0x929F81, &gRenderBGOFlag>()).Apply();
    // -20: Foreground BGOs
    PATCH(0x929F81).NOP().NOP().CALL(GetRenderBelowPriorityHook<-15>()).Apply();
    // -15: Foreground NPCs
    PATCH(0x92B428).NOP().NOP().CALL(GetRenderBelowPriorityHook<-10>()).Apply();
    // -10: Foreground Blocks
    PATCH(0x92BAC0).NOP().NOP().CALL(GetRenderBelowPriorityHook<-5>()).Apply();
    //  -5: Foreground Effects (all not at 0091DD90)
    // Handle Priority 5 from LevelHUDHook
    //   5: HUD
    PATCH(0x939977).NOP().NOP().CALL(GetRenderBelowPriorityHook<100>()).Apply();

    // Change Mode Hook
    // Runs when the game starts or the game mode changes.
    PATCH(0x8BF4E3).CALL(runtimeHookSmbxChangeModeHookRaw).NOP_PAD_TO_SIZE<10>().Apply();

    // Load level hook
    PATCH(0x8D8F40).JMP(runtimeHookLoadLevel).NOP_PAD_TO_SIZE<6>().Apply();

    // .lvl header reading hooks
    PATCH(0x8F7E8E).PUSH_EBX().CALL(runtimeHookLoadLevelHeader).NOP_PAD_TO_SIZE<12>().Apply();
    PATCH(0x8F7EA3).JMP(0x8F7EDB).NOP_PAD_TO_SIZE<56>().Apply();

    // Save game hook
    PATCH(0x8E47D0).JMP(runtimeHookSaveGame).NOP_PAD_TO_SIZE<6>().Apply();

    PATCH(0x8DC6E0).JMP(runtimeHookCleanupLevel).NOP_PAD_TO_SIZE<6>().Apply();

    PATCH(0x8D6BB0).JMP(runtimeHookExitMainGame).NOP_PAD_TO_SIZE<6>().Apply();

    PATCH(0x8DF5B0).JMP(runtimeHookLoadWorld).NOP_PAD_TO_SIZE<6>().Apply();

    PATCH(0x8E2E40).JMP(runtimeHookCleanupWorld).NOP_PAD_TO_SIZE<6>().Apply();

    // Close window hook
    PATCH(0x8BE3DA).CALL(runtimeHookCloseWindow).Apply();

    // Anti-Fullscreen hook
    PATCH(0x95429A).CALL(runtimeHookChangeResolution).Apply();
    PATCH(0xA98142).CALL(runtimeHookChangeResolution).Apply();
    PATCH(0xA98166).CALL(runtimeHookChangeResolution).Apply();
    PATCH(0x96ADD7).CALL(runtimeHookSmbxCheckWindowedRaw).NOP_PAD_TO_SIZE<8>().Apply();

    PATCH(0x9DB1D8).JMP(runtimeHookBlockBumpableRaw).NOP_PAD_TO_SIZE<6>().Apply();

    PATCH(0xA28FE3).JMP(runtimeHookNPCVulnerabilityRaw).Apply();

    PATCH(0x9A9D33).JMP(runtimeHookNPCSpinjumpSafeRaw).NOP_PAD_TO_SIZE<10>().Apply();
    PATCH(0xA0A991).JMP(runtimeHookNPCNoWaterPhysicsRaw).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xA0A6FB).CALL(runtimeHookNPCWaterSplashAnimRaw).Apply();

    PATCH(0xA0B969).JMP(runtimeHookNPCHarmlessGrabRaw).NOP_PAD_TO_SIZE<183>().Apply();
    PATCH(0xA181AD).JMP(runtimeHookNPCHarmlessThrownRaw).NOP_PAD_TO_SIZE<6>().Apply();

    PATCH(0xA75079).JMP(runtimeHookCheckInputRaw).NOP_PAD_TO_SIZE<7>().Apply();

    //-----------------------------------------------------------------------//
    // In general, we want to disable default graphics loading code, which   //
    // means nop-ing out the calls except for ones we want to hook.          //
    // ----------------------------------------------------------------------//

    // Calls to loadDefaultGraphics
    PATCH(0x8BF327).CALL(&runtimeHookLoadDefaultGraphics).Apply();
    // Calls to loadDefaultWorldGraphics
    PATCH(0x8DEF6E).NOP_PAD_TO_SIZE<5>().Apply();
    PATCH(0x8E3204).NOP_PAD_TO_SIZE<5>().Apply();
    // Calls to unkLoadGraphics
    PATCH(0x8D8BEC).NOP_PAD_TO_SIZE<5>().Apply();
    PATCH(0x8DC7BB).NOP_PAD_TO_SIZE<5>().Apply();
    PATCH(0x8DF526).NOP_PAD_TO_SIZE<5>().Apply();
    PATCH(0x8E31FF).NOP_PAD_TO_SIZE<5>().Apply();
    // Calls to unkLoadLocalGraphics
    PATCH(0x8D8C78).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x8D978A).NOP_PAD_TO_SIZE<6>().Apply();
    // Calls to clearGraphics
    PATCH(0x8D6CA0).NOP_PAD_TO_SIZE<5>().Apply();
    // Calls to loadLocalGraphics
    PATCH(0x8C137E).CALL(&LoadLocalGfxHook).Apply();
    PATCH(0x8D8BF1).CALL(&LoadLocalGfxHook).Apply();
    PATCH(0x8D9611).CALL(&LoadLocalGfxHook).Apply();
    PATCH(0x8DF52B).CALL(&LoadLocalGfxHook).Apply();
    PATCH(0x8DFF7C).CALL(&LoadLocalGfxHook).Apply();
    // Calls to loadCustomWorldGraphics
    PATCH(0x8DEF73).CALL(&LoadLocalOverworldGfxHook).Apply();
    PATCH(0x8DF808).CALL(&LoadLocalOverworldGfxHook).Apply();

    // Implement hook for when the render target HDC is set...
    PATCH(0x94F71A).CALL(&runtimeHookSetHDCRaw).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x96B4B0).CALL(&runtimeHookSetHDCRaw).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0x96AE66).CALL(&runtimeHookSetHDCRaw).NOP_PAD_TO_SIZE<6>().Apply();

    // Hooks got initGameHDC and initGameWindow
    PATCH(0x8BF2AD).CALL(&runtimeHookInitGameHDC).Apply();
    PATCH(0x8BD9A0).CALL(&runtimeHookInitGameWindow).Apply();
    PATCH(0x8BF596).CALL(&runtimeHookInitGameWindow).Apply();
    PATCH(0x8CE9FA).CALL(&runtimeHookInitGameWindow).Apply();
    PATCH(0x8E6C75).CALL(&runtimeHookInitGameWindow).Apply();
    PATCH(0xA02AEE).CALL(&runtimeHookInitGameWindow).Apply();

    //Shorten reload thingy? TEMP
    PATCH(0x8C142B).NOP_PAD_TO_SIZE<10>().Apply();

    // Patch piranah divide by zero bug
    PATCH(0xA55FB3).CALL(&runtimeHookPiranahDivByZero).NOP_PAD_TO_SIZE<6>().Apply();

    // Hook block hits
    PATCH(0x9DA620).JMP(&runtimeHookHitBlock).NOP_PAD_TO_SIZE<6>().Apply();

    // Patch 16384 block bug
    PATCH(0xA98936).bytes(
        0x0F, 0xBF, 0xF0, // movsx esi,ax
        0x0F, 0xBF, 0xC1, // movsx eax,cx
        0x01, 0xF0,       // add eax,esi
        0xD1, 0xF8,       // sar eax,1
        0x89, 0xC6,       // mov esi,eax
        0xEB, 0x4B        // jmp 0xA9898F
        ).NOP_PAD_TO_SIZE<98>().Apply();

    // Enable custom load screens
    LunaLoadScreenSetEnable(true);

    // Logging for NPC collisions
    //PATCH(0xa281b0).JMP(GET_RETADDR_TRACE_HOOK<&runtimeHookLogCollideNpc>()).NOP_PAD_TO_SIZE<6>().Apply();

    // Hooks for onNPCHarm support
    PATCH(0xa281b0).JMP(&runtimeHookCollideNpc).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa291d2).JMP(&runtimeHookNpcHarmRaw_a291d8).NOP_PAD_TO_SIZE<8>().Apply();
    PATCH(0xa29272).JMP(&runtimeHookNpcHarmRaw_a29272).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa293ea).JMP(&runtimeHookNpcHarmRaw_a293ee).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa29442).JMP(&runtimeHookNpcHarmRaw_a29442).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2946b).JMP(&runtimeHookNpcHarmRaw_a2946b).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa294aa).JMP(&runtimeHookNpcHarmRaw_a294aa).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa29f8a).JMP(&runtimeHookNpcHarmRaw_a29f8a).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a24c).JMP(&runtimeHookNpcHarmRaw_a2a24c).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a304).JMP(&runtimeHookNpcHarmRaw_a2a304).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a36e).JMP(&runtimeHookNpcHarmRaw_a2a36e).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a3e3).JMP(&runtimeHookNpcHarmRaw_a2a3e3).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a450).JMP(&runtimeHookNpcHarmRaw_a2a454).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a48c).JMP(&runtimeHookNpcHarmRaw_a2a48c).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a542).JMP(&runtimeHookNpcHarmRaw_a2a542).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a55e).JMP(&runtimeHookNpcHarmRaw_a2a55e).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a57f).JMP(&runtimeHookNpcHarmRaw_a2a57f).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a5f1).JMP(&runtimeHookNpcHarmRaw_a2a5f4).NOP_PAD_TO_SIZE<5>().Apply();
    PATCH(0xa2a623).JMP(&runtimeHookNpcHarmRaw_a2a627).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a65f).JMP(&runtimeHookNpcHarmRaw_a2a662).NOP_PAD_TO_SIZE<5>().Apply();
    PATCH(0xa2a691).JMP(&runtimeHookNpcHarmRaw_a2a695).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a6d9).JMP(&runtimeHookNpcHarmRaw_a2a6d9).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a700).JMP(&runtimeHookNpcHarmRaw_a2a704).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a77e).JMP(&runtimeHookNpcHarmRaw_a2a782).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a79f).JMP(&runtimeHookNpcHarmRaw_a2a7a3).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a7d8).JMP(&runtimeHookNpcHarmRaw_a2a7db).NOP_PAD_TO_SIZE<5>().Apply();
    PATCH(0xa2a90e).JMP(&runtimeHookNpcHarmRaw_a2a90e).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2a92c).JMP(&runtimeHookNpcHarmRaw_a2a92c).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2aa57).JMP(&runtimeHookNpcHarmRaw_a2aa5b).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2aa7f).JMP(&runtimeHookNpcHarmRaw_a2aa7f).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2aaa3).JMP(&runtimeHookNpcHarmRaw_a2aaa3).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2ab38).JMP(&runtimeHookNpcHarmRaw_a2ab3c).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2ab7b).JMP(&runtimeHookNpcHarmRaw_a2ab7f).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2ab99).JMP(&runtimeHookNpcHarmRaw_a2ab99).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2ac38).JMP(&runtimeHookNpcHarmRaw_a2ac38).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2acaf).JMP(&runtimeHookNpcHarmRaw_a2acaf).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2ad2a).JMP(&runtimeHookNpcHarmRaw_a2ad2a).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2adca).JMP(&runtimeHookNpcHarmRaw_a2adca).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2adef).JMP(&runtimeHookNpcHarmRaw_a2adef).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2ae9e).JMP(&runtimeHookNpcHarmRaw_a2ae9e).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2af1e).JMP(&runtimeHookNpcHarmRaw_a2af22).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2af6e).JMP(&runtimeHookNpcHarmRaw_a2af72).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2b019).JMP(&runtimeHookNpcHarmRaw_a2b01d).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2b116).JMP(&runtimeHookNpcHarmRaw_a2b116).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2b139).JMP(&runtimeHookNpcHarmRaw_a2b139).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2b676).JMP(&runtimeHookNpcHarmRaw_a2b676).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2b8ad).JMP(&runtimeHookNpcHarmRaw_a2b8b6).NOP_PAD_TO_SIZE<11>().Apply();
    PATCH(0xa2b8c0).JMP(&runtimeHookNpcHarmRaw_a2b8c4).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2b9c0).JMP(&runtimeHookNpcHarmRaw_a2b9c0).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2ba11).JMP(&runtimeHookNpcHarmRaw_a2ba11).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2ba77).JMP(&runtimeHookNpcHarmRaw_a2ba77).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2badd).JMP(&runtimeHookNpcHarmRaw_a2badd).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2bcdc).JMP(&runtimeHookNpcHarmRaw_a2bcdc).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2bdeb).JMP(&runtimeHookNpcHarmRaw_a2bdeb).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2be20).JMP(&runtimeHookNpcHarmRaw_a2be29).NOP_PAD_TO_SIZE<11>().Apply();
    PATCH(0xa2bf41).JMP(&runtimeHookNpcHarmRaw_a2bf45).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2bfc9).JMP(&runtimeHookNpcHarmRaw_a2bfc9).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c12c).JMP(&runtimeHookNpcHarmRaw_a2c130).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c13b).JMP(&runtimeHookNpcHarmRaw_a2c13f).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c18b).JMP(&runtimeHookNpcHarmRaw_a2c18b).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c300).JMP(&runtimeHookNpcHarmRaw_a2c300).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c400).JMP(&runtimeHookNpcHarmRaw_a2c404).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c432).JMP(&runtimeHookNpcHarmRaw_a2c437).NOP_PAD_TO_SIZE<7>().Apply();
    PATCH(0xa2c4b1).JMP(&runtimeHookNpcHarmRaw_a2c4b1).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c50b).JMP(&runtimeHookNpcHarmRaw_a2c50b).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c524).JMP(&runtimeHookNpcHarmRaw_a2c524).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c537).JMP(&runtimeHookNpcHarmRaw_a2c537).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c5ad).JMP(&runtimeHookNpcHarmRaw_a2c5ad).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c63c).JMP(&runtimeHookNpcHarmRaw_a2c640).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c69b).JMP(&runtimeHookNpcHarmRaw_a2c69f).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c6bb).JMP(&runtimeHookNpcHarmRaw_a2c6bf).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c712).JMP(&runtimeHookNpcHarmRaw_a2c712).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c7ab).JMP(&runtimeHookNpcHarmRaw_a2c7ab).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c808).JMP(&runtimeHookNpcHarmRaw_a2c80c).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c81a).JMP(&runtimeHookNpcHarmRaw_a2c81e).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c888).JMP(&runtimeHookNpcHarmRaw_a2c88b).NOP_PAD_TO_SIZE<5>().Apply();
    PATCH(0xa2c8a1).JMP(&runtimeHookNpcHarmRaw_a2c8a1).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2c966).JMP(&runtimeHookNpcHarmRaw_a2c966).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2cd3c).JMP(&runtimeHookNpcHarmRaw_a2cd3c).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2cffe).JMP(&runtimeHookNpcHarmRaw_a2cffe).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2d1b9).JMP(&runtimeHookNpcHarmRaw_a2d1b9).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2d2f1).JMP(&runtimeHookNpcHarmRaw_a2d2f5).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2d30d).JMP(&runtimeHookNpcHarmRaw_a2d311).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2d79f).JMP(&runtimeHookNpcHarmRaw_a2d79f).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2d7ae).JMP(&runtimeHookNpcHarmRaw_a2d7ae).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2d863).JMP(&runtimeHookNpcHarmRaw_a2d867).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2d8e7).JMP(&runtimeHookNpcHarmRaw_a2d8ec).NOP_PAD_TO_SIZE<7>().Apply();
    PATCH(0xa2d96c).JMP(&runtimeHookNpcHarmRaw_a2d96c).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2d977).JMP(&runtimeHookNpcHarmRaw_a2d977).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2d9b6).JMP(&runtimeHookNpcHarmRaw_a2d9bf).NOP_PAD_TO_SIZE<11>().Apply();
    PATCH(0xa2da36).JMP(&runtimeHookNpcHarmRaw_a2da36).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2dac4).JMP(&runtimeHookNpcHarmRaw_a2dac8).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2daca).JMP(&runtimeHookNpcHarmRaw_a2dace).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2dad0).JMP(&runtimeHookNpcHarmRaw_a2dad4).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2df69).JMP(&runtimeHookNpcHarmRaw_a2df6d).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2df77).JMP(&runtimeHookNpcHarmRaw_a2df77).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2e018).JMP(&runtimeHookNpcHarmRaw_a2e01d).NOP_PAD_TO_SIZE<7>().Apply();
    PATCH(0xa2e058).JMP(&runtimeHookNpcHarmRaw_a2e058).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2e0c2).JMP(&runtimeHookNpcHarmRaw_a2e0c2).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2e155).JMP(&runtimeHookNpcHarmRaw_a2e155).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2e274).JMP(&runtimeHookNpcHarmRaw_a2e278).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2e2f5).JMP(&runtimeHookNpcHarmRaw_a2e2f5).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2e556).JMP(&runtimeHookNpcHarmRaw_a2e556).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2e641).JMP(&runtimeHookNpcHarmRaw_a2e641).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2e647).JMP(&runtimeHookNpcHarmRaw_a2e64b).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2e6c2).JMP(&runtimeHookNpcHarmRaw_a2e6c2).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2e753).JMP(&runtimeHookNpcHarmRaw_a2e75c).NOP_PAD_TO_SIZE<11>().Apply();
    PATCH(0xa2e78c).JMP(&runtimeHookNpcHarmRaw_a2e795).NOP_PAD_TO_SIZE<11>().Apply();
    PATCH(0xa2e800).JMP(&runtimeHookNpcHarmRaw_a2e800).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2f26c).JMP(&runtimeHookNpcHarmRaw_a2f26c).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2f272).JMP(&runtimeHookNpcHarmRaw_a2f27a).NOP_PAD_TO_SIZE<10>().Apply();
    PATCH(0xa2f2e4).JMP(&runtimeHookNpcHarmRaw_a2f2ed).NOP_PAD_TO_SIZE<11>().Apply();
    PATCH(0xa2f376).JMP(&runtimeHookNpcHarmRaw_a2f376).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2f792).JMP(&runtimeHookNpcHarmRaw_a2f792).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2f79d).JMP(&runtimeHookNpcHarmRaw_a2f79d).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2f822).JMP(&runtimeHookNpcHarmRaw_a2f822).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2f8f5).JMP(&runtimeHookNpcHarmRaw_a2f8fa).NOP_PAD_TO_SIZE<7>().Apply();
    PATCH(0xa2f9b8).JMP(&runtimeHookNpcHarmRaw_a2f9bc).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2f9d9).JMP(&runtimeHookNpcHarmRaw_a2f9d9).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2fa5d).JMP(&runtimeHookNpcHarmRaw_a2fa5d).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2fae5).JMP(&runtimeHookNpcHarmRaw_a2fae5).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2fb05).JMP(&runtimeHookNpcHarmRaw_a2fb09).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2fb13).JMP(&runtimeHookNpcHarmRaw_a2fb13).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2fb29).JMP(&runtimeHookNpcHarmRaw_a2fb29).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2fb34).JMP(&runtimeHookNpcHarmRaw_a2fb34).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2fbaf).JMP(&runtimeHookNpcHarmRaw_a2fbaf).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2fde3).JMP(&runtimeHookNpcHarmRaw_a2fde3).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2fef5).JMP(&runtimeHookNpcHarmRaw_a2fef9).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2ff2f).JMP(&runtimeHookNpcHarmRaw_a2ff33).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2ff4d).JMP(&runtimeHookNpcHarmRaw_a2ff4d).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa2ff9a).JMP(&runtimeHookNpcHarmRaw_a2ff9f).NOP_PAD_TO_SIZE<7>().Apply();
    PATCH(0xa30011).JMP(&runtimeHookNpcHarmRaw_a30011).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa30128).JMP(&runtimeHookNpcHarmRaw_a3012c).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa30162).JMP(&runtimeHookNpcHarmRaw_a30166).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa30180).JMP(&runtimeHookNpcHarmRaw_a30180).NOP_PAD_TO_SIZE<6>().Apply();
    PATCH(0xa3045e).JMP(&runtimeHookNpcHarmRaw_a30465).NOP_PAD_TO_SIZE<9>().Apply();

    // Hooks for color switch hitting (jg hooks to capture loop exits)
    PATCH(0xA31FA0).JG(&runtimeHookColorSwitchRedNpc).Apply();
    PATCH(0xA3214E).JG(&runtimeHookColorSwitchGreenNpc).Apply();
    PATCH(0xA322FC).JG(&runtimeHookColorSwitchBlueNpc).Apply();
    PATCH(0xA324AA).JG(&runtimeHookColorSwitchYellowNpc).Apply();
    PATCH(0x9DB37E).JG(&runtimeHookColorSwitchYellowBlock).Apply();
    PATCH(0x9DB519).JG(&runtimeHookColorSwitchBlueBlock).Apply();
    PATCH(0x9DB6B9).JG(&runtimeHookColorSwitchGreenBlock).Apply();
    PATCH(0x9DB84E).JG(&runtimeHookColorSwitchRedBlock).Apply();

    PATCH(0x8BF020).CALL(&runtimeHookLoadDefaultControls).Apply();

    // Hook for animation spawning
    // CURRENTLY DISABLED: Sometimes this can be triggered by things currentely handled by FFI
    //                     function, making this an unsafe event to emit at present.
    //                     Also, cancelling this is not entirely safe for now, because of lots
    //                     of code that assumes success of GM_RUN_ANIM.
    // PATCH(0x9E7380).JMP(runtimeHookRunAnimInternal).NOP_PAD_TO_SIZE<6>().Apply();

    PATCH(0xA12050).JMP(runtimeHookSemisolidInteractionHook_Raw).NOP_PAD_TO_SIZE<69>().Apply();

    /************************************************************************/
    /* Import Table Patch                                                   */
    /************************************************************************/
    __vbaR4Var = (float(*)(VARIANTARG*))0x00401124;
    *(void**)0x00401124 = (void*)&vbaR4VarHook;
    rtcMsgBox = (int(__stdcall *)(VARIANTARG*, DWORD, DWORD, DWORD, DWORD))(*(void**)0x004010A8);
    *(void**)0x004010A8 = (void*)&rtcMsgBoxHook;
}

