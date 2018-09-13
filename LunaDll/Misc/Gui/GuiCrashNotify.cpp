#include <windows.h>

#include "GuiCrashNotify.h"
#include "GuiUtils.h"

#include "../../resource.h"
#include "../../Globals.h"
#include "../../GlobalFuncs.h"
#include "../../Utils/StringUtils.h"

#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")



GuiCrashNotify::GuiCrashNotify(const std::string& crashText) :
    m_doSend(false),
    m_crashText(crashText),
    m_username(""),
    m_usercomment("")
{}


GuiCrashNotify::~GuiCrashNotify()
{}



void GuiCrashNotify::show()
{
    static bool defaultSkipMessageBox = false;
    static bool defaultDoSend = false;

    LunaLua::StringUtils::replace(m_crashText, "\n", "\r\n");
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

                HWND usercommentCtrl = GetDlgItem(hwnd, IDC_EDITTEXT_WHATDIDYOUDO);
                SendMessageA(usercommentCtrl, EM_SETLIMITTEXT, (WPARAM)500, 0);

                HWND usernameCtrl = GetDlgItem(hwnd, IDC_EDITTEXT_YOURNAME);
                SendMessageA(usernameCtrl, EM_SETLIMITTEXT, (WPARAM)100, 0);


                return TRUE;
            }

            case WM_COMMAND:
                switch (LOWORD(wParam))
                {
                case IDC_BNT_SEND:
                case IDC_BNT_NOSEND:
                    GuiCrashNotify* crashNotify = getParentClass<GuiCrashNotify>(hwnd);

                    defaultSkipMessageBox = IsDlgButtonChecked(hwnd, IDC_CHECK_NOASK) == BST_CHECKED;
                    
                    HWND usernameCtrl = GetDlgItem(hwnd, IDC_EDITTEXT_YOURNAME);
                    HWND usercommentCtrl = GetDlgItem(hwnd, IDC_EDITTEXT_WHATDIDYOUDO);

                    int lenOfUsername = GetWindowTextLengthA(usernameCtrl) + 1;
                    int lenOfUsercomment = GetWindowTextLengthA(usercommentCtrl) + 1;

                    std::string usernameBuf(lenOfUsername, ' ');
                    std::string usercommentBuf(lenOfUsercomment, ' ');

                    GetWindowTextA(usernameCtrl, &usernameBuf[0], lenOfUsername);
                    GetWindowTextA(usercommentCtrl, &usercommentBuf[0], lenOfUsercomment);

                    usernameBuf.resize(lenOfUsername - 1);
                    usercommentBuf.resize(lenOfUsercomment - 1);
                    usernameBuf.shrink_to_fit();
                    usercommentBuf.shrink_to_fit();

                    crashNotify->setUsername(std::move(usernameBuf));
                    crashNotify->setUsercomment(std::move(usercommentBuf));

                    break;
                }

                switch (LOWORD(wParam))
                {
                case IDC_BNT_SEND:
                    getParentClass<GuiCrashNotify>(hwnd)->setShouldSend(true);
                    EndDialog(hwnd, IDC_BNT_SEND);
                    break;
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

const std::string& GuiCrashNotify::getUsername() const
{
    return m_username;
}

const std::string& GuiCrashNotify::getUsercomment() const
{
    return m_usercomment;
}

void GuiCrashNotify::setShouldSend(bool shouldSend)
{
    m_doSend = shouldSend;
}

void GuiCrashNotify::setUsername(std::string&& username) noexcept
{
    m_username = std::move(username);
}

void GuiCrashNotify::setUsercomment(std::string&& usercomment) noexcept
{
    m_usercomment = std::move(usercomment);
}
