#include "RichTextDialog.h"
#include "../../resource.h"
#include "../../Globals.h"
#include "GuiUtils.h"

#include <tchar.h>
#include <Richedit.h>
#include <iostream>

constexpr const _TCHAR* RichEditClsName = TEXT("Riched20.dll");

RichTextDialog::RichTextDialog(const std::string& title, const std::string& rtfText) : 
    m_title(title),
    m_rtfText(rtfText)
{
    if(!GetModuleHandle(RichEditClsName))
        LoadLibrary(RichEditClsName);
}

void RichTextDialog::show()
{
    DialogBoxParam(gHInstance, MAKEINTRESOURCE(IDD_RICHTEXTDIALOG), NULL, 
        [](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) -> BOOL {
        switch (message)
        {
        case WM_INITDIALOG:
        {
            setWindowToCenter(hwnd);

            RichTextDialog* dialogCls = reinterpret_cast<RichTextDialog*>(lParam);
            std::string txt = dialogCls->getRtfText();

            SetWindowTextA(hwnd, dialogCls->getTitle().c_str());

            HWND richEditControl = GetDlgItem(hwnd, IDC_RICHTEXTCONTENT);
            
            SETTEXTEX setTextInfo;
            setTextInfo.flags = ST_DEFAULT;
            setTextInfo.codepage = CP_ACP;
            SendMessageA(richEditControl, EM_SETTEXTEX, reinterpret_cast<WPARAM>(&setTextInfo), reinterpret_cast<LPARAM>(txt.c_str()));

            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDOK:
                EndDialog(hwnd, IDOK);
                break;
            }
            break;
        default:
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(this));
}

std::string RichTextDialog::getRtfText() const
{
    return m_rtfText;
}

std::string RichTextDialog::getTitle() const
{
    return m_title;
}
