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

void LuaProxy::Camera::mem(int offset, LuaProxy::L_FIELDTYPE ftype, const luabind::object &value, lua_State *L)
{
    SMBX_CameraInfo *pCamera = ::SMBX_CameraInfo::Get(m_index);
    void* ptr = ((&(*(byte*)pCamera)) + offset);
    LuaProxy::mem((int)ptr, ftype, value, L);
}

luabind::object LuaProxy::Camera::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State *L) const
{
    SMBX_CameraInfo *pCamera = ::SMBX_CameraInfo::Get(m_index);
    void* ptr = ((&(*(byte*)pCamera)) + offset);
    return LuaProxy::mem((int)ptr, ftype, L);
}

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

double LuaProxy::Camera::renderX()
{
    return SMBX_CameraInfo::Get(m_index)->x;
}

double LuaProxy::Camera::renderY()
{
    return SMBX_CameraInfo::Get(m_index)->y;
}

void LuaProxy::Camera::setRenderX(double renderX) {
    SMBX_CameraInfo::Get(m_index)->x = renderX;
}

void LuaProxy::Camera::setRenderY(double renderY) {
    SMBX_CameraInfo::Get(m_index)->y = renderY;
}

double LuaProxy::Camera::width()
{
    return SMBX_CameraInfo::Get(m_index)->width;
}

double LuaProxy::Camera::height()
{
    return SMBX_CameraInfo::Get(m_index)->height;
}

void LuaProxy::Camera::setWidth(double width) {
    SMBX_CameraInfo::Get(m_index)->width = width;
}

void LuaProxy::Camera::setHeight(double height) {
    SMBX_CameraInfo::Get(m_index)->height = height;
}
