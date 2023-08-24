#ifndef GLContextManager_hhhh
#define GLContextManager_hhhh

#include <windows.h>
#include <GL/glew.h>
#include "GLTextureStore.h"
#include "GLFramebuffer.h"

class GLEngineCmd_RedirectCameraFB;

class GLContextManager {
public:
    struct GLConstants
    {
        const char* pcVENDOR;
        const char* pcRENDERER;
        const char* pcVERSION;
        const char* pcSHADING_LANGUAGE_VERSION;
        int iMAJOR_VERSION;
        int iMINOR_VERSION;
        int iMAX_COMPUTE_SHADER_STORAGE_BLOCKS;
        int iMAX_COMBINED_SHADER_STORAGE_BLOCKS;
        int iMAX_COMPUTE_UNIFORM_BLOCKS;
        int iMAX_COMPUTE_TEXTURE_IMAGE_UNITS;
        int iMAX_COMPUTE_UNIFORM_COMPONENTS;
        int iMAX_COMPUTE_ATOMIC_COUNTERS;
        int iMAX_COMPUTE_ATOMIC_COUNTER_BUFFERS;
        int iMAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS;
        int iMAX_COMPUTE_WORK_GROUP_INVOCATIONS;
        int iMAX_COMPUTE_WORK_GROUP_COUNT;
        int iMAX_COMPUTE_WORK_GROUP_SIZE;
        int iMAX_DEBUG_GROUP_STACK_DEPTH;
        int iMAX_3D_TEXTURE_SIZE;
        int iMAX_ARRAY_TEXTURE_LAYERS;
        int iMAX_CLIP_DISTANCES;
        int iMAX_COLOR_TEXTURE_SAMPLES;
        int iMAX_COMBINED_ATOMIC_COUNTERS;
        int iMAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS;
        int iMAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS;
        int iMAX_COMBINED_TEXTURE_IMAGE_UNITS;
        int iMAX_COMBINED_UNIFORM_BLOCKS;
        int iMAX_COMBINED_VERTEX_UNIFORM_COMPONENTS;
        int iMAX_CUBE_MAP_TEXTURE_SIZE;
        int iMAX_DEPTH_TEXTURE_SAMPLES;
        int iMAX_DRAW_BUFFERS;
        int iMAX_DUAL_SOURCE_DRAW_BUFFERS;
        int iMAX_ELEMENTS_INDICES;
        int iMAX_ELEMENTS_VERTICES;
        int iMAX_FRAGMENT_ATOMIC_COUNTERS;
        int iMAX_FRAGMENT_SHADER_STORAGE_BLOCKS;
        int iMAX_FRAGMENT_INPUT_COMPONENTS;
        int iMAX_FRAGMENT_UNIFORM_COMPONENTS;
        int iMAX_FRAGMENT_UNIFORM_VECTORS;
        int iMAX_FRAGMENT_UNIFORM_BLOCKS;
        int iMAX_FRAMEBUFFER_WIDTH;
        int iMAX_FRAMEBUFFER_HEIGHT;
        int iMAX_FRAMEBUFFER_LAYERS;
        int iMAX_FRAMEBUFFER_SAMPLES;
        int iMAX_GEOMETRY_ATOMIC_COUNTERS;
        int iMAX_GEOMETRY_SHADER_STORAGE_BLOCKS;
        int iMAX_GEOMETRY_INPUT_COMPONENTS;
        int iMAX_GEOMETRY_OUTPUT_COMPONENTS;
        int iMAX_GEOMETRY_TEXTURE_IMAGE_UNITS;
        int iMAX_GEOMETRY_UNIFORM_BLOCKS;
        int iMAX_GEOMETRY_UNIFORM_COMPONENTS;
        int iMAX_INTEGER_SAMPLES;
        int iMAX_LABEL_LENGTH;
        int iMAX_PROGRAM_TEXEL_OFFSET;
        int iMAX_RECTANGLE_TEXTURE_SIZE;
        int iMAX_RENDERBUFFER_SIZE;
        int iMAX_SAMPLE_MASK_WORDS;
        int iMAX_SERVER_WAIT_TIMEOUT;
        int iMAX_SHADER_STORAGE_BUFFER_BINDINGS;
        int iMAX_TESS_CONTROL_ATOMIC_COUNTERS;
        int iMAX_TESS_EVALUATION_ATOMIC_COUNTERS;
        int iMAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS;
        int iMAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS;
        int iMAX_TEXTURE_BUFFER_SIZE;
        int iMAX_TEXTURE_IMAGE_UNITS;
        int iMAX_TEXTURE_LOD_BIAS;
        int iMAX_TEXTURE_SIZE;
        int iMAX_UNIFORM_BUFFER_BINDINGS;
        int iMAX_UNIFORM_BLOCK_SIZE;
        int iMAX_UNIFORM_LOCATIONS;
        int iMAX_VARYING_COMPONENTS;
        int iMAX_VARYING_VECTORS;
        int iMAX_VARYING_FLOATS;
        int iMAX_VERTEX_ATOMIC_COUNTERS;
        int iMAX_VERTEX_ATTRIBS;
        int iMAX_VERTEX_SHADER_STORAGE_BLOCKS;
        int iMAX_VERTEX_TEXTURE_IMAGE_UNITS;
        int iMAX_VERTEX_UNIFORM_COMPONENTS;
        int iMAX_VERTEX_UNIFORM_VECTORS;
        int iMAX_VERTEX_OUTPUT_COMPONENTS;
        int iMAX_VERTEX_UNIFORM_BLOCKS;
        int iMAX_VIEWPORT_DIMS;
        int iMAX_VIEWPORTS;
        int iMAX_VERTEX_ATTRIB_RELATIVE_OFFSET;
        int iMAX_VERTEX_ATTRIB_BINDINGS;
        int iMAX_ELEMENT_INDEX;
    };
public:
    static const int MAX_CAMERAS = 2;

    GLContextManager();
    bool Init(HDC hDC);
    bool IsInitialized();

    void BindScreen();
    void BindPrimaryFB();
    void BindCameraFB();
    void RedirectCameraFB(const GLEngineCmd_RedirectCameraFB* cmd);
    void UnRedirectCameraFB(const GLEngineCmd_RedirectCameraFB* startCmd);
    inline const GLDraw::Texture& GetPrimaryFBTex()
    {
        static const GLDraw::Texture nullTex(0, 0, 0);
        if (mPrimaryFB == nullptr)
        {
            return nullTex;
        }
        return mPrimaryFB->AsTexture();
    }
    inline const GLDraw::Texture& GetCurrentCameraFBTex()
    {
        static const GLDraw::Texture nullTex(0, 0, 0);
        if (mCurrentCameraFB == nullptr)
        {
            return nullTex;
        }
        return mCurrentCameraFB->AsTexture();
    }
    inline const GLDraw::Texture& GetCameraFBTex(int cameraIdx)
    {
        static const GLDraw::Texture nullTex(0, 0, 0);
        if ((cameraIdx < 0) || (cameraIdx > MAX_CAMERAS) || (mCameraFramebuffers[cameraIdx] == nullptr))
        {
            return nullTex;
        }
        return mCameraFramebuffers[cameraIdx]->AsTexture();
    }
    inline GLFramebuffer* GetCurrentFB() { return mCurrentFB; }
    inline GLFramebuffer* GetCurrentCameraFB() { return mCurrentCameraFB; }
    inline void SetCurrentFB(GLFramebuffer* fb) { mCurrentFB = fb; }
    void EnsureMainThreadCTXApplied();
    inline const GLConstants& Constants() { return mConstants; }

    void SetActiveCamera(int cameraIdx);

    // Function to set the bound framebuffer based on what GetCurrentFB returned.
    // This is significant because if it returned null, this should bind the screen.
    void RestoreBoundFB(GLFramebuffer* fb);

private:
    bool  mInitialized;
    bool  mHadError;
    bool  mMainThreadCTXApplied;  // Whether wglMakeCurrent has been called for the main thread or not
    int   mMainFBWidth;
    int   mMainFBHeight;

    // Context variables
    int   mOldPixelFormat;
    HDC   hDC;
    HGLRC hQueueThreadCTX;
    HGLRC hMainThreadCTX;

    // Framebuffer variables
    GLFramebuffer* mCurrentFB;
    GLFramebuffer* mPrimaryFB;
    GLFramebuffer* mCurrentCameraFB;
    int            mCurrentCameraIdx;
    GLFramebuffer* mCameraFramebuffers[MAX_CAMERAS+1];
    std::vector<const GLEngineCmd_RedirectCameraFB*> mActiveRedirects;

    // Constants list
    GLConstants mConstants;

    // Init functions
    bool InitContextFromHDC(HDC hDC);
    bool InitFramebuffer();
    bool InitProjectionAndState();

    // Release functions
    void ReleaseContext();
    void ReleaseFramebuffer();

public:
    // Framebuffer config
    void SetMainFramebufferSize(int width, int height);

    inline int GetMainFBWidth() { return mMainFBWidth; }
    inline int GetMainFBHeight() { return mMainFBHeight; }
};

// Instance
extern GLContextManager g_GLContextManager;

#endif
