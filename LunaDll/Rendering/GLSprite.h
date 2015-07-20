#ifndef GLSprite_hhhh
#define GLSprite_hhhh

#include <windows.h>
#include <gl/glew.h>
#include <stdint.h>
#include "GLDraw.h"

class GLSprite {
public:
    GLSprite(void* data, GLint format, uint32_t xOff, uint32_t yOff, uint32_t width, uint32_t height);
    ~GLSprite();
    
    void Draw(int xDest, int yDest, int width, int height, int xSrc, int ySrc, GLDraw::RenderMode mode) const;
    void BindTexture() const;

    inline bool IsValid() const { return valid; }
private:
    bool valid;
    GLDraw::Texture tex;
};

#endif
