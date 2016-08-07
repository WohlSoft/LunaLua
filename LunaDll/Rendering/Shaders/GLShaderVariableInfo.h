#pragma once

#include <gl/glew.h>
#include <string>
#include "GLShaderVariableType.h"

class GLShaderVariableInfo
{
public:
    enum VariableType {
        Attribute,
        Uniform
    };
    GLShaderVariableInfo(GLShaderVariableType varType, GLint id, GLint sizeOfVariable, GLint type, const std::string& name);
    ~GLShaderVariableInfo();

    GLShaderVariableType getVarType() const;
    GLint getId() const;
    GLint getSizeOfVariable() const;
    GLenum getType() const;
    std::string getName() const;


private:
    GLShaderVariableType m_varType;
    GLint m_id;
    GLint m_sizeOfVariable;
    GLenum m_type;
    std::string m_name;
};

