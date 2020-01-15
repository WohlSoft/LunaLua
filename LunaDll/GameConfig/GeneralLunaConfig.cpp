#include "GeneralLunaConfig.h"

#include "../libs/simpleini/SimpleIni.h"

GeneralLunaConfig::GeneralLunaConfig()
{
    doDefaults();
}

GeneralLunaConfig::~GeneralLunaConfig()
{
}

void GeneralLunaConfig::doDefaults()
{
    m_renderer_opengl = GLModeAuto;
    m_renderer_useLetterbox = true;
    m_renderer_forceDisableFullscreen = false;
    m_lua_enable_http = false;
}

bool GeneralLunaConfig::save()
{
    CSimpleIniW generalConfig;

    const wchar_t* openglValueStr;
    switch (m_renderer_opengl)
    {
    case GLModeHard: openglValueStr = L"hard"; break;
    case GLModeSoft: openglValueStr = L"soft"; break;
    default:         openglValueStr = L"auto"; break;
    }
    generalConfig.SetValue(L"Renderer", L"opengl", openglValueStr, L"# Choose 'soft' to force software gl only, or 'hard' to force hardware gl only, or 'auto' for automatic selection.", true);

    generalConfig.SetBoolValue(L"Renderer", L"use_letterbox", m_renderer_useLetterbox, L"# Choose true if you want to use forced 4:3. Otherwise it will use the default scretch mode.", true);
    
    generalConfig.SetBoolValue(L"Renderer", L"force_disable_fullscreen", m_renderer_forceDisableFullscreen, L"# Choose true to disable the engine's fullscreen feature, in case it's acting up.", true);

    generalConfig.SetBoolValue(L"Lua", L"enable-http", m_lua_enable_http, L"# Set to true if you want to have the HTTP API enabled. This might be a security risk, only activate if you trust the episode/api.", true);

    return generalConfig.SaveFile(m_settingFilename.c_str(), true) == SI_OK;
}

void GeneralLunaConfig::loadOrDefault()
{
    if (!load())
        doDefaults();
}

bool GeneralLunaConfig::load()
{
    CSimpleIniW configToLoad;
    if (configToLoad.LoadFile(m_settingFilename.c_str()) != SI_OK)
        return false;

    const wchar_t* openglValueStr = configToLoad.GetValue(L"Renderer", L"opengl", L"auto");
    if (_wcsicmp(openglValueStr, L"hard") == 0)
    {
        m_renderer_opengl = GLModeHard;
    }
    else if (_wcsicmp(openglValueStr, L"soft") == 0)
    {
        m_renderer_opengl = GLModeSoft;
    }
    else
    {
        m_renderer_opengl = GLModeAuto;
    }
    m_renderer_useLetterbox = configToLoad.GetBoolValue(L"Renderer", L"use_letterbox", true);
    m_renderer_forceDisableFullscreen = configToLoad.GetBoolValue(L"Renderer", L"force_disable_fullscreen", false);
    m_lua_enable_http = configToLoad.GetBoolValue(L"Lua", L"enable-http", false);

    save();
    return true;
}
