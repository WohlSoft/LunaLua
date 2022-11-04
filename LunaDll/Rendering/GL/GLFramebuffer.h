#ifndef GLFramebuffer_hhhh
#define GLFramebuffer_hhhh

#include <windows.h>
#include <GL/glew.h>
#include "GLTextureStore.h"

class GLFramebuffer {
public:
    GLFramebuffer(int w, int h, bool haveAlpha);
    ~GLFramebuffer();

    void Bind();
    void Clear();
    void Clear(const GLclampf color[4]);
    inline const GLDraw::Texture& AsTexture() const { return mBufTex; }

private:
    // Framebuffer variables
    GLuint mFB;
    GLuint mDepthRB;
    GLDraw::Texture mBufTex;
};

#endif
