#ifndef GLDraw_hhhh
#define GLDraw_hhhh

#include <windows.h>
#include <gl/glew.h>
#include <cstdint>

class GLDraw {
public:
    struct Texture {
        GLuint name;
        uint32_t w;
        uint32_t h;
    };
    enum RenderMode {
        RENDER_MODE_ALPHA,
        RENDER_MODE_MULTIPLY,
        RENDER_MODE_MAX,
        RENDER_MODE_AND,
        RENDER_MODE_OR
    };

public:
    GLuint mLastTexName;
    
    GLDraw();
    void Unbind();
    void Draw(int nXDest, int nYDest, int nWidth, int nHeight, const Texture* tex, int nXSrc, int nYSrc, RenderMode mode);
    void DrawRectangle(int nXDest, int nYDest, int nWidth, int nHeight);
private:
    bool GetFromHDC(HDC hDC);
};

// Instance
extern GLDraw g_GLDraw;

#endif
