#include "GLShaderVariableEntry.h"

#include <cstdlib>
#include <utility>
#include <vector>
#include <memory>
#include "../GL/GLSprite.h"
#include "../GL/GLTextureStore.h"
#include "../GL/GLFramebuffer.h"
#include "GLShader.h"
class LunaImage;

GLShaderVariableEntry::GLShaderVariableEntry(GLShaderVariableType type, GLuint location, GLenum typeData, size_t count, void* data, void* imgs) :
    m_type(type),
    m_location(location),
    m_typeData(typeData),
    m_count(count),
    m_data(data),
    m_imgs(imgs)
{}

GLShaderVariableEntry::GLShaderVariableEntry(GLShaderVariableEntry&& other) noexcept
{
    m_type = other.m_type;
    m_location = other.m_location;
    m_typeData = other.m_typeData;
    m_count = other.m_count;
    m_data = other.m_data;
    m_imgs = other.m_imgs;

    other.m_data = nullptr;
    other.m_imgs = nullptr;
}

GLShaderVariableEntry::~GLShaderVariableEntry()
{
    if (m_data)
    {
        std::free(m_data);
    }

    if (m_imgs)
    {
        delete reinterpret_cast<GLShaderVariableEntry::SamplerVector*>(m_imgs);
    }
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

int* GLShaderVariableEntry::getTexPtr(GLShader& shader) const
{
    if ((m_typeData == GL_SAMPLER_2D) && (m_imgs != nullptr) && (m_data == nullptr))
    {
        auto& imgs = *reinterpret_cast<GLShaderVariableEntry::SamplerVector*>(m_imgs);
        const_cast<GLShaderVariableEntry*>(this)->m_data = malloc(sizeof(unsigned int) * m_count);
        auto texPtr = reinterpret_cast<unsigned int*>(m_data);

        for (unsigned int i = 0; i < m_count; i++)
        {
            typedef GLShaderVariableEntry::SamplerVectorEntry::SamplerType SamplerType;
            const GLSprite* sprite = nullptr;
            const GLFramebuffer* fb = nullptr;
            if (imgs[i].type == SamplerType::ELunaImage)
            {
                sprite = g_GLTextureStore.SpriteFromLunaImage(imgs[i].img);
            }
            else if ((imgs[i].type == SamplerType::ECaptureBuffer) || (imgs[i].type == SamplerType::EDepthBuffer))
            {
                fb = imgs[i].cap->mFramebuffer;
            }

            if (sprite != nullptr)
            {
                texPtr[i] = shader.getSamplerForTexture(sprite->GetTexId());
            }
            else if (fb != nullptr)
            {
                if (imgs[i].type == SamplerType::EDepthBuffer)
                {
                    texPtr[i] = shader.getSamplerForTexture(fb->AsDepthTexture().name);
                }
                else
                {
                    texPtr[i] = shader.getSamplerForTexture(fb->AsTexture().name);
                }
            }
            else
            {
                texPtr[i] = shader.getSamplerForTexture(0);
            }
        }
    }
    //std::pai
    return reinterpret_cast<int*>(m_data);
}