#pragma once

#include <windows.h>

inline void setWindowToCenter(HWND hwnd) {
    // 1. Center the window
    RECT windowSize;
    GetWindowRect(hwnd, &windowSize);
    int nWidth = windowSize.right - windowSize.left;
    int nHeight = windowSize.bottom - windowSize.top;

    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    SetWindowPos(hwnd, NULL, nScreenWidth / 2 - nWidth / 2, nScreenHeight / 2 - nHeight / 2, 0, 0, SWP_NOSIZE);
}
