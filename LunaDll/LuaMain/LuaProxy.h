// LuaHelper.h Helper Functions
#pragma once

#include <Windows.h>
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>

namespace LuaProxy {
    enum L_FIELDTYPE{
        LFT_INVALID = 0,
        LFT_BYTE = 1,
        LFT_WORD = 2,
        LFT_DWORD = 3,
        LFT_FLOAT = 4,
        LFT_DFLOAT = 5,
        LFT_STRING = 6
    };

    void windowDebug(const char* debugText);
    void print(const char *text, int x, int y);
    void print(const char *text, int type, int x, int y);
    int totalNPCs();
    luabind::object npcs(lua_State *L);
    luabind::object findNPCs(int ID, int section, lua_State *L);
    void mem(int offset, L_FIELDTYPE ftype, luabind::object value);
    luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);
    void triggerEvent(const char* evName);
    void playSFX(int index);
    void playSFX(const char* filename);

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
    };

    static LuaEventData evPlayer1;
    static LuaEventData evPlayer2;

    LuaEventData* getEvData(int playerIndex);

    char pressTest(short oldp, short newp);
    void processKeyboardEvent(short oldp, short newp, int index, int playerIndex, lua_State *L);
    void processKeyboardEvents(lua_State *L);
    void processJumpEvent(lua_State *L);
    void finishEventHandling();

    struct RECTd{
        double left;
        double top;
        double right;
        double bottom;
    };

    class Section{
    public:
        Section (int sectionNum);
        RECTd boundary();
        void setBoundary(RECTd boundary);
    private:
        int m_secNum;
    };

    class NPC{
    public:
        NPC (int index);
        int id();
        float direction();
        void setDirection(float direction);
        double x();
        void setX(double x);
        double y();
        void setY(double y);
        double speedX();
        void setSpeedX(double speedX);
        double speedY();
        void setSpeedY(double speedY);
        void mem(int offset, L_FIELDTYPE ftype, luabind::object value);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);
    private:
        bool isValid();
        int m_index;
    };

    class Player{
    public:
        Player ();
        Player (int index);
        int section();
        Section sectionObj();
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
        void mem(int offset, L_FIELDTYPE ftype, luabind::object value);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);
        bool isValid();
    private:
        int m_index;
    };


}
