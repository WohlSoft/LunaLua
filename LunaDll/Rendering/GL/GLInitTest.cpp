#include <windows.h>
#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>
#include <glbinding/Version.h>
#include <glbinding/ContextInfo.h>
#include "GLCompat.h"
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

        // Init binding for context
        glbinding::Binding::useCurrentContext();
        glcompat::SetupContext();
    }
    catch (const GLException &e) {
        if (NULL != rc) glbinding::Binding::releaseCurrentContext();
        if (NULL != rc) wglMakeCurrent(NULL, NULL);
        if (NULL != rc) wglDeleteContext(rc);
        throw e;
    }

    return rc;
}

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
        wc.lpszClassName = "GLBINDING";
        if (0 == RegisterClassA(&wc)) return true;
        // create window
        wnd = CreateWindowA("GLBINDING", "GLBINDING", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL);
        if (NULL == wnd) throw GLException("Couldn't create GL test window");
        // get the device context
        dc = GetDC(wnd);
        if (NULL == dc) throw GLException("Couldn't get GL test window HDC");

        // Get GL context
        rc = GetGLRCFromHDC(dc);

        auto ext = glbinding::ContextInfo::extensions();

        bool have_VERSION_1_1 = glbinding::ContextInfo::supported(glbinding::Version(1, 1));
        bool have_VERSION_1_4 = glbinding::ContextInfo::supported(glbinding::Version(1, 4));
        bool have_VERSION_3_0 = glbinding::ContextInfo::supported(glbinding::Version(3, 0));

        bool have_ARB_blend_minmax = have_VERSION_1_4;
        bool have_EXT_blend_minmax = (ext.find(gl::GLextension::GL_EXT_blend_minmax) != ext.end());

        bool have_ARB_blend_func_separate = have_VERSION_1_4;
        bool have_EXT_blend_func_separate = (ext.find(gl::GLextension::GL_EXT_blend_func_separate) != ext.end());

        bool have_ARB_framebuffer_object = have_VERSION_3_0 || (ext.find(gl::GLextension::GL_ARB_framebuffer_object) != ext.end());
        bool have_EXT_framebuffer_object = (ext.find(gl::GLextension::GL_EXT_framebuffer_object) != ext.end());

        // Test for optional OpenGL features we require
        if (!have_VERSION_1_1) {
            throw GLException("Missing OpenGL >=1.1 support");
        }
        if (!(have_ARB_blend_minmax || have_EXT_blend_minmax)) {
            throw GLException("Missing EXT_blend_minmax");
        }
        if (!(have_ARB_blend_func_separate || have_EXT_blend_func_separate)) {
            throw GLException("Missing EXT_blend_func_separate");
        }
        if (!(have_ARB_framebuffer_object || have_EXT_framebuffer_object)) {
            throw GLException("Missing EXT_framebuffer_object");
        }
    }
    catch (const GLException &e) {
        ret = false;
        dbgboxA(("Using GDI renderer, so some advanced LunaDLL effects may not be present in some levels.\r\n\r\nCould not use OpenGL Renderer because:\r\n\t" + e.msg).c_str());
    }

    if (NULL != rc) glbinding::Binding::releaseCurrentContext();
    if (NULL != rc) wglMakeCurrent(NULL, NULL);
    if (NULL != rc) wglDeleteContext(rc);
    if (NULL != wnd && NULL != dc) ReleaseDC(wnd, dc);
    if (NULL != wnd) DestroyWindow(wnd);
    UnregisterClassA("GLEW", GetModuleHandle(NULL));

    return ret;
}

