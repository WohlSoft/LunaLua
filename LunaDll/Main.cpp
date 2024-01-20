#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
using std::min;//Workaround for GDI+ that requires MINMAX be presented
using std::max;
#include <Windows.h>
#include <GdiPlus.h>
#include <io.h>
#include <fcntl.h>
#include <delayimp.h>

#include "Main.h"
#include "Globals.h"
#include "GlobalFuncs.h"
#include "Defines.h"
#include "SMBXInternal/Layer.h"
#include "LevelCodes/LevelCodes.h"
#include "SMBXInternal/PlayerMOB.h"
#include "Rendering/Rendering.h"
#include "SMBXInternal/Level.h"
#include "SMBXInternal/NPCs.h"
#include "SMBXInternal/Blocks.h"
#include "Misc/MiscFuncs.h"
#include "SMBXInternal/Sound.h"
#include "Input/Input.h"
#include "SMBXInternal/SMBXEvents.h"
#include "LuaMain/LunaLuaMain.h"
#include "Misc/RuntimeHook.h"
#include "SdlMusic/MusicManager.h"
#include "Misc/Playground.h"
#include "Rendering/GL/GLEngine.h"
#include "Rendering/GL/GLInitTest.h"
#include "Rendering/WindowSizeHandler.h"
#include "Misc/AsmPatch.h"
#include "Misc/LoadScreen.h"
#include "../LunaLoader/LunaLoaderPatch.h"

static bool LevelCustomSounds = false;

/*!
 * \brief Standard DLL loader main
 */
BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID /*lpvReserved*/)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        //Init file paths variables
        initAppPaths();
        gHInstance = (HINSTANCE)hinstDLL;
        //Init hooks and main LunaLUA modules
        SetupThunRTMainHook();
        timeBeginPeriod(1);

        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        timeEndPeriod(1);
        CleanUp();
        break;
    }
    return TRUE;
}

static DWORD __stdcall GetCurrentProcessorNumberXP(void)
{
    __asm {
        MOV EAX, 1
        CPUID
        SHR EBX, 24
        MOV EAX, EBX
    }
}

// We don't call this directly from DLL_PROCESS_ATTACH because if we do things
// can break when we're loaded via LoadLibrary
// Instead this is called by LunaDLLInitHook, which is set up by
// SetupLunaDLLInitHook that runs from DLL_PROCESS_ATTACH
void LunaDLLInit()
{
    InitGlobals();

    //Check for arguments and write them in gStartupSettings
    ParseArgs(splitCmdArgsW(std::wstring(GetCommandLineW())));

    // Load Luna Config
    // Either in root or in config folder. The config folder is recommended however.
    gGeneralConfig.setFilename(getLatestConfigFile(L"luna.ini"));
    gGeneralConfig.loadOrDefault();

    // If command line arguments did not specify a mode, see if the config file specifies
    if (!gStartupSettings.softwareGL && !gStartupSettings.forceHardGL)
    {
        GeneralLunaConfig::GLMode glMode = gGeneralConfig.getRendererOpenGL();
        switch (glMode)
        {
        case GeneralLunaConfig::GLModeHard:
            gStartupSettings.forceHardGL = true;
            break;
        case GeneralLunaConfig::GLModeSoft:
            gStartupSettings.softwareGL = true;
            break;
        }
    }

    // Get whether the DLL for software GL is found
    bool haveSoftwareGLSupport = (GetFileAttributesA("softgl/OPENGL32.dll") != INVALID_FILE_ATTRIBUTES);

    // If loading in software GL mode, set the DLL directory appropriately before trying
    if (gStartupSettings.softwareGL)
    {
        if (!haveSoftwareGLSupport)
        {
            MessageBoxA(0, "Missing DLL for Software GL support", "Error", 0);
            exit(1);
        }
        SetDllDirectoryA("softgl");
    }
    __HrLoadAllImportsForDll("OPENGL32.dll");

    // Test OpenGL support
    if (LunaDLLTestGLFeatures()) {
        // Success. OpenGL works
        g_GLEngine.Enable();
    }
    else if (haveSoftwareGLSupport && !gStartupSettings.softwareGL && !gStartupSettings.forceHardGL)
    {
        // Regular GL doesn't work, but we've not tried software GL yet, so make a child process for that

        // Make new command line
        std::wstring newCmdLine(GetCommandLineW());
        newCmdLine += L" --softGL";

        // Get working dir
        unsigned int workingDirLen = GetCurrentDirectoryW(0, NULL) + 1;
        std::unique_ptr<wchar_t[]> workingDir(new wchar_t[workingDirLen]);
        GetCurrentDirectoryW(workingDirLen, workingDir.get());

        // Get module filename
        wchar_t moduleFileName[4096];
        GetModuleFileNameW(nullptr, moduleFileName, 4096);

        // Launch child process, inheriting stdin/stdout
        LunaLoaderResult ret = LunaLoaderRun(moduleFileName, newCmdLine.c_str(), workingDir.get());
        exit(0);
    }
    else
    {
        std::string errmsg("Could not start renderer for the following reason:\n");
        errmsg += LunaDLLTestGLFeaturesGetMessage();
        if (gStartupSettings.softwareGL)
        {
            errmsg += "\n(Error using software renderer)";
        }
        MessageBoxA(0, errmsg.c_str(), "Error", 0);
        exit(1);
    }

    TrySkipPatch();

    // Set processor affinity for the main thread. Switching cores is bad for stable frame rate
    if (gGeneralConfig.getEngineCpuLockAffinity())
    {
        DWORD curProcessor = GetCurrentProcessorNumberXP();
        SetThreadAffinityMask(GetCurrentThread(), 1 << curProcessor);
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    }

    // Initialize GDI+ so we can make use of it
    ULONG_PTR m_gdiplusToken;   // class member
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

// *EXPORT* On Level Load -- Run once as a level is loaded (including title screen level)
int OnLvlLoad() {

    // Restore some code the hook overwrote
    *(DWORD*)0x00B25958 = 0;

    // Make sure we init the renderer before we start LunaLua when entering levels
    GLEngineProxy::CheckRendererInit();

    ResetLunaModule();

    // WIP
    // dumpTypeLibrary((IDispatch*)*(DWORD*)0xB2D7E8, std::wcout);


    std::string custPath = WStr2Str(getCustomFolderPath());
    std::string wldPath = WStr2Str(GM_FULLDIR);
    std::string SndRoot = MusicManager::SndRoot();
    replaceSubStr(wldPath, "\"", "");
    replaceSubStr(wldPath, "\\\\", "\\");
    replaceSubStr(wldPath, "/", "\\");

    replaceSubStr(SndRoot, "\"", "");
    replaceSubStr(SndRoot, "\\\\", "\\");
    replaceSubStr(SndRoot, "/", "\\");

    bool doSoundLoading = false;

    if ((!custPath.empty()) && (file_existsX(custPath + "\\sounds.ini"))) {
        //If custom-level specific sounds.ini detected
        doSoundLoading = true;
        LevelCustomSounds = true;
    }
    else if (LevelCustomSounds) {
        //If custom-level specific sounds.ini was NOT detected, but was loaded recently - reload episode specific sounds
        doSoundLoading = true;
        LevelCustomSounds = false;
    }

    if (!episodeStarted) {
        //Load custom sounds if episode is not finally started
        if (wldPath != SndRoot) doSoundLoading = true;
    }

    if (doSoundLoading) MusicManager::loadCustomSounds(wldPath + "\\", custPath);

    // Start deferring music
    g_EventHandler.loadTimeDeferMusic();

    if(gLunaEnabled) {
        // Load autocode
        gAutoMan.Clear(false);
        gAutoMan.ReadFile((std::wstring)GM_FULLDIR);

        // Try to load world codes
        gAutoMan.ReadWorld((std::wstring)GM_FULLDIR);

        // Init var bank
        gSavedVarBank.TryLoadWorldVars();
        gSavedVarBank.CheckSaveDeletion();
        gSavedVarBank.CopyBank(&gAutoMan.m_UserVars);

        //  Don't try to call the CLunaLua constructor... It's already
        //  constructed automatically once, and trying to do this will call
        //  the constructor extra times *without* ever calling the destructor,
        //  which can result in a memory leak of the whole Lua state!
        //    gLunaLua = CLunaLua();
        gLunaLua.init(CLunaLua::LUNALUA_LEVEL, (std::wstring)GM_FULLDIR, Level::GetName());
        gLunaLua.setReady(true);

        // Do some stuff
        gAutoMan.DoEvents(true); // do with init

        // Init some stuff
        InitLevel();
        gAutoMan.m_Hearts = 2;
    }

    //PGE DBG STUFF
    //readAndWriteNPCSettings();
    //overwriteFunc();

    return 0;
}

// *EXPORT* Test Func -- Run once per gameplay frame
int TestFunc()
{
    // Clean up
    gAutoMan.ClearExpired();
    gSavedVarBank.CheckSaveDeletion();

    // Update inputs
    Input::CheckSpecialCheats();
    Input::UpdateInputTasks();

    if(gLunaEnabled) {
#if COMPILE_PLAYGROUND
        Playground::doPlaygroundStuff();
#endif
        g_EventHandler.hookLevelLoop();

        // Run autocode
        gAutoMan.DoEvents(false);

        // Update some stuff
        gFrames++;
        gSavedVarBank.SaveIfNeeded();

        // Run any framecode
        TestFrameCode();
        LevelFrameCode();
    }

    Blocks::DoSortingIfRequired();
    return 0;
}

void OnLevelHUDDraw(int cameraIdx) {

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> inputEvent = std::make_shared<Event>("onHUDUpdate", false);
        inputEvent->setDirectEventName("onHUDUpdate");
        inputEvent->setLoopable(false);
        gLunaLua.callEvent(inputEvent, cameraIdx);
    }

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> inputEvent = std::make_shared<Event>("onHUDDraw", false);
        inputEvent->setDirectEventName("onHUDDraw");
        inputEvent->setLoopable(false);
        gLunaLua.callEvent(inputEvent, cameraIdx);
    }

    gSpriteMan.RunSprites();
}


// TEST CODE - This code will run every frame everywhere, making for easy testing
void TestFrameCode() {

    /// DEBUG STUFF//

    //- Uncomment to test variable bank
    //for each(pair<wstring, double> kvp in gSavedVarBank.m_VarBank) {
    //    Renderer::Get().DebugPrint(kvp.first, kvp.second);
    //}

    //static double bgX = 0;
    //double** pBGs = (double **)0x00B2B984;
    //pBGs[0][1] = bgX;
    //bgX -= 0.1;

}

// LEVEL FRAME CODE - This will be run every frame of leveltime. Currently a low-cost enumeration switch based on the loaded level
void LevelFrameCode() {

    switch(gLevelEnum)
    {

    case DllTestLvl:
        dlltestlvlCode();
        break;

    case QraestoliaCaverns:
        QraestoliaCavernsCode();
        break;

    case TheFloorIsLava:
        TheFloorisLavaCode();
        break;

    case Calleoca:
        CalleocaCode();
        break;

    case Snowbordin:
        SAJSnowbordin::SnowbordinCode();
        break;

    case Science:
        ScienceBattle::ScienceCode();

    case CurtainTortoise:
        //CurtainTortoiseCode();
        break;

    case AbstractAssault:
        AbstractAssaultCode();
        break;

    case DemosBrain:
        DemosBrainCode();
        break;

    case EuroShellRandD:
        //EuroShellRandDCode();
        break;

    case ThouStartsANewVideo:
        KilArmoryCode();
        break;

    case Invalid:
    default:
        break;
    }
}

// INIT LEVEL - This should be called during the level load hook function. It'll also figure out which level is being played.
//                Also put init code in here if you want.
void InitLevel() {

    // Reset some variables
    gFrames = 0;
    gLevelEnum = Invalid;

    std::wstring curlvl = Level::GetName();
    PlayerMOB* demo = Player::Get(1);


    /// INITIALIZATION BLOCKS ///

    // Example init block
    if(curlvl == L"dlltest.lvl") {
        gLevelEnum = DllTestLvl;
    }

    // Qraestolia Caverns init block
    else if(curlvl == L"SAJewers-QraestoliaCaverns.lvl") {
        gLevelEnum = QraestoliaCaverns;
    }

    // The Floor is Lava init block
    else if(curlvl == L"Docopoper-TheFloorisLava.lvl") {
        gLevelEnum = TheFloorIsLava;
        demo -> Identity = CHARACTER_MARIO;
    }

    // Curtain Tortoise init block
    else if(curlvl == L"JosephStaleknight-CurtainTortoise.lvl") {
        gLevelEnum = CurtainTortoise;
    }

    // Abtract Assault init block
    else if(curlvl == L"Docopoper-AbstractAssault.lvl") {
        gLevelEnum = AbstractAssault;
    }

    // Snowbordin init block
    else if(curlvl == L"SAJewers-Snowboardin.lvl") {
        gLevelEnum = Snowbordin;
        SAJSnowbordin::SnowbordinInitCode();
    }

    // Calleoca init block
    else if(curlvl == L"Docopoper-Calleoca.lvl") {
        gLevelEnum = Calleoca;
        CalleocaInitCode();
    }

    // Science init block
    else if(curlvl == L"Talkhaus-Science_Final_Battle.lvl") {
        gLevelEnum = Science;
        ScienceBattle::ScienceInitCode();
    }

    // EuroShellRandD
    else if(curlvl == L"ztarwuff-EuroShellRandD.lvl") {
        gLevelEnum = EuroShellRandD;
    }

    else if(curlvl == L"Kil-DemosBrain.lvl") {
        gLevelEnum = DemosBrain;
        Player::FilterToSmall(Player::Get(1));
        Player::FilterMount(Player::Get(1));
        Player::FilterReservePowerup(Player::Get(1));
    }

    else if(curlvl == L"LUNA12-thou_starts_a_new_video.lvl") {
        gLevelEnum = ThouStartsANewVideo;
        Player::FilterToBig(Player::Get(1));
        Player::FilterMount(Player::Get(1));
        Player::FilterReservePowerup(Player::Get(1));
        Player::Get(1)->Identity = CHARACTER_MARIO;
    }

}

__declspec(naked) int UltimateProxy()
{
    __asm {
        POP EAX
        JMP EAX
    }
}

int HUDHook()
{
#ifndef __MINGW32__
    __asm {
        MOV AX, WORD PTR DS : 0x00B25130
            CMP AX, 5
            MOV BX, 0
    }
#else
    asm(".intel_syntax noprefix\n"
        "MOV AX, WORD PTR DS: 0x00B25130\n"
        "CMP AX, 5\n"
        "MOV BX, 0\n"
        ".att_syntax\n");
#endif
    return *(unsigned short*)0x00B25130;
}
