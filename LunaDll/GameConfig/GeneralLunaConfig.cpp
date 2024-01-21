#include "GeneralLunaConfig.h"
#include "../GlobalFuncs.h"
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
    m_renderer_vsync = VSyncModeOff;
    m_renderer_useLetterbox = true;
    m_audio_samplerate = 44100;
    m_audio_bufferlen = 2048;
    m_audio_driver = "default";
    m_controller_battery_reporting = true;
    m_controller_rumble_enabled = true;
    m_lua_enable_http = false;
    m_engine_cpu_lock_affinity = false;
}

bool GeneralLunaConfig::save()
{
    CSimpleIniW generalConfig;

    const wchar_t* openglValueStr;
    switch (m_renderer_opengl)
    {
        default:
        case GLModeAuto: openglValueStr = L"auto"; break;
        case GLModeHard: openglValueStr = L"hard"; break;
        case GLModeSoft: openglValueStr = L"soft"; break;
    }
    generalConfig.SetValue(L"Renderer", L"opengl", openglValueStr, L"# Choose 'soft' to force software gl only, or 'hard' to force hardware gl only, or 'auto' for automatic selection.", true);

    const wchar_t* vsyncValueStr;
    switch (m_renderer_vsync)
    {
        default:
        case VSyncModeOff:      vsyncValueStr = L"off"; break;
        case VSyncModeOn:       vsyncValueStr = L"on"; break;
        case VSyncModeAdaptive: vsyncValueStr = L"adaptive"; break;
    }
    generalConfig.SetValue(L"Renderer", L"vsync", vsyncValueStr, L"# Valid options are 'off', 'on', or 'adaptive'. Being enabled eliminates tearing, but worsens latency.", true);

    generalConfig.SetBoolValue(L"Renderer", L"use_letterbox", m_renderer_useLetterbox, L"# Choose true if you want to use forced 4:3. Otherwise it will use the default scretch mode.", true);
    
    generalConfig.SetLongValue(L"Audio", L"sample_rate", m_audio_samplerate, L"# Native audio sample rate for the engine. Default is 44100Hz");

    generalConfig.SetLongValue(L"Audio", L"buffer_length", m_audio_bufferlen, L"# Audio buffer length. Default is 2048. Smaller may be lower latency but carries risk of audio issues.");

    generalConfig.SetValue(L"Audio", L"driver", Str2WStr(m_audio_driver).c_str(), L"# Audio driver, may be comma seperated for a list to try in priority order. Valid options are 'default', 'wasapi', 'directsound', 'winmm' and 'dummy'.");

    generalConfig.SetBoolValue(L"Controller", L"battery_reporting", m_controller_battery_reporting, L"# Set to false to disable controller battery reporting. True by default.", true);

    generalConfig.SetBoolValue(L"Controller", L"rumble_enabled", m_controller_rumble_enabled, L"# Set to false to disable controller rumble support. True by default.", true);

    generalConfig.SetBoolValue(L"Lua", L"enable-http", m_lua_enable_http, L"# Set to true if you want to have the HTTP API enabled. This might be a security risk, only activate if you trust the episode/api.", true);

    generalConfig.SetBoolValue(L"Engine", L"cpu_lock_affinity", m_engine_cpu_lock_affinity, L"# Set to true if you want the main thread to be locked to one CPU core at startup.", true);

    return generalConfig.SaveFile(m_settingFilename.c_str(), true) == SI_OK;
}

void GeneralLunaConfig::loadOrDefault()
{
    if (!load())
    {
        doDefaults();
        save();
    }
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

    const wchar_t* vsyncValueStr = configToLoad.GetValue(L"Renderer", L"vsync", L"off");
    if (_wcsicmp(vsyncValueStr, L"on") == 0)
    {
        m_renderer_vsync = VSyncModeOn;
    }
    else if (_wcsicmp(vsyncValueStr, L"adaptive") == 0)
    {
        m_renderer_vsync = VSyncModeAdaptive;
    }
    else
    {
        m_renderer_vsync = VSyncModeOff;
    }

    m_renderer_useLetterbox = configToLoad.GetBoolValue(L"Renderer", L"use_letterbox", true);
    m_audio_samplerate = configToLoad.GetLongValue(L"Audio", L"sample_rate", 44100);
    m_audio_bufferlen = configToLoad.GetLongValue(L"Audio", L"buffer_length", 2048);
    m_audio_driver = WStr2Str(configToLoad.GetValue(L"Audio", L"driver", L"default"));
    m_controller_battery_reporting = configToLoad.GetBoolValue(L"Controller", L"battery_reporting", true);
    m_controller_rumble_enabled = configToLoad.GetBoolValue(L"Controller", L"rumble_enabled", true);
    m_lua_enable_http = configToLoad.GetBoolValue(L"Lua", L"enable-http", false);
    m_engine_cpu_lock_affinity = configToLoad.GetBoolValue(L"Engine", L"cpu_lock_affinity", false);

    save();
    return true;
}
