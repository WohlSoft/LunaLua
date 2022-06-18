#include "WindowSizeHandler.h"
#include "GL/GLContextManager.h"
#include "../Globals.h"

// Global instance
WindowSizeHandler gWindowSizeHandler(800, 600);

void WindowSizeHandler::SetWindowSize(int w, int h)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mState.windowSize.x = w;
    mState.windowSize.y = h;
    RecalculateInteral();
}

void WindowSizeHandler::Recalculate()
{
    std::lock_guard<std::mutex> lock(mMutex);
    RecalculateInteral();
}

void WindowSizeHandler::RecalculateInteral()
{
    int windowWidth = mState.windowSize.x;
    int windowHeight = mState.windowSize.y;
    int fbWidth = g_GLContextManager.GetMainFBWidth();
    int fbHeight = g_GLContextManager.GetMainFBHeight();

    // Compute x/y scale of window relative to framebuffer
    double xScale = windowWidth / static_cast<double>(fbWidth);
    double yScale = windowHeight / static_cast<double>(fbHeight);

    // Adjust scale and offsets for letterboxing
    double xOffset = 0.0;
    double yOffset = 0.0;
    if (gGeneralConfig.getRendererUseLetterbox()) {
        if (xScale > yScale) {
            xScale = yScale;
            xOffset = (windowWidth - xScale * fbWidth) * 0.5;
        }
        else {
            yScale = xScale;
            yOffset = (windowHeight - yScale * fbHeight) * 0.5;
        }
    }

    mState.fbOffset.x = xOffset;
    mState.fbOffset.y = yOffset;
    mState.fbScale.x = xScale;
    mState.fbScale.y = yScale;
}
