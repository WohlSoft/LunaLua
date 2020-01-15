#include <windows.h>
#include <gl/glew.h>
#include <GL/wglew.h>
#include <exception>
#include "../../Defines.h"

class GLException : std::exception {
public:
    std::string msg;
    inline GLException(const std::string msg) {
        this->msg = msg;
    }
};

static HGLRC GetGLRCFromHDC(HDC dc) {
    HGLRC rc = NULL;
    PIXELFORMATDESCRIPTOR pfd;
    int pixelformat = -1;

    try {
        /* find pixel format */
        ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
        if (pixelformat == -1) /* find default */
        {
            pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
            pixelformat = ChoosePixelFormat(dc, &pfd);
            if (pixelformat == 0) throw GLException("Couldn't find suitable GL pixel format");
        }
        /* set the pixel format for the dc */
        if (FALSE == SetPixelFormat(dc, pixelformat, &pfd)) throw GLException("Couldn't set GL pixel format");
        /* create rendering context */
        rc = wglCreateContext(dc);
        if (NULL == rc) throw GLException("Couldn't create initial GL context");
        if (FALSE == wglMakeCurrent(dc, rc)) throw GLException("Couldn't make initial GL context current");

        if (glewInit() != GLEW_OK) throw GLException("Couldn't initialize GLEW");
    }
    catch (const GLException &e) {
        if (NULL != rc) wglMakeCurrent(NULL, NULL);
        if (NULL != rc) wglDeleteContext(rc);
        throw e;
    }

    return rc;
}

static std::string testGlFeaturesMsg;

bool LunaDLLTestGLFeatures(void)
{
    int pixelformat = -1, major = 0, minor = 0, profile = 0, flags = 0;
    HWND wnd = NULL;
    HDC dc = NULL;
    HGLRC rc = NULL;
    WNDCLASSA wc;
    bool ret = true;

    try {
        // register window class
        ZeroMemory(&wc, sizeof(WNDCLASSA));
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpfnWndProc = DefWindowProc;
        wc.lpszClassName = "GLEW";
        if (0 == RegisterClassA(&wc)) return GL_TRUE;
        // create window
        wnd = CreateWindowA("GLEW", "GLEW", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL);
        if (NULL == wnd) throw GLException("Couldn't create GL test window");
        // get the device context
        dc = GetDC(wnd);
        if (NULL == dc) throw GLException("Couldn't get GL test window HDC");

        // Get GL context
        rc = GetGLRCFromHDC(dc);

        // Test for optional OpenGL features we require
        if (!GLEW_VERSION_1_1) {
            throw GLException("Missing OpenGL >=1.1 support");
        }
        if (!(GLEW_VERSION_1_4 || GLEW_EXT_blend_minmax)) {
            throw GLException("Missing EXT_blend_minmax");
        }
        if (!(GLEW_VERSION_1_4 || GLEW_EXT_blend_func_separate)) {
            throw GLException("Missing EXT_blend_func_separate");
        }
        if (!(GLEW_VERSION_3_0 || GLEW_ARB_framebuffer_object || GLEW_EXT_framebuffer_object)) {
            throw GLException("Missing EXT_framebuffer_object");
        }
        testGlFeaturesMsg = "";
    }
    catch (const GLException &e) {
        ret = false;
        testGlFeaturesMsg = e.msg;
    }

    if (NULL != rc) wglMakeCurrent(NULL, NULL);
    if (NULL != rc) wglDeleteContext(rc);
    if (NULL != wnd && NULL != dc) ReleaseDC(wnd, dc);
    if (NULL != wnd) DestroyWindow(wnd);
    UnregisterClassA("GLEW", GetModuleHandle(NULL));

    return ret;
}

const char* LunaDLLTestGLFeaturesGetMessage()
{
    return testGlFeaturesMsg.c_str();
}
