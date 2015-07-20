#include "GLSprite.h"

GLSprite::GLSprite(void* data, GLint format, uint32_t xOff, uint32_t yOff, uint32_t width, uint32_t height) {
    valid = false;
    tex.h = height;
    tex.w = width;
    tex.name = 0;

    // Try to allocate texture
    glGenTextures(1, &tex.name);
    if (tex.name == 0) return;

    g_GLDraw.BindTexture(&tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.w, tex.h, 0, format, GL_UNSIGNED_BYTE, data);

    valid = true;
}

GLSprite::~GLSprite() {
    if (valid) glDeleteTextures(1, &tex.name);
}

void GLSprite::Draw(int xDest, int yDest, int width, int height, int xSrc, int ySrc, GLDraw::RenderMode mode) const {
    if (!valid) return;

    g_GLDraw.DrawSprite(xDest, yDest, width, height, &tex, xSrc, ySrc, mode);
}

void GLSprite::BindTexture() const {
    g_GLDraw.BindTexture(&tex);
}


