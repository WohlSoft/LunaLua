#include "GLShaderVariableEntry.h"

#include <cstdlib>
#include <utility>

using namespace gl;

GLShaderVariableEntry::GLShaderVariableEntry(GLShaderVariableType type, GLuint location, GLenum typeData, size_t count, void* data) : 
    m_type(type),
    m_location(location),
    m_typeData(typeData),
    m_count(count),
    m_data(data)
{}

GLShaderVariableEntry::GLShaderVariableEntry(GLShaderVariableEntry&& other) noexcept
{
    m_type = other.m_type;
    m_location = other.m_location;
    m_typeData = other.m_typeData;
    m_count = other.m_count;
    m_data = other.m_data;

    other.m_data = nullptr;
}

GLShaderVariableEntry::~GLShaderVariableEntry()
{
    if (m_data)
        std::free(m_data);
}

GLShaderVariableType GLShaderVariableEntry::getVariableType() const
{
    return m_type;
}

GLuint GLShaderVariableEntry::getLocation() const
{
    return m_location;
}

GLenum GLShaderVariableEntry::getGLType() const
{
    return m_typeData;
}

size_t GLShaderVariableEntry::getNumberOfElements() const
{
    return m_count;
}

void* GLShaderVariableEntry::getDataPtr() const
{
    return m_data;
}

double* GLShaderVariableEntry::getDoublePtr() const
{
    return reinterpret_cast<double*>(getDataPtr());
}

float* GLShaderVariableEntry::getFloatPtr() const
{
    return reinterpret_cast<float*>(getDataPtr());
}

int* GLShaderVariableEntry::getIntPtr() const
{
    return reinterpret_cast<int*>(getDataPtr());
}

unsigned int* GLShaderVariableEntry::getUIntPtr() const
{
    return reinterpret_cast<unsigned int*>(getDataPtr());
}
