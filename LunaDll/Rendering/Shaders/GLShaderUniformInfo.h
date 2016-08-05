#pragma once

#include "GLShaderVariableInfo.h"

class GLShaderUniformInfo : public GLShaderVariableInfo
{
public:
    GLShaderUniformInfo(GLint id, GLint sizeOfVariable, GLint type, const std::string& name);
    ~GLShaderUniformInfo();
};

