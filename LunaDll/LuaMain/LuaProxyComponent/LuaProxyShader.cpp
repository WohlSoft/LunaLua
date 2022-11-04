#include "../LuaProxy.h"

#include "../../GlobalFuncs.h"
#include "../../Rendering/GL/GLEngineProxy.h"

LuaProxy::Shader::Shader()
{}

void LuaProxy::Shader::compileFromSource(const std::string& vertexSource, const std::string& fragmentSource, lua_State* L)
{
    if(isCompiled())
    {
        luaL_error(L, "Shader is already compiled!");
        return;
    }

    m_internalShader = g_GLEngine.CreateNewShader(vertexSource, fragmentSource);
    
    if (!m_internalShader->isValid()) {
        luaL_error(L, "Failed to compile shader: \n%s", m_internalShader->getLastErrorMsg().c_str());
        return;
    }

    // GL operation on the main thread
    g_GLEngine.EnsureMainThreadCTXApplied();

    m_cachedAttributeInfo = m_internalShader->getAllAttributes();
    m_cachedUniformInfo = m_internalShader->getAllUniforms();
}

void LuaProxy::Shader::compileFromFile(const std::string& fileNameVertex, const std::string& fileNameFragment, lua_State* L)
{
    if (isCompiled())
    {
        luaL_error(L, "Shader is already compiled!");
        return;
    }

    std::string vertexSource;
    std::string fragmentSource;

    auto readFromObject = [L](std::string& readTo, const std::string& filename, const std::string shaderType) -> bool {
        bool success = readFile(readTo, resolveIfNotAbsolutePath(filename));
        if (!success) {
            luaL_error(L, "Failed to open file!");
            return false;
        }
        return true;
    };

    if (!readFromObject(vertexSource, fileNameVertex, "vertex"))
        return;
    if (!readFromObject(fragmentSource, fileNameFragment, "fragment"))
        return;

    compileFromSource(vertexSource, fragmentSource, L);
}

bool LuaProxy::Shader::isCompiled() const
{
    if (!m_internalShader)
        return false;
    return m_internalShader->isValid();
}


luabind::object convertShaderVariableInfo(const GLShaderVariableInfo* info, lua_State* L)
{
    luabind::object infoTbl = luabind::newtable(L);
    infoTbl["id"] = info->getId();
    infoTbl["name"] = info->getName();
    infoTbl["rawName"] = info->getRawName();
    infoTbl["arrayCount"] = info->arrayCount();
    infoTbl["type"] = info->getType();
    infoTbl["varInfoType"] = static_cast<int>(info->getVarType());
    infoTbl["arrayDepth"] = info->getArrayDepth();
    return infoTbl;
}

luabind::object LuaProxy::Shader::getAttributeInfo(lua_State* L) const
{
    if (!isCompiled()) {
        luaL_error(L, "Tried to get attributes on invalid shader!");
        return luabind::object();
    }
    
    luabind::object resultTbl = luabind::newtable(L);
    for (const GLShaderAttributeInfo& nextInfo : m_cachedAttributeInfo)
        resultTbl[nextInfo.getRawName()] = convertShaderVariableInfo(&nextInfo, L);

    return resultTbl;
    
}

luabind::object LuaProxy::Shader::getUniformInfo(lua_State* L) const
{
    if (!isCompiled()) {
        luaL_error(L, "Tried to get uniforms on invalid shader!");
        return luabind::object();
    }

    luabind::object resultTbl = luabind::newtable(L);
    for(const GLShaderUniformInfo& nextInfo : m_cachedUniformInfo)
        resultTbl[nextInfo.getRawName()] = convertShaderVariableInfo(&nextInfo, L);

    return resultTbl;
}

std::shared_ptr<GLShader> LuaProxy::Shader::getInternalShader() const
{
    return m_internalShader;
}






