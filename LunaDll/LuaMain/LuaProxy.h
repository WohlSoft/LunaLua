// LuaHelper.h Helper Functions
#pragma once

#include <Windows.h>
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>

namespace LuaProxy {
    void windowDebug(const char* debugText);
    void print(const char *text, int x, int y);
    void print(const char *text, int type, int x, int y);

    class NPC{
    public:
        NPC (int index);
        int id();
        int direction();
        void setDirection(int direction);
    private:
        int m_index;
    };

    class Player{
    public:
        Player ();
        int section();
        void kill();
        void harm(); //untested
        RECT screen();
        double x();
        void setX(double x);
        double y();
        void setY(double y);
        double speedX();
        void setSpeedX(double speedX);
        double speedY();
        void setSpeedY(double speedY);
        int powerup();
        void setPowerup(int powerup);
        int reservePowerup();
        void setReservePowerup(int reservePowerup);
        luabind::object holdingNPC(lua_State *L);
    };


}
