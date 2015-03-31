#pragma once
#include <string>

class GuiCrashNotify
{
public:
    GuiCrashNotify(const std::string& crashText);
    void show();
    ~GuiCrashNotify();

    bool doSend;
    std::string crashText;

};

