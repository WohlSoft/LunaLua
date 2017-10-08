#ifndef GLSprite_hhhh
#define GLSprite_hhhh

#include <windows.h>
#include <gl/glew.h>
#include <stdint.h>
#include "GLDraw.h"

class GLSprite {
public:
    // Factory method
    static GLSprite* Create(void* data, GLint format, int32_t width, int32_t height);
protected:
    GLSprite();
public:
    virtual ~GLSprite();
    virtual void Draw(const SRect<double>& dest, const SRect<double>& src, float opacity, GLDraw::RenderMode mode) const = 0;
    virtual void BindTexture() const = 0;
    virtual unsigned int GetTexId() const = 0;
    virtual bool IsValid() const = 0;


    inline void Draw(int xDest, int yDest, int width, int height, int xSrc, int ySrc, GLDraw::RenderMode mode) const
    {
        Draw(SRect<double>::fromXYWH(xDest, yDest, width, height), SRect<double>::fromXYWH(xSrc, ySrc, width, height), 1.0f, mode);
    }
};

class GLBasicSprite : public GLSprite {
public:
    GLBasicSprite(void* data, GLint format, uint32_t dataWidth, uint32_t dataHeight, uint32_t xOff, uint32_t yOff, uint32_t width, uint32_t height);
    virtual ~GLBasicSprite();

    virtual void Draw(const SRect<double>& dest, const SRect<double>& src, float opacity, GLDraw::RenderMode mode) const;
    virtual void BindTexture() const;
    virtual unsigned int GetTexId() const {
        if (valid)
            return tex.name;
        return 0;
    };
    virtual bool IsValid() const { return valid; }
private:
    bool valid;
    GLDraw::Texture tex;
};

#include "GLSplitSprite.h"

#endif
