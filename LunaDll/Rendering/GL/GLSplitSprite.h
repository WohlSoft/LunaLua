#ifndef GLSplitSprite_hhhh
#define GLSplitSprite_hhhh

#include <windows.h>
#include <glbinding/gl/gl.h>
#include <stdint.h>
#include <vector>
#include "GLDraw.h"
#include "GLSprite.h"

class GLSplitSprite : public GLSprite {
public:
    static gl::GLint getMaxTextureSize() {
        static gl::GLint maxTextureSize = 0;
        if (maxTextureSize > 0) {
            return maxTextureSize;
        }
        maxTextureSize = 0;
		gl::glGetIntegerv(gl::GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        GLERRORCHECK();
        if (maxTextureSize <= 0) maxTextureSize = 1024;
        return maxTextureSize;
    }
public:
    GLSplitSprite(void* data, gl::GLenum format, int32_t width, int32_t height);
    virtual ~GLSplitSprite();

    virtual void Draw(int xDest, int yDest, int width, int height, int xSrc, int ySrc, GLDraw::RenderMode mode) const;
    virtual void BindTexture() const;

    virtual bool IsValid() const { return valid; }
private:
    bool valid;
    int32_t segsWide;
    int32_t segsTall;
    int32_t width;
    int32_t height;
    std::vector<const GLBasicSprite*> subSprites;
};

#endif
