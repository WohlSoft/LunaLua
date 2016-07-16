#include <Windows.h>
#include <string>
#include <mutex>
#include "../../libs/json/json.hpp"
#include "../Defines.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../EventStateMachine.h"
#include "../IPC/IPCPipeServer.h"
#include "MiscFuncs.h"
#include "AsmPatch.h"
#include "RuntimeHook.h"
#include "WaitForTickEnd.h"

#include "TestModeMenu.h"
#include "TestMode.h"

using json = nlohmann::json;

//////////////////////////////////////////////
//============ GLOBAL VARIABLES ============//
//////////////////////////////////////////////
static std::mutex g_testModeMutex;

///////////////////////////////////////////////
//============== UTILITY PATCH ==============//
///////////////////////////////////////////////

// Patch to allow exiting the pause menu. Apply when the vanilla pause/textbox
// should be instantly exited always. Unapply when this should not be the case.
static auto exitPausePatch = PATCH(0x8E6564).NOP().NOP().NOP().NOP().NOP().NOP();

//////////////////////////////////////////////
//================ SETTINGS ================//
//////////////////////////////////////////////

STestModeSettings::STestModeSettings()
{
    ResetToDefault();
}
void STestModeSettings::ResetToDefault(void)
{
    enabled = false;
    levelPath = L"";
    levelData = "";
    playerCount = 1;
	showFPS = false;
	godMode = false;
    players[0].identity = CHARACTER_MARIO;
    players[0].powerup = 1;
    players[0].mountType = 0;
    players[0].mountColor = 0;
    players[1].identity = CHARACTER_LUIGI;
    players[1].powerup = 1;
    players[1].mountType = 0;
    players[1].mountColor = 0;
}

static STestModeSettings testModeSettings;

STestModeSettings getTestModeSettings()
{
    return testModeSettings;
}
void setTestModeSettings(const STestModeSettings& settings)
{
    testModeSettings = settings;
}

/////////////////////////////////////////////
//============ GAME MODE SETUP ============//
/////////////////////////////////////////////

void testModeRestartLevel(void)
{
    // Start by stopping any Lua things
    gLunaLua.exitLevel();

    // Make sure we unpause
    exitPausePatch.Apply();
    GM_STR_MSGBOX = "";
    GM_PAUSE_OPEN = 0;
    GM_CUR_MENUCHOICE = 0;
    GM_CUR_MENUTYPE = 0;

    // Exit level with no destination set
    GM_ISLEVELEDITORMODE = 0;
    GM_CREDITS_MODE = 0;
    GM_LEVEL_MODE = 0; // Intro
    GM_EPISODE_MODE = -1;
    GM_IS_EDITOR_TESTING_NON_FULLSCREEN = 0;

    GM_NEXT_LEVEL_WARPIDX = 0;
    GM_NEXT_LEVEL_FILENAME = "";
}

static bool testModeSetupForLoading()
{
    const std::wstring& path = testModeSettings.levelPath;

    // Check that the file exists, but only if we don't have raw level data
    if ((testModeSettings.levelData.size() == 0) && (FileExists(path.c_str()) == 0))
    {
        return false;
    }
    
    // Start by stopping any Lua things
    gLunaLua.exitLevel();

    // Stop music if any is still going
    native_stopMusic();

    // Cleanup custom level resources
    native_cleanupLevel();
	
    // Reset character templates
    for (int i = 1; i <= 5; i++)
    {
        PlayerMOB* playerTemplate = &((PlayerMOB*)GM_PLAYERS_TEMPLATE)[i];
        memset(playerTemplate, 0, sizeof(PlayerMOB));
        playerTemplate->Hearts = 1;
        playerTemplate->CurrentPowerup = 1;
        playerTemplate->Identity = (Characters)i;
    }

    // Zero the whole players data structure
    GM_PLAYERS_COUNT = 0;
    memset(GM_PLAYERS_PTR, 0, sizeof(PlayerMOB) * 201);

    // Initialize Players
    GM_PLAYERS_COUNT = testModeSettings.playerCount;
    for (int i = 1; i <= testModeSettings.playerCount; i++) {
        PlayerMOB* player = Player::Get(i);
        player->Hearts = 1;
        player->CurrentPowerup = testModeSettings.players[i - 1].powerup;
        player->MountType = testModeSettings.players[i - 1].mountType;
        player->MountColor = testModeSettings.players[i - 1].mountColor;
        player->Identity = testModeSettings.players[i - 1].identity;

        // Copy this player over the player template
        PlayerMOB* playerTemplate = &((PlayerMOB*)GM_PLAYERS_TEMPLATE)[player->Identity];
        memcpy(playerTemplate, player, sizeof(PlayerMOB));
    }

    // Overwrite episode list data
    size_t pos = path.find_last_of(L"/\\");
    GM_EP_LIST_COUNT = 1;
    EpisodeListItem* ep = EpisodeListItem::GetRaw(0);
    ep->episodeName = "Test Mode";
    ep->episodePath = path.substr(0, pos+1);
    ep->episodeWorldFile = "";
    ep->unknown_C = 0;
    ep->unknown_10 = 0;
    ep->unknown_14 = "";

	// God Mode cheat code
	GM_PLAYER_INVULN = COMBOOL(testModeSettings.godMode);

    // Show FPS counter
    GM_SHOW_FPS = COMBOOL(testModeSettings.showFPS);
    if(!testModeSettings.showFPS)
    {
        //Must be zeroed or it will still shown!
        GM_FPS_VALUE = 0.0;
    }

    // Select dummy episode entry
    GM_CUR_MENULEVEL = 1;

    // Write warp destination data
    GM_NEXT_LEVEL_WARPIDX = 0;
    GM_NEXT_LEVEL_FILENAME = path.substr(pos+1);

    // Don't use any save slot
    GM_CUR_SAVE_SLOT = 0;

    // Reset scores/counts/lives
    GM_STAR_COUNT = 0;
    GM_COINS = 0;
    GM_PLAYER_LIVES = 99;

    // Clear some state
    GM_STR_MSGBOX = "";
    GM_PAUSE_OPEN = 0;
    GM_WINNING = 0;
    GM_WINS_T = 0;

    // Set mode settings
    GM_ISLEVELEDITORMODE = 0;
    GM_CREDITS_MODE = 0;
    GM_LEVEL_MODE = 0; // Intro
    GM_EPISODE_MODE = -1;
    GM_IS_EDITOR_TESTING_NON_FULLSCREEN = 0;

    // Unapply exit pause patch
    exitPausePatch.Unapply();

    return true;
}

//////////////////////////////////////////////
//================ NEW HOOK ================//
//////////////////////////////////////////////

// Helper function to get the main window
// TODO: Consider replacing with something better, that uses some memory
//       address or something instead.
static HWND GetMainWindow(void)
{
    // This here is a big mess of a workaround... but it works
    HWND hWindow = NULL;
    DWORD dwCurrentProcessId = GetCurrentProcessId();
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD dwCurrentProcessId = GetCurrentProcessId();
        HWND& hWindow = *reinterpret_cast<HWND*>(lParam);
        // Check that it's our process
        DWORD dwProcessId = 0x0;
        GetWindowThreadProcessId(hWnd, &dwProcessId);
        if (dwCurrentProcessId == dwProcessId) {
            // Now check the class and if it's top level
            wchar_t className[24] = { 0 };
            wchar_t windowName[24] = { 0 };
            HWND hParent = GetParent(hWnd);
            GetWindowTextW(hWnd, windowName, sizeof(windowName));
            GetClassNameW(hWnd, className, sizeof(className));
            if ((wcscmp(className, L"ThunderRT6FormDC") == 0) && (wcscmp(windowName, L"Graphics") != 0))
            {
                hWindow = hWnd;
                SetLastError(ERROR_SUCCESS);
                return FALSE;
            }
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&hWindow));
    return hWindow;
}

// Utility function to create a temporary file and write data to it. The
// filename is returned if successful, or an empty string if unsuccessful.
static std::wstring WriteTemporaryFile(const std::string& data)
{
    wchar_t tempPath[MAX_PATH];
    wchar_t tempFileName[MAX_PATH];

    // Get a temporary file allocated, first try to use the temporary path
    // specified by Windows, but if this fails, we can also try the application
    // path.
    uint32_t pathRet = GetTempPathW(MAX_PATH, tempPath);
    uint32_t tempNameRet = 0;
    if (pathRet < MAX_PATH-14 && pathRet != 0)
    {
        tempNameRet = GetTempFileNameW(tempPath, L"LunaLevel", 0, tempFileName);
    }
    if (tempNameRet == 0)
    {
        tempNameRet = GetTempFileNameW(gAppPathWCHAR.c_str(), L"LunaLevel", 0, tempFileName);
    }
    if (tempNameRet == 0)
    {
        return L"";
    }

    FILE* file = nullptr;
    if (_wfopen_s(&file, tempFileName, L"wb") != 0)
    {
        DeleteFileW(tempFileName);
        return L"";
    }

    if (fwrite(data.c_str(), 1, data.size(), file) != data.size())
    {
        fclose(file);
        DeleteFileW(tempFileName);
        return L"";
    }

    fclose(file);
    return tempFileName;
}

static VB6StrPtr temporaryLevelFn;
void __stdcall testModeVbaFileOpenHook(DWORD arg1, DWORD arg2, DWORD arg3, BSTR* filename)
{
    auto vbaFileOpenPtr = (void(__stdcall *)(DWORD, DWORD, DWORD, BSTR*))IMP_vbaFileOpen;

    vbaFileOpenPtr(arg1, arg2, arg3, (BSTR*)temporaryLevelFn.ptr);
}

bool testModeLoadLevelHook(VB6StrPtr* filename)
{
    // Skip if not enabled
    if (!testModeSettings.enabled) return false;
    
    // If the filename matches the one we're testing, and we have raw level data, let's use it
    if (*filename == testModeSettings.levelPath && testModeSettings.levelData.size() > 0)
    {
        auto testModeVbaFileOpenHookPatch = PATCH(0x8D97D1).CALL(testModeVbaFileOpenHook).NOP_PAD_TO_SIZE<6>();

        // Create a temporary file with the level data
        std::wstring tempFile = WriteTemporaryFile(testModeSettings.levelData).c_str();
        if (tempFile.size() == 0)
        {
            dbgboxA("Could not write temporary file!");
            return false;
        }
        
        // Load level with data from the temporary file
        temporaryLevelFn = tempFile;
        testModeVbaFileOpenHookPatch.Apply();
        loadLevel_OrigFunc(filename);
        testModeVbaFileOpenHookPatch.Unapply();

        // Delete the temporary file
        DeleteFileW(tempFile.c_str());

        return true;
    }

    return false;
}

void testModeSmbxChangeModeHook(void)
{
    // Skip if not enabled
    if (!testModeSettings.enabled) return;

    if (GM_ISLEVELEDITORMODE || GM_CREDITS_MODE || GM_LEVEL_MODE ||
        (GM_EPISODE_MODE && (GM_NEXT_LEVEL_FILENAME.length() == 0)))
    {
        // Preprate to load/reload
        testModeSetupForLoading();
    }
}

static AsmPatch<10U> shortenReloadPatch =
    PATCH(0x8C142B).NOP_PAD_TO_SIZE<10>();

// 008CA487 | E8 34 B0 01 00 | call <smbx.GF_MSGBOX>
// 008CA597 | E8 24 AF 01 00 | call <smbx.GF_MSGBOX>
// TODO: Figure out if 0x8CA597 is the right only place, or if others should be patched
//    TESTED: patching of the 0x8CA487 causes crash on loading
static void __stdcall pauseTestModeHook(short* pPlayer);
static AsmPatch<5U> pauseOverridePatch = PATCH(0x8CA597).CALL(pauseTestModeHook);
static void __stdcall pauseTestModeHook(short* pPlayer)
{
    testModePauseMenu(true);
}

// 008C23C6 | E8 B5 53 0F 00 | call <smbx.doPlayerDeadCode>
static void __stdcall playerDeathTestModeHook(void);
static AsmPatch<5U> playerDeathOverridePatch = PATCH(0x8C23C6).CALL(playerDeathTestModeHook);
static void __stdcall playerDeathTestModeHook(void)
{
    testModePauseMenu(false);
}

bool testModeEnable(const STestModeSettings& settings)
{
    // Get the full path if necessary
    std::wstring path = settings.levelPath;
    std::wstring fullPath;
    if (isAbsolutePath(path)) {
        fullPath = path;
    }
    else
    {
        fullPath = gAppPathWCHAR + L"\\worlds\\" + path;
    }

    // Check that the file exists, but only if we don't have raw level data
    if ((settings.levelData.size() == 0) && (FileExists(fullPath.c_str()) == 0))
    {
        return false;
    }

    testModeSettings = settings;
    testModeSettings.enabled = true;
    testModeSettings.levelPath = fullPath;

    shortenReloadPatch.Apply();
    playerDeathOverridePatch.Apply();
    pauseOverridePatch.Apply();

    return true;
}

void testModeDisable(void)
{
    testModeSettings.ResetToDefault();
    testModeSettings.enabled = false;

    shortenReloadPatch.Unapply();
    playerDeathOverridePatch.Unapply();
    pauseOverridePatch.Unapply();
    exitPausePatch.Unapply();
}

// IPC Command
json IPCTestLevel(const json& params)
{
    std::lock_guard<std::mutex> testModeLock(g_testModeMutex);

    if (!params.is_object()) throw IPCInvalidParams();
    json::const_iterator filenameIt = params.find("filename");
    if ((filenameIt == params.cend()) || (!filenameIt.value().is_string())) throw IPCInvalidParams();

    // Default to the last settings for characters, if not changed by IPC
    // command
    STestModeSettings settings = testModeSettings;
    settings.enabled = true;
    settings.levelData = "";
    settings.levelPath = Str2WStr(filenameIt.value());

    // Get character/player information
    json::const_iterator playersIt = params.find("players");
    if (playersIt != params.cend())
    {
        const json& players = playersIt.value();
        if (!players.is_array()) throw IPCInvalidParams();
        if (players.size() < 1 || players.size() > 2) throw IPCInvalidParams();
        settings.playerCount = players.size();

        int i = 0;
        for (json::const_iterator playersIt = players.cbegin(); playersIt != players.cend(); playersIt++, i++) {
            const json& player = playersIt.value();
            STestModePlayerSettings& playerSettings = settings.players[i];
            if (!player.is_object()) throw IPCInvalidParams();

            // Set character
            json::const_iterator characterIt = player.find("character");
            if (characterIt != player.cend())
            {
                if (!characterIt.value().is_number_integer()) throw IPCInvalidParams();
                short characterInt = static_cast<short>(characterIt.value());
                playerSettings.identity = static_cast<Characters>(characterInt);
            }
            
            // Set powerup
            json::const_iterator powerupIt = player.find("powerup");
            if (powerupIt != player.cend())
            {
                if (!powerupIt.value().is_number_integer()) throw IPCInvalidParams();
                playerSettings.powerup = static_cast<short>(powerupIt.value());
            }

            // Set mountType
            json::const_iterator mountTypeIt = player.find("mountType");
            if (mountTypeIt != player.cend())
            {
                if (!mountTypeIt.value().is_number_integer()) throw IPCInvalidParams();
                playerSettings.mountType = static_cast<short>(mountTypeIt.value());
            }

            // Set mountColor
            json::const_iterator mountColorIt = player.find("mountColor");
            if (mountColorIt != player.cend())
            {
                if (!mountColorIt.value().is_number_integer()) throw IPCInvalidParams();
                playerSettings.mountColor = static_cast<short>(mountColorIt.value());
            }
        }
    }

    json::const_iterator levelDataIt = params.find("leveldata");
    if (levelDataIt != params.cend() && !levelDataIt.value().is_null())
    {
        if (!levelDataIt.value().is_string()) throw IPCInvalidParams();
        settings.levelData = static_cast<const std::string&>(levelDataIt.value()); 
    }

    // Set godMode flag
    json::const_iterator godModeFlag = params.find("godMode");
    if (godModeFlag != params.cend() && !levelDataIt.value().is_null())
    {
	    if (!godModeFlag.value().is_boolean()) throw IPCInvalidParams();
	    settings.godMode = static_cast<bool>(godModeFlag.value());
    }

    // Set godMode flag
    json::const_iterator showFpsFlag = params.find("showFPS");
    if (showFpsFlag != params.cend() && !levelDataIt.value().is_null())
    {
        if (!showFpsFlag.value().is_boolean()) throw IPCInvalidParams();
        settings.showFPS = static_cast<bool>(showFpsFlag.value());
    }

    // Before checking for tick end... bring to top if we need to
    HWND hWindow = GetMainWindow();
    if (hWindow)
    {
        ShowWindow(hWindow, SW_SHOW);

        SetWindowPos(hWindow, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
        SetWindowPos(hWindow, HWND_NOTOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);

        BringWindowToTop(hWindow);
        SetForegroundWindow(hWindow);
        SetFocus(hWindow);
    }

    {
        // Make sure we're at a tick end
        WaitForTickEnd tickEndLock;

        // Attempt to enable the test
        if (!testModeEnable(settings))
        {
            return false;
        }
        testModeRestartLevel();

        // If we were waiting on IPC, stop waiting
        gStartupSettings.currentlyWaitingForIPC = false;
    }
    return true;
}

bool TestModeCheckHideWindow(void)
{
    // If we started by waiting for IPC, we want to hide the window in place of exiting
    if (gStartupSettings.waitForIPC)
    {
        std::lock_guard<std::mutex> testModeLock(g_testModeMutex);
        
        // Close the level using testModeRestartLevel, but flag that we'll be
        // waiting for IPC again.
        testModeRestartLevel();
        gStartupSettings.currentlyWaitingForIPC = true;
        HWND hWindow = GetMainWindow();
        if (hWindow)
        {
            ShowWindow(hWindow, SW_HIDE);
        }

        return true;
    }

    return false;
}

json IPCGetWindowHandle(const json& /*params*/)
{
    HWND window = GetMainWindow();
    unsigned long ptr = ULONG_PTR(window);
    return ptr;
}
