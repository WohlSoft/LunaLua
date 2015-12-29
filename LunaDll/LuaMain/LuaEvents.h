#ifndef LuaEvents_hhh
#define LuaEvents_hhh

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>

namespace LuaEvents {

    struct LuaEventData{
        short playerUPressing;
        short playerDPressing;
        short playerLPressing;
        short playerRPressing;
        short playerJPressing;
        short playerSJPressing;
        short playerXPressing;
        short playerRNPressing;
        short playerSELPressing;
        short playerSTRPressing;
        short playerJumping;
        short section;
    };

    static LuaEventData evPlayer1;
    static LuaEventData evPlayer2;

    void resetToDefaults();

    LuaEventData* getEvData(int playerIndex);

    char pressTest(short oldp, short newp);
    void processKeyboardEvent(short oldp, short newp, int index, int playerIndex, lua_State *L, std::string eventTable);
    void processKeyboardEvents(lua_State *L, std::string eventTable);
    void processJumpEvent(lua_State *L, std::string eventTable);
    void processSectionEvents(lua_State *L, std::string eventTable);
    void proccesEvents(lua_State *L, std::string eventTable);
    void finishEventHandling();
}

#endif
