#ifndef EventStateMachine_Hhh
#define EventStateMachine_Hhh

#include "../../SdlMusic/SdlMusPlayer.h"

class EventStateMachine {
// Member variables
private:
    bool m_onTickReady;
    bool m_onTickEndReady;
    bool m_onDrawEndReady;

    bool m_RequestPause;
    bool m_RequestPauseAtFrameEnd;
    bool m_RequestUnpause;
    bool m_IsPaused;

    PGE_MusPlayer::DeferralLock m_loadTimeMusicDeferral;

// Constructor and Destructor
public:
    EventStateMachine() : m_loadTimeMusicDeferral() { reset(); }
    ~EventStateMachine() { }

// Public methods (Notifications of State)
public:
    void reset(void);
    void hookLevelLoop(void);
    void hookWorldLoop(void);
    void hookInputUpdate(void);
    void hookLevelRenderStart(void);
    void hookLevelRenderFirstCameraStart(void);
    void hookLevelRenderEnd(void);
    void hookWorldRenderStart(void);
    void hookWorldRenderEnd(void);
    void loadTimeDeferMusic(void);

// Private methods (Outgoing events)
private:
    void sendOnLoop(void);
    void sendOnInputUpdate(void);
    void sendOnTick(void);
    void sendOnTickEnd(void);
    void sendOnDraw(void);
    void sendOnDrawEnd(void);

// Public methods (pause requests)
public:
    void requestPause(bool atFrameEnd);
    void requestUnpause(void);
    bool isPaused(void);
    void checkPause(void);

// Paused game logic
private:
    void runPause(void);
public:
};

// Global instance
extern EventStateMachine g_EventHandler;

// Utility Methods
void LunaDllRenderAndWaitFrame(void);
void LunaDllWaitFrame(bool allowMaxFPS = true);

#endif
