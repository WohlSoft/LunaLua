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
    m_renderer_useLetterbox = true;
    m_renderer_forceDisableFullscreen = false;
    m_lua_enable_http = false;
}

bool GeneralLunaConfig::save()
{
    CSimpleIniW generalConfig;
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

    m_renderer_useLetterbox = configToLoad.GetBoolValue(L"Renderer", L"use_letterbox", true);
    m_renderer_forceDisableFullscreen = configToLoad.GetBoolValue(L"Renderer", L"force_disable_fullscreen", false);
    m_lua_enable_http = configToLoad.GetBoolValue(L"Lua", L"enable-http", false);

    save();
    return true;
}
