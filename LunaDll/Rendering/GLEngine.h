#ifndef GLEngine_hhhh
#define GLEngine_hhhh

#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <unordered_map>

class GLEngine {
public:
    struct Texture {
        GLuint name;
        uint32_t w;
        uint32_t h;
    };
    enum RenderMode {
         RENDER_MODE_ALPHA,
         RENDER_MODE_MULTIPLY,
         RENDER_MODE_MAX
    };
private:
    bool mInitialized;
    bool mHadError;
    bool mEnabled;
    SDL_Window *mScreen;
    SDL_Renderer *mRenderer;
    SDL_GLContext mGLContext;
    std::vector<const Texture> mTexList;
    std::unordered_map<HDC, unsigned int> mHDCMap;
    GLuint mLastTexName;

    GLuint mFB;
    GLuint mColorRB;
    GLuint mDepthRB;
protected:
    // Internal routines
    void Init();
    const Texture* TextureFromBitmapHDC(HDC hdc);
    void Draw(int nXDest, int nYDest, int nWidth, int nHeight, const Texture* tex, int nXSrc, int nYSrc, RenderMode mode);

public:
    GLEngine();
    ~GLEngine();

    // External commands
    void WriteFrame(void* pixels);
    void ClearTextures();
    void EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
    inline bool IsEnabled() { return mEnabled; };
    inline bool Enable() { mEnabled = true; }
    inline bool Disable() { mEnabled = false; }
};

extern GLEngine g_GLEngine;

#endif
