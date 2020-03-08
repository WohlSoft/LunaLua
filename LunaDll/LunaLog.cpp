#include <cstdio>
#include <string>
#include <cwctype>
#include <algorithm>
#include <windows.h>

#include "GlobalFuncs.h"
#include "LunaLog.h"

static inline std::wstring normalizeFullPath(const std::wstring& in)
{
    if (iswalpha(in[0]) == 0) return L"";
    if (in[1] != L':') return L"";
    if ((in[2] != L'\\') && (in[2] != L'/')) return L"";

    std::wstring out(L"\\\\?\\");
    if (in.rfind(L"\\\\?\\", 0) == 0)
    {
        out.clear();
    }
    out += in;

    std::replace(out.begin(), out.end(), L'\\', L'/');

    return out;
}

static FILE* g_logFile = nullptr;

void LunaLog::StartLog(const std::wstring& appPath)
{
    std::wstring path(appPath);
    path += L"\\logs";
    path = normalizeFullPath(path);

    CreateDirectoryW(path.c_str(), NULL);

    path += L"\\LunaLog.txt";

    _wfopen_s(&g_logFile, path.c_str(), L"wb");
    if (g_logFile != nullptr)
    {
        fwrite("\xEF\xBB\xBF", 3, 1, g_logFile);
        fflush(g_logFile);
    }
}

void LunaLog::Log(const char* func, int line, const std::wstring& msg)
{
    if (g_logFile == nullptr) return;
    Log(func, line, WStr2Str(msg));
}

void LunaLog::Log(const char* func, int line, const std::string& msg)
{
    if (g_logFile == nullptr) return;
    std::string out;
    out += func;
    out += ":";
    out += std::to_string(line);
    out += " ";
    while (out.size() < 40)
    {
        out += " ";
    }
    out += msg;
    out += "\r\n";
    fwrite(out.c_str(), out.length(), 1, g_logFile);
    fflush(g_logFile);
}
