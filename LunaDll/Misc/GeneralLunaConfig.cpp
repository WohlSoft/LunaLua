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
}

bool GeneralLunaConfig::save()
{
    CSimpleIniW generalConfig;
    generalConfig.SetBoolValue(L"Renderer", L"use_letterbox", m_renderer_useLetterbox, L"Choose true if you want to use forced 4:3. Otherwise it will use the default scretch mode.", true);
    
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

    return true;
}
