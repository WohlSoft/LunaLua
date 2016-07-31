#pragma once
#include <string>

class GuiCrashNotify
{
private:
    bool m_doSend;
    std::string m_crashText;
    std::string m_username;
    std::string m_usercomment;

public:
    GuiCrashNotify(const std::string& crashText);
    void show();
    ~GuiCrashNotify();

    bool shouldSend() const;
    const std::string& getCrashText() const;
    const std::string& getUsername() const;
    const std::string& getUsercomment() const;

    void setShouldSend(bool shouldSend);
    void setUsername(std::string&& username) noexcept;
    void setUsercomment(std::string&& usercomment) noexcept;
};

