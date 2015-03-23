#include "../RuntimeHook.h"
#include <comutil.h>
#include "../../GlobalFuncs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../SdlMusic/MusicManager.h"
#include "../../HardcodedGraphics/HardcodedGraphicsManager.h"
#include "../ErrorReporter.h"

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
                    GM_FULLPATH = (DWORD)SysAllocString(lvlName.c_str());
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

static unsigned int LatePatch(void)
{
    /************************************************************************/
    /* Engine Limit patches                                                 */
    /************************************************************************/
    fixup_WarpLimit();

    /* Do what the place we patched this in is supposed to do: */
    /* 008BEC61 | mov eax,dword ptr ds:[B2D788] */
    return *((unsigned int*)(0xB2D788));
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

    /************************************************************************/
    /* Simple ASM Source Patches                                            */
    /************************************************************************/
    fixup_TypeMismatch13();
    fixup_ErrorReporting();

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
    *(void**)0xB2F244 = (void*)&mciSendStringHookA;

    PATCH_FUNC(0x8D6BB6, &forceTermination);

    PATCH_FUNC(0x8C11D5, &LoadWorld);

    PATCH_FUNC(0x8C16F7, &WorldLoop);

    PATCH_FUNC(0x932353, &printLunaLuaVersion);

    PATCH_FUNC(0x9090F5, &WorldRender);

    PATCH_FUNC(0xAA4352, &__vbaStrCmp_TriggerSMBXEventHook);
    *(BYTE*)(0xAA4357) = INSTR_NOP;

    PATCH_FUNC(0x8C23CB, &checkLevelShutdown);
    *(BYTE*)(0x8C23D0) = INSTR_NOP;
    *(BYTE*)(0x8C23D1) = INSTR_NOP;

    /************************************************************************/
    /* Import Table Patch                                                   */
    /************************************************************************/
    __vbaR4Var = (float(*)(VARIANTARG*))0x00401124;
    *(void**)0x00401124 = (void*)&vbaR4VarHook;
    rtcMsgBox = (int(__stdcall *)(VARIANTARG*, DWORD, DWORD, DWORD, DWORD))(*(void**)0x004010A8);
    *(void**)0x004010A8 = (void*)&rtcMsgBoxHook;
}

