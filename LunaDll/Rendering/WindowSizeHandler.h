#ifndef WINDOW_SIZE_HANDLER_H_
#define WINDOW_SIZE_HANDLER_H_

#include <utility>
#include <mutex>

class WindowSizeHandler
{
public:
    template <typename T>
    struct Vec2 {
        T x;
        T y;
    };

    struct State {
        Vec2<int> windowSize;
        Vec2<double> fbOffset;
        Vec2<double> fbScale;
    };
private:
    State mState;
    std::mutex mMutex;

    void RecalculateInteral();

public:
    WindowSizeHandler(int defaultW, int defaultH) :
        mState{ {defaultW, defaultH}, {0.0, 0.0}, {1.0, 1.0} },
        mMutex()
    {
    }

    // Getters by value (only use from main thread)
    inline Vec2<int> getWindowSize() { return mState.windowSize; }
    inline Vec2<double> getFramebufferOffset() { return mState.fbOffset; }
    inline Vec2<double> getFramebufferScale() { return mState.fbScale; }

    // In place coordinate translation
    inline void WindowToFramebuffer(double& x, double& y)
    {
        x = (x - mState.fbOffset.x) / mState.fbScale.x;
        y = (y - mState.fbOffset.y) / mState.fbScale.y;
    }
    inline void FramebufferToWindow(double& x, double& y)
    {
        x = (x * mState.fbScale.x) + mState.fbOffset.x;
        y = (y * mState.fbScale.y) + mState.fbOffset.y;
    }

    // Copy whole state (thread safe)
    inline State getStateThreadSafe() { std::lock_guard<std::mutex> lock(mMutex); return mState; }

    // Functions to set window (thread safe)
    void SetWindowSize(int w, int h);
    void Recalculate();
};

// Global instance
extern WindowSizeHandler gWindowSizeHandler;

#endif
