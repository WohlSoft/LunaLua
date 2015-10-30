#include <windows.h>
#include <gl/glew.h>
#include "../Defines.h"
#include "GLContextManager.h"
#include "GLCompat.h"

// Instance
GLContextManager g_GLContextManager;

// Constructor
GLContextManager::GLContextManager() :
	hDC(NULL), hCTX(NULL),
	mInitialized(false), mHadError(false),
	mFB(0), mDepthRB(0),
	mBufTex(NULL, 800, 600) {
}

bool GLContextManager::Init(HDC hDC) {
	// If we're switching HDCs, deal with it...
	if (mInitialized && !mHadError && hDC != this->hDC) {
		// If we're switching HDCs, deal with it...
		g_GLDraw.UnbindTexture(); // Unbind current texture
		g_GLTextureStore.Reset(); // Delete all textures
		ReleaseFramebuffer(); // Release framebuffer
		ReleaseContext(); // Release context
		mInitialized = false;
	}

	// Don't re-run if already run
    if (mInitialized || mHadError) return true;

    if (InitContextFromHDC(hDC) &&
		InitFramebuffer() &&
		InitProjectionAndState())
	{
		mInitialized = true;
	} else {
		mHadError = true;
	}

	return mInitialized;
}

bool GLContextManager::IsInitialized() {
    return mInitialized;
}

void GLContextManager::BindScreen() {
	glBindFramebufferANY(GL_FRAMEBUFFER_EXT, 0);
	GLERRORCHECK();
}

void GLContextManager::BindFramebuffer() {
	glBindFramebufferANY(GL_FRAMEBUFFER_EXT, mFB);
	GLERRORCHECK();

	glViewport(0, 0, mBufTex.pw, mBufTex.ph);
	GLERRORCHECK();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLERRORCHECK();
	glOrtho(0.0f, ((float)mBufTex.pw), 0.0f, ((float)mBufTex.ph), -1.0f, 1.0f);
	GLERRORCHECK();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	GLERRORCHECK();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLERRORCHECK();
}

// Set up a new context from a HDC
bool GLContextManager::InitContextFromHDC(HDC hDC) {
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

	mOldPixelFormat = GetPixelFormat(hDC);

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

bool GLContextManager::InitFramebuffer() {
	// Set up framebuffer object
	glGenFramebuffersANY(1, &mFB);
	GLERRORCHECK();
	glBindFramebufferANY(GL_FRAMEBUFFER_EXT, mFB);
	GLERRORCHECK();

	glGenTextures(1, &mBufTex.name);
	GLERRORCHECK();
	g_GLDraw.BindTexture(&mBufTex);
	GLERRORCHECK();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mBufTex.pw, mBufTex.ph, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	GLERRORCHECK();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GLERRORCHECK();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GLERRORCHECK();

	glGenRenderbuffersANY(1, &mDepthRB);
	GLERRORCHECK();
	glBindRenderbufferANY(GL_RENDERBUFFER_EXT, mDepthRB);
	GLERRORCHECK();
	glRenderbufferStorageANY(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, mBufTex.pw, mBufTex.ph);
	GLERRORCHECK();
	glFramebufferRenderbufferANY(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mDepthRB);
	GLERRORCHECK();

	glFramebufferTexture2DANY(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mBufTex.name, 0);
	GLERRORCHECK();

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0_EXT };
	glDrawBuffers(1, DrawBuffers);
	GLERRORCHECK();

	GLenum status = glCheckFramebufferStatusANY(GL_FRAMEBUFFER_EXT);
	GLERRORCHECK();
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		return false;
	}

	// Bind framebuffer
	glBindFramebufferANY(GL_FRAMEBUFFER_EXT, mFB);
	GLERRORCHECK();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	GLERRORCHECK();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLERRORCHECK();

	return true;
}

bool GLContextManager::InitProjectionAndState() {
    // Set projection
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLERRORCHECK();
    glOrtho(0.0f, (float)mBufTex.pw, 0.0f, (float)mBufTex.ph, -1.0f, 1.0f);
    GLERRORCHECK();
    glColor3f(1, 1, 1);
    GLERRORCHECK();
    glDisable(GL_LIGHTING);
    GLERRORCHECK();
    glDisable(GL_DEPTH_TEST);
    GLERRORCHECK();
    glDisable(GL_CULL_FACE);
    GLERRORCHECK();
    glEnable(GL_BLEND);
    GLERRORCHECK();
    glEnable(GL_TEXTURE_2D);
    GLERRORCHECK();
    glEnableClientState(GL_VERTEX_ARRAY);
    GLERRORCHECK();
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    GLERRORCHECK();

	return true;
}


void GLContextManager::ReleaseFramebuffer() {
	// Unbind framebuffer
	glBindFramebufferANY(GL_FRAMEBUFFER_EXT, 0);
	GLERRORCHECK();

	// Unbind texture just in case
	g_GLDraw.UnbindTexture();

	// Delete framebuffer
	if (mFB) {
		glDeleteFramebuffersANY(1, &mFB);
		GLERRORCHECK();
		mFB = 0;
	}

	// Delete depth renderbuffer
	if (mDepthRB) {
		glDeleteRenderbuffersANY(1, &mDepthRB);
		GLERRORCHECK();
		mDepthRB = 0;
	}

	// Delete texture
	if (mBufTex.name) {
		glDeleteTextures(1, &mBufTex.name);
		GLERRORCHECK();
		mBufTex.name = 0;
	}
}

void GLContextManager::ReleaseContext() {
	// Delete Context
	if (hCTX) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hCTX);
		hCTX = NULL;
	}

	// Restore pixel format if necessary
	if (mOldPixelFormat != GetPixelFormat(hDC)) {
		PIXELFORMATDESCRIPTOR pfd;
		DescribePixelFormat(hDC, mOldPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		SetPixelFormat(hDC, mOldPixelFormat, &pfd);
	}

	// Clear hDC
	hDC = NULL;
}