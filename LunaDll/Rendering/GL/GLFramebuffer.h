#ifndef GLFramebuffer_hhhh
#define GLFramebuffer_hhhh

#include <windows.h>
#include <glbinding/gl/gl.h>
#include "GLTextureStore.h"

class GLFramebuffer {
public:
    GLFramebuffer(int w, int h);
    ~GLFramebuffer();

    void Bind();
    void Clear();
    inline GLDraw::Texture& AsTexture() { return mBufTex; }

private:
    // Framebuffer variables
	gl::GLuint mFB;
	gl::GLuint mDepthRB;
    GLDraw::Texture mBufTex;
};

#endif
