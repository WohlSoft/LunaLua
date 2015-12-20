#include <windows.h>
#include <gl/glew.h>
#include "GLEngine.h"
#include "../Defines.h"
#include "../Globals.h"
#include "RenderUtils.h"
#include "GLCompat.h"
#include "GLDraw.h"
#include "GLTextureStore.h"
#include "GLContextManager.h"

GLEngine::GLEngine() :
    mEnabled(true), mBitwiseCompat(false),
    mHwnd(NULL),
    mScreenshot(false)
{
}

GLEngine::~GLEngine() {
}

void GLEngine::ClearSMBXSprites() {
    g_GLDraw.UnbindTexture();
    g_GLTextureStore.ClearSMBXSprites();
}

void GLEngine::ClearLunaTexture(const BMPBox& bmp) {
    g_GLDraw.UnbindTexture();
    g_GLTextureStore.ClearLunaTexture(bmp);
}

BOOL GLEngine::RenderCameraToScreen(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
    HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
    DWORD dwRop)
{
	// Load Post-Processing Shader somewhere here

	static bool runOnce = true;
    if (!g_GLContextManager.Init(hdcDest)) {
        dbgboxA("Failed to init...");
    } else {
		// TODO: Move mGifRecorder initialization somewhere else. It can't be
		//       in the constructor due to when the constructor 
		if (runOnce) {
			mGifRecorder.init();
			runOnce = false;
		}
    }

	if (!g_GLContextManager.IsInitialized()) return FALSE;

    // Get window size
    RECT clientRect;
    mHwnd = WindowFromDC(hdcDest);
    if (!GetClientRect(mHwnd, &clientRect)) return FALSE;
    if (clientRect.right <= 0) clientRect.right = 1;
    if (clientRect.bottom <= 0) clientRect.bottom = 1;
    int32_t windowWidth = clientRect.right;
    int32_t windowHeight = clientRect.bottom;

    g_GLDraw.UnbindTexture();

    // Unbind the texture from the framebuffer (Bind screen)
	g_GLContextManager.BindScreen();

    // Implement letterboxing correction
    float scaledWidth = windowWidth / 800.0f;
    float scaledHeight = windowHeight / 600.0f;
    float xScale = 1.0f;
    float yScale = 1.0f;

    if (gGeneralConfig.getRendererUseLetterbox()) {
        if (scaledWidth > scaledHeight) {
            xScale = scaledHeight / scaledWidth;
        }
        else if (scaledWidth < scaledHeight) {
            yScale = scaledWidth / scaledHeight;
        }
    }

    float xOffset = (windowWidth / xScale - windowWidth) * 0.5f;
    float yOffset = (windowHeight / yScale - windowHeight) * 0.5f;

    // Set viewport for window size
    glViewport(0, 0, windowWidth, windowHeight);
    GLERRORCHECK();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLERRORCHECK();
    glOrtho(-xOffset, (float)windowWidth + xOffset, (float)windowHeight + yOffset, -yOffset, -1.0f, 1.0f);
    GLERRORCHECK();

    // Draw the buffer, flipped/stretched as appropriate
    g_GLDraw.DrawStretched(nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, &g_GLContextManager.GetBufTex(), nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, 1.0f);
    GLERRORCHECK();
    glFlush();
    GLERRORCHECK();

    // Get ready to draw some more
	g_GLContextManager.BindFramebuffer();

    return TRUE;
}

void GLEngine::EndFrame(HDC hdcDest)
{
	// Bind screen
	g_GLContextManager.BindScreen();

    // Generate screenshot...
    if (mScreenshot) {
        RECT clientRect;
        mScreenshot = false;
        if (GetClientRect(mHwnd, &clientRect)) {
            if (clientRect.right < 0) clientRect.right = 1;
            if (clientRect.bottom < 0) clientRect.bottom = 1;
            GenerateScreenshot(0, 0, clientRect.right, clientRect.bottom);
        }
    }

    if (mGifRecorder.isRunning())
    {
        RECT clientRect;
        if (GetClientRect(mHwnd, &clientRect)) {
            if (clientRect.right < 0) clientRect.right = 1;
            if (clientRect.bottom < 0) clientRect.bottom = 1;
            GifRecorderNextFrame(0, 0, clientRect.right, clientRect.bottom);
        }
    }

	// Display Frame
    SwapBuffers(hdcDest);

	// Clear screen backbuffer
    GLERRORCHECK();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    GLERRORCHECK();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLERRORCHECK();

	// Bind framebuffer
	g_GLContextManager.BindFramebuffer();
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
        GlobalUnlock(handle);
        GlobalFree(handle);
        mScreenshotCallback = nullptr;
        return false;
    }

    
    bool releaseMem = true;
    if (mScreenshotCallback) {
        bool releaseMem = mScreenshotCallback(handle, &header, (void*)pPixelData, mHwnd);
    }

    if ((GlobalFlags(handle) & GMEM_LOCKCOUNT) >= 1){
        GlobalUnlock(handle);
    }

    if (releaseMem){
        GlobalFree(handle);
    }
    
    mScreenshotCallback = nullptr;
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
    BYTE* pixData = new BYTE[w * h * 3];
    
    // Read pixels
    glReadPixels(x, y, w, h, GL_BGR, GL_UNSIGNED_BYTE, pixData);
    if (glGetError() != GL_NO_ERROR) {
        return;
    }

    mGifRecorder.addNextFrameToProcess(w, h, pixData);
}
