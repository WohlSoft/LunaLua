#pragma once

#include <vector>
#include <memory>
#include <gl/glew.h>
#include "GLShaderVariableType.h"


class GLShader;
class LunaImage;
class CaptureBuffer;

class GLShaderVariableEntry
{
    GLShaderVariableType m_type;
    GLuint m_location;
    GLenum m_typeData;
    size_t m_count;
    void* m_data;
    void* m_imgs;
public:
    typedef std::vector<std::pair<std::shared_ptr<LunaImage>, std::shared_ptr<CaptureBuffer>>> SamplerVector;
    
    GLShaderVariableEntry(GLShaderVariableType type, GLuint location, GLenum typeData, size_t m_count, void* data, void* imgs=nullptr);
    GLShaderVariableEntry(const GLShaderVariableEntry& other) = delete; // To prevent double-freeing
    GLShaderVariableEntry(GLShaderVariableEntry&& other) noexcept;
    ~GLShaderVariableEntry();

    GLShaderVariableType getVariableType() const;
    GLuint getLocation() const;
    GLenum getGLType() const;
    size_t getNumberOfElements() const;
    void* getDataPtr() const;
    
    double* getDoublePtr() const;
    float* getFloatPtr() const;
    int* getIntPtr() const;
    unsigned int* getUIntPtr() const;
    int* getTexPtr(GLShader& shader) const;

};

