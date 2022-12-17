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
    inline const GLDraw::Texture& AsDepthTexture() const { return mDepthTex; }

private:
    // Framebuffer variables
    GLuint mFB;
    GLDraw::Texture mBufTex;
    GLDraw::Texture mDepthTex;
};

#endif
