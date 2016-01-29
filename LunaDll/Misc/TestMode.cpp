#include <string>
#include "../Defines.h"
#include "../Globals.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "AsmPatch.h"
#include "RuntimeHook.h"

static void TestModeInitSettings()
{
    GM_STR_CHECKPOINT = ""; // Optionally?

    // No message box or pause menu open
    GM_STR_MSGBOX = "";
    GM_PAUSE_OPEN = 0;

    // Reset winning state
    GM_WINNING = 0;
    GM_WINS_T = 0;

    // Reset game mode setttings
    GM_EPISODE_MODE = 0;
    GM_LEVEL_MODE = 0;
    GM_ISLEVELEDITORMODE = 0;
    GM_IS_EDITOR_TESTING_NON_FULLSCREEN = 0;
    GM_UNK_IS_CONNECTED = 0;
    GM_CUR_MENUTYPE = 0;
    GM_CUR_MENULEVEL = 0;
    GM_CUR_LVL = 0;
    GM_UNK_B2B9E4 = 0;
    GM_UNK_B2C5A0 = 0;
    GM_UNK_B2C6DA = 0;
    GM_UNK_B2C8E4 = 0;
    GM_UNK_B2D742 = 0;

    // Don't use any save slot
    GM_CUR_SAVE_SLOT = 0;

    // Reset scores/counts/lives
    GM_STAR_COUNT = 0;
    GM_COINS = 0;
    GM_PLAYER_LIVES = 99;

    // Reset character templates
    for (int i = 1; i <= 5; i++)
    {
        PlayerMOB* playerTemplate = &((PlayerMOB*)GM_PLAYERS_TEMPLATE)[i];
        memset(playerTemplate, 0, sizeof(PlayerMOB));
        playerTemplate->CurrentPowerup = 1;
        playerTemplate->Identity = (Characters)i;
    }

    // Zero the whole players data structure
    GM_PLAYERS_COUNT = 0;
    memset(GM_PLAYERS_PTR, 0, sizeof(PlayerMOB) * 201);

    // Initialize Player 1
    GM_PLAYERS_COUNT = 1;
    {
        PlayerMOB* player = Player::Get(1);
        player->Hearts = 1;
        player->CurrentPowerup = 1;
        player->MountType = 0;
        player->MountColor = 0;
        player->Identity = CHARACTER_MARIO;

        // Copy this player over the player template
        PlayerMOB* playerTemplate = &((PlayerMOB*)GM_PLAYERS_TEMPLATE)[player->Identity];
        memcpy(playerTemplate, player, sizeof(PlayerMOB));
    }
}

void TestModeLoadLevel(std::wstring fullPath)
{
    // Start by stopping any Lua things
    gLunaLua.exitLevel();

    // Stop music if any is still going
    native_stopMusic();

    // Cleanup custom level resources
    native_cleanupLevel();
	
    // Set our test mode settings
    TestModeInitSettings();
	
    // TODO: Consider setting episode settings?

    // Load the specified level
	GM_FULLPATH = fullPath;
	native_loadLevel(GM_FULLPATH_CONSTPTR);
	
    // Camera initialization
    GM_CAMERA_CONTROL = 0; // Maybe? May need to be different for 2p
    native_initCamera();
	
	// Run this after? Double check that... Some code doesn't run this right after, but some does...
    native_initLevelEnv();
}