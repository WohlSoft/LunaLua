#include "MouseHandler.h"
#include "../Rendering/WindowSizeHandler.h"
#include "../Globals.h"
#include <windows.h>

// Global instance
MouseHandler gMouseHandler;

void MouseHandler::OnMouseMove(int x, int y, uint8_t buttonState)
{
    if (mInClientArea && (mClientX == x) && (mClientY == y) && !gMainWindowInBackground)
    {
        // No change.
        return;
    }

    if ((!mInClientArea) && (gMainWindowHwnd != nullptr))
    {
        // Make sure we get WM_MOUSELEAVE
        TRACKMOUSEEVENT params;
        params.cbSize = sizeof(params);
        params.dwFlags = TME_LEAVE;
        params.hwndTrack = gMainWindowHwnd;
        params.dwHoverTime = 0;
        TrackMouseEvent(&params);
    }

    mClientX = x;
    mClientY = y;
    mButtonState = buttonState;
    mInClientArea = true;
    Recalculate();
}

void MouseHandler::OnMouseLeave()
{
    if (mInClientArea)
    {
        mInClientArea = false;
        Recalculate();
    }
}

void MouseHandler::OnMouseButtonEvent(ButtonEnum button, ButtonEvtEnum state)
{
    if (gLunaLua.isValid() && !gMainWindowInBackground) {
        std::shared_ptr<Event> event = std::make_shared<Event>("onMouseButtonEvent", false);
        event->setDirectEventName("onMouseButtonEvent");
        event->setLoopable(false);
        gLunaLua.callEvent(event, button, state, mFramebufferX, mFramebufferY);
    }
}

void MouseHandler::OnMouseWheelEvent(WheelEnum wheel, int delta)
{
    if (gLunaLua.isValid() && !gMainWindowInBackground) {
        std::shared_ptr<Event> event = std::make_shared<Event>("onMouseWheelEvent", false);
        event->setDirectEventName("onMouseWheelEvent");
        event->setLoopable(false);
        gLunaLua.callEvent(event, wheel, delta, mFramebufferX, mFramebufferY);
    }
}

// Recalculate FB coordinates
void MouseHandler::Recalculate()
{
    if (mInClientArea)
    {
        double newX = mClientX;
        double newY = mClientY;
        gWindowSizeHandler.WindowToFramebuffer(newX, newY);
        mFramebufferX = newX;
        mFramebufferY = newY;
    }
    else
    {
        mFramebufferX = NAN;
        mFramebufferY = NAN;
    }
}
