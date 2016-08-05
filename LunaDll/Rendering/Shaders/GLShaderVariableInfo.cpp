#include "GLShaderVariableInfo.h"


GLShaderVariableInfo::GLShaderVariableInfo(VariableType varType, GLint id, GLint sizeOfVariable, GLint type, const std::string& name) :
    m_varType(varType),
    m_id(id),
    m_sizeOfVariable(sizeOfVariable),
    m_type(type),
    m_name(name)
{}

GLShaderVariableInfo::~GLShaderVariableInfo()
{}

GLShaderVariableInfo::VariableType GLShaderVariableInfo::getVarType() const
{
    return m_varType;
}

GLint GLShaderVariableInfo::getId() const
{
    return m_id;
}

GLint GLShaderVariableInfo::getSizeOfVariable() const
{
    return m_sizeOfVariable;
}

GLenum GLShaderVariableInfo::getType() const
{
    return m_type;
}

std::string GLShaderVariableInfo::getName() const
{
    return m_name;
}

