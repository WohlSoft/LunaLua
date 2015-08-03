#include "GLSprite.h"
#include "GLSplitSprite.h"

GLSprite* GLSprite::Create(void* data, GLint format, int32_t width, int32_t height)
{
    GLSprite* sprite;

    if (width <= 0 || height <= 0) return NULL;

    int32_t maxTextureSize = GLSplitSprite::getMaxTextureSize();
    if (height > maxTextureSize || width > maxTextureSize) {
        sprite = new GLSplitSprite(data, format, width, height);
    } else {
        sprite = new GLBasicSprite(data, format, width, height, 0, 0, width, height);
    }
    

    return sprite;
}

GLSprite::GLSprite() {
}

GLSprite::~GLSprite() {
}

GLBasicSprite::GLBasicSprite(void* data, GLint format, uint32_t dataWidth, uint32_t dataHeight, uint32_t xOff, uint32_t yOff, uint32_t width, uint32_t height) :
    tex(0, width, height)
{
    valid = false;

    // Limit width/height
    if (xOff >= dataWidth) return;
    if (yOff >= dataHeight) return;
    if (dataWidth - xOff < width) width = dataWidth - xOff;
    if (dataHeight - yOff < height) height = dataHeight - yOff;
    tex = GLDraw::Texture(0, width, height);

    // Try to allocate texture
    glGenTextures(1, &tex.name);
    GLERRORCHECK();
    if (tex.name == 0) return;

    g_GLDraw.BindTexture(&tex);
    GLERRORCHECK();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    GLERRORCHECK();
    float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLERRORCHECK();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    GLERRORCHECK();

    glPixelStorei(GL_UNPACK_ROW_LENGTH, dataWidth);
    GLERRORCHECK();
    char* subData = (char*)data + (xOff + yOff*dataWidth) * 4;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.pw, tex.ph, 0, format, GL_UNSIGNED_BYTE, NULL);
    GLERRORCHECK();

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex.w, tex.h, format, GL_UNSIGNED_BYTE, subData);
    GLERRORCHECK();

    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.pw, tex.ph, 0, format, GL_UNSIGNED_BYTE, subData);
    //GLERRORCHECK();

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    GLERRORCHECK();

    valid = true;
}

GLBasicSprite::~GLBasicSprite() {
    if (valid) glDeleteTextures(1, &tex.name);
    GLERRORCHECK();
}

void GLBasicSprite::Draw(int xDest, int yDest, int width, int height, int xSrc, int ySrc, GLDraw::RenderMode mode) const {
    if (!valid) return;

    if (xSrc >= tex.w) return;
    if (ySrc >= tex.h) return;
    if (xSrc + width <= 0) return;
    if (ySrc + height <= 0) return;

    g_GLDraw.DrawSprite(xDest, yDest, width, height, &tex, xSrc, ySrc, mode);
}

void GLBasicSprite::BindTexture() const {
    g_GLDraw.BindTexture(&tex);
}


