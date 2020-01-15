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

private:
    GLMode m_renderer_opengl;
    bool   m_renderer_useLetterbox;
    bool   m_renderer_forceDisableFullscreen;
    bool   m_lua_enable_http;

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
    inline void setRendererUseLetterbox(bool rendererUseLetterbox) { m_renderer_useLetterbox = rendererUseLetterbox; }
    inline bool getRendererUseLetterbox() const { return m_renderer_useLetterbox; }
    inline void setForceDisableFullscreen(bool forceDisableFullscreen) { m_renderer_forceDisableFullscreen = forceDisableFullscreen; }
    inline bool getForceDisableFullscreen() const { return m_renderer_forceDisableFullscreen; }
    bool getLuaEnableHTTP() const { return m_lua_enable_http; }
    void getLuaEnableHTTP(bool val) { m_lua_enable_http = val; }
};

#endif