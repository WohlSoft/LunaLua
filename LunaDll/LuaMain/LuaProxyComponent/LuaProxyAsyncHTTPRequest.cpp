#include "../LuaProxy.h"
#include "../../Globals.h"

LuaProxy::AsyncHTTPRequest::AsyncHTTPRequest()
{
    m_client = make_shared<AsyncHTTPClient>();
}

void LuaProxy::AsyncHTTPRequest::addArgument(const std::string& argName, const std::string& data, lua_State* L)
{
    if (!isReady()) {
        luaL_error(L, "Cannot add argument: HTTP Request is processing or is finished!");
        return;
    }
    m_client->addArgument(argName, data);
}

void LuaProxy::AsyncHTTPRequest::send(lua_State* L)
{
    if (!gGeneralConfig.getLuaEnableHTTP()) {
        luaL_error(L, "Cannot send request: HTTP API is not enabled!");
        return;
    }
    if (!isReady()) {
        luaL_error(L, "Cannot send request: HTTP Request is already processing or is finished!");
        return;
    }
    m_client->asynSend();
}


void LuaProxy::AsyncHTTPRequest::wait(lua_State* L)
{
    if (!isProcessing()) {
        luaL_error(L, "Cannot wait: No HTTP Request is processing! Forgot to call :send?");
        return;
    }
    m_client->waitUntilResponse();
}


bool LuaProxy::AsyncHTTPRequest::isReady() const
{
    return m_client->getStatus() == AsyncHTTPClient::HTTP_READY;
}

bool LuaProxy::AsyncHTTPRequest::isProcessing() const
{
    return m_client->getStatus() == AsyncHTTPClient::HTTP_PROCESSING;
}

bool LuaProxy::AsyncHTTPRequest::isFinished() const
{
    return m_client->getStatus() == AsyncHTTPClient::HTTP_FINISHED;
}

void LuaProxy::AsyncHTTPRequest::setUrl(const std::string& url, lua_State* L)
{
    if (!isReady()) {
        luaL_error(L, "Cannot set Url: HTTP Request is already processing or is finished!");
        return;
    }
    m_client->setUrl(url);
}

std::string LuaProxy::AsyncHTTPRequest::getUrl(lua_State* L) const
{
    if (!isReady()) {
        luaL_error(L, "Cannot get Url: HTTP Request is already processing or is finished!");
        return "";
    }
    return m_client->getUrl();
}

void LuaProxy::AsyncHTTPRequest::setMethod(AsyncHTTPClient::AsyncHTTPMethod method, lua_State* L)
{
    if (!isReady()) {
        luaL_error(L, "Cannot set HTTP method: HTTP Request is already processing or is finished!");
        return;
    }
    m_client->setMethod(method);
}

AsyncHTTPClient::AsyncHTTPMethod LuaProxy::AsyncHTTPRequest::getMethod(lua_State* L) const
{
    if (!isReady()) {
        luaL_error(L, "Cannot set HTTP method: HTTP Request is already processing or is finished!");
        return AsyncHTTPClient::HTTP_GET;
    }
    return m_client->getMethod();
}



std::string LuaProxy::AsyncHTTPRequest::responseText(lua_State* L) const
{
    if (!isFinished()) {
        luaL_error(L, "Cannot get response text: HTTP Request is currently processing or didn't start!");
        return "";
    }
    return m_client->getResponseData();
}

int LuaProxy::AsyncHTTPRequest::statusCode(lua_State* L) const
{
    if (!isFinished()) {
        luaL_error(L, "Cannot get status code: HTTP Request is currently processing or didn't start!");
        return 0;
    }
    return m_client->getStatusCode();
}
