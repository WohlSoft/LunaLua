#include <map>

#include "../Globals.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../SMBXInternal/Sound.h"
#include "../Rendering/LunaImage.h"
#include "../Rendering/ImageLoader.h"
#include "../Rendering/Rendering.h"
#include "../Rendering/RenderOps/RenderStringOp.h"
#include "../Rendering/RenderOps/RenderRectOp.h"
#include "../Rendering/RenderOps/RenderBitmapOp.h"
#include "../SMBXInternal/CustomGraphics.h"
#include "../SdlMusic/SdlMusPlayer.h"
#include "LoadScreen.h"
#include "TestMode.h"
#include "TestModeMenu.h"

//////////////////////////////////////////////
//========== MENU RUNNER FUNCTION ==========//
//////////////////////////////////////////////

static bool keepRunningPauseMenu = false;
static bool restartTrigger = false;
static bool exitTrigger = false;
static bool skipTrigger = false;

void testModeClosePauseMenu(bool restart, bool exit)
{
    keepRunningPauseMenu = false;
    restartTrigger = restart;
    exitTrigger = exit;
    skipTrigger = false;
}

void testModeMenuSkipTick()
{
    if (keepRunningPauseMenu)
    {
        skipTrigger = true;
        restartTrigger = false;
        exitTrigger = false;
        keepRunningPauseMenu = false;
    }
}

bool testModeMenuIsSkipTickPending()
{
    return skipTrigger && !gIsTestModePauseActive;
}

void testModeMenuCheckOpenAfterSkipTick()
{
    if (!skipTrigger) return;
    if (gIsTestModePauseActive) return;

    skipTrigger = false;
    testModePauseMenu(true, true);
}

void testModeCheckTriggers()
{
    if (exitTrigger)
    {
        if (!TestModeCheckHideWindow())
        {
            _exit(0);
        }
    }
    else if (restartTrigger)
    {
        testModeRestartLevel();
    }
}

void testModePauseMenu(bool allowContinue, bool skipEnded)
{
    // Skip if already active, don't activate recursively
    if (gIsTestModePauseActive) return;

    // If we're not allowing continue, skip frame needs to be cancelled
    if (!allowContinue)
    {
        skipTrigger = false;
    }

    // Don't re-trigger in this way if skip frame is pending
    if (skipTrigger) return;

    Renderer::QueueStateStacker renderStack;

    gIsTestModePauseActive = true;

    keepRunningPauseMenu = true;
    restartTrigger = false;
    exitTrigger = false;

    // Reset last keymap values
    for (int i = 0; i < 2; i++)
    {
        gRawKeymap[i + 2] = gRawKeymap[i];
    }

    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> inputEvent = std::make_shared<Event>("onStartTestModeMenu", false);
        inputEvent->setDirectEventName("onStartTestModeMenu");
        inputEvent->setLoopable(false);
        gLunaLua.callEvent(inputEvent, allowContinue, skipEnded);
    }
    else
    {
        keepRunningPauseMenu = false;
        restartTrigger = true;
    }
    
    while (keepRunningPauseMenu)
    {
        // Handle un-focused state
        if (!gMainWindowFocused && !LunaLoadScreenIsActive() && !gStartupSettings.runWhenUnfocused)
        {
            // During this block of code, pause music if it was playing
            PGE_MusPlayer::DeferralLock musicPauseLock(true);

            // Wait for focus
            TestModeSendNotification("suspendWhileUnfocusedNotification");
            while (!gMainWindowFocused && !LunaLoadScreenIsActive() && keepRunningPauseMenu && !GM_EPISODE_MODE)
            {
                Sleep(100);
                LunaDllWaitFrame(false);
            }
            TestModeSendNotification("resumeAfterUnfocusedNotification");

            if (!keepRunningPauseMenu || GM_EPISODE_MODE) break;
        }

        // Read input...
        short oldPauseOpen = GM_PAUSE_OPEN;
        GM_PAUSE_OPEN = COMBOOL(true);
        native_updateInput();
        GM_PAUSE_OPEN = oldPauseOpen;

        if (gLunaLua.isValid()) {
            std::shared_ptr<Event> inputEvent = std::make_shared<Event>("onTestModeMenu", false);
            inputEvent->setDirectEventName("onTestModeMenu");
            inputEvent->setLoopable(false);
            gLunaLua.callEvent(inputEvent);
        }
        else
        {
            keepRunningPauseMenu = false;
            restartTrigger = true;
        }

        // Check if we should exit the menu
        if (!keepRunningPauseMenu) break;

        // Render the frame and wait
        LunaDllRenderAndWaitFrame();

        // Exit pause if we're trying to switch
        if (GM_EPISODE_MODE) break;
    }

    testModeCheckTriggers();

    gIsTestModePauseActive = false;
}
