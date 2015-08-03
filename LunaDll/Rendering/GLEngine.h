#ifndef GLEngine_hhhh
#define GLEngine_hhhh

#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <unordered_map>
#include "GLDraw.h"
#include "BMPBox.h"

class GLEngine {
private:
    bool mInitialized;
    bool mHadError;
    bool mEnabled;
    bool mBitwiseCompat;
    SDL_Window *mScreen;
    SDL_Renderer *mRenderer;
    SDL_GLContext mGLContext;

    GLuint mFB;
    GLuint mColorRB;
    GLuint mDepthRB;

    GLDraw::Texture mBufTex;

    HWND mHwnd;
    bool mScreenshot;
protected:
    // Internal routines
    void Init();

public:
    GLEngine();
    ~GLEngine();
    void ClearSMBXSprites();
    void ClearLunaTexture(const BMPBox& bmp);

    // External commands
    inline bool IsEnabled() { return mEnabled; };
    inline void Enable() { mEnabled = true; }
    inline void Disable() { mEnabled = false; }
    inline void TriggerScreenshot() { mScreenshot = true; }

    void EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
    BOOL EmulatedStretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
        DWORD dwRop);
    void DrawLunaSprite(int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        const BMPBox& bmp, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc);
    void EndFrame(HDC hdcDest);

    void SetTex(const BMPBox* bmp, uint32_t color);
    void Draw2DArray(GLuint type, const float* vert, float* tex, uint32_t count);

    bool GenerateScreenshot(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
};

#include "GLEngineProxy.h"

#endif
