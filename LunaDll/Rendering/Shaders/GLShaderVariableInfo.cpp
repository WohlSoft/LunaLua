#include "GLShaderVariableInfo.h"

#include <regex>

GLShaderVariableInfo::GLShaderVariableInfo(GLShaderVariableType varType, GLint id, GLint arrayCount, GLint type, const std::string& name) :
    m_varType(varType),
    m_id(id),
    m_arrayCount(arrayCount),
    m_type(type),
    m_name(name),
    m_rawName(name),
    m_arrayDepth(0u)
{
    // Calculate array depth
    static std::regex indexMatcher("\\[\\d*\\]", std::regex_constants::ECMAScript);
    m_arrayDepth = std::distance(std::sregex_iterator(name.begin(), name.end(), indexMatcher), std::sregex_iterator());

    if (m_arrayDepth < 1) {
        m_rawName = m_name;
    } else {
        m_rawName = m_name.substr(0, m_name.find('['));
    }
}

GLShaderVariableInfo::~GLShaderVariableInfo()
{}

GLShaderVariableType GLShaderVariableInfo::getVarType() const
{
    return m_varType;
}

GLint GLShaderVariableInfo::getId() const
{
    return m_id;
}

GLint GLShaderVariableInfo::arrayCount() const
{
    return m_arrayCount;
}

GLenum GLShaderVariableInfo::getType() const
{
    return m_type;
}

std::string GLShaderVariableInfo::getName() const
{
    return m_name;
}

const char* GLShaderVariableInfo::getNamePtr() const
{
    return m_name.c_str();
}

std::string GLShaderVariableInfo::getRawName() const
{
    return m_rawName;
}

const char* GLShaderVariableInfo::getRawNamePtr() const
{
    return m_rawName.c_str();
}

size_t GLShaderVariableInfo::getArrayDepth() const
{
    return m_arrayDepth;
}

