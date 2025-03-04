#include <windows.h>
#include <GL/glew.h>
#include "../../Defines.h"
#include "../../Globals.h"
#include "../WindowSizeHandler.h"
#include "GLContextManager.h"
#include "GLCompat.h"

// Instance
GLContextManager g_GLContextManager;

// Constructor
GLContextManager::GLContextManager() :
    hDC(nullptr), hQueueThreadCTX(nullptr), hMainThreadCTX(nullptr),
    mInitialized(false), mHadError(false), mMainThreadCTXApplied(false),
    mMainFBWidth(800), mMainFBHeight(600),
    mOldPixelFormat(0), mCurrentFB(nullptr), mPrimaryFB(nullptr),
    mCurrentCameraFB(nullptr), mCurrentCameraIdx(0), mCameraFramebuffers(),
    mActiveRedirects(),
    mConstants()
{
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
        // Fill out constants
        memset(&mConstants, 0, sizeof(GLConstants));
        mConstants.pcVENDOR = (const char*)glGetString(GL_VENDOR);
        mConstants.pcRENDERER = (const char*)glGetString(GL_RENDERER);
        mConstants.pcVERSION = (const char*)glGetString(GL_VERSION);
        mConstants.pcSHADING_LANGUAGE_VERSION = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
        glGetIntegerv(GL_MAJOR_VERSION, &mConstants.iMAJOR_VERSION);
        glGetIntegerv(GL_MINOR_VERSION, &mConstants.iMINOR_VERSION);
        glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &mConstants.iMAX_COMPUTE_SHADER_STORAGE_BLOCKS);
        glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &mConstants.iMAX_COMBINED_SHADER_STORAGE_BLOCKS);
        glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &mConstants.iMAX_COMPUTE_UNIFORM_BLOCKS);
        glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &mConstants.iMAX_COMPUTE_TEXTURE_IMAGE_UNITS);
        glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &mConstants.iMAX_COMPUTE_UNIFORM_COMPONENTS);
        glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &mConstants.iMAX_COMPUTE_ATOMIC_COUNTERS);
        glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &mConstants.iMAX_COMPUTE_ATOMIC_COUNTER_BUFFERS);
        glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, &mConstants.iMAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS);
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &mConstants.iMAX_COMPUTE_WORK_GROUP_INVOCATIONS);
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_COUNT, &mConstants.iMAX_COMPUTE_WORK_GROUP_COUNT);
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_SIZE, &mConstants.iMAX_COMPUTE_WORK_GROUP_SIZE);
        glGetIntegerv(GL_MAX_DEBUG_GROUP_STACK_DEPTH, &mConstants.iMAX_DEBUG_GROUP_STACK_DEPTH);
        glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &mConstants.iMAX_3D_TEXTURE_SIZE);
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &mConstants.iMAX_ARRAY_TEXTURE_LAYERS);
        glGetIntegerv(GL_MAX_CLIP_DISTANCES, &mConstants.iMAX_CLIP_DISTANCES);
        glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &mConstants.iMAX_COLOR_TEXTURE_SAMPLES);
        glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS, &mConstants.iMAX_COMBINED_ATOMIC_COUNTERS);
        glGetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &mConstants.iMAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS);
        glGetIntegerv(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, &mConstants.iMAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS);
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &mConstants.iMAX_COMBINED_TEXTURE_IMAGE_UNITS);
        glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &mConstants.iMAX_COMBINED_UNIFORM_BLOCKS);
        glGetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &mConstants.iMAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
        glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &mConstants.iMAX_CUBE_MAP_TEXTURE_SIZE);
        glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &mConstants.iMAX_DEPTH_TEXTURE_SAMPLES);
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &mConstants.iMAX_DRAW_BUFFERS);
        glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &mConstants.iMAX_DUAL_SOURCE_DRAW_BUFFERS);
        glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &mConstants.iMAX_ELEMENTS_INDICES);
        glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &mConstants.iMAX_ELEMENTS_VERTICES);
        glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS, &mConstants.iMAX_FRAGMENT_ATOMIC_COUNTERS);
        glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &mConstants.iMAX_FRAGMENT_SHADER_STORAGE_BLOCKS);
        glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &mConstants.iMAX_FRAGMENT_INPUT_COMPONENTS);
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &mConstants.iMAX_FRAGMENT_UNIFORM_COMPONENTS);
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &mConstants.iMAX_FRAGMENT_UNIFORM_VECTORS);
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &mConstants.iMAX_FRAGMENT_UNIFORM_BLOCKS);
        glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &mConstants.iMAX_FRAMEBUFFER_WIDTH);
        glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &mConstants.iMAX_FRAMEBUFFER_HEIGHT);
        glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &mConstants.iMAX_FRAMEBUFFER_LAYERS);
        glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &mConstants.iMAX_FRAMEBUFFER_SAMPLES);
        glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTERS, &mConstants.iMAX_GEOMETRY_ATOMIC_COUNTERS);
        glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &mConstants.iMAX_GEOMETRY_SHADER_STORAGE_BLOCKS);
        glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &mConstants.iMAX_GEOMETRY_INPUT_COMPONENTS);
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &mConstants.iMAX_GEOMETRY_OUTPUT_COMPONENTS);
        glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &mConstants.iMAX_GEOMETRY_TEXTURE_IMAGE_UNITS);
        glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &mConstants.iMAX_GEOMETRY_UNIFORM_BLOCKS);
        glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &mConstants.iMAX_GEOMETRY_UNIFORM_COMPONENTS);
        glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &mConstants.iMAX_INTEGER_SAMPLES);
        glGetIntegerv(GL_MAX_LABEL_LENGTH, &mConstants.iMAX_LABEL_LENGTH);
        glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &mConstants.iMAX_PROGRAM_TEXEL_OFFSET);
        glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &mConstants.iMAX_RECTANGLE_TEXTURE_SIZE);
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &mConstants.iMAX_RENDERBUFFER_SIZE);
        glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &mConstants.iMAX_SAMPLE_MASK_WORDS);
        glGetIntegerv(GL_MAX_SERVER_WAIT_TIMEOUT, &mConstants.iMAX_SERVER_WAIT_TIMEOUT);
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &mConstants.iMAX_SHADER_STORAGE_BUFFER_BINDINGS);
        glGetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS, &mConstants.iMAX_TESS_CONTROL_ATOMIC_COUNTERS);
        glGetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS, &mConstants.iMAX_TESS_EVALUATION_ATOMIC_COUNTERS);
        glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, &mConstants.iMAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS);
        glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &mConstants.iMAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS);
        glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &mConstants.iMAX_TEXTURE_BUFFER_SIZE);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mConstants.iMAX_TEXTURE_IMAGE_UNITS);
        glGetIntegerv(GL_MAX_TEXTURE_LOD_BIAS, &mConstants.iMAX_TEXTURE_LOD_BIAS);
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mConstants.iMAX_TEXTURE_SIZE);
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &mConstants.iMAX_UNIFORM_BUFFER_BINDINGS);
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &mConstants.iMAX_UNIFORM_BLOCK_SIZE);
        glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &mConstants.iMAX_UNIFORM_LOCATIONS);
        glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &mConstants.iMAX_VARYING_COMPONENTS);
        glGetIntegerv(GL_MAX_VARYING_VECTORS, &mConstants.iMAX_VARYING_VECTORS);
        glGetIntegerv(GL_MAX_VARYING_FLOATS, &mConstants.iMAX_VARYING_FLOATS);
        glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS, &mConstants.iMAX_VERTEX_ATOMIC_COUNTERS);
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &mConstants.iMAX_VERTEX_ATTRIBS);
        glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &mConstants.iMAX_VERTEX_SHADER_STORAGE_BLOCKS);
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &mConstants.iMAX_VERTEX_TEXTURE_IMAGE_UNITS);
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &mConstants.iMAX_VERTEX_UNIFORM_COMPONENTS);
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &mConstants.iMAX_VERTEX_UNIFORM_VECTORS);
        glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &mConstants.iMAX_VERTEX_OUTPUT_COMPONENTS);
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &mConstants.iMAX_VERTEX_UNIFORM_BLOCKS);
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &mConstants.iMAX_VIEWPORT_DIMS);
        glGetIntegerv(GL_MAX_VIEWPORTS, &mConstants.iMAX_VIEWPORTS);
        glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &mConstants.iMAX_VERTEX_ATTRIB_RELATIVE_OFFSET);
        glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &mConstants.iMAX_VERTEX_ATTRIB_BINDINGS);
        glGetIntegerv(GL_MAX_ELEMENT_INDEX, &mConstants.iMAX_ELEMENT_INDEX);
        while (glGetError() != GL_NO_ERROR) {}

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
    g_GLContextManager.SetCurrentFB(nullptr);
    GLERRORCHECK();
}

void GLContextManager::BindPrimaryFB() {
    if (mPrimaryFB == nullptr) return;

    // Bind the primary framebuffer
    mPrimaryFB->Bind();
}

void GLContextManager::BindCameraFB() {
    if (mCurrentCameraFB == nullptr) return;

    // Bind the current camera framebuffer
    mCurrentCameraFB->Bind();
}

void GLContextManager::EnsureMainThreadCTXApplied()
{
    if(!mMainThreadCTXApplied)
    {
        wglMakeCurrent(hDC, hMainThreadCTX);
        mMainThreadCTXApplied = true;
    }
}

void GLContextManager::SetActiveCamera(int cameraIdx)
{
    if ((cameraIdx < 0) || (cameraIdx > MAX_CAMERAS))
    {
        // Invalid index
        return;
    }

    bool isCameraBufferBound = (mCurrentCameraFB == mCurrentFB);
    if (mCurrentCameraIdx != cameraIdx)
    {
        mCurrentCameraIdx = cameraIdx;
        mCurrentCameraFB = mCameraFramebuffers[cameraIdx];
        mActiveRedirects.clear();
        if (isCameraBufferBound)
        {
            BindCameraFB();
        }
    }
}

void GLContextManager::RedirectCameraFB(const GLEngineCmd_RedirectCameraFB* cmd)
{
    if (cmd == nullptr)
    {
        // If cmd is nullptr, interpret this as removing the current redirect
        UnRedirectCameraFB(nullptr);
        return;
    }

    GLFramebuffer* target = cmd->getFB();

    // Make note of if the currently set camera FB was already bound
    bool isCameraBufferBound = (mCurrentCameraFB == mCurrentFB);

    // If the target is null, treat that as setting back to the default
    if ((target == nullptr) && (mCurrentCameraIdx >= 0) && (mCurrentCameraIdx <= MAX_CAMERAS))
    {
        target = mCameraFramebuffers[mCurrentCameraIdx];
    }

    if (mCurrentCameraFB != target)
    {
        // Set camera FB
        mCurrentCameraFB = target;

        // If the camera buffer was bound, bind the newly set buffer
        if (isCameraBufferBound)
        {
            BindCameraFB();
        }
    }

    // Append to the stack of active redirects
    // Note: We use pointers to the original redirect command both as a 'handle' for the redirect
    //       that we can compare, as well as storing a reference to a framebuffer we might need to
    //       switch back if the redirects become nested.
    mActiveRedirects.push_back(cmd);
}

void GLContextManager::UnRedirectCameraFB(const GLEngineCmd_RedirectCameraFB* startCmd)
{
    if (startCmd == nullptr)
    {
        // If startCmd is nullptr, interpret this as removing the current redirect
        if (mActiveRedirects.empty())
        {
            // Abort if there's no redirect
            return;
        }
        startCmd = mActiveRedirects.back();
    }

    if ((!mActiveRedirects.empty()) && (mActiveRedirects.back() == startCmd))
    {
        // For the case that we're undoing the redirect that is active
        mActiveRedirects.pop_back();

        // Make note of if the currently set camera FB was already bound
        bool isCameraBufferBound = (mCurrentCameraFB == mCurrentFB);

        if (mActiveRedirects.size() > 0)
        {
            // If there was some other redirect active, switch back to that
            mCurrentCameraFB = mActiveRedirects.back()->getFB();
        }
        else if ((mCurrentCameraIdx >= 0) && (mCurrentCameraIdx <= MAX_CAMERAS))
        {
            // Otherwise, switch back to the normal camera buffer
            mCurrentCameraFB = mCameraFramebuffers[mCurrentCameraIdx];
        }
        else
        {
            // If the camera index is invalid?
            mCurrentCameraFB = nullptr;
        }

        // If the camera buffer was bound, bind the newly set buffer
        if (isCameraBufferBound)
        {
            BindCameraFB();
        }
    }
    else
    {
        // We're ending some other redirect that wasn't active... meaning just remove it from the list
        mActiveRedirects.erase(std::remove(mActiveRedirects.begin(), mActiveRedirects.end(), startCmd), mActiveRedirects.end());
    }
}

void GLContextManager::RestoreBoundFB(GLFramebuffer* fb)
{
    if (fb)
    {
        // A framebuffer object was bound, restore it
        fb->Bind();
    }
    else
    {
        // fb was null, meaning the screen was bound, restore that instead
        g_GLContextManager.BindScreen();
    }
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

    // TODO: Set queueThreadCTX and mainThreadCTX
    HGLRC queueThreadTempCTX = wglCreateContext(hDC);
    HGLRC mainThreadTempCTX = wglCreateContext(hDC);
    if (NULL == queueThreadTempCTX || NULL == mainThreadTempCTX)
        return false;

    bResult = wglShareLists(queueThreadTempCTX, mainThreadTempCTX);
    if (FALSE == bResult)
    {
        wglDeleteContext(queueThreadTempCTX);
        wglDeleteContext(mainThreadTempCTX);
        return false;
    }

    bResult = wglMakeCurrent(hDC, queueThreadTempCTX);
    if (FALSE == bResult)
    {
        wglDeleteContext(queueThreadTempCTX);
        return false;
    }

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(queueThreadTempCTX);
        return false;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0.0f, mMainFBWidth, 0.0f, mMainFBHeight, -100.0f, 100.0f);
    glColor3f(1, 1, 1);
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    // Prefer v-sync off (but will work fine if forced on too)
    typedef BOOL(APIENTRY * PFNWGLSWAPINTERVALEXTPROC)(int);
    static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = 0;
    wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
    if (wglSwapIntervalEXT != 0) {
        switch (gGeneralConfig.getRendererVSync())
        {
            case GeneralLunaConfig::VSyncModeOff:      wglSwapIntervalEXT(0); break;
            case GeneralLunaConfig::VSyncModeOn:       wglSwapIntervalEXT(1); break;
            case GeneralLunaConfig::VSyncModeAdaptive: wglSwapIntervalEXT(-1); break;
        }
    }

    this->hDC = hDC;
    this->hQueueThreadCTX = queueThreadTempCTX;
    this->hMainThreadCTX = mainThreadTempCTX;
    return true;
}

bool GLContextManager::InitFramebuffer() {
    try
    {
        mPrimaryFB = new GLFramebuffer(mMainFBWidth, mMainFBHeight, false);
        mCameraFramebuffers[0] = mPrimaryFB;
        for (int i = 1; i <= MAX_CAMERAS; i++)
        {
            mCameraFramebuffers[i] = new GLFramebuffer(mMainFBWidth, mMainFBHeight, false);
        }
        mCurrentCameraFB = mCameraFramebuffers[mCurrentCameraIdx];
    }
    catch (...)
    {
        ReleaseFramebuffer();
        return false;
    }
    return true;
}

bool GLContextManager::InitProjectionAndState() {
    // Set projection
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLERRORCHECK();
    glOrtho(0.0f, (float)mMainFBWidth, 0.0f, (float)mMainFBHeight, -1.0f, 1.0f);
    GLERRORCHECK();
    glColor3f(1, 1, 1);
    GLERRORCHECK();
    glDisable(GL_LIGHTING);
    GLERRORCHECK();
    glEnable(GL_DEPTH_TEST);
    GLERRORCHECK();
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
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
    if (mPrimaryFB) delete mPrimaryFB;
    mPrimaryFB = nullptr;
    for (int i = 1; i <= MAX_CAMERAS; i++)
    {
        if (mCameraFramebuffers[i])
        {
            if (mCameraFramebuffers[i] == mCurrentCameraFB)
            {
                mCurrentCameraFB = nullptr;
            }
            delete mCameraFramebuffers[i];
            mCameraFramebuffers[i] = nullptr;
        }
    }
}

void GLContextManager::ReleaseContext() {
    // Delete Context
    if (hQueueThreadCTX) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(hQueueThreadCTX);
        hQueueThreadCTX = nullptr;
    }

    // Restore pixel format if necessary
    if (mOldPixelFormat != GetPixelFormat(hDC)) {
        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat(hDC, mOldPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
        SetPixelFormat(hDC, mOldPixelFormat, &pfd);
    }

    // Clear hDC
    hDC = nullptr;
}

void GLContextManager::SetMainFramebufferSize(int width, int height)
{
    mMainFBWidth = width;
    mMainFBHeight = height;
    gWindowSizeHandler.Recalculate(); // Recalculate framebuffer position in window
    if (!mInitialized) return;

    // Adjust screen projection
    BindScreen();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0.0f, mMainFBWidth, 0.0f, mMainFBHeight, -100.0f, 100.0f);

    // Delete and re-init framebuffer
    ReleaseFramebuffer();
    InitFramebuffer();

    // Bind camera framebuffer and clear
    g_GLContextManager.BindCameraFB();
    GLFramebuffer* fb = g_GLContextManager.GetCurrentFB();
    if (fb) fb->Clear();

    // Re-init some state just in case
    InitProjectionAndState();
}
