#include <string>
#include "Globals.h"
#include "EventStateMachine.h"

// Global instance
EventStateMachine g_EventHandler;

// Helper function
template <typename... Ts>
static inline void sendSimpleLuaEvent(const std::string& eventName, Ts&&... args) {
    if (gLunaLua.isValid()) {
        Event inputEvent(eventName, false);
        inputEvent.setDirectEventName(eventName);
        inputEvent.setLoopable(false);
        gLunaLua.callEvent(&inputEvent, std::forward<Ts>(args)...);
    }
}

// Public methods (Notifications of State)
void EventStateMachine::reset(void) {
    m_onTickReady = false;
    m_onTickEndReady = false;
    m_onDrawEndReady = false;
}

void EventStateMachine::hookLevelLoop(void) {
    sendOnLoop();
}

void EventStateMachine::hookWorldLoop(void) {
    sendOnLoop();
}

void EventStateMachine::hookInputUpdate(void) {
    sendOnInputUpdate();

    if (m_onTickReady) {
        sendOnTick();
    }
}

void EventStateMachine::hookLevelRenderStart(void) {
    if (m_onTickEndReady) {
        sendOnTickEnd();
    }

    sendOnDraw();
}
void EventStateMachine::hookLevelRenderEnd(void) {
    if (m_onDrawEndReady) {
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
        sendOnDrawEnd();
    }
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
    sendSimpleLuaEvent("onDraw");

    m_onDrawEndReady = true;
}

void EventStateMachine::sendOnDrawEnd(void) {
    m_onDrawEndReady = false;

    sendSimpleLuaEvent("onDrawEnd");
}