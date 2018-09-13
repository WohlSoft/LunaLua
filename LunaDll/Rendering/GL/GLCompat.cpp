#include "GLCompat.h"
#include <glbinding/Meta.h>
#include <glbinding/Binding.h>
#include <glbinding/Version.h>
#include <glbinding/ContextInfo.h>
#include <glbinding/gl/extension.h>

thread_local void(*glcompat::glGenFramebuffersANY)(gl::GLsizei n, gl::GLuint *ids) = nullptr;
thread_local void(*glcompat::glBindFramebufferANY)(gl::GLenum target, gl::GLuint framebuffer) = nullptr;
thread_local void(*glcompat::glGenRenderbuffersANY)(gl::GLsizei n, gl::GLuint *renderbuffers) = nullptr;
thread_local void(*glcompat::glBindRenderbufferANY)(gl::GLenum target, gl::GLuint renderbuffer) = nullptr;
thread_local void(*glcompat::glRenderbufferStorageANY)(gl::GLenum target, gl::GLenum internalformat, gl::GLsizei width, gl::GLsizei height) = nullptr;
thread_local void(*glcompat::glFramebufferRenderbufferANY)(gl::GLenum target, gl::GLenum attachment, gl::GLenum renderbuffertarget, gl::GLuint renderbuffer) = nullptr;
thread_local void(*glcompat::glFramebufferTexture2DANY)(gl::GLenum target, gl::GLenum attachment, gl::GLenum textarget, gl::GLuint texture, gl::GLint level) = nullptr;
thread_local gl::GLenum(*glcompat::glCheckFramebufferStatusANY)(gl::GLenum target) = nullptr;
thread_local void(*glcompat::glDeleteFramebuffersANY)(gl::GLsizei n, const gl::GLuint *framebuffers) = nullptr;
thread_local void(*glcompat::glDeleteRenderbuffersANY)(gl::GLsizei n, const gl::GLuint *renderbuffers) = nullptr;
thread_local void(*glcompat::glBlendFuncSeparateANY)(gl::GLenum srcRGB, gl::GLenum dstRGB, gl::GLenum srcAlpha, gl::GLenum dstAlpha) = nullptr;
thread_local void(*glcompat::glBlendEquationANY)(gl::GLenum mode) = nullptr;

bool glcompat::SetupContext()
{
    auto ext = glbinding::ContextInfo::extensions();
    
    bool have_VERSION_1_4 = glbinding::ContextInfo::supported(glbinding::Version(1, 4));
    bool have_VERSION_3_0 = glbinding::ContextInfo::supported(glbinding::Version(3, 0));

    bool have_ARB_blend_minmax        = have_VERSION_1_4;
    bool have_EXT_blend_minmax        = (ext.find(gl::GLextension::GL_EXT_blend_minmax) != ext.end());

    bool have_ARB_blend_func_separate = have_VERSION_1_4;
    bool have_EXT_blend_func_separate = (ext.find(gl::GLextension::GL_EXT_blend_func_separate) != ext.end());

    bool have_ARB_framebuffer_object  = have_VERSION_3_0 || (ext.find(gl::GLextension::GL_ARB_framebuffer_object) != ext.end());
    bool have_EXT_framebuffer_object  = (ext.find(gl::GLextension::GL_EXT_framebuffer_object) != ext.end());

    bool success = true;

    // GL_VERSION_3_0, GL_ARB_framebuffer_object or GL_EXT_framebuffer_object
    if (have_ARB_framebuffer_object)
    {
        glGenFramebuffersANY         = gl::glGenFramebuffers;
        glBindFramebufferANY         = gl::glBindFramebuffer;
        glGenRenderbuffersANY        = gl::glGenRenderbuffers;
        glBindRenderbufferANY        = gl::glBindRenderbuffer;
        glRenderbufferStorageANY     = gl::glRenderbufferStorage;
        glFramebufferRenderbufferANY = gl::glFramebufferRenderbuffer;
        glFramebufferTexture2DANY    = gl::glFramebufferTexture2D;
        glCheckFramebufferStatusANY  = gl::glCheckFramebufferStatus;
        glDeleteFramebuffersANY      = gl::glDeleteFramebuffers;
        glDeleteRenderbuffersANY     = gl::glDeleteRenderbuffers;
    }
    else if (have_EXT_framebuffer_object)
    {
        glGenFramebuffersANY         = gl::glGenFramebuffersEXT;
        glBindFramebufferANY         = gl::glBindFramebufferEXT;
        glGenRenderbuffersANY        = gl::glGenRenderbuffersEXT;
        glBindRenderbufferANY        = gl::glBindRenderbufferEXT;
        glRenderbufferStorageANY     = gl::glRenderbufferStorageEXT;
        glFramebufferRenderbufferANY = gl::glFramebufferRenderbufferEXT;
        glFramebufferTexture2DANY    = gl::glFramebufferTexture2DEXT;
        glCheckFramebufferStatusANY  = gl::glCheckFramebufferStatusEXT;
        glDeleteFramebuffersANY      = gl::glDeleteFramebuffersEXT;
        glDeleteRenderbuffersANY     = gl::glDeleteRenderbuffersEXT;
    }
    else
    {
        glGenFramebuffersANY         = nullptr;
        glBindFramebufferANY         = nullptr;
        glGenRenderbuffersANY        = nullptr;
        glBindRenderbufferANY        = nullptr;
        glRenderbufferStorageANY     = nullptr;
        glFramebufferRenderbufferANY = nullptr;
        glFramebufferTexture2DANY    = nullptr;
        glCheckFramebufferStatusANY  = nullptr;
        glDeleteFramebuffersANY      = nullptr;
        glDeleteRenderbuffersANY     = nullptr;
        success = false;
    }

    // GL_VERSION_1_4 or GL_EXT_blend_func_separate
    if (have_ARB_blend_func_separate)
    {
        glBlendFuncSeparateANY = gl::glBlendFuncSeparate;
    }
    else if (have_EXT_blend_minmax)
    {
        glBlendFuncSeparateANY = gl::glBlendFuncSeparateEXT;
    }
    else
    {
        glBlendFuncSeparateANY = nullptr;
        success = false;
    }
    
    // GL_VERSION_1_4 or GL_EXT_blend_minmax
    if (have_ARB_blend_minmax)
    {
        glBlendEquationANY = gl::glBlendEquation;
    }
    else if (have_EXT_blend_func_separate)
    {
        glBlendEquationANY = gl::glBlendEquationEXT;
    }
    else
    {
        glBlendEquationANY = nullptr;
        success = false;
    }

    return success;
}
