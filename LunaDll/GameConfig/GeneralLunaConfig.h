#ifndef GeneralLunaConfig_hhhhh
#define GeneralLunaConfig_hhhhh

#include <string>

class GeneralLunaConfig
{
public:
    enum GLMode
    {
        GLModeAuto=0,
        GLModeSoft,
        GLModeHard
    };

    enum VSyncMode
    {
        VSyncModeOff = 0,
        VSyncModeOn,
        VSyncModeAdaptive
    };

private:
    GLMode      m_renderer_opengl;
    VSyncMode   m_renderer_vsync;
    bool        m_renderer_useLetterbox;
    int         m_audio_samplerate;
    int         m_audio_bufferlen;
    std::string m_audio_driver;
    bool        m_controller_battery_reporting;
    bool        m_controller_rumble_enabled;
    bool        m_lua_enable_http;
    bool        m_engine_cpu_lock_affinity;

    std::wstring m_settingFilename;
public:
    GeneralLunaConfig();
    ~GeneralLunaConfig();

    void doDefaults();
    bool save();
    
    void loadOrDefault();
    bool load();

    inline void setFilename(const std::wstring& filename) { m_settingFilename = filename; }
    inline std::wstring filename() const { return m_settingFilename; }

    inline GLMode getRendererOpenGL() const { return m_renderer_opengl; }
    inline VSyncMode getRendererVSync() const { return m_renderer_vsync; }
    inline void setRendererUseLetterbox(bool rendererUseLetterbox) { m_renderer_useLetterbox = rendererUseLetterbox; }
    inline bool getRendererUseLetterbox() const { return m_renderer_useLetterbox; }
    inline int getAudioSampleRate() const { return m_audio_samplerate; }
    inline int getAudioBufferLength() const { return m_audio_bufferlen; }
    inline std::string getAudioDriver() const { return m_audio_driver; }
    inline bool getControllerBatteryReporting() const { return m_controller_battery_reporting; }
    inline bool getControllerRumbleEnabled() const { return m_controller_rumble_enabled; }
    bool getLuaEnableHTTP() const { return m_lua_enable_http; }
    inline bool getEngineCpuLockAffinity() const { return m_engine_cpu_lock_affinity; }
};

#endif