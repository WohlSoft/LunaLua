#ifndef GLDraw_hhhh
#define GLDraw_hhhh

#include <windows.h>
#include <gl/glew.h>
#include <cstdint>

class GLDraw {
private:
    GLuint mLastTexName;
public:
    struct Texture {
        GLuint name;
        int32_t w;
        int32_t h;
    };
    enum RenderMode {
        RENDER_MODE_ALPHA,
        RENDER_MODE_MULTIPLY,
        RENDER_MODE_MAX,
        RENDER_MODE_AND,
        RENDER_MODE_OR
    };

public:
    GLDraw();
    inline void BindTexture(const Texture* tex) {
        GLuint textName = tex ? tex->name : 0;
        if (mLastTexName != textName)
        {
            mLastTexName = textName;
            glBindTexture(GL_TEXTURE_2D, textName);
        }
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
    inline void UnbindTexture() {
        if (mLastTexName == 0) return;
        mLastTexName = 0;
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    void DrawSprite(int nXDest, int nYDest, int nWidth, int nHeight, const Texture* tex, int nXSrc, int nYSrc, RenderMode mode);
    void DrawRectangle(int nXDest, int nYDest, int nWidth, int nHeight);
    void DrawStretched(int nXDest, int nYDest, int nWidth, int nHeight, const Texture* tex, int nXSrc, int nYSrc, int nSrcWidth, int nSrcHeight);
};

// Instance
extern GLDraw g_GLDraw;

#endif
