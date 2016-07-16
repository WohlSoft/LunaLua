#pragma once
#include <string>
#include <vector>

class RichTextDialog
{
private:
    std::string m_title;
    std::string m_rtfText;
    bool m_isReadOnly;
public:
    RichTextDialog(const std::string& title, const std::string& rtfText, bool isReadOnly);
    
    void show();
    
    void setRtfText(const std::string& rtfText);
    void setRtfText(std::string&& rtfText) noexcept;
    const std::string& getRtfText() const noexcept;
    const std::string& getTitle() const noexcept;
    bool isReadOnly() const noexcept;
};


