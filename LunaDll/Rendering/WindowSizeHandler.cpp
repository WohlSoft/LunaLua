#include "WindowSizeHandler.h"
#include "GL/GLContextManager.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../Input/MouseHandler.h"

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

    // Recalculate framebutton position to match
    gMouseHandler.Recalculate();
}

// This function tries to set DPI awareness to the maximum level regardless of the OS :-)
// Returns the level that could be set given the OS
static int SetDpiAwareness_impl()
{
    DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);
    enum PROCESS_DPI_AWARENESS {
        PROCESS_DPI_UNAWARE = 0,
        PROCESS_SYSTEM_DPI_AWARE = 1,
        PROCESS_PER_MONITOR_DPI_AWARE = 2
    };
    enum DPI_AWARENESS {
        DPI_AWARENESS_INVALID = -1,
        DPI_AWARENESS_UNAWARE = 0,
        DPI_AWARENESS_SYSTEM_AWARE = 1,
        DPI_AWARENESS_PER_MONITOR_AWARE = 2
    };

    // Requires Vista or newer
    if (!Luna_IsWindowsVistaOrNewer())
    {
        return PROCESS_DPI_UNAWARE;
    }

    // Approach for Windows 10 version 1703 and newer
    static auto setProcessDpiAwarenessContext = Luna_GetProc<BOOL(__stdcall *)(DPI_AWARENESS_CONTEXT)>("User32.dll", "SetProcessDpiAwarenessContext");
    if (setProcessDpiAwarenessContext)
    {
        // Try to set per-monitor awareness (v2)
        if (setProcessDpiAwarenessContext((DPI_AWARENESS_CONTEXT)-4) == TRUE)
        {
            return PROCESS_PER_MONITOR_DPI_AWARE;
        }

        // Try to set per-monitor awareness (v1)
        if (setProcessDpiAwarenessContext((DPI_AWARENESS_CONTEXT)-3) == TRUE)
        {
            return PROCESS_PER_MONITOR_DPI_AWARE;
        }

        // Try to set system awareness
        if (setProcessDpiAwarenessContext((DPI_AWARENESS_CONTEXT)-2) == TRUE)
        {
            return PROCESS_SYSTEM_DPI_AWARE;
        }

        // Failing that, check the awareness level that is set
        static auto getThreadDpiAwarenessContext = Luna_GetProc<DPI_AWARENESS_CONTEXT(__stdcall *)(void)>("User32.dll", "GetThreadDpiAwarenessContext");
        static auto getAwarenessFromDpiAwarenessContext = Luna_GetProc<DPI_AWARENESS(__stdcall *)(DPI_AWARENESS_CONTEXT)>("User32.dll", "GetAwarenessFromDpiAwarenessContext");
        if (getThreadDpiAwarenessContext && getAwarenessFromDpiAwarenessContext)
        {
            // Note that the values for DPI_AWARENESS line up with PROCESS_DPI_AWARENESS
            return getAwarenessFromDpiAwarenessContext(getThreadDpiAwarenessContext());
        }
    }

    // Approach for Windows 8.1 and newer
    static auto setProcessDpiAwareness = Luna_GetProc<HRESULT(__stdcall *)(PROCESS_DPI_AWARENESS)>("Shcore.dll", "SetProcessDpiAwareness");
    if (setProcessDpiAwareness)
    {
        // Try to set per-monitor awareness
        if (setProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE) == S_OK)
        {
            return PROCESS_PER_MONITOR_DPI_AWARE;
        }

        // Try to set system awareness otherwise
        if (setProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE) == S_OK)
        {
            return PROCESS_SYSTEM_DPI_AWARE;
        }

        // Failing that, check the awareness level that is set
        static auto getProcessDpiAwareness = Luna_GetProc<HRESULT(__stdcall *)(HANDLE, PROCESS_DPI_AWARENESS*)>("Shcore.dll", "GetProcessDpiAwareness");
        PROCESS_DPI_AWARENESS ret;
        if (getProcessDpiAwareness && (getProcessDpiAwareness(NULL, &ret) == S_OK))
        {
            return ret;
        }

        return PROCESS_DPI_UNAWARE;
    }

    // Approach for Vista and newer
    static auto setProcessDPIAware = Luna_GetProc<BOOL(__stdcall *)(void)>("User32.dll", "SetProcessDPIAware");
    if (setProcessDPIAware)
    {
        // Try to set DPI Awareness
        if (setProcessDPIAware() != 0)
        {
            return PROCESS_SYSTEM_DPI_AWARE;
        }

        // Failing that, check the awareness level that is set
        static auto isProcessDPIAware = Luna_GetProc<BOOL(__stdcall *)(void)>("User32.dll", "IsProcessDPIAware");
        if (isProcessDPIAware && (isProcessDPIAware() == TRUE))
        {
            return PROCESS_SYSTEM_DPI_AWARE;
        }

        return PROCESS_DPI_UNAWARE;
    }

    return PROCESS_DPI_UNAWARE;
}

// Get the "DPI" from Windows
// NOTE: This isn't a true DPI, but rather represents Windows' configured
//       scaling factor, with a value of 96 representing "100%"
static int GetDpi_impl()
{
    // Method for Windows 10, version 1607
    static auto getDpiForWindow = Luna_GetProc<UINT(__stdcall *)(HWND)>("User32.dll", "GetDpiForWindow");
    if (getDpiForWindow)
    {
        return getDpiForWindow(gMainWindowHwnd);
    }

    // Method for Windows 8.1
    enum MONITOR_DPI_TYPE {
        MDT_EFFECTIVE_DPI = 0,
        MDT_ANGULAR_DPI = 1,
        MDT_RAW_DPI = 2,
        MDT_DEFAULT
    };
    static auto getDpiForMonitor = Luna_GetProc<HRESULT(__stdcall *)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*)>("Shcore.dll", "GetDpiForMonitor");
    if (getDpiForMonitor)
    {
        UINT dpiX, dpiY;
        if (getDpiForMonitor(MonitorFromWindow(gMainWindowHwnd, MONITOR_DEFAULTTONEAREST), MDT_EFFECTIVE_DPI, &dpiX, &dpiY) == S_OK)
        {
            return (dpiX + dpiY) / 2;
        }
    }

    // Method for pre-8.1 Windows
    HDC hDC = ::GetDC(gMainWindowHwnd);
    if (hDC)
    {
        int dpiX = ::GetDeviceCaps(hDC, LOGPIXELSX);
        int dpiY = ::GetDeviceCaps(hDC, LOGPIXELSY);
        ::ReleaseDC(NULL, hDC);
        return (dpiX + dpiY) / 2;
    }

    return 96; // Default number
}

void WindowSizeHandler::InitDPIAwareness()
{
    mDpiAwarenessState = SetDpiAwareness_impl();
}

int WindowSizeHandler::SetInitialWindowSize()
{
    // Only do this when we can tell the OS that we have DPI awareness
    if (mDpiAwarenessState <= 0) return 96;

    // Get initial client size we'll be modifying
    // NOTE: GL won't actually be set up now normally... but we can still get the default FB size just fine.
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = g_GLContextManager.GetMainFBWidth();
    rc.bottom = g_GLContextManager.GetMainFBHeight();

    // Get DPI
    int dpi = GetDpi_impl();

    // Get scale factor for DPI
    double dpiScale = static_cast<double>(dpi) / 96.0;

    // Compute scaled size
    rc.right = static_cast<LONG>(::round(dpiScale * rc.right));
    rc.bottom = static_cast<LONG>(::round(dpiScale * rc.bottom));

    // Add window frame
    AdjustWindowRectEx(&rc, GetWindowLong(gMainWindowHwnd, GWL_STYLE),
        GetMenu(gMainWindowHwnd) != 0, GetWindowLong(gMainWindowHwnd, GWL_EXSTYLE));

    // Get new height/width
    int newW = rc.right - rc.left;
    int newH = rc.bottom - rc.top;

    // Center by default (?)
    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(MonitorFromWindow(gMainWindowHwnd, MONITOR_DEFAULTTONEAREST), &monitorInfo);
    RECT& workRect = monitorInfo.rcWork;
    int newX = (workRect.left + workRect.right - newW) / 2;
    int newY = (workRect.top + workRect.bottom - newH) / 2;
    if (newX < workRect.left) newX = workRect.left;
    if (newY < workRect.top) newY = workRect.top;

    // Adjust the window size
    SetWindowPos(gMainWindowHwnd, nullptr, newX, newY, newW, newH, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOREDRAW);

    return dpi;
}
