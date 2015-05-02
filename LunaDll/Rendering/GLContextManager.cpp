#include <windows.h>
#include <gl/glew.h>
#include "../Defines.h"
#include "GLContextManager.h"

// Instance
GLContextManager g_GLContextManager;

// Constructor
GLContextManager::GLContextManager() : hDC(NULL), hCTX(NULL) {
}

bool GLContextManager::Init(HDC hDC) {
    if (IsInitialized()) return true;
    return InitFromHDC(hDC);
}

bool GLContextManager::IsInitialized() {
    return hCTX != NULL;
}

// Set up a new context from a HDC
bool GLContextManager::InitFromHDC(HDC hDC) {
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int nPixelFormat = ChoosePixelFormat(hDC, &pfd);
    if (0 == nPixelFormat)
        return false;

    BOOL bResult = SetPixelFormat(hDC, nPixelFormat, &pfd);
    if (FALSE == bResult)
        return false;

    HGLRC tempContext = wglCreateContext(hDC);
    if (NULL == tempContext)
        return false;

    bResult = wglMakeCurrent(hDC, tempContext);
    if (FALSE == bResult)
    {
        wglDeleteContext(tempContext);
        return false;
    }

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(tempContext);
        return false;
    }

    glLoadIdentity();
    glOrtho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    glColor3f(1, 1, 1);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Prefer v-sync off (but will work fine if forced on too)
    typedef BOOL(APIENTRY * PFNWGLSWAPINTERVALEXTPROC)(int);
    static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = 0;
    wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
    if (wglSwapIntervalEXT != 0) {
        wglSwapIntervalEXT(0);
    }

    this->hDC = hDC;
    this->hCTX = tempContext;
    return true;
}



