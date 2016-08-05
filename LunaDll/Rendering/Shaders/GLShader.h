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
    static bool compileShaderSource(GLuint shaderID, const std::string& source);
    static std::string getLastShaderError(GLuint shaderID);

    GLuint m_shaderID;
    std::string m_name;
    std::string m_vertexSource;
    std::string m_fragmentSource;
    std::string m_lastErrorMsg;
    bool m_isValid;

public:
    GLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
    GLShader(const GLShader& other) = delete;
    ~GLShader();

    inline bool isValid() const { return m_isValid; }
    inline std::string getLastErrorMsg() const { return m_lastErrorMsg; }
    
    std::vector<GLShaderAttributeInfo> getAllAttributes() const;
    std::vector<GLShaderUniformInfo> getAllUniforms() const;

    // Unsafe functions (can only be called from GL queue thread)
    void bind();
    void unbind();

    void applyAttribute(GLint location, GLenum type, void* data);
    void applyUniform(GLint location, GLenum type, void* data);

private:
    void load();

    // using GetActiveVariableFunc = void (__stdcall *) (GLuint program, GLuint index, GLsizei maxLength, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
    // TODO: Move to local source file
    template<typename VariableInfoT, typename GetActiveVariableFunc, typename GetVariableLocationFunc>
    std::vector<VariableInfoT> getAllVariables(GLint programAttribute, GetActiveVariableFunc getActiveVariableFunc, GetVariableLocationFunc getVariableLocationFunc) const
    {
        std::vector<VariableInfoT> results;

        g_GLEngine.SafeCall([this, &results, programAttribute, getActiveVariableFunc, getVariableLocationFunc] {

            GLint count = 0;
            glGetProgramiv(m_shaderID, programAttribute, &count);
            results.reserve(count);

            for (GLint i = 0; i < count; i++)
            {
                constexpr static const GLsizei MAX_LENGTH_NAME = 512;
                // GLint id, GLint sizeOfVariable, GLint type, const std::string& name
                GLsizei lengthOfName = 0;
                GLint sizeofVariable = 0;
                GLenum variableType = 0;
                std::string nameBuffer(MAX_LENGTH_NAME, ' ');

                getActiveVariableFunc(m_shaderID, (GLuint)i, MAX_LENGTH_NAME, &lengthOfName, &sizeofVariable, &variableType, &nameBuffer[0]);
                GLERRORCHECK();
                nameBuffer.resize(lengthOfName);

                GLint location = getVariableLocationFunc(m_shaderID, nameBuffer.c_str());
                GLERRORCHECK();

                results.emplace_back(location, sizeofVariable, variableType, nameBuffer);
            }

        });

        return results;
    }

    union UniversalArray {
        float* farr;
        double* darr;
        int* iarr;
        unsigned int* uiarr;
        void* data;
    };

};

#endif
