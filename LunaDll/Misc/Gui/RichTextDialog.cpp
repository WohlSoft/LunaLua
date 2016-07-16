#include "RichTextDialog.h"
#include "../../resource.h"
#include "../../Globals.h"
#include "GuiUtils.h"

#include <tchar.h>
#include <Richedit.h>
#include <iostream>

constexpr const _TCHAR* RichEditClsName = TEXT("Riched20.dll");
inline void ensureRichEditLibraryLoaded() {
    if (!GetModuleHandle(RichEditClsName))
        LoadLibrary(RichEditClsName);
}

RichTextDialog::RichTextDialog(const std::string& title, const std::string& rtfText, bool isReadOnly) : 
    m_title(title),
    m_rtfText(rtfText),
    m_isReadOnly(isReadOnly)
{
    ensureRichEditLibraryLoaded();
}


void RichTextDialog::show()
{
    DialogBoxParam(gHInstance, MAKEINTRESOURCE(IDD_RICHTEXTDIALOG), NULL, 
        [](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) -> BOOL {
        switch (message)
        {
        case WM_INITDIALOG:
        {
            // Set window to center   
            setWindowToCenter(hwnd);

            // Get dialog data
            RichTextDialog* dialogCls = reinterpret_cast<RichTextDialog*>(lParam);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(dialogCls));
            const std::string& txt = dialogCls->getRtfText();

            SetWindowTextA(hwnd, dialogCls->getTitle().c_str());

            HWND richEditControl = GetDlgItem(hwnd, IDC_RICHTEXTCONTENT);
            
            SETTEXTEX setTextInfo;
            setTextInfo.flags = ST_DEFAULT;
            setTextInfo.codepage = CP_ACP;
            SendMessageA(richEditControl, EM_SETTEXTEX, reinterpret_cast<WPARAM>(&setTextInfo), reinterpret_cast<LPARAM>(txt.c_str()));
            if(dialogCls->isReadOnly())
                SendMessageA(richEditControl, EM_SETREADONLY, TRUE, NULL);

            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDOK:
                RichTextDialog* dialogCls = reinterpret_cast<RichTextDialog*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
                HWND richEditControl = GetDlgItem(hwnd, IDC_RICHTEXTCONTENT);

                GETTEXTLENGTHEX textLengthProps;
                textLengthProps.flags = GTL_DEFAULT;
                textLengthProps.codepage = CP_ACP;
                LRESULT numOfChars = SendMessageA(richEditControl, EM_GETTEXTLENGTHEX, reinterpret_cast<WPARAM>(&textLengthProps), NULL);

                std::string buf(static_cast<size_t>(numOfChars), ' ');
                GETTEXTEX textProps;
                textProps.cb = numOfChars;
                textProps.codepage = CP_ACP;
                textProps.flags = GT_DEFAULT;
                textProps.lpDefaultChar = nullptr;
                textProps.lpUsedDefChar = nullptr;
                SendMessageA(richEditControl, EM_GETTEXTEX, reinterpret_cast<WPARAM>(&textProps), reinterpret_cast<LPARAM>(&buf[0]));

                dialogCls->setRtfText(std::move(buf));

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

void RichTextDialog::setRtfText(const std::string& rtfText)
{
    m_rtfText = rtfText;
}

void RichTextDialog::setRtfText(std::string&& rtfText) noexcept
{
    m_rtfText = std::move(rtfText);
}

const std::string& RichTextDialog::getRtfText() const noexcept
{
    return m_rtfText;
}

const std::string& RichTextDialog::getTitle() const noexcept
{
    return m_title;
}

bool RichTextDialog::isReadOnly() const noexcept
{
    return m_isReadOnly;
}
