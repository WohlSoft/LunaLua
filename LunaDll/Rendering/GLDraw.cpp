#include <windows.h>
#include <gl/glew.h>
#include "../Defines.h"
#include "GLDraw.h"
#include "GLCompat.h"

// Instance
GLDraw g_GLDraw;

GLDraw::GLDraw() : mLastTexName(0)
{
}

void GLDraw::Unbind()
{
    mLastTexName = 0;
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLDraw::DrawSprite(int nXDest, int nYDest, int nWidth, int nHeight, const Texture* tex, int nXSrc, int nYSrc, RenderMode mode)
{
    // Trim the coordinates to fit the texture
    if (nXSrc < 0) {
        nXDest -= nXSrc;
        nWidth += nXSrc;
        nXSrc = 0;
    }
    if (nYSrc < 0) {
        nYDest -= nYSrc;
        nHeight += nYSrc;
        nYSrc = 0;
    }
    if (nWidth > ((int)tex->w - nXSrc)) {
        nWidth = (int)tex->w - nXSrc;
    }
    if (nHeight > ((int)tex->h - nYSrc)) {
        nHeight = (int)tex->h - nYSrc;
    }

    // Don't render if calculated width or height is <= 0
    if (nHeight <= 0) return;
    if (nWidth <= 0) return;

    // Generate our floating point coordinates
    float texw = (float)tex->w;
    float texh = (float)tex->h;
    float x1 = (float)nXDest;
    float y1 = (float)nYDest;
    float x2 = x1 + nWidth;
    float y2 = y1 + nHeight;
    float tx1 = nXSrc / texw;
    float ty1 = nYSrc / texh;
    float tx2 = tx1 + nWidth / texw;
    float ty2 = ty1 + nHeight / texh;

    // Set rendering mode for this draw operation
    switch (mode) {
    case RENDER_MODE_MULTIPLY:
        glBlendEquationANY(GL_FUNC_ADD);
        glBlendFuncSeparateANY(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
        break;
    case RENDER_MODE_MAX:
        glBlendEquationANY(GL_MAX);
        break;
    case RENDER_MODE_AND:
        glBlendEquationANY(GL_LOGIC_OP);
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(GL_AND);
        break;
    case RENDER_MODE_OR:
        glBlendEquationANY(GL_LOGIC_OP);
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(GL_OR);
        break;
    case RENDER_MODE_ALPHA:
    default:
        glBlendEquationANY(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
    }

    if (mLastTexName != tex->name)
    {
        mLastTexName = tex->name;
        glBindTexture(GL_TEXTURE_2D, tex->name);
    }

    GLfloat Vertices[] = {
        x1, y1, 0,
        x2, y1, 0,
        x2, y2, 0,
        x1, y2, 0
    };
    GLfloat TexCoord[] = {
        tx1, ty1,
        tx2, ty1,
        tx2, ty2,
        tx1, ty2
    };
    GLubyte indices[] = {
        0, 1, 2, // (bottom left - top left - top right)
        0, 2, 3  // (bottom left - top right - bottom right)
    };

    glVertexPointer(3, GL_FLOAT, 0, Vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

    // Disable color logic op if we enabled it
    if ((mode == RENDER_MODE_AND) || (mode == RENDER_MODE_OR))
    {
        glDisable(GL_COLOR_LOGIC_OP);
    }
}

void GLDraw::DrawRectangle(int nXDest, int nYDest, int nWidth, int nHeight)
{
    Unbind();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    // Generate our floating point coordinates
    float x1 = (float)nXDest;
    float y1 = (float)nYDest;
    float x2 = x1 + nWidth;
    float y2 = y1 + nHeight;

    GLfloat Vertices[] = {
        x1, y1, 0,
        x2, y1, 0,
        x2, y2, 0,
        x1, y2, 0
    };
    GLubyte indices[] = {
        0, 1, 2, // (bottom left - top left - top right)
        0, 2, 3  // (bottom left - top right - bottom right)
    };

    glVertexPointer(3, GL_FLOAT, 0, Vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
}


void GLDraw::DrawStretched(int nXDest, int nYDest, int nWidth, int nHeight, const Texture* tex, int nXSrc, int nYSrc, int nSrcWidth, int nSrcHeight)
{
    // Generate our floating point coordinates
    float texw = (float)tex->w;
    float texh = (float)tex->h;
    float x1 = (float)nXDest;
    float y1 = (float)nYDest;
    float x2 = x1 + nWidth;
    float y2 = y1 + nHeight;
    float tx1 = nXSrc / texw;
    float ty1 = nYSrc / texh;
    float tx2 = tx1 + nSrcWidth / texw;
    float ty2 = ty1 + nSrcHeight / texh;

    // Set rendering mode for this draw operation
    glBlendEquationANY(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    if (mLastTexName != tex->name)
    {
        mLastTexName = tex->name;
        glBindTexture(GL_TEXTURE_2D, tex->name);
    }

    GLfloat Vertices[] = {
        x1, y1, 0,
        x2, y1, 0,
        x2, y2, 0,
        x1, y2, 0
    };
    GLfloat TexCoord[] = {
        tx1, ty1,
        tx2, ty1,
        tx2, ty2,
        tx1, ty2
    };
    GLubyte indices[] = {
        0, 1, 2, // (bottom left - top left - top right)
        0, 2, 3  // (bottom left - top right - bottom right)
    };

    glVertexPointer(3, GL_FLOAT, 0, Vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
}

