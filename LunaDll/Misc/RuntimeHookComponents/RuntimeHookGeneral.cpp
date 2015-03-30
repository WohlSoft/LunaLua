#include "../../Main.h"
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

static void NpcBitbltPatch(void);
static unsigned int __stdcall LatePatch(void);

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

static unsigned int __stdcall LatePatch(void)
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
    fixup_Credits();

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

    PATCH_FUNC(0x932353, &printLunaLuaVersion);

    PATCH_FUNC(0x9090F5, &WorldRender);

    PATCH_FUNC(0xAA4352, &__vbaStrCmp_TriggerSMBXEventHook);
    *(BYTE*)(0xAA4357) = INSTR_NOP;

    PATCH_FUNC(0x8C23CB, &checkLevelShutdown);
    *(BYTE*)(0x8C23D0) = INSTR_NOP;
    *(BYTE*)(0x8C23D1) = INSTR_NOP;

    // Graphics Bitblt hooks
    NpcBitbltPatch();

    /************************************************************************/
    /* Import Table Patch                                                   */
    /************************************************************************/
    __vbaR4Var = (float(*)(VARIANTARG*))0x00401124;
    *(void**)0x00401124 = (void*)&vbaR4VarHook;
    rtcMsgBox = (int(__stdcall *)(VARIANTARG*, DWORD, DWORD, DWORD, DWORD))(*(void**)0x004010A8);
    *(void**)0x004010A8 = (void*)&rtcMsgBoxHook;
}

static void NpcBitbltPatch(void)
{
    // Start of unkRender_909290 (909290 to 94DF08)

    // NPC loop from 91274F to 915316
    // Unknown purpose
    PATCH_FUNC(0x912D2F, &npcMaskBitbltHook);
    PATCH_FUNC(0x912FA5, &npcBitbltHook);
    PATCH_FUNC(0x9132AF, &npcMaskBitbltHook);
    PATCH_FUNC(0x9135A6, &npcBitbltHook);
    PATCH_FUNC(0x913843, &npcMaskBitbltHook);
    PATCH_FUNC(0x913AB9, &npcBitbltHook);
    PATCH_FUNC(0x913D95, &npcMaskBitbltHook);
    PATCH_FUNC(0x914084, &npcBitbltHook);
    PATCH_FUNC(0x91434C, &npcMaskBitbltHook);
    PATCH_FUNC(0x9145BE, &npcBitbltHook);
    PATCH_FUNC(0x9149A3, &npcMaskBitbltHook);
    PATCH_FUNC(0x914DA1, &npcBitbltHook);

    // Unknown purpose
    PATCH_FUNC(0x915DD7, &npcMaskBitbltHook);
    PATCH_FUNC(0x915F96, &npcBitbltHook);
    PATCH_FUNC(0x91615C, &npcMaskBitbltHook);
    PATCH_FUNC(0x91631A, &npcBitbltHook);
    PATCH_FUNC(0x91CEA6, &npcMaskBitbltHook);
    PATCH_FUNC(0x91D064, &npcBitbltHook);
    PATCH_FUNC(0x91D228, &npcMaskBitbltHook);
    PATCH_FUNC(0x91D3E9, &npcBitbltHook);

    // NPC loop from 91E1F9 to 91F802
    // Coins and similar
    PATCH_FUNC(0x91E81F, &npcMaskBitbltHook);
    PATCH_FUNC(0x91EA78, &npcBitbltHook);
    PATCH_FUNC(0x91EE7A, &npcMaskBitbltHook);
    PATCH_FUNC(0x91F28D, &npcBitbltHook);

    // NPC loop from 91F809 to 920040
    // Unknown purpose, also calls unkPaintNPCOnScreen(009920D0)

    // NPC loop from 920047 to 922D00
    // Most normal non-foreground NPCs
    PATCH_FUNC(0x9207EB, &npcMaskBitbltHook);
    PATCH_FUNC(0x920A41, &npcBitbltHook);
    PATCH_FUNC(0x920F74, &npcMaskBitbltHook);
    PATCH_FUNC(0x921134, &npcBitbltHook);
    PATCH_FUNC(0x921531, &npcMaskBitbltHook);
    PATCH_FUNC(0x921944, &npcBitbltHook);
    // Not NPC gfx: 0x9220D6
    // Not NPC gfx: 0x922256
    // Not NPC gfx: 0x9223B7
    // Not NPC gfx: 0x922533

    // NPC loop from 922D07 to 923786
    // Not NPC gfx: 0x9232BA
    // Not NPC gfx: 0x923737

    // NPC loop from 927F28 to 928EA5
    // Unknown purpose, also calls unkPaintNPCOnScreen(009920D0)
    PATCH_FUNC(0x928411, &npcMaskBitbltHook);
    PATCH_FUNC(0x928667, &npcBitbltHook);
    PATCH_FUNC(0x928A66, &npcMaskBitbltHook);
    PATCH_FUNC(0x928E7D, &npcBitbltHook);

    // NPC loop from 929F88 to 92B428
    // Foreground NPCs
    PATCH_FUNC(0x92A3F3, &npcMaskBitbltHook);
    PATCH_FUNC(0x92A64C, &npcBitbltHook);
    PATCH_FUNC(0x92AA4E, &npcMaskBitbltHook);
    PATCH_FUNC(0x92AE61, &npcBitbltHook);

    // NPC loop from 92EC30 to 92FC3F
    // Falling reserve powerups (npc.containedWithin == 2)
    PATCH_FUNC(0x92EFD7, &npcMaskBitbltHook);
    PATCH_FUNC(0x92F214, &npcBitbltHook);
    PATCH_FUNC(0x92F5F9, &npcMaskBitbltHook);
    PATCH_FUNC(0x92F9DB, &npcBitbltHook);

    // Unknown purpose
    PATCH_FUNC(0x93A2E4, &npcMaskBitbltHook);
    PATCH_FUNC(0x93B05D, &npcMaskBitbltHook);
    PATCH_FUNC(0x93BC5D, &npcMaskBitbltHook);

    // NPC loop from 93BE67 to 93DB45
    // Something to do with herbs and eggs
    PATCH_FUNC(0x93C795, &npcMaskBitbltHook);
    PATCH_FUNC(0x93CA6D, &npcBitbltHook);
    PATCH_FUNC(0x93D4AE, &npcMaskBitbltHook);
    PATCH_FUNC(0x93DB2E, &npcBitbltHook);

    // Unknown purpose
    PATCH_FUNC(0x93A5B0, &npcBitbltHook);
    PATCH_FUNC(0x93B6BC, &npcBitbltHook);
    PATCH_FUNC(0x93BE49, &npcBitbltHook);

    // Unknown purpose
    PATCH_FUNC(0x946B1F, &npcMaskBitbltHook);
    PATCH_FUNC(0x946D5F, &npcBitbltHook);
    PATCH_FUNC(0x94728E, &npcMaskBitbltHook);
    PATCH_FUNC(0x947450, &npcBitbltHook);
    PATCH_FUNC(0x947830, &npcMaskBitbltHook);
    PATCH_FUNC(0x947C12, &npcBitbltHook);

    // End of unkRender_909290

    // unknown functions
    PATCH_FUNC(0x96C7E6, &npcMaskBitbltHook);
    PATCH_FUNC(0x96C995, &npcBitbltHook);
    PATCH_FUNC(0x96E920, &npcMaskBitbltHook);
    PATCH_FUNC(0x9754EE, &npcMaskBitbltHook);
    PATCH_FUNC(0x9756AB, &npcBitbltHook);
    PATCH_FUNC(0x97E178, &npcMaskBitbltHook);
    PATCH_FUNC(0x97E327, &npcBitbltHook);
    PATCH_FUNC(0x989EF0, &npcMaskBitbltHook);
    PATCH_FUNC(0x98A059, &npcBitbltHook);
    PATCH_FUNC(0x98A1AE, &npcMaskBitbltHook);
    PATCH_FUNC(0x98A317, &npcBitbltHook);
    PATCH_FUNC(0x99001F, &npcMaskBitbltHook);
    PATCH_FUNC(0x990231, &npcBitbltHook);
    PATCH_FUNC(0x9905B0, &npcMaskBitbltHook);
    PATCH_FUNC(0x990943, &npcBitbltHook);

    // Start of unkPaintNPCOnScreen (9920D0 until 992F40)
    // Unknown purpose
    PATCH_FUNC(0x992376, &npcMaskBitbltHook);
    PATCH_FUNC(0x9924B8, &npcBitbltHook);
    PATCH_FUNC(0x99260E, &npcMaskBitbltHook);
    PATCH_FUNC(0x99276F, &npcBitbltHook);
    PATCH_FUNC(0x9928BA, &npcMaskBitbltHook);
    PATCH_FUNC(0x992A10, &npcBitbltHook);
    PATCH_FUNC(0x992B5B, &npcMaskBitbltHook);
    PATCH_FUNC(0x992CB1, &npcBitbltHook);
    PATCH_FUNC(0x992DEE, &npcMaskBitbltHook);
    PATCH_FUNC(0x992F2E, &npcBitbltHook);
    // End of unkPaintNPCOnScreen
}
