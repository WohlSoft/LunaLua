#include <string>
#include "Globals.h"
#include "GlobalFuncs.h"
#include "EventStateMachine.h"
#include "Misc/RuntimeHook.h"
#include "Misc/TestMode.h"
#include "Misc/TestModeMenu.h"
#include "Misc/LoadScreen.h"
#include "Rendering/GL/GLEngineProxy.h"
#include "SdlMusic/SdlMusPlayer.h"
#include "SMBXInternal/Reconstructed/EpisodeMain.h"

// Global instance
EventStateMachine g_EventHandler;

// Helper function
template <typename... Ts>
static inline void sendSimpleLuaEvent(const std::string& eventName, Ts&&... args) {
    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> inputEvent = std::make_shared<Event>(eventName, false);
        inputEvent->setDirectEventName(eventName);
        inputEvent->setLoopable(false);
        gLunaLua.callEvent(inputEvent, std::forward<Ts>(args)...);
    }
}

void LunaDllRenderAndWaitFrame(void)
{
    // Render the world
    if (gIsOverworld) {
        // TODO: Is there any animation we want to update in the overworld?
        native_renderWorld();
    }
    else {
        native_updateBlockAnim();
        native_renderLevel();
    }

    // Audio management...
    native_audioManagement();

    LunaDllWaitFrame();
}

void LunaDllWaitFrame(bool allowMaxFPS)
{
    native_rtcDoEvents();

    if (allowMaxFPS)
    {
        if (Luna_IsWindowsVistaOrNewer()) {
            FrameTimingMaxFPSHookQPC();
        }
        else {
            FrameTimingMaxFPSHook();
        }
    }
    else
    {
        if (Luna_IsWindowsVistaOrNewer()) {
            FrameTimingHookQPC();
        }
        else {
            FrameTimingHook();
        }
    }

    native_rtcDoEvents();
}

// Public methods (Notifications of State)
void EventStateMachine::reset(void) {
    m_onTickReady = false;
    m_onTickEndReady = false;
    m_onDrawEndReady = false;
    m_RequestPause = false;
    m_RequestPauseAtFrameEnd = false;
    m_RequestUnpause = false;
    m_IsPaused = false;
}

void EventStateMachine::hookLevelLoop(void) {
    if (gLunaLua.isValid() && (gLunaLua.getType() == CLunaLua::LUNALUA_LEVEL))
    {
        // Check if we should pause
        checkPause();

        if (!gIsTestModePauseActive)
        {
            sendOnLoop();
        }
    }
}

void EventStateMachine::hookWorldLoop(void) {
    if (gLunaLua.isValid() && (gLunaLua.getType() == CLunaLua::LUNALUA_WORLD))
    {
        // Check if we should pause
        checkPause();

        if (!gIsTestModePauseActive)
        {
            sendOnLoop();
        }
    }
}

void EventStateMachine::hookInputUpdate(void) {
    if (!gIsTestModePauseActive)
    {
        sendOnInputUpdate();

        if (m_onTickReady) {
            sendOnTick();
        }
    }
}

void EventStateMachine::hookLevelRenderStart(void) {
    if (m_onTickEndReady) {
        sendOnTickEnd();
    }
}

void EventStateMachine::hookLevelRenderFirstCameraStart(void) {
    sendOnDraw();
}

void EventStateMachine::hookLevelRenderEnd(void) {
    if (m_onDrawEndReady) {
        m_loadTimeMusicDeferral.Unlock();
        sendOnDrawEnd();
    }
}

void EventStateMachine::hookWorldRenderStart(void) {
    if (m_onTickEndReady) {
        sendOnTickEnd();
    }

    sendOnDraw();
}
void EventStateMachine::hookWorldRenderEnd(void) {
    if (m_onDrawEndReady) {
        m_loadTimeMusicDeferral.Unlock();
        sendOnDrawEnd();
    }
}

void EventStateMachine::loadTimeDeferMusic(void)
{
    m_loadTimeMusicDeferral.Lock();
}

// Private methods (Outgoing events)
void EventStateMachine::sendOnLoop(void) {
    // We're ready for onTick after any onLoop
    m_onTickReady = true;

    // TODO: Consider moving onStart handling into this class
    gLunaLua.doEvents();
}

void EventStateMachine::sendOnInputUpdate(void) {
    sendSimpleLuaEvent("onInputUpdate");
}

void EventStateMachine::sendOnTick(void) {
    m_onTickReady = false;

    sendSimpleLuaEvent("onTick");
    m_onTickEndReady = true;
}

void EventStateMachine::sendOnTickEnd(void) {
    m_onTickEndReady = false;

    sendSimpleLuaEvent("onTickEnd");
}

void EventStateMachine::sendOnDraw(void) {
    GLEngineProxy::CheckRendererInit();
    sendSimpleLuaEvent("onDraw");

    m_onDrawEndReady = true;
}

void EventStateMachine::sendOnDrawEnd(void) {
    GLEngineProxy::CheckRendererInit();
    m_onDrawEndReady = false;

    sendSimpleLuaEvent("onDrawEnd");

    if (!m_IsPaused && m_RequestPauseAtFrameEnd) {
        m_RequestPauseAtFrameEnd = false;
        runPause();
    }

    // Check to see if the test mode menu needs to be re-opened after a skipped tick
    testModeMenuCheckOpenAfterSkipTick();
}

// Public methods (pause requests)
void EventStateMachine::requestPause(bool atFrameEnd) {
    if (!m_IsPaused) {
        if (atFrameEnd)
        {
            m_RequestPauseAtFrameEnd = true;
        }
        else
        {
            m_RequestPause = true;
        }

    }
    m_RequestUnpause = false;
}

void EventStateMachine::requestUnpause(void) {
    if (m_IsPaused) {
        m_RequestUnpause = true;
    }
    m_RequestPause = false;
    m_RequestPauseAtFrameEnd = false;
}

bool EventStateMachine::isPaused(void) {
    return m_IsPaused;
}

// Paused game logic
void EventStateMachine::checkPause(void) {
    if (!m_IsPaused && m_RequestPause) {
        m_RequestPause = false;
        runPause();
    }
}

void EventStateMachine::runPause(void) {
    Renderer::QueueStateStacker renderStack;
    m_IsPaused = true;
    while (!m_RequestUnpause) {
        // Handle un-focused state
        if (!gMainWindowFocused && !LunaLoadScreenIsActive())
        {
            // During this block of code, pause music if it was playing
            PGE_MusPlayer::DeferralLock musicPauseLock(true);

            // Wait for focus
            TestModeSendNotification("suspendWhileUnfocusedNotification");
            while (!gMainWindowFocused && !LunaLoadScreenIsActive() && !m_RequestUnpause)
            {
                Sleep(100);
                LunaDllWaitFrame(false);
            }
            TestModeSendNotification("resumeAfterUnfocusedNotification");

            if (m_RequestUnpause) break;
        }

        // Read input
        short oldPauseOpen = GM_PAUSE_OPEN;
        GM_PAUSE_OPEN = COMBOOL(true);
        native_updateInput();
        GM_PAUSE_OPEN = oldPauseOpen;

        // Render the frame and wait
        LunaDllRenderAndWaitFrame();
    }
    m_RequestUnpause = false;
    m_IsPaused = false;
}