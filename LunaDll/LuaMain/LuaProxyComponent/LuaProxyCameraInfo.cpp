#include "../LuaProxy.h"
#include "../../SMBXInternal/CameraInfo.h"

luabind::object LuaProxy::Camera::get(lua_State *L)
{
    return LuaHelper::getObjList(3, [](unsigned short i){ return LuaProxy::Camera(i); }, [](unsigned int i) { return i != 0; }, L);
}


double LuaProxy::Camera::getX(unsigned short index)
{
    return SMBX_CameraInfo::getCameraX(index);
}

double LuaProxy::Camera::getY(unsigned short index)
{
    return SMBX_CameraInfo::getCameraY(index);
}


LuaProxy::Camera::Camera(unsigned short index) : m_index(index)
{}


double LuaProxy::Camera::x()
{
    return SMBX_CameraInfo::getCameraX(m_index);
}

double LuaProxy::Camera::y()
{
    return SMBX_CameraInfo::getCameraY(m_index);
}

void LuaProxy::Camera::setX(double x) {
    SMBX_CameraInfo::setCameraX(m_index, x);
}

void LuaProxy::Camera::setY(double y) {
    SMBX_CameraInfo::setCameraY(m_index, y);
}

double LuaProxy::Camera::width()
{
    return SMBX_CameraInfo::Get(m_index)->width;
}

double LuaProxy::Camera::height()
{
    return SMBX_CameraInfo::Get(m_index)->height;
}
