#pragma once

#include <string>

namespace LunaLog
{
    void StartLog(const std::wstring& appPath);
    void Log(const char* func, int line, const std::wstring& msg);
    void Log(const char* func, int line, const std::string& msg);
};

#define LUNALOG(msg) LunaLog::Log(__func__, __LINE__, (msg))
