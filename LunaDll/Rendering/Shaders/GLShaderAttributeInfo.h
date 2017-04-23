#pragma once

#include "GLShaderVariableInfo.h"

class GLShaderAttributeInfo : public GLShaderVariableInfo
{
public:
    GLShaderAttributeInfo(gl::GLint id, gl::GLint sizeOfVariable, gl::GLenum type, const std::string& name);
    ~GLShaderAttributeInfo();
};

