#include <windows.h>
#include <gl/glew.h>
#include "../../Defines.h"
#include "../../Globals.h"
#include "../RenderUtils.h"
#include "GLCompat.h"
#include "GLDraw.h"
#include "GLTextureStore.h"
#include "GLContextManager.h"
#include "GLEngine.h"
#include "../Shaders/GLShader.h"
#include "../WindowSizeHandler.h"

#include "../LunaImage.h"
#include "../AsyncGifRecorderImgs.h"

static std::shared_ptr<LunaImage> recImage = LunaImage::fromData(rec_image.width, rec_image.height, rec_image.pixel_data);
static std::shared_ptr<LunaImage> encImage = LunaImage::fromData(enc_image.width, enc_image.height, enc_image.pixel_data);

static const char* upscaleShaderVertSrc = "#version 120\n\
\n\
uniform vec2 inputSize;\n\
uniform vec2 crispScale;\n\
varying vec2 crispTexel;\n\
\n\
void main()\n\
{\n\
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n\
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
    gl_FrontColor = gl_Color;\n\
    \n\
    crispTexel = gl_TexCoord[0].xy * inputSize.xy;\n\
}\n";
static const char* upscaleShaderFragSrc = "#version 120\n\
uniform sampler2D iChannel0;\n\
uniform vec2 inputSize;\n\
uniform vec2 crispScale;\n\
varying vec2 crispTexel;\n\
\n\
void main()\n\
{\n\
    vec2 texel = crispTexel;\n\
    vec2 scale = crispScale;\n\
    vec2 texelFloor = floor(texel);\n\
    vec2 texelFrac = fract(texel);\n\
    vec2 range = 0.5 - 0.5 / scale;\n\
    vec2 centerDist = texelFrac - 0.5;\n\
    vec2 newFrac = (centerDist - clamp(centerDist, -range, range)) * scale + 0.5;\n\
    vec2 newTexel = texelFloor + newFrac;\n\
\n\
    vec4 c = texture2D(iChannel0, newTexel / inputSize);\n\
    \n\
    gl_FragColor = c*gl_Color;\n\
}\n";

GLEngine::GLEngine() :
    mEnabled(true), mBitwiseCompat(false),
    mHwnd(NULL),
    mScreenshot(false),
    mCameraX(0.0), mCameraY(0.0),
    mpUpscaleShader(nullptr)
{
}

GLEngine::~GLEngine() {
}

void GLEngine::InitForHDC(HDC hdcDest)
{
    static bool runOnce = true;
    if (!g_GLContextManager.Init(hdcDest)) {
        dbgboxA("Failed to init...");
    }
    else {
        // TODO: Move mGifRecorder initialization somewhere else. It can't be
        //       in the constructor due to when the constructor
        if (runOnce) {
            runOnce = false;
            mGifRecorder.init();

            // Try to build the upscale shader
            mpUpscaleShader = new GLShader(upscaleShaderVertSrc, upscaleShaderFragSrc);

            // A little trick to ensure early rendering works
            EndFrame(hdcDest, false);
        }
    }
}

void GLEngine::EndFrame(HDC hdcDest, bool skipFlipToScreen)
{

    static HDC cachedHDC = NULL;
    if (hdcDest == NULL)
    {
        if (cachedHDC != NULL)
        {
            hdcDest = cachedHDC;
        }
        else
        {
            return;
        }
    }
    else
    {
        cachedHDC = hdcDest;
    }

    if (!skipFlipToScreen)
    {
        int fbWidth = g_GLContextManager.GetMainFBWidth();
        int fbHeight = g_GLContextManager.GetMainFBHeight();

        // Primary FB for screenshot and gif recorder purposes
        g_GLContextManager.BindPrimaryFB();

        // Generate screenshot...
        if (mScreenshot) {
            GenerateScreenshot(0, 0, fbWidth, fbHeight);
        }

        if (mGifRecorder.isRunning())
        {
            // Record next frame
            GifRecorderNextFrame(0, 0, fbWidth, fbHeight);

            const GLSprite* sprite = g_GLTextureStore.SpriteFromLunaImage(recImage);
            if (sprite)
            {
                const GLDraw::Texture tex(sprite->GetTexId(), recImage->getW(), recImage->getH());
                g_GLDraw.DrawStretched(10, 10, tex.w, tex.h, &tex, 0, tex.h, tex.w, -tex.h, 1.0f, nullptr);
            }
        }
        else if (mGifRecorder.isEncoding())
        {
            const GLSprite* sprite = g_GLTextureStore.SpriteFromLunaImage(encImage);
            if (sprite)
            {
                const GLDraw::Texture tex(sprite->GetTexId(), encImage->getW(), encImage->getH());
                g_GLDraw.DrawStretched(10, 10, tex.w, tex.h, &tex, 0, tex.h, tex.w, -tex.h, 1.0f, nullptr);
            }
        }

        // Bind screen
        g_GLContextManager.BindScreen();

        // Get window size
        const auto winState = gWindowSizeHandler.getStateThreadSafe();
        const int32_t& windowWidth = winState.windowSize.x;
        const int32_t& windowHeight = winState.windowSize.y;
        const double& xOffset = winState.fbOffset.x;
        const double& yOffset = winState.fbOffset.y;
        const double& xScale = winState.fbScale.x;
        const double& yScale = winState.fbScale.y;

        // Derive expected destination coordinates
        int nXOriginDest = static_cast<int>(std::round(xOffset));
        int nYOriginDest = static_cast<int>(std::round(yOffset));
        int nWidthDest = static_cast<int>(std::round(fbWidth * xScale));
        int nHeightDest = static_cast<int>(std::round(fbHeight * yScale));

        // Set viewport for window size
        glViewport(0, 0, windowWidth, windowHeight);
        GLERRORCHECK();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        GLERRORCHECK();
        glOrtho(0, static_cast<GLdouble>(windowWidth), static_cast<GLdouble>(windowHeight), 0, -1.0f, 1.0f);
        GLERRORCHECK();

        // Use upscaling shader if we're upscaling and it compiled right
        GLShader* upscaleShader = nullptr;
        if (mpUpscaleShader->isValid() && ((xScale > 1.0f) || (yScale > 1.0f)))
        {
            upscaleShader = mpUpscaleShader;
        }

        // Draw the buffer, flipped/stretched as appropriate
        g_GLDraw.DrawStretched(nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, &g_GLContextManager.GetPrimaryFBTex(), 0, 0, fbWidth, fbHeight, 1.0f, upscaleShader);
        GLERRORCHECK();
        glFlush();
        GLERRORCHECK();

        // Display Frame
        SwapBuffers(hdcDest);

        // Clear screen backbuffer
        GLERRORCHECK();
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClearDepth(100.0f);
        GLERRORCHECK();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GLERRORCHECK();
    }

    // Bind primary framebuffer
    g_GLContextManager.BindPrimaryFB();
    GLFramebuffer* fb = g_GLContextManager.GetCurrentFB();
    if (fb) fb->Clear();
}

bool GLEngine::GenerateScreenshot(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    uint32_t byteSize = 3 * w * h;

    // Generate handle
    HANDLE handle = (HANDLE)GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + byteSize);
    if (handle == NULL) return false;

    // Lock
    uint8_t *pData = (uint8_t *)GlobalLock(handle);
    BITMAPINFOHEADER &header = *((BITMAPINFOHEADER*)pData);
    uint8_t *pPixelData = &pData[sizeof(BITMAPINFOHEADER)];

    // Set up headers
    memset(&header, 0, sizeof(BITMAPINFOHEADER));
    header.biWidth = w;
    header.biHeight = h;
    header.biSizeImage = byteSize;
    header.biSize = 40;
    header.biPlanes = 1;
    header.biBitCount = 3 * 8;
    header.biCompression = 0;
    header.biXPelsPerMeter = 0;
    header.biYPelsPerMeter = 0;
    header.biClrUsed = 0;
    header.biClrImportant = 0;

    // Read pixels
    glReadPixels(x, y, w, h, GL_BGR, GL_UNSIGNED_BYTE, pPixelData);
    if (glGetError() != GL_NO_ERROR) {
        GlobalFree(handle);
        handle = nullptr;
        return false;
    }

    // Flip pixels
    int wBytes = 3 * static_cast<int>(w);
    for (int y = 0; y < static_cast<int>(h) / 2; y++)
    {
        int y2 = h - 1 - y;
        BYTE* row1 = &pPixelData[y * w * 3];
        BYTE* row2 = &pPixelData[y2 * w * 3];
        for (int i = 0; i < wBytes; i++)
        {
            BYTE tmp = row1[i];
            row1[i] = row2[i];
            row2[i] = tmp;
        }
    }

    bool releaseMem = true;
    if (mScreenshotCallback) {
        bool releaseMem = mScreenshotCallback(handle, &header, (void*)pPixelData, mHwnd);
    }
    else
    {
        GlobalFree(handle);
        handle = nullptr;
        return false;
    }

    mScreenshotCallback = nullptr;
    mScreenshot = false;
    return true;
}

bool GLEngine::GifRecorderToggle()
{
    if (!mGifRecorder.isRunning()){
        mGifRecorder.start();
        return true;
    }
    else
    {
        mGifRecorder.stop();
        return false;
    }

}

void GLEngine::GifRecorderNextFrame(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    // Skip every second frame
    static DWORD lastTime = 0;

    // Don't allow more than 10 seconds of raw buffered footage
    if (mGifRecorder.bufferLen() > 10 * 32)
    {
        return;
    }

    // Record no more than 1 frame per 30ms
    DWORD timestamp = GetTickCount();
    DWORD timeDiff = timestamp - lastTime;
    if (timeDiff < 30)
    {
        return;
    }
    lastTime = timestamp;

    BYTE* pixData = new BYTE[w * (h + 1) * 4];

    // Read pixels
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixData);
    if (glGetError() != GL_NO_ERROR) {
        delete pixData;
        return;
    }

    mGifRecorder.addNextFrameToProcess(w, h, pixData, timestamp);
}
