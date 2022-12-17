#pragma once

#include <vector>
#include <memory>
#include <GL/glew.h>
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
    struct SamplerVectorEntry
    {
        enum SamplerType {
            ENone = 0,
            ELunaImage = 1,
            ECaptureBuffer = 2,
            EDepthBuffer = 3
        };
        SamplerType type;
        std::shared_ptr<LunaImage> img;
        std::shared_ptr<CaptureBuffer> cap;

        SamplerVectorEntry(SamplerType _type, std::shared_ptr<LunaImage> _img, std::shared_ptr<CaptureBuffer> _cap) :
            type(_type),
            img(_img),
            cap(_cap)
        {}
    };
    typedef std::vector<SamplerVectorEntry> SamplerVector;
    
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

