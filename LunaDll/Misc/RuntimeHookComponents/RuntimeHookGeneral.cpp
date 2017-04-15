#include "../../Main.h"
#include "../RuntimeHook.h"
#include <comutil.h>
#include "../../GlobalFuncs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../SdlMusic/MusicManager.h"
#include "../ErrorReporter.h"
#include "../../GameConfig/GameConfiguration.h"
#include "../../Globals.h"
#include "../AsmPatch.h"

#include "../SHMemServer.h"

#include "../TestMode.h"
#include "../../IPC/IPCPipeServer.h"

#include "../../Utils/StringUtils.h"
#include "../../Utils/EncodeUtils.h"


bool episodeStarted = false;

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

    //CWPRETSTRUCT* wData = (CWPRETSTRUCT*)lParam;
    LPCWPSTRUCT wData = (LPCWPSTRUCT)lParam;


    if (wData->message == WM_COPYDATA){
        PCOPYDATASTRUCT pcds = reinterpret_cast<PCOPYDATASTRUCT>(wData->lParam);
        if (pcds->cbData == 1){
            if (pcds->dwData == 0xDEADC0DE){
                std::wstring lvlName = gShMemServer.read();
                if (!lvlName.empty()){
                    GM_FULLPATH = lvlName;
                }
                gHook_SkipTestMsgBox = true;
                ((void(*)())0x00A02220)();
            }
        }
    }

    /*if(wData->message == WM_CREATE){
    wchar_t clName[501];
    GetClassNameW(wData->hwnd, clName, 500);
    if(!wcscmp(clName, L"ThunderRT6MDIForm")){
    TBBUTTON * bnt = new TBBUTTON;
    bnt->iBitmap = I_IMAGENONE;
    bnt->idCommand = 1000;
    bnt->fsState = TBSTATE_ENABLED;
    bnt->fsStyle = TBSTYLE_AUTOSIZE;
    bnt->dwData = NULL;
    char* myText = "Hallo";
    const int numButtons = 1;
    TBBUTTON tbButtonsAdd[numButtons] =
    {
    *bnt
    };
    SendMessage(wData->hwnd, TB_ADDBUTTONS, numButtons, (LPARAM)tbButtonsAdd);
    }
    }*/

    return CallNextHookEx(HookWnd, nCode, wParam, lParam);
}

void ParseArgs(const std::vector<std::wstring>& args)
{
    auto isInArgs = [&args](std::wstring toFind){
        return std::find(args.cbegin(), args.cend(), toFind) != args.cend();
    };

    if (isInArgs(L"--patch"))
        gStartupSettings.patch = true;

    if (isInArgs(L"--game"))
        gStartupSettings.game = true;

    if (isInArgs(L"--leveleditor"))
        gStartupSettings.lvlEditor = true;

    if (isInArgs(L"--noframeskip"))
        gStartupSettings.frameskip = false;

    if (isInArgs(L"--nosound"))
        gStartupSettings.noSound = true;

    if (isInArgs(L"--debugger"))
        gStartupSettings.debugger = true;

    if (isInArgs(L"--logger"))
        gStartupSettings.logger = true;

    if (isInArgs(L"--newlauncher"))
        gStartupSettings.newLauncher = true;

    if (isInArgs(L"--console"))
        gStartupSettings.console = true;

    if (isInArgs(L"--nogl"))
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

    if (isInArgs(L"--waitForIPC"))
    {
        gStartupSettings.waitForIPC = true;
        gStartupSettings.currentlyWaitingForIPC = true;
        gStartupSettings.patch = true;
    }

    if (isInArgs(L"--hideOnCloseIPC"))
    {
        gStartupSettings.waitForIPC = true;
        gStartupSettings.patch = true;
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
    ParseArgs(LunaLua::StringUtils::splitCmdArgs(GetCommandLineW()));

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

    // Either in root or in config folder. The config folder is recommended however.
    gGeneralConfig.setFilename(getLatestConfigFile(L"luna.ini"));
    gGeneralConfig.loadOrDefault();
    //game.ini reader
    GameConfiguration::runPatchByIni(INIReader(LunaLua::EncodeUtils::WStr2Str(getLatestConfigFile(L"game.ini"))));

    /************************************************************************/
    /* Simple ASM Source Patches                                            */
    /************************************************************************/
    fixup_TypeMismatch13();
    fixup_Credits();
    fixup_Mushbug();
    fixup_Veggibug();
    fixup_NativeFuncs();
    fixup_BGODepletion();

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


    // Graphics Bitblt hooks
    PATCH(0x8C137E).CALL(&LoadLocalGfxHook).Apply();
    PATCH(0x8D8BF1).CALL(&LoadLocalGfxHook).Apply();
    PATCH(0x8D9611).CALL(&LoadLocalGfxHook).Apply();
    PATCH(0x8DF52B).CALL(&LoadLocalGfxHook).Apply();
    PATCH(0x8DFF7C).CALL(&LoadLocalGfxHook).Apply();

    PATCH(0x8DEF73).CALL(&LoadLocalOverworldGfxHook).Apply();
    PATCH(0x8DF808).CALL(&LoadLocalOverworldGfxHook).Apply();

    //PATCH(0x4242D0).JMP(GET_RETADDR_TRACE_HOOK<&BitBltTraceHook>()).Apply();
    PATCH(0x4242D0).JMP(&BitBltHook).Apply();
    PATCH(0x424314).JMP(&StretchBltHook).Apply();

    PATCH(0x8E54EC)
        .CALL(&MessageBoxOpenHook)
        .NOP()
        .Apply();

    // Okay redigit, I know your debug values are in general pretty dumb, but right now they are awesome for easy patching! Thx mate!
    PATCH(0x90C856)
        .CALL(&CameraUpdateHook_Wrapper)
        .NOP()
        .NOP()
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

    // Logging for NPC collisions
    //PATCH(0xA281B0).JMP(GET_RETADDR_TRACE_HOOK<&collideNPCLoggingHook>()).NOP().Apply();

    // Level and world render hooks
    PATCH(0x909290).JMP(RenderLevelHook).NOP().Apply();
    PATCH(0x8FEB10).JMP(RenderWorldHook).NOP().Apply();

    // Level rendering layering hooks

    //PATCH(0x90C856).NOP().NOP().CALL(GetRenderBelowPriorityHook<-95>()).Apply();
    //-100: Level Background
    PATCH(0x90F4FA).NOP().NOP().CALL(GetRenderBelowPriorityHook<-95>()).Apply();
    // -95: Furthest back BGOs
    PATCH(0x910433).NOP().NOP().CALL(GetRenderBelowPriorityHook<-90>()).Apply();
    // -90: Sizable Blocks
    PATCH(0x910E5D).NOP().NOP().CALL(GetRenderBelowPriorityHook<-85>()).Apply();
    // -85: Some more BGOs
    PATCH(0x911F19).NOP().NOP().CALL(GetRenderBelowPriorityHook<-80>()).Apply();
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
    PATCH(0x928F0A).NOP().NOP().CALL(GetRenderBelowPriorityHook<-20>()).Apply();
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

    PATCH(0xA75079).JMP(runtimeHookCheckInputRaw).NOP_PAD_TO_SIZE<7>().Apply();

    /************************************************************************/
    /* Import Table Patch                                                   */
    /************************************************************************/
    __vbaR4Var = (float(*)(VARIANTARG*))0x00401124;
    *(void**)0x00401124 = (void*)&vbaR4VarHook;
    rtcMsgBox = (int(__stdcall *)(VARIANTARG*, DWORD, DWORD, DWORD, DWORD))(*(void**)0x004010A8);
    *(void**)0x004010A8 = (void*)&rtcMsgBoxHook;
}

