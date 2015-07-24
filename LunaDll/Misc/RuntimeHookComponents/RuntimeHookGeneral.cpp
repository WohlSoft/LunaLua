#include "../../Main.h"
#include "../RuntimeHook.h"
#include <comutil.h>
#include "../../GlobalFuncs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../SdlMusic/MusicManager.h"
#include "../../HardcodedGraphics/HardcodedGraphicsManager.h"
#include "../ErrorReporter.h"
#include "../../GameConfig/GameConfiguration.h"


#include "../SHMemServer.h"

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

#include <CommCtrl.h>

static void NpcBitbltPatch(void);
static unsigned int __stdcall LatePatch(void);

void SetupThunRTMainHook()
{
    // Remove protection on smbx.text section
    DWORD oldprotect;
    VirtualProtect((void*)0x401000, 0x724000, PAGE_EXECUTE_READWRITE, &oldprotect);

    // Set up hook that will launch LunaDLLInit
    PATCH_FUNC(0x40BDDD, &ThunRTMainHook);
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

void ParseArgs(const std::vector<std::string>& args)
{
    if (vecStrFind(args, std::string("--patch")))
        gStartupSettings.patch = true;

    if (vecStrFind(args, std::string("--game")))
        gStartupSettings.game = true;

    if (vecStrFind(args, std::string("--leveleditor")))
        gStartupSettings.lvlEditor = true;

    if (vecStrFind(args, std::string("--noframeskip")))
        gStartupSettings.frameskip = false;

    if (vecStrFind(args, std::string("--nosound")))
        gStartupSettings.noSound = true;

    if (vecStrFind(args, std::string("--debugger")))
        gStartupSettings.debugger = true;

    if (vecStrFind(args, std::string("--logger")))
        gStartupSettings.logger = true;

    if (vecStrFind(args, std::string("--newlauncher")))
        gStartupSettings.newLauncher = true;
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

    /* Do what the place we patched this in is supposed to do: */
    /* 008BEC61 | mov eax,dword ptr ds:[B2D788] */
    return *((unsigned int*)(0xB2D788));
}

static bool IsWindowsVistaOrNewer() {
    OSVERSIONINFOEX osVersionInfo;
    DWORDLONG conditionMask = 0;

    memset(&osVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osVersionInfo.dwMajorVersion = 6;
    osVersionInfo.dwMinorVersion = 0;
    osVersionInfo.wServicePackMajor = 0;
    osVersionInfo.wServicePackMinor = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_SERVICEPACKMINOR, VER_GREATER_EQUAL);

    return VerifyVersionInfo(
        &osVersionInfo,
        VER_MAJORVERSION | VER_MINORVERSION |
        VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
        conditionMask);
}

void TrySkipPatch()
{
    //Check for arguments and write them in gStartupSettings
    ParseArgs(splitCmdArgs(std::string(GetCommandLineA())));

    if (gStartupSettings.patch){
        memset((void*)0x8BECF2, INSTR_NOP, 0x1B5); //nop out the loader code
        *(WORD*)(0xB25046) = -1; //set run to true

        PATCH_FUNC(0x8BED00, &InitHook);
    }

    // Insert callback for patching which must occur after the runtime has started
    // (0x8BEC61 is not quite as early as would be ideal for this, but it's convenient)
    PATCH_FUNC(0x8BEC61, &LatePatch);

    //Load graphics from the HardcodedGraphicsManager
    HardcodedGraphicsManager::loadGraphics();

    //game.ini reader
    GameConfiguration::runPatchByIni(INIReader(utf8_encode(getModulePath())+ "\\game.ini"));

    /************************************************************************/
    /* Simple ASM Source Patches                                            */
    /************************************************************************/
    fixup_TypeMismatch13();
    fixup_Credits();
    fixup_Mushbug();
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

    /************************************************************************/
    /* Source Code Function Patch                                           */
    /************************************************************************/
    PATCH_FUNC(0x8D9446, &OnLvlLoad);
    *(BYTE*)(0x8D944B) = INSTR_NOP;
    *(BYTE*)(0x8D944C) = INSTR_NOP;

    PATCH_FUNC(0x8CA23B, &TestFunc);
    *(BYTE*)(0x8CA240) = INSTR_NOP;

    PATCH_FUNC(0x96C030, &HUDHook);
    *(BYTE*)(0x96C035) = INSTR_NOP;

    *(void**)0xB2F244 = (void*)&mciSendStringHookA;

    PATCH_FUNC(0x8D6BB6, &forceTermination);

    PATCH_FUNC(0x8C11D5, &LoadWorld);

    PATCH_FUNC(0x8C16F7, &WorldLoop);

    PATCH_FUNC(0x8C0E6D, &LoadIntro);

    PATCH_FUNC(0x932353, &printLunaLuaVersion);

    PATCH_FUNC(0x9090F5, &WorldRender);

    PATCH_FUNC(0xAA4352, &__vbaStrCmp_TriggerSMBXEventHook);
    *(BYTE*)(0xAA4357) = INSTR_NOP;

    PATCH_FUNC(0x8C23CB, &checkLevelShutdown);
    *(BYTE*)(0x8C23D0) = INSTR_NOP;
    *(BYTE*)(0x8C23D1) = INSTR_NOP;

    PATCH_FUNC(0xA755D2, &UpdateInputHook_Wrapper);

    // Graphics Bitblt hooks
    PATCH_FUNC(0x8C137E, &LoadLocalGfxHook);
    PATCH_FUNC(0x8D8BF1, &LoadLocalGfxHook);
    PATCH_FUNC(0x8D9611, &LoadLocalGfxHook);
    PATCH_FUNC(0x8DF52B, &LoadLocalGfxHook);
    PATCH_FUNC(0x8DFF7C, &LoadLocalGfxHook);
    PATCH_JMP(0x4242D0, &BitBltHook);
    PATCH_JMP(0x424314, &StretchBltHook);

    PATCH_FUNC(0x8E54EC, &MessageBoxOpenHook);
    *(BYTE*)(0x8E54F1) = INSTR_NOP;

    // Hook to fix 100% CPU when window is inactive
    *(BYTE*)(0x8E6FE1) = INSTR_NOP;
    PATCH_FUNC(0x8E6FE2, &WindowInactiveHook);

    // Don't trust QPC as much on WinXP
    void* frameTimingHookPtr;
    void* frameTimingMaxFPSHookPtr;
    if (IsWindowsVistaOrNewer()) {
        frameTimingHookPtr = (void*)&FrameTimingHookQPC;
        frameTimingMaxFPSHookPtr = (void*)&FrameTimingMaxFPSHookQPC;
    }
    else {
        frameTimingHookPtr = (void*)&FrameTimingHook;
        frameTimingMaxFPSHookPtr = (void*)&FrameTimingMaxFPSHook;
    }

    // Hooks to fix 100% CPU during operation
    // These ones are normally not sensitive to the "max FPS" setting
    memset((void*)0x8BFD4A, INSTR_NOP, 0x40);
    memset((void*)0x8C0488, INSTR_NOP, 0x40);
    memset((void*)0x8C0EE6, INSTR_NOP, 0x40);
    PATCH_FUNC_CALL_SAFE(0x8BFD4A, frameTimingHookPtr);
    PATCH_FUNC_CALL_SAFE(0x8C0488, frameTimingHookPtr);
    PATCH_FUNC_CALL_SAFE(0x8C0EE6, frameTimingHookPtr);
    // These ones are normally sensitive to the "max FPS" setting
    memset((void*)0x8C15A7, INSTR_NOP, 0x4A);
    memset((void*)0x8C20FC, INSTR_NOP, 0x4A);
    memset((void*)0x8E2AED, INSTR_NOP, 0x4A);
    memset((void*)0x8E56ED, INSTR_NOP, 0x4A);
    PATCH_FUNC_CALL_SAFE(0x8C15A7, frameTimingMaxFPSHookPtr);
    PATCH_FUNC_CALL_SAFE(0x8C20FC, frameTimingMaxFPSHookPtr);
    PATCH_FUNC_CALL_SAFE(0x8E2AED, frameTimingMaxFPSHookPtr);
    PATCH_FUNC_CALL_SAFE(0x8E56ED, frameTimingMaxFPSHookPtr);

    /************************************************************************/
    /* Import Table Patch                                                   */
    /************************************************************************/
    __vbaR4Var = (float(*)(VARIANTARG*))0x00401124;
    *(void**)0x00401124 = (void*)&vbaR4VarHook;
    rtcMsgBox = (int(__stdcall *)(VARIANTARG*, DWORD, DWORD, DWORD, DWORD))(*(void**)0x004010A8);
    *(void**)0x004010A8 = (void*)&rtcMsgBoxHook;
}
