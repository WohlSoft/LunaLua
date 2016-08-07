#include "../LuaProxy.h"

#include "../../GlobalFuncs.h"
#include "../../Rendering/GL/GLEngineProxy.h"

LuaProxy::NativeShader::NativeShader()
{}

void LuaProxy::NativeShader::compileFromSource(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, lua_State* L)
{
    m_internalShader = g_GLEngine.CreateNewShader(name, vertexSource, fragmentSource);
    
    if (!m_internalShader->isValid()) {
        luaL_error(L, "Failed to compile shader: %s", m_internalShader->getLastErrorMsg());
        return;
    }

    m_cachedAttributeInfo = m_internalShader->getAllAttributes();
    m_cachedUniformInfo = m_internalShader->getAllUniforms();
}

void LuaProxy::NativeShader::compileFromFile(const std::string& name, const luabind::object& fileNameVertex, const luabind::object& fileNameFragment, lua_State* L)
{
    std::string vertexSource("");
    std::string fragmentSource("");

    auto readFromObject = [L](std::string& readTo, const luabind::object& readFrom, const std::string shaderType) -> bool {
        if (!readFrom.is_valid())
            return true;

        boost::optional<std::string> optionalFileName = luabind::object_cast_nothrow<std::string>(readFrom);
        if (optionalFileName == boost::none) {
            luaL_error(L, "Failed to read %s shader, filename is not a string!", shaderType.c_str());
            return false;
        }

        bool success = readFile(readTo, resolveIfNotAbsolutePath(*optionalFileName));
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

    compileFromSource(name, vertexSource, fragmentSource, L);
}

bool LuaProxy::NativeShader::isCompiled() const
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
    infoTbl["sizeOfVariable"] = info->getSizeOfVariable();
    infoTbl["type"] = info->getType();
    infoTbl["varInfoType"] = static_cast<int>(info->getVarType());
    return infoTbl;
}

luabind::object LuaProxy::NativeShader::getAttributeInfo(lua_State* L) const
{
    if (!isCompiled()) {
        luaL_error(L, "Tried to get attributes on invalid shader!");
        return luabind::object();
    }
    
    luabind::object resultTbl = luabind::newtable(L);
    for (const GLShaderAttributeInfo& nextInfo : m_cachedAttributeInfo) // TODO: Make it named --> tbl[attribute_name] = {}
        resultTbl[nextInfo.getName()] = convertShaderVariableInfo(&nextInfo, L);

    return resultTbl;
    
}

luabind::object LuaProxy::NativeShader::getUniformInfo(lua_State* L) const
{
    if (!isCompiled()) {
        luaL_error(L, "Tried to get uniforms on invalid shader!");
        return luabind::object();
    }

    luabind::object resultTbl = luabind::newtable(L);
    for(const GLShaderUniformInfo& nextInfo : m_cachedUniformInfo) // TODO: Make it named --> tbl[uniform_name] = {}
        resultTbl[nextInfo.getName()] = convertShaderVariableInfo(&nextInfo, L);

    return resultTbl;
}

std::shared_ptr<GLShader> LuaProxy::NativeShader::getInternalShader() const
{
    return m_internalShader;
}






