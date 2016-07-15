#pragma once
#include <string>

class RichTextDialog
{
private:
    std::string m_title;
    std::string m_rtfText;
public:
    RichTextDialog(const std::string& title, const std::string& rtfText);
    void show();
    
    std::string getRtfText() const;
    std::string getTitle() const;
};


