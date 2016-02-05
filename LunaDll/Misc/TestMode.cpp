#include <Windows.h>
#include <string>
#include "../Defines.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../SMBXInternal/Sound.h"
#include "../Rendering/Rendering.h"
#include "../Rendering/RenderOps/RenderStringOp.h"
#include "../Rendering/RenderOps/RenderRectOp.h"
#include "MiscFuncs.h"
#include "AsmPatch.h"
#include "RuntimeHook.h"
#include "TestMode.h"

/////////////////////////////////////////////
//================ DEFINES ================//
/////////////////////////////////////////////

static void testModePauseMenu(bool allowContinue);
static bool testModeSetupForLoading();
static void testModeRestartLevel(void);

//////////////////////////////////////////////
//================ SETTINGS ================//
//////////////////////////////////////////////

struct STestModePlayerSettings
{
    Characters identity;
    short powerup;
    short mountType;
    short mountColor;
};

struct STestModeSettings
{
    std::wstring levelPath;
    int playerCount;
    STestModePlayerSettings players[2];

    STestModeSettings()
    {
        ResetToDefault();
    }
    void ResetToDefault(void)
    {
        levelPath = L"";
        playerCount = 1;
        players[0].identity = CHARACTER_MARIO;
        players[0].powerup = 1;
        players[0].mountType = 0;
        players[0].mountColor = 0;
        players[1].identity = CHARACTER_LUIGI;
        players[1].powerup = 1;
        players[1].mountType = 0;
        players[1].mountColor = 0;
    }
};

static STestModeSettings testModeSettings;

////////////////////////////////////////////////
//================ PAUSE MENU ================//
////////////////////////////////////////////////

enum PAUSE_MENU_OPTIONS {
    PAUSE_MENU_CONTINUE = 0,
    PAUSE_MENU_RESTART = 1,
    PAUSE_MENU_QUIT = 2,

    PAUSE_MENU_OPTION_MIN = PAUSE_MENU_CONTINUE,
    PAUSE_MENU_OPTION_MAX = PAUSE_MENU_QUIT,
};

static const wchar_t* pauseMenuOptionStrings[] = {
    L"Continue",
    L"Restart Level",
    L"Quit",
};

static void testModePauseMenu(bool allowContinue)
{
    KeyMap lastKeymap = Player::Get(1)->keymap;
    int topOption = allowContinue ? PAUSE_MENU_CONTINUE : PAUSE_MENU_RESTART;
    int selectedOption = topOption;
    float menuX = 200.0f;
    float menuY = 200.0f;
    float menuW = 400.0f;
    float menuH = 200.0f;
    float lineSpacing = 20.0f;
    float charWidth = 20.0f;

    // Pause Sound
    SMBXSound::PlaySFX(30);

    while(true)
    {
        // Read input...
        native_updateInput();

        KeyMap keymap = Player::Get(1)->keymap;
        if ((keymap.jumpKeyState && !lastKeymap.jumpKeyState) || (GetKeyState(VK_RETURN) & 0x1000))
        {
            break;
        }
        if (keymap.downKeyState && !lastKeymap.downKeyState)
        {
            selectedOption++;
            if (selectedOption > PAUSE_MENU_OPTION_MAX)
            {
                selectedOption = topOption;
            }
            SMBXSound::PlaySFX(71);
        }
        if (keymap.upKeyState && !lastKeymap.upKeyState)
        {
            selectedOption--;
            if (selectedOption < topOption)
            {
                selectedOption = PAUSE_MENU_OPTION_MAX;
            }
            SMBXSound::PlaySFX(71);
        }
        lastKeymap = keymap;

        RenderRectOp* rect = new RenderRectOp();
        rect->x1 = menuX - 5;
        rect->y1 = menuY - 5;
        rect->x2 = menuX + menuW + 5;
        rect->y2 = menuY + menuH + 5;
        rect->fillColor = RenderOpColor(0.0f, 0.0f, 0.0f, 0.7f);
        rect->borderColor = RenderOpColor(0.5f, 0.0f, 0.0f, 1.0f);
        gLunaRender.AddOp(rect);


        gLunaRender.AddOp(new RenderStringOp(L"Level Testing Menu", 4, menuX, menuY));
        for (int displayOption = topOption; displayOption <= PAUSE_MENU_OPTION_MAX; displayOption++)
        {
            float yIdx = menuY + lineSpacing*(displayOption - topOption + 1.5f);
            gLunaRender.AddOp(new RenderStringOp(pauseMenuOptionStrings[displayOption], 4, menuX + charWidth, yIdx));
            if (selectedOption == displayOption) {
                gLunaRender.AddOp(new RenderStringOp(L">", 4, menuX, yIdx));
            }
        }

        // Render the world
        native_updateBlockAnim();
        native_renderLevel();
    
        // Audio management...
        native_audioManagement();

        // Wait for next frame
        if (gIsWindowsVistaOrNewer) {
            FrameTimingMaxFPSHookQPC();
        }
        else {
            FrameTimingMaxFPSHook();
        }

        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    if (selectedOption == PAUSE_MENU_QUIT)
    {
        _exit(0);
    }
    else if (selectedOption == PAUSE_MENU_RESTART)
    {
        // Consider a sound shorter than the unpause one?
        //SMBXSound::PlaySFX(30);

        // Re-load the level
        testModeRestartLevel();
    }
    else
    {
        // Pause Sound
        SMBXSound::PlaySFX(30);

        // TODO: Somehow prevent jump when unpausing
    }
}

/////////////////////////////////////////////
//============ GAME MODE sETUP ============//
/////////////////////////////////////////////

static bool testModeSetupForLoading()
{
    const std::wstring& path = testModeSettings.levelPath;

    // Check that the file exists
    if (FileExists(path.c_str()) == 0)
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

    return true;
}

static void testModeRestartLevel(void)
{
    // Exit level with no destination set
    GM_ISLEVELEDITORMODE = 0;
    GM_CREDITS_MODE = 0;
    GM_LEVEL_MODE = 0; // Intro
    GM_EPISODE_MODE = -1;
    GM_IS_EDITOR_TESTING_NON_FULLSCREEN = 0;

    GM_NEXT_LEVEL_WARPIDX = 0;
    GM_NEXT_LEVEL_FILENAME = "";
}

//////////////////////////////////////////////
//================ NEW HOOK ================//
//////////////////////////////////////////////

static void smbxChangeModeHook(void)
{
    if (GM_ISLEVELEDITORMODE || GM_CREDITS_MODE || GM_LEVEL_MODE ||
        (GM_EPISODE_MODE && (GM_NEXT_LEVEL_FILENAME.length() == 0)))
    {
        testModeSetupForLoading();
    }
}
static __declspec(naked) void __stdcall smbxChangeModeHookRaw(void)
{
    __asm {
        pushf
        push eax
        push ecx
        push edx
    }
    smbxChangeModeHook();
    __asm {
        pop edx
        pop ecx
        pop eax
        popf
        or ebx, 0xFFFFFFFF
        cmp word ptr ds : [0xB2C620], bx
        ret
    }
}
static AsmPatch<10U> smbxChangeModePatch =
    PATCH(0x8BF4E3)
    .CALL(smbxChangeModeHookRaw)
    .NOP_PAD_TO_SIZE<10>();

static AsmPatch<10U> shortenReloadPatch =
    PATCH(0x8C142B).NOP_PAD_TO_SIZE<10>();

// 008CA487 | E8 34 B0 01 00 | call <smbx.GF_MSGBOX>
// 008CA597 | E8 24 AF 01 00 | call <smbx.GF_MSGBOX>
// TODO: Figure out if 0x8CA597 is the right only place, or if others should be patched
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

bool testModeEnable(const std::wstring& path)
{
    // Get the full path if necessary
    std::wstring fullPath;
    if (isAbsolutePath(path)) {
        fullPath = path;
    }
    else
    {
        fullPath = getModulePath() + L"\\worlds\\" + path;
    }

    // Check that the file exists
    if (FileExists(fullPath.c_str()) == 0)
    {
        return false;
    }

    testModeSettings.ResetToDefault();
    testModeSettings.levelPath = fullPath;

    smbxChangeModePatch.Apply();
    shortenReloadPatch.Apply();
    playerDeathOverridePatch.Apply();
    pauseOverridePatch.Apply();
}
