#pragma once

#include "GLShaderVariableInfo.h"

class GLShaderAttributeInfo : public GLShaderVariableInfo
{
public:
    GLShaderAttributeInfo(GLint id, GLint sizeOfVariable, GLint type, const std::string& name);
    ~GLShaderAttributeInfo();
};

