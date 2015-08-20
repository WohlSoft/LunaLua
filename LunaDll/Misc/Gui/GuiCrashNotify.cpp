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
static std::string username = std::string("");
static std::string usercomment = std::string("");


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

        HWND usercommentCtrl = GetDlgItem(hwnd, IDC_EDITTEXT_WHATDIDYOUDO);
        SendMessageA(usercommentCtrl, EM_SETLIMITTEXT, (WPARAM)500, 0);
        
        HWND usernameCtrl = GetDlgItem(hwnd, IDC_EDITTEXT_YOURNAME);
        SendMessageA(usernameCtrl, EM_SETLIMITTEXT, (WPARAM)100, 0);


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
        case IDC_BNT_NOSEND:
            defSkipMsgBox = IsDlgButtonChecked(hwnd, IDC_CHECK_NOASK) == BST_CHECKED;

            HWND usernameCtrl = GetDlgItem(hwnd, IDC_EDITTEXT_YOURNAME);
            HWND usercommentCtrl = GetDlgItem(hwnd, IDC_EDITTEXT_WHATDIDYOUDO);

            int lenOfUsername = GetWindowTextLengthA(usernameCtrl) + 1;
            int lenOfUsercomment = GetWindowTextLengthA(usercommentCtrl) + 1;

            username = std::string(lenOfUsername, ' ');
            usercomment = std::string(lenOfUsercomment, ' ');

            GetWindowTextA(usernameCtrl, &username[0], lenOfUsername);
            GetWindowTextA(usercommentCtrl, &usercomment[0], lenOfUsercomment);
            
            username.resize(lenOfUsername - 1);
            usercomment.resize(lenOfUsercomment - 1);
            username.shrink_to_fit();
            usercomment.shrink_to_fit();

            break;
        }

        switch (LOWORD(wParam))
        {
        case IDC_BNT_SEND:
            defDoSend = true;
            EndDialog(hwnd, IDC_BNT_SEND);
            break;
        case IDC_BNT_NOSEND:
            defDoSend = false;
            EndDialog(hwnd, IDC_BNT_NOSEND);
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}



GuiCrashNotify::GuiCrashNotify(const std::string& crashText) :
    doSend(false),
    crashText(crashText),
    username(""),
    usercomment("")
{}


GuiCrashNotify::~GuiCrashNotify()
{}

void GuiCrashNotify::show()
{
    replaceSubStr(crashText, "\n", "\r\n");
    theCrashText = crashText;
    if (!defSkipMsgBox){
        DialogBox(gHInstance, MAKEINTRESOURCE(IDD_CRASHDIALOG), NULL, &CrashDlgProc);
    }
    
    this->usercomment = ::usercomment;
    this->username = ::username;
    doSend = defDoSend;
}
