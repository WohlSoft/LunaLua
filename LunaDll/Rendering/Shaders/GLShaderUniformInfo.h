#pragma once

#include "GLShaderVariableInfo.h"

class GLShaderUniformInfo : public GLShaderVariableInfo
{
public:
    GLShaderUniformInfo(gl::GLint id, gl::GLint sizeOfVariable, gl::GLenum type, const std::string& name);
    ~GLShaderUniformInfo();
};

