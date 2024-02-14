#pragma once
#include <string>

class GuiCrashNotify
{
private:
    bool m_doSend;
    std::string m_crashText;

public:
    GuiCrashNotify(const std::string& crashText);
    void show();
    ~GuiCrashNotify();

    bool shouldSend() const;
    const std::string& getCrashText() const;

    void setShouldSend(bool shouldSend);
};

