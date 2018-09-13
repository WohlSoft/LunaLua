#ifndef GLShader_hhhhh
#define GLShader_hhhhh

#include "../GL/GLCompat.h"
#include <string>

#include <vector>
#include "GLShaderUniformInfo.h"
#include "GLShaderAttributeInfo.h"
#include "../GL/GLEngineProxy.h"

class GLShader
{
private:
    static bool compileShaderSource(gl::GLuint shaderID, const std::string& source);
    static std::string getLastShaderError(gl::GLuint shaderID);

    gl::GLuint m_shaderID;
    std::string m_name;
    std::string m_vertexSource;
    std::string m_fragmentSource;
    std::string m_lastErrorMsg;
    bool m_isValid;

    std::vector<gl::GLuint> m_attributeBuffers;

    std::vector<gl::GLuint> m_samplerTexNames;
public:
    GLShader(const std::string& vertexSource, const std::string& fragmentSource);
    GLShader(const std::string& fragmentSource);
    GLShader(const GLShader& other) = delete;
    ~GLShader();

    inline bool isValid() const { return m_isValid; }
    inline std::string getLastErrorMsg() const { return m_lastErrorMsg; }
    
    std::vector<GLShaderAttributeInfo> getAllAttributes() const;
    std::vector<GLShaderUniformInfo> getAllUniforms() const;

    // Unsafe functions (can only be called from GL queue thread)
    
    void bind();
    void unbind();

    void applyAttribute(const GLShaderVariableEntry& entry);
    void applyUniform(const GLShaderVariableEntry& entry);

    gl::GLuint getSamplerForTexture(gl::GLuint name);
    void clearSamplers();

private:
    void load();

    // using GetActiveVariableFunc = void (__stdcall *) (GLuint program, GLuint index, GLsizei maxLength, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
    // TODO: Move to local source file
    template<typename VariableInfoT, typename GetActiveVariableFunc, typename GetVariableLocationFunc>
    std::vector<VariableInfoT> getAllVariables(gl::GLenum programVariableType, GetActiveVariableFunc getActiveVariableFunc, GetVariableLocationFunc getVariableLocationFunc) const
    {
        std::vector<VariableInfoT> results;

        gl::GLint count = 0;
        gl::glGetProgramiv(m_shaderID, programVariableType, &count);
        GLERRORCHECK();

        results.reserve(count);

        for (gl::GLuint i = 0; i < count; i++)
        {
            constexpr static const gl::GLsizei MAX_LENGTH_NAME = 512;
            // GLint id, GLint sizeOfVariable, GLint type, const std::string& name
            gl::GLsizei lengthOfName = 0;
            gl::GLint sizeOfVariable = 0;
            gl::GLenum variableType = static_cast<gl::GLenum>(0);
            std::string nameBuffer(MAX_LENGTH_NAME, ' ');

            // Get variable propertiess
            getActiveVariableFunc(m_shaderID, i, MAX_LENGTH_NAME, &lengthOfName, &sizeOfVariable, &variableType, &nameBuffer[0]);
            GLERRORCHECK();
            nameBuffer.resize(lengthOfName);

            // Get location (ID)
            GLint location = getVariableLocationFunc(m_shaderID, nameBuffer.c_str());
            GLERRORCHECK();

            results.emplace_back(location, sizeOfVariable, variableType, nameBuffer);
        }


        return results;
    }


};

#endif
