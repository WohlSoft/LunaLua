#ifndef EventStateMachine_Hhh
#define EventStateMachine_Hhh

class EventStateMachine {
// Member variables
private:
    bool m_onTickReady;
    bool m_onTickEndReady;
    bool m_onDrawEndReady;

// Constructor and Destructor
public:
    EventStateMachine() { reset(); }
    ~EventStateMachine() { }

// Public methods (Notifications of State)
public:
    void reset(void);
    void hookLevelLoop(void);
    void hookWorldLoop(void);
    void hookInputUpdate(void);
    void hookLevelRenderStart(void);
    void hookLevelRenderEnd(void);
    void hookWorldRenderStart(void);
    void hookWorldRenderEnd(void);

// Private methods (Outgoing events)
private:
    void sendOnLoop(void);
    void sendOnInputUpdate(void);
    void sendOnTick(void);
    void sendOnTickEnd(void);
    void sendOnDraw();
    void sendOnDrawEnd();
};

// Global instance
extern EventStateMachine g_EventHandler;

#endif
