#include "GuiCrashNotify.h"

#include "../../resource.h"
#include "../../Globals.h"
#include "../../GlobalFuncs.h"

#include <windows.h>


#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")

static bool defSkipMsgBox = false;
static bool defDoSend = false;
static std::string theCrashText = std::string("");


BOOL CALLBACK CrashDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message)
    {
    case WM_INITDIALOG:
    {
        // 1. Center the window
        RECT windowSize;
        GetWindowRect(hwnd, &windowSize);
        int nWidth = windowSize.right - windowSize.left;
        int nHeight = windowSize.bottom - windowSize.top;

        int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
        int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);


        SetWindowPos(hwnd, NULL, nScreenWidth / 2 - nWidth / 2, nScreenHeight / 2 - nHeight / 2, 0, 0, SWP_NOSIZE);


        // 2. Update the edit contol with the actual crash message:
        HWND editControl = GetDlgItem(hwnd, IDC_EDITTEXT_CRASHMSG);
        SendMessageA(editControl, WM_SETTEXT, 0, (LPARAM)theCrashText.c_str());

        

        return TRUE;
    }
    
    //Kinda not working as I wanted D:
    /*case WM_CTLCOLORSTATIC:
    {
        HDC hdc;
        HWND hwndCtl;
        hwndCtl = (HWND)lParam;
        hdc = (HDC)wParam;

        if (GetWindowLong(hwnd, GWL_ID) == IDC_STATICTEXT_SENDTOSERVER)
        {
            SetBkMode(hdc, TRANSPARENT);
            HFONT hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            
            LOGFONT modFont;
            GetObject(hDefFont, sizeof(LOGFONT), &modFont);
            modFont.lfWeight = FW_BOLD;

            HFONT newFont = CreateFontIndirect(&modFont);

            //SelectObject(hdc, hBoldFont);     // but you want this...
            SelectObject(hdc, newFont);
            return (INT_PTR)GetSysColorBrush(COLOR_BTNFACE);
            //return 0L; // if visual themes are enabled (common controls 6) then 0 is better.
        }
        // default processing
        return 0;
    }*/
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BNT_SEND:
            defDoSend = true;
            defSkipMsgBox = IsDlgButtonChecked(hwnd, IDC_CHECK_NOASK) == BST_CHECKED;
            EndDialog(hwnd, IDC_BNT_SEND);
            break;
        case IDC_BNT_NOSEND:
            defDoSend = false;
            defSkipMsgBox = IsDlgButtonChecked(hwnd, IDC_CHECK_NOASK) == BST_CHECKED;
            EndDialog(hwnd, IDC_BNT_NOSEND);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}



GuiCrashNotify::GuiCrashNotify(const std::string& crashText)
{
    this->crashText = crashText;
}


GuiCrashNotify::~GuiCrashNotify()
{}

void GuiCrashNotify::show()
{
    replaceSubStr(crashText, "\n", "\r\n");
    theCrashText = crashText;
    if (!defSkipMsgBox){
        DialogBox(gHInstance, MAKEINTRESOURCE(IDD_CRASHDIALOG), NULL, &CrashDlgProc);
    }
    doSend = defDoSend;
}
