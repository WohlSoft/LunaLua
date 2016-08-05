#pragma once

#include <gl/glew.h>
#include <string>

class GLShaderVariableInfo
{
public:
    enum VariableType {
        Attribute,
        Uniform
    };
    GLShaderVariableInfo(VariableType varType, GLint id, GLint sizeOfVariable, GLint type, const std::string& name);
    ~GLShaderVariableInfo();

    VariableType getVarType() const;
    GLint getId() const;
    GLint getSizeOfVariable() const;
    GLenum getType() const;
    std::string getName() const;


private:
    VariableType m_varType;
    GLint m_id;
    GLint m_sizeOfVariable;
    GLenum m_type;
    std::string m_name;
};

