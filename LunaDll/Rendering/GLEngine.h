#ifndef GLEngine_hhhh
#define GLEngine_hhhh

#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <unordered_map>
#include "GLDraw.h"

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
protected:
    // Internal routines
    void Init();

public:
    GLEngine();
    ~GLEngine();
    void ClearTextures();

    // External commands
    inline bool IsEnabled() { return mEnabled; };
    inline bool Enable() { mEnabled = true; }
    inline bool Disable() { mEnabled = false; }

    void EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
    BOOL EmulatedStretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
        DWORD dwRop);
};

#include "GLEngineProxy.h"

#endif
