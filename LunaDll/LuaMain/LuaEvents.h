#pragma once

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

	struct SMBXEventQueueItem{
		std::string event;
		int unkVal;
		int callType;
	};
	extern std::vector<SMBXEventQueueItem> SMBXEventQueue;


    static LuaEventData evPlayer1;
    static LuaEventData evPlayer2;

    LuaEventData* getEvData(int playerIndex);

    char pressTest(short oldp, short newp);
    void processKeyboardEvent(short oldp, short newp, int index, int playerIndex, lua_State *L);
    void processKeyboardEvents(lua_State *L);
    void processJumpEvent(lua_State *L);
    void processSectionEvents(lua_State *L);
    void proccesEvents(lua_State *L);
	void processSMBXEvents(lua_State *L);
    void finishEventHandling();
}
