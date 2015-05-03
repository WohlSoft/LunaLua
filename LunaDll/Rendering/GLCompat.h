#ifndef GLCompat_hhhh
#define GLCompat_hhhh

#include <gl/glew.h>

// Core in >=3.0, or ARB_framebuffer_object or EXT_framebuffer_object
static inline void glGenFramebuffersANY(GLsizei n, GLuint *ids) {
    if (GLEW_VERSION_3_0 || GLEW_ARB_framebuffer_object) {
        glGenFramebuffers(n, ids);
    } else if (GLEW_EXT_framebuffer_object) {
        glGenFramebuffersEXT(n, ids);
    }
}
static inline void glBindFramebufferANY(GLenum target, GLuint framebuffer) {
    if (GLEW_VERSION_3_0 || GLEW_ARB_framebuffer_object) {
        glBindFramebuffer(target, framebuffer);
    }
    else if (GLEW_EXT_framebuffer_object) {
        glBindFramebufferEXT(target, framebuffer);
    }
}
static inline void glGenRenderbuffersANY(GLsizei n, GLuint *renderbuffers) {
    if (GLEW_VERSION_3_0 || GLEW_ARB_framebuffer_object) {
        glGenRenderbuffers(n, renderbuffers);
    }
    else if (GLEW_EXT_framebuffer_object) {
        glGenRenderbuffersEXT(n, renderbuffers);
    }
}
static inline void glBindRenderbufferANY(GLenum target, GLuint renderbuffer) {
    if (GLEW_VERSION_3_0 || GLEW_ARB_framebuffer_object) {
        glBindRenderbuffer(target, renderbuffer);
    }
    else if (GLEW_EXT_framebuffer_object) {
        glBindRenderbufferEXT(target, renderbuffer);
    }
}
static inline void glRenderbufferStorageANY(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
    if (GLEW_VERSION_3_0 || GLEW_ARB_framebuffer_object) {
        glRenderbufferStorage(target, internalformat, width, height);
    }
    else if (GLEW_EXT_framebuffer_object) {
        glRenderbufferStorageEXT(target, internalformat, width, height);
    }
}
static inline void glFramebufferRenderbufferANY(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
    if (GLEW_VERSION_3_0 || GLEW_ARB_framebuffer_object) {
        glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
    }
    else if (GLEW_EXT_framebuffer_object) {
        glFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
    }
}
static inline void glFramebufferTexture2DANY(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
    if (GLEW_VERSION_3_0 || GLEW_ARB_framebuffer_object) {
        glFramebufferTexture2D(target, attachment, textarget, texture, level);
    }
    else if (GLEW_EXT_framebuffer_object) {
        glFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
    }
}
static inline GLenum glCheckFramebufferStatusANY(GLenum target) {
    if (GLEW_VERSION_3_0 || GLEW_ARB_framebuffer_object) {
        return glCheckFramebufferStatus(target);
    }
    else if (GLEW_EXT_framebuffer_object) {
        return glCheckFramebufferStatusEXT(target);
    }
    return GL_FRAMEBUFFER_UNSUPPORTED_EXT;
}

// Core in OpenGL >=1.4
static inline void glBlendFuncSeparateANY(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    if (GLEW_VERSION_1_4) {
        glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    }
    else if (GLEW_EXT_blend_func_separate) {
        glBlendFuncSeparateEXT(srcRGB, dstRGB, srcAlpha, dstAlpha);
    }
}
static inline void glBlendEquationANY(GLenum mode)
{
    if (GLEW_VERSION_1_4) {
        glBlendEquation(mode);
    }
    else if (GLEW_EXT_blend_minmax) {
        glBlendEquationEXT(mode);
    }
}

#endif
