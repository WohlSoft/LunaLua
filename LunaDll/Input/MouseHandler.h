#ifndef MOUSE_HANDLER_H_
#define MOUSE_HANDLER_H_

#include <cmath>
#include <windows.h>

class MouseHandler
{
public:
    // CONSTANTS
    enum ButtonEnum
    {
        BUTTON_L = 0,
        BUTTON_R = 1,
        BUTTON_M = 2
    };

    enum ButtonEvtEnum
    {
        EVT_UP   = 0,
        EVT_DOWN = 1,
        EVT_DBL  = 2
    };

    enum WheelEnum
    {
        WHEEL_V = 0,
        WHEEL_H
    };

private:
    // Mouse position in window client area
    int mClientX, mClientY;
    bool mInClientArea;

    // Mouse position in framebuffer coordinates
    double mFramebufferX, mFramebufferY;

    // Button state, stored in terms of the MK_* constants from windows.h
    unsigned char mButtonState;

public:
    // Constructor
    MouseHandler() :
        mClientX(0), mClientY(0),
        mInClientArea(false),
        mFramebufferX(NAN), mFramebufferY(NAN),
        mButtonState(0)
    {
    }

    // Event handlers
    void OnMouseMove(int x, int y, unsigned char buttonState);
    void OnMouseLeave();
    void OnMouseButtonEvent(ButtonEnum button, ButtonEvtEnum state);
    void OnMouseWheelEvent(WheelEnum wheel, int delta);

    // Recalculate FB coordinates
    void Recalculate();

    // Getters
    inline double GetX() { return mFramebufferX; }
    inline double GetY() { return mFramebufferY; }
    inline bool GetButtonState(ButtonEnum button) {
        unsigned char buttonMask = 0;
        switch (button)
        {
        case BUTTON_L:
            buttonMask = MK_LBUTTON;
            break;
        case BUTTON_R:
            buttonMask = MK_RBUTTON;
            break;
        case BUTTON_M:
            buttonMask = MK_MBUTTON;
            break;
        }
        return (mButtonState & buttonMask) != 0;
    }
};

// Global instance
extern MouseHandler gMouseHandler;

#endif
