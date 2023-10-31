#include <unordered_set>
#include "LunaPathValidator.h"
#include "../Globals.h"
#include "../Misc/LoadScreen.h"
#include "../../../FileManager/LoadFile_Save.h"

// Instances
LunaPathValidator gLunaPathValidator;
LunaPathValidator gLunaPathValidatorLoadscreen;

// 
static std::unordered_set<std::wstring> naughtyExtensionMap(
    { L"application", L"bat", L"cmd", L"com", L"cpl", L"dll", L"exe", L"gadget", L"hta", L"inf", L"jar", L"js", L"jse", L"lnk", L"lua", L"msc", L"msi", L"msp", L"pif", L"ps1", L"ps1xml", L"ps2", L"ps2xml", L"psc1", L"psc2", L"py", L"pyw", L"reg", L"scf", L"scr", L"vb", L"vbe", L"vbs", L"ws", L"wsf", L"wsh" }
);


LunaPathValidator::LunaPathValidator() :
    mEnginePath(), mMatchingEnginePath(), mMatchingEpisodePath(), mMatchingSavesPath()
{
}

LunaPathValidator::~LunaPathValidator()
{
}

void LunaPathValidator::SetPaths()
{
    mEnginePath = NormalizedPath<std::string>(gAppPathUTF8);
    if ((mEnginePath.size() > 0) && (mEnginePath[mEnginePath.size() - 1] != '\\'))
    {
        mEnginePath += "\\";
    }
    mMatchingEnginePath = NormalizedPath<std::wstring>(gAppPathWCHAR);
    if ((mMatchingEnginePath.size() > 0) && (mMatchingEnginePath[mMatchingEnginePath.size() - 1] != L'\\'))
    {
        mMatchingEnginePath += L"\\";
    }
    std::transform(mMatchingEnginePath.begin(), mMatchingEnginePath.end(), mMatchingEnginePath.begin(), towlower);
    mMatchingEpisodePath = NormalizedPath<std::wstring>(GM_FULLDIR);
    if ((mMatchingEpisodePath.size() > 0) && (mMatchingEpisodePath[mMatchingEpisodePath.size() - 1] != L'\\'))
    {
        mMatchingEpisodePath += L"\\";
    }
    std::transform(mMatchingEpisodePath.begin(), mMatchingEpisodePath.end(), mMatchingEpisodePath.begin(), towlower);
    mMatchingSavesPath = GetSavesPathW();
    if (mMatchingSavesPath.size() > 0)
    {
        mMatchingSavesPath = NormalizedPath<std::wstring>(mMatchingSavesPath);
        if ((mMatchingSavesPath.size() > 0) && (mMatchingSavesPath[mMatchingSavesPath.size() - 1] != L'\\'))
        {
            mMatchingSavesPath += L"\\";
        }
        std::transform(mMatchingSavesPath.begin(), mMatchingSavesPath.end(), mMatchingSavesPath.begin(), towlower);
    }
}

LunaPathValidator::Result* LunaPathValidator::CheckPath(const char* path)
{
    // Normalize path and make it absolute if necessary
    if ((
        ((path[0] >= 'A') && (path[0] <= 'Z')) ||
        ((path[0] >= 'a') && (path[0] <= 'z'))
        ) &&
        (path[1] == ':') &&
        ((path[2] == '/') || (path[2] == '\\'))
        )
    {
        // It's an absolute path already
        mNormalPath = NormalizedPath<std::string>(path);
    }
    else
    {
        // Not absolute path
        mNormalPath = NormalizedPath<std::string>(mEnginePath + path);
    }

    // Get wchar_t version of path for checking what it starts with, so that we use towlower to better handle unicode case insensitivity 
    std::wstring wNormalPath = Str2WStr(mNormalPath);
    std::transform(wNormalPath.begin(), wNormalPath.end(), wNormalPath.begin(), towlower);

    if ((mMatchingEpisodePath.size() > 0) && (mMatchingEpisodePath == wNormalPath.substr(0, mMatchingEpisodePath.size())))
    {
        // If episode path matches
        mResult = { mNormalPath.c_str(), mNormalPath.length(), true };
    }
    else if ((mMatchingSavesPath.size() > 0) && (mMatchingSavesPath == wNormalPath.substr(0, mMatchingSavesPath.size())))
    {
        // If saves path matches
        mResult = { mNormalPath.c_str(), mNormalPath.length(), true };
    }
    else if ((mMatchingEnginePath.size() > 0) && (mMatchingEnginePath == wNormalPath.substr(0, mMatchingEnginePath.size())))
    {
        // If engine path matches
        bool canWrite = ((wNormalPath.substr(mMatchingEnginePath.size(), 5) == L"logs\\") ||
                         (wNormalPath.substr(mMatchingEnginePath.size(), std::wstring::npos) == L"worlds\\mario challenge\\data.json"));
        mResult = { mNormalPath.c_str(), mNormalPath.length(), canWrite };
    }
    else
    {
        return nullptr;
    }

    if (mResult.canWrite)
    {
        // Check extension
        std::wstring fileExt = L"";
        std::wstring::size_type pathIdx = wNormalPath.rfind(L'\\');
        std::wstring::size_type extIdx = wNormalPath.rfind(L'.');
        if (
            (extIdx != std::wstring::npos) &&
            ((pathIdx == std::wstring::npos) || (extIdx > pathIdx))
            )
        {
            std::wstring fileExt = wNormalPath.substr(extIdx + 1);
            mResult.canWrite = (naughtyExtensionMap.find(fileExt) == naughtyExtensionMap.cend());
        }
    }

    return &mResult;
}

LunaPathValidator& LunaPathValidator::GetForThread()
{
    if (LunaLoadScreenIsCurrentThread())
    {
        return gLunaPathValidatorLoadscreen;
    }
    return gLunaPathValidator;
}
