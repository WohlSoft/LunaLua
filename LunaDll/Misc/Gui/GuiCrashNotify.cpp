#include <windows.h>

#include "GuiCrashNotify.h"
#include "GuiUtils.h"

#include "../../resource.h"
#include "../../Globals.h"
#include "../../GlobalFuncs.h"

#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")



GuiCrashNotify::GuiCrashNotify(const std::string& crashText) :
    m_doSend(false),
    m_crashText(crashText)
{}


GuiCrashNotify::~GuiCrashNotify()
{}



void GuiCrashNotify::show()
{
    static bool defaultSkipMessageBox = false;
    static bool defaultDoSend = false;

    replaceSubStr(m_crashText, "\n", "\r\n");
    if (!defaultSkipMessageBox){
        DialogBoxParam(gHInstance, MAKEINTRESOURCE(IDD_CRASHDIALOG), NULL,
            [](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) -> BOOL
        {
            switch (message)
            {
            case WM_INITDIALOG:
            {
                GuiCrashNotify* crashNotify = bindParentClassParam<GuiCrashNotify>(hwnd, lParam);

                setWindowToCenter(hwnd);

                // 2. Update the edit control with the actual crash message:
                HWND editControl = GetDlgItem(hwnd, IDC_EDITTEXT_CRASHMSG);
                HFONT font = CreateFontA(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CLEARTYPE_QUALITY, 0, "Courier New");
                SendMessageA(editControl, WM_SETFONT, (WPARAM)font, TRUE);
                SendMessageA(editControl, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(crashNotify->getCrashText().c_str()));

                return TRUE;
            }

            case WM_COMMAND:
                switch (LOWORD(wParam))
                {
                case IDC_BNT_NOSEND:
                    getParentClass<GuiCrashNotify>(hwnd)->setShouldSend(false);
                    EndDialog(hwnd, IDC_BNT_NOSEND);
                    break;
                }
                break;
            default:
                return FALSE;
            }
            return TRUE;
        }, reinterpret_cast<LPARAM>(this));
    } else {
        m_doSend = defaultDoSend;
    }
    
}

bool GuiCrashNotify::shouldSend() const
{
    return m_doSend;
}

const std::string& GuiCrashNotify::getCrashText() const
{
    return m_crashText;
}

void GuiCrashNotify::setShouldSend(bool shouldSend)
{
    m_doSend = shouldSend;
}
