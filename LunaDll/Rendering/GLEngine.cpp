#include <windows.h>
#include <SDL2/SDL.h>
#include <gl/glew.h>
#include "GLEngine.h"
#include "../Defines.h"
#include "../Globals.h"

GLEngine g_GLEngine;

GLEngine::GLEngine() :
    mInitialized(false), mHadError(false), mEnabled(false),
    mScreen(NULL), mRenderer(NULL),
    mGLContext(NULL), mLastTexName(0),
    mFB(0), mColorRB(0), mDepthRB(0) {
    mTexList.clear();
    mHDCMap.clear();
}

GLEngine::~GLEngine() {
    if (mGLContext) {
        SDL_GL_DeleteContext(mGLContext);
        mGLContext = NULL;
    }
    if (mScreen) {
        SDL_DestroyWindow(mScreen);
        mScreen = NULL;
    }
}

void GLEngine::Init() {
    int ret;

    if (mInitialized || mHadError) return;
    mHadError = true;

    SDL_Init(SDL_INIT_VIDEO);

    // Set up GL context
    ret = SDL_CreateWindowAndRenderer(
        800, 600,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL,
        &mScreen, &mRenderer);
    if (ret)
    {
        dbgboxA(SDL_GetError());
        return;
    }
    mGLContext = SDL_GL_CreateContext(mScreen);

    // Init GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        dbgboxA((const char*)glewGetErrorString(err));
        return;
    }

    // Set up framebuffer object
    glGenFramebuffersEXT(1, &mFB);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);
    glGenRenderbuffersEXT(1, &mColorRB);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mColorRB);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, 800, 600);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, mColorRB);
    glGenRenderbuffersEXT(1, &mDepthRB);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mDepthRB);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 800, 600);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mDepthRB);
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        dbgboxA("error setting up");
    }

    // Bind framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set projection (test)
    glOrtho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    glColor3f(1, 1, 1);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    mLastTexName = 0;

    mInitialized = true;
    mHadError = false;
}

const GLEngine::Texture* GLEngine::TextureFromBitmapHDC(HDC hdc) {
    Texture tex = { 0, 0, 0 };

    // Get associated texture from cache if possible
    auto it = mHDCMap.find(hdc);
    if (it != mHDCMap.end()) {
        return &mTexList[it->second];
    }

    {
        BITMAP bmp;
        HBITMAP hbmp;

        // Get handle to bitmap
        hbmp = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
        if (hbmp == NULL) return 0;

        // Get bitmap structure to check the height/width
        GetObject(hbmp, sizeof(BITMAP), &bmp);
        tex.w = bmp.bmWidth;
        tex.h = bmp.bmHeight;
    }

    // Convert to 24bpp BGR in memory that's accessible
    BITMAPINFO convBMI;
    memset(&convBMI, 0, sizeof(BITMAPINFO));
    convBMI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    convBMI.bmiHeader.biWidth = tex.w;
    convBMI.bmiHeader.biHeight = -(int)tex.h;
    convBMI.bmiHeader.biPlanes = 1;
    convBMI.bmiHeader.biBitCount = 24;
    convBMI.bmiHeader.biCompression = BI_RGB;
    HDC screenHDC = GetDC(NULL);
    if (screenHDC == NULL)
    {
        return 0;
    }
    void* pData = NULL;
    HBITMAP convHBMP = CreateDIBSection(screenHDC, &convBMI, DIB_RGB_COLORS, &pData, NULL, 0);
    HDC convHDC = CreateCompatibleDC(screenHDC);
    SelectObject(convHDC, convHBMP);
    BitBlt(convHDC, 0, 0, tex.w, tex.h, hdc, 0, 0, SRCCOPY);
    ReleaseDC(NULL, convHDC);
    convHDC = NULL;
    ReleaseDC(NULL, screenHDC);
    screenHDC = NULL;
    
    // Move into texture
    glGenTextures(1, &tex.name);
    if (tex.name == 0)
    {
        DeleteObject(convHBMP);
        convHBMP = NULL;
        return 0;
    }
    glBindTexture(GL_TEXTURE_2D, tex.name);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.w, tex.h, 0, GL_BGR, GL_UNSIGNED_BYTE, pData);

    // Delete conversion DIB section
    DeleteObject(convHBMP);
    convHBMP = NULL;

    // Cache new texture
    unsigned int texIdx = mTexList.size();
    mTexList.push_back(tex);
    mHDCMap[hdc] = texIdx;

    return &mTexList[texIdx];
}

void GLEngine::Draw(int nXDest, int nYDest, int nWidth, int nHeight, const Texture* tex, int nXSrc, int nYSrc, RenderMode mode)
{
    if (!mInitialized) Init();
    if (!mInitialized) return;
    
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
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
        break;
    case RENDER_MODE_MAX:
        glBlendEquation(GL_MAX);
        break;
    case RENDER_MODE_ALPHA:
    default:
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
}

void GLEngine::WriteFrame(void* pixels)
{
    if (!mInitialized) Init();
    if (!mInitialized) return;

    glReadPixels(0, 0, 800, 600, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

    // Prepare for next frame
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLEngine::ClearTextures()
{
    mLastTexName = 0;
    for (unsigned int i = 0; i < mTexList.size(); i++) {
        glDeleteTextures(1, &mTexList[i].name);
    }
    mTexList.clear();
    mHDCMap.clear();
}

void GLEngine::EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
    if (!mInitialized) Init();
    if (!mInitialized) return;

    const Texture* tex = TextureFromBitmapHDC(hdcSrc);
    if (tex == NULL) {
        return;
    }

    RenderMode mode;
    switch (dwRop) {
    case SRCAND:
        mode = RENDER_MODE_MULTIPLY;
        break;
    case SRCPAINT:
        mode = RENDER_MODE_MAX;
        break;
    default:
        mode = RENDER_MODE_ALPHA;
        break;
    }

    Draw(nXDest, nYDest, nWidth, nHeight, tex, nXSrc, nYSrc, mode);
}

