#include <Windows.h>
#include <string>
#include <mutex>
#include "../../libs/json/json.hpp"
#include "../../libs/PGE_File_Formats/file_formats.h"
#include "../Defines.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../FileManager/SMBXFileManager.h"
#include "../EventStateMachine.h"
#include "../IPC/IPCPipeServer.h"
#include "MiscFuncs.h"
#include "AsmPatch.h"
#include "RuntimeHook.h"
#include "WaitForTickEnd.h"
#include "PGEEditorCmdSender.h"
#include "LoadScreen.h"

#include "TestModeMenu.h"
#include "TestMode.h"

using json = nlohmann::json;

//////////////////////////////////////////////
//============ GLOBAL VARIABLES ============//
//////////////////////////////////////////////
static std::mutex g_testModeMutex;
static struct {
    volatile bool requestFlag;
    STestModeSettings settings;
} g_testModeIPCRequest = { false, {} };
static volatile bool g_testModePendingLoad = false;

static STestModeSettings testModeSettings;
static STestModeData     testModeData;

///////////////////////////////////////////////
//============== UTILITY PATCH ==============//
///////////////////////////////////////////////

// Patch to allow exiting the pause menu. Apply when the vanilla pause/textbox
// should be instantly exited always. Unapply when this should not be the case.
static auto exitPausePatch = PATCH(0x8E6564).NOP().NOP().NOP().NOP().NOP().NOP();

//////////////////////////////////////////////
//================ SETTINGS ================//
//////////////////////////////////////////////

void STestModeData::clear()
{
    levelRawData.clear();
    FileFormats::CreateLevelData(levelData);
}

STestModeSettings::STestModeSettings()
{
    ResetToDefault();
}
void STestModeSettings::ResetToDefault(void)
{
    enabled = false;
    levelPath = L"";
    rawData = "";
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
    entranceIndex = 0;
}

STestModeSettings getTestModeSettings()
{
    return testModeSettings;
}

static void sendTestSettingsToPgeEditor(const STestModeSettings &settings)
{
    PGE_EditorCommandSender feedBack;
    std::string cmd = "CMD:testSetup:";
    cmd += std::to_string(0) + ",";
    cmd += std::to_string(settings.players[0].identity) + ",";
    cmd += std::to_string(settings.players[0].powerup) + ",";
    cmd += std::to_string(settings.players[0].mountType) + ",";
    cmd += std::to_string(settings.players[0].mountColor);
    feedBack.sendCommandUTF8(cmd);
}

void setTestModeSettings(const STestModeSettings& settings)
{
    sendTestSettingsToPgeEditor(settings);
    testModeSettings = settings;
}

LevelData &getCurrentLevelData()
{
    return testModeData.levelData;
}


/////////////////////////////////////////////
//============ GAME MODE SETUP ============//
/////////////////////////////////////////////

void testModeRestartLevel(void)
{
    // Start by stopping any Lua things
    gLunaLua.exitContext();

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

    g_testModePendingLoad = true;
}

static bool testModeSetupForLoading()
{
    const std::wstring& path = testModeSettings.levelPath;

    // Check that the file exists, but only if we don't have raw level data
    if ((testModeData.levelRawData.size() == 0) && (FileExists(path.c_str()) == 0))
    {
        return false;
    }

    // Start by stopping any Lua things
    gLunaLua.exitContext();

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

    // Set episode path...
    GM_FULLDIR = path.substr(0, pos + 1);

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
    GM_NEXT_LEVEL_WARPIDX = testModeSettings.entranceIndex;
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

    // Make sure load screen is started
    LunaLoadScreenStart();

    return true;
}

//////////////////////////////////////////////
//================ NEW HOOK ================//
//////////////////////////////////////////////

bool testModeLoadLevelHook(VB6StrPtr* filename)
{
    // Skip if not enabled
    if (!testModeSettings.enabled) return false;

    // Clear pending load status
    g_testModePendingLoad = false;

    // If the filename matches the one we're testing, and we have raw level data, let's use it
    if (*filename == testModeSettings.levelPath && testModeData.levelRawData.size() > 0)
    {
        SMBXLevelFileBase base;
        base.ReadFileMem(testModeData.levelRawData, getCurrentLevelData(), testModeSettings.levelPath);
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
        fullPath = normalizePathSlashes(path);
        replaceSubStrW(fullPath, L"/", L"\\");
    }
    else
    {
        fullPath = normalizePathSlashes(gCwdPathWCHAR + L"\\" + path);
        replaceSubStrW(fullPath, L"/", L"\\");
        if (!fileExists(fullPath))
        {
            std::wstring worldsPath = normalizePathSlashes(gAppPathWCHAR + L"\\worlds\\" + path);
            replaceSubStrW(worldsPath, L"/", L"\\");
            if (fileExists(worldsPath))
            {
                fullPath = worldsPath;
            }
        }
    }

    const std::string &newLevelData = settings.rawData;

    // Check that the file exists, but only if we don't have raw level data
    if (newLevelData.empty() && (FileExists(fullPath.c_str()) == 0))
    {
        return false;
    }

    testModeData.levelRawData.clear();
    if(!newLevelData.empty())
        testModeData.levelRawData = newLevelData;
    testModeSettings = settings;
    testModeSettings.enabled = true;
    testModeSettings.levelPath = fullPath;

    //shortenReloadPatch.Apply();
    playerDeathOverridePatch.Apply();
    pauseOverridePatch.Apply();

    return true;
}

void testModeDisable(void)
{
    testModeData.clear();
    testModeSettings.ResetToDefault();
    testModeSettings.enabled = false;

    //shortenReloadPatch.Unapply();
    playerDeathOverridePatch.Unapply();
    pauseOverridePatch.Unapply();
    exitPausePatch.Unapply();
}

// IPC Command
json IPCTestLevel(const json& params)
{
    if (!params.is_object()) throw IPCInvalidParams();
    json::const_iterator filenameIt = params.find("filename");
    if ((filenameIt == params.cend()) || (!filenameIt.value().is_string())) throw IPCInvalidParams();

    // Default to the last settings for characters, if not changed by IPC
    // command
    STestModeSettings settings;
    {
        std::lock_guard<std::mutex> testModeLock(g_testModeMutex);
        settings = testModeSettings;
    }
    settings.enabled = true;
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
                if (characterInt >= 1)
                {
                    playerSettings.identity = static_cast<Characters>(characterInt);
                }
                else if (playerSettings.identity <= 0)
                {
                    playerSettings.identity = static_cast<Characters>(1);
                }
            }

            // Set powerup
            json::const_iterator powerupIt = player.find("powerup");
            if (powerupIt != player.cend())
            {
                if (!powerupIt.value().is_number_integer()) throw IPCInvalidParams();
                short powerupInt = static_cast<short>(powerupIt.value());
                if (powerupInt >= 1)
                {
                    playerSettings.powerup = powerupInt;
                }
                else if (playerSettings.powerup <= 0)
                {
                    playerSettings.powerup = 1;
                }
            }

            // Set mountType
            json::const_iterator mountTypeIt = player.find("mountType");
            if (mountTypeIt != player.cend())
            {
                if (!mountTypeIt.value().is_number_integer()) throw IPCInvalidParams();
                short mountTypeInt = static_cast<short>(mountTypeIt.value());
                if (mountTypeInt >= 0)
                {
                    playerSettings.mountType = mountTypeInt;
                }
                else if (playerSettings.mountType < 0)
                {
                    playerSettings.mountType = 0;
                }
            }

            // Set mountColor
            json::const_iterator mountColorIt = player.find("mountColor");
            if (mountColorIt != player.cend())
            {
                if (!mountColorIt.value().is_number_integer()) throw IPCInvalidParams();
                short mountColorInt = static_cast<short>(mountColorIt.value());
                if (mountColorInt >= 0)
                {
                    playerSettings.mountColor = mountColorInt;
                }
                else if (playerSettings.mountColor < 0)
                {
                    playerSettings.mountColor = 0;
                }
            }
        }
    }

    json::const_iterator levelDataIt = params.find("leveldata");
    if (levelDataIt != params.cend() && !levelDataIt.value().is_null())
    {
        if (!levelDataIt.value().is_string()) throw IPCInvalidParams();
        settings.rawData = static_cast<const std::string&>(levelDataIt.value());
    }
    else
    {
        settings.rawData = "";
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

    // Copy to pending request
    {
        std::lock_guard<std::mutex> testModeLock(g_testModeMutex);
        g_testModeIPCRequest.settings = settings;
        g_testModeIPCRequest.requestFlag = true;
    }

    return true;
}

void TestModeCheckPendingIPCRequest()
{
    // Copy to pending request
    if (g_testModeIPCRequest.requestFlag)
    {
        STestModeSettings settings;
        {
            std::lock_guard<std::mutex> testModeLock(g_testModeMutex);
            if (!g_testModeIPCRequest.requestFlag) return;
            settings = g_testModeIPCRequest.settings;
            g_testModeIPCRequest.requestFlag = false;
        }

        // Attempt to enable the test
        if (!testModeEnable(settings))
        {
            std::wstring path = L"SMBX received no level data from the editor. Please try again.";
            MessageBoxW(0, path.c_str(), L"Error", MB_ICONERROR);
            _exit(1);
            return;
        }
        testModeRestartLevel();

        // If we were waiting on IPC, stop waiting
        gStartupSettings.currentlyWaitingForIPC = false;

        // Show and bring to top when we start loading in test mode
        ShowAndFocusWindow(gMainWindowHwnd);
    }
}

bool TestModeCheckHideWindow(void)
{
    // If we started by waiting for IPC, we want to hide the window in place of exiting
    if (gStartupSettings.waitForIPC)
    {
        if (g_testModePendingLoad || LunaLoadScreenIsActive())
        {
            // Ignore close request if mid-loading
            return true;
        }

        {
            std::lock_guard<std::mutex> testModeLock(g_testModeMutex);

            // Start by stopping any Lua things
            gLunaLua.exitContext();

            // Kill any active load screen
            LunaLoadScreenKill();

            // Stop music if any is still going
            native_stopMusic();

            // Close the level using testModeRestartLevel, but flag that we'll be
            // waiting for IPC again.
            gStartupSettings.currentlyWaitingForIPC = true;
            HWND hWindow = gMainWindowHwnd;
            if (hWindow)
            {
                ShowWindow(hWindow, SW_HIDE);
            }
        }

        TestModeSendNotification("closedToBackgroundNotification");
        while (gStartupSettings.currentlyWaitingForIPC)
        {
            Sleep(100);
            LunaDllWaitFrame(false);
        }
        TestModeSendNotification("showFromBackgroundNotification");

        return true;
    }

    return false;
}

bool TestModeIsEnabled(void)
{
    return testModeSettings.enabled;
}

bool TestModeIsLoadPending(void)
{
    return testModeSettings.enabled && g_testModePendingLoad;
}

json IPCResetCheckPoint(const json& /*params*/)
{
    GM_STR_CHECKPOINT = "";
    return true;
}

json IPCSetCheckPoint(const json& params)
{
    if (!params.is_object()) throw IPCInvalidParams();
    json::const_iterator warpEntranceIt = params.find("warpEntrance");
    if ((warpEntranceIt == params.cend()) || (!warpEntranceIt.value().is_string())) throw IPCInvalidParams();

    return true;
}


json IPCGetWindowHandle(const json& /*params*/)
{
    unsigned long ptr = ULONG_PTR(gMainWindowHwnd);
    return ptr;
}

void TestModeSendNotification(const std::string& notificationCmd)
{
    gIPCServer.SendSimpleNotification(notificationCmd);
}
