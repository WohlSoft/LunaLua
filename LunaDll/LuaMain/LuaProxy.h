// LuaHelper.h Helper Functions
#ifndef LuaProxy_Hhh
#define LuaProxy_Hhh

#include <windows.h>
#include <type_traits>
#include <memory>
#include <stdio.h>
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>
#include "LuaHelper.h"
#include "../Defines.h"
#include "../Misc/AsyncHTTPClient.h"
#include "../SMBXInternal/Blocks.h"

class BMPBox;

namespace LuaProxy {

    template<typename T>
    bool luaUserdataCompare(const luabind::object& arg1, const luabind::object& arg2, lua_State* L) {
        boost::optional<T*> arg1Casted = luabind::object_cast_nothrow<T*>(arg1);
        boost::optional<T*> arg2Casted = luabind::object_cast_nothrow<T*>(arg2);

        T* rawArg1 = nullptr;
        T* rawArg2 = nullptr;
        if (arg1Casted != boost::none) 
        {
            rawArg1 = arg1Casted.get();
        }
        else 
        {
            luaL_error(L, "Failed to compare %s == %s:\nLeft compare argument is not %s", typeid(T).name(), typeid(T).name(), typeid(T).name());
            return false;
        }
        if (arg2Casted != boost::none)
        {
            rawArg2 = arg2Casted.get();
        }
        else
        {
            luaL_error(L, "Failed to compare %s == %s:\nRight compare argument is not %s", typeid(T).name(), typeid(T).name(), typeid(T).name());
            return false;
        }

        return rawArg1 == rawArg2;
    };

    template<typename C, typename IndexType, IndexType C::* Index>
    bool luaUserdataIndexCompare(const luabind::object& arg1, const luabind::object& arg2, lua_State* L){
        static_assert(std::is_integral<IndexType>::value, "Data member type must be integral!");

        boost::optional<C*> arg1Casted = luabind::object_cast_nothrow<C*>(arg1);
        boost::optional<C*> arg2Casted = luabind::object_cast_nothrow<C*>(arg2);

        C* rawArg1 = nullptr;
        C* rawArg2 = nullptr;
        if (arg1Casted != boost::none)
        {
            rawArg1 = arg1Casted.get();
        }
        else
        {
            luaL_error(L, "Failed to compare %s == %s:\nLeft compare argument is not %s", typeid(C).name(), typeid(C).name(), typeid(C).name());
            return false;
        }
        if (arg2Casted != boost::none)
        {
            rawArg2 = arg2Casted.get();
        }
        else
        {
            luaL_error(L, "Failed to compare %s == %s:\nRight compare argument is not %s", typeid(C).name(), typeid(C).name(), typeid(C).name());
            return false;
        }

        return rawArg1->*Index == rawArg2->*Index;
    };
//&LuaProxy::luaUserdataIndexCompare<LuaProxy::Player, decltype(LuaProxy::Player::m_index), &LuaProxy::Player::m_index>
#define LUAPROXY_DEFUSERDATAINEDXCOMPARE(def_class, def_datamember) &LuaProxy::luaUserdataIndexCompare<def_class, decltype( ## def_class ## :: ## def_datamember ## ), & ## def_class ## :: ## def_datamember ## >

    enum L_FIELDTYPE{
        LFT_INVALID = FT_INVALID,
        LFT_BYTE = FT_BYTE,
        LFT_WORD = FT_WORD,
        LFT_DWORD = FT_DWORD,
        LFT_FLOAT = FT_FLOAT,
        LFT_DFLOAT = FT_DFLOAT,
        LFT_STRING = FT_MAX + 1,
        LFT_BOOL = FT_MAX + 2
    };

    
    //Deprecated
    namespace SaveBankProxy {
        void setValue(const std::string& key, double value);
        luabind::object getValue(const std::string& key, lua_State *L);
        bool isValueSet(const std::string& key);
        luabind::object values(lua_State* L);
        void save();
    }

    class Data {
    public:
        enum DataType{
            DATA_LEVEL,
            DATA_WORLD,
            DATA_GLOBAL
        };
        Data(DataType dataType);
        Data(DataType dataType, bool useSaveSlot);

        Data(DataType dataType, const std::string &sectionName);
        Data(DataType dataType, const std::string &sectionName, bool useSaveSlot);

        ~Data();

        void set(const std::string &key, const std::string &value);
        std::string get(const std::string &key) const;
        luabind::object get(lua_State* L) const;

        void save();
        void save(const std::string &sectionName);

        DataType dataType() const;
        void setDataType(DataType dataType);

        std::string sectionName() const;
        void setSectionName(const std::string &sectionName);

        bool useSaveSlot() const;
        void setUseSaveSlot(bool useSaveSlot);

    private:
        DataType m_dataType;
        std::string m_sectionName;
        std::map<std::string, std::string> m_data;
        bool m_useSaveSlot;

        void init();
        std::string resolvePathFromSection();

    };

    class AsyncHTTPRequest {
        std::shared_ptr<AsyncHTTPClient> m_client;
    public:
        AsyncHTTPRequest();
        void addArgument(const std::string& argName, const std::string& data, lua_State* L);
        void setUrl(const std::string& url, lua_State* L);
        std::string getUrl(lua_State* L) const;
        void setMethod(AsyncHTTPClient::AsyncHTTPMethod method, lua_State* L);
        AsyncHTTPClient::AsyncHTTPMethod getMethod(lua_State* L) const;
        void send(lua_State* L);
        void wait(lua_State* L);

        bool isReady() const;
        bool isProcessing() const;
        bool isFinished() const;
        std::string responseText(lua_State* L) const;
        int statusCode(lua_State* L) const;
        
    };

    struct RECTd{
        double left;
        double top;
        double right;
        double bottom;
    };

    

    class VBStr{
    public:
        VBStr (wchar_t* ptr);
        VBStr (long ptr);
        std::string str() const;
        void setStr(const std::string &str);
        int length() const;
        void setLength(int len);
        void clear();
        bool isValid() const;
        static std::string LuaProxy::VBStr::luaConcatToString(const luabind::object &value, lua_State *L);
        static std::string luaConcat(const luabind::object &arg1, const luabind::object &arg2, lua_State *L);

        // Allow << operator to act as friend
        friend std::ostream& operator<<(std::ostream& os, const VBStr& wStr);

        // Allow assignVB6StrPtr access to m_wcharptr
        friend void LuaHelper::assignVB6StrPtr(VB6StrPtr* ptr, const luabind::object &value, lua_State* L);
    private:
        wchar_t* m_wcharptr;
    };

    std::ostream& operator<<(std::ostream& os, const VBStr& wStr);

    class Console {
    public:
        Console();

        void print(const std::string& outText);
        void println(const std::string& outText);
    private:
        bool m_isDefaultConsole;
    };



    class Warp{
    public:
        static int count();
        static luabind::object get(lua_State* L);
        static luabind::object getIntersectingEntrance(double x1, double y1, double x2, double y2, lua_State* L);
        static luabind::object getIntersectingExit(double x1, double y1, double x2, double y2, lua_State* L);

        Warp(int warpIndex);
        void mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L) const;

        bool isHidden() const;
        void setIsHidden(bool isHidden);

        double entranceX() const;
        void setEntranceX(double entranceX);
        double entranceY() const;
        void setEntranceY(double entranceY);

        double exitX() const;
        void setExitX(double exitX);
        double exitY() const;
        void setExitY(double exitY);

        std::string levelFilename();
        void setLevelFilename(const luabind::object &value, lua_State* L);

        int m_index;
    };

    class Animation{
    public:
        static int count();
        static luabind::object get(lua_State* L);
        static luabind::object get(luabind::object idFilter, lua_State* L);
        static luabind::object getIntersecting(double x1, double y1, double x2, double y2, lua_State* L);
        static LuaProxy::Animation spawnEffect(short effectID, double x, double y, lua_State* L);
        static LuaProxy::Animation spawnEffect(short effectID, double x, double y, float animationFrame, lua_State* L);

        Animation (int animationIndex);
        void mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L) const;
        short id() const;
        void setId(short id);
        bool isHidden() const;
        void setIsHidden(bool isHidden);
        double x() const;
        void setX(double x);
        double y() const;
        void setY(double y);
        double speedX() const;
        void setSpeedX(double speedX);
        double speedY() const;
        void setSpeedY(double speedY);
        double height() const;
        void setHeight(double height);
        double width() const;
        void setWidth(double width);
        float subTimer() const;
        void setSubTimer(float subTimer);
        short timer() const;
        void setTimer(short timer);
        short npcID() const;
        void setNpcID(short npcID);
        bool drawOnlyMask() const;
        void setDrawOnlyMask(bool drawOnlyMask);
        short animationFrame() const;
        void setAnimationFrame(short animationFrame);
        bool isValid() const;

        int m_animationIndex;
    };

    class Layer{
    public:
        static luabind::object get(lua_State *L);
        static luabind::object get(const std::string& layerName, lua_State* L);
        static luabind::object find(const std::string& layerName, lua_State* L);

        Layer (int layerIndex);
        VBStr layerName() const;
        float speedX() const;
        void setSpeedX(float speedX);
        float speedY() const;
        void setSpeedY(float speedY);
        bool isHidden();
        
        void stop();
        void show(bool noSmoke);
        void hide(bool noSmoke);
        void toggle(bool noSmoke);

        int layerIndex() const;

        int m_layerIndex;
    };

    class Section{
    public:
        static luabind::object get(lua_State* L);
        static LuaProxy::Section get(short secNumber, lua_State* L);
        
        Section (int sectionNum);
        RECTd boundary() const;
        void setBoundary(const RECTd &boundary);
        short musicID() const;
        void setMusicID(short musicID);
        bool isLevelWarp() const;
        void setIsLevelWarp(bool isLevelWarp);
        bool hasOffscreenExit() const;
        void setHasOffscreenExit(bool hasOffscreenExit);
        short backgroundID() const;
        void setBackgroundID(short backgroundID);
        bool noTurnBack() const;
        void setNoTurnBack(bool noTurnBack);
        bool isUnderwater() const;
        void setIsUnderwater(bool isUnderwater);

        int m_secNum;
    };

    class NPC{
    public:
        static int count();
        static luabind::object get(lua_State* L);
        static luabind::object get(luabind::object idFilter, lua_State* L);
        static luabind::object get(luabind::object idFilter, luabind::object sectionFilter, lua_State* L);
        static luabind::object getIntersecting(double x1, double y1, double x2, double y2, lua_State* L);
        static LuaProxy::NPC spawn(short npcid, double x, double y, short section, lua_State* L);
        static LuaProxy::NPC spawn(short npcid, double x, double y, short section, bool respawn, lua_State* L);
        static LuaProxy::NPC spawn(short npcid, double x, double y, short section, bool respawn, bool centered, lua_State* L);

        NPC (int index);
        int idx() const;
        int id(lua_State* L) const;
        void setId(int id, lua_State* L);
        bool isHidden(lua_State* L) const;
        void setIsHidden(bool isHidden, lua_State* L);
        float direction(lua_State* L) const;
        void setDirection(float direction, lua_State* L);
        double x(lua_State* L) const;
        void setX(double x, lua_State* L);
        double y(lua_State* L) const;
        void setY(double y, lua_State* L);
        double width(lua_State *L) const;
        void setWidth(double width, lua_State *L);
        double height(lua_State *L) const;
        void setHeight(double height, lua_State *L);
        double speedX(lua_State* L) const;
        void setSpeedX(double speedX, lua_State* L);
        double speedY(lua_State* L) const;
        void setSpeedY(double speedY, lua_State* L);
        void mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L) const;
        void kill(lua_State* L);
        void kill(int killEffectID, lua_State* L);
        VBStr attachedLayerName(lua_State* L) const;
        void setAttachedLayerName(const luabind::object &value, lua_State* L);
        luabind::object attachedLayerObj(lua_State *L) const;
        void setAttachedLayerObj(const LuaProxy::Layer &value, lua_State *L);
        VBStr activateEventName(lua_State* L) const;
        void setActivateEventName(const luabind::object &value, lua_State* L);
        VBStr deathEventName(lua_State* L) const;
        void setDeathEventName(const luabind::object &value, lua_State* L);
        VBStr talkEventName(lua_State* L) const;
        void setTalkEventName(const luabind::object &value, lua_State* L);
        VBStr noMoreObjInLayer(lua_State* L) const;
        void setNoMoreObjInLayer(const luabind::object &value, lua_State* L);
        VBStr msg(lua_State* L) const;
        void setMsg(const luabind::object &value, lua_State* L);
        VBStr layerName(lua_State* L) const;
        void setLayerName(const luabind::object &value, lua_State* L);
        luabind::object layerObj(lua_State *L) const;
        void setLayerObj(const LuaProxy::Layer &value, lua_State *L);
        double ai1(lua_State* L) const;
        void setAi1(double ai1, lua_State* L);
        double ai2(lua_State* L) const;
        void setAi2(double ai2, lua_State* L);
        double ai3(lua_State* L) const;
        void setAi3(double ai3, lua_State* L);
        double ai4(lua_State* L) const;
        void setAi4(double ai4, lua_State* L);
        double ai5(lua_State* L) const;
        void setAi5(double ai5, lua_State* L);
        bool drawOnlyMask(lua_State* L) const;
        void setDrawOnlyMask(bool drawOnlyMask, lua_State* L);
        bool isInvincibleToSword(lua_State* L) const;
        void setIsInvincibleToSword(bool isInvincibleToSword, lua_State* L);
        bool legacyBoss(lua_State* L) const;
        void setLegacyBoss(bool legacyBoss, lua_State *L);
        bool friendly(lua_State* L) const;
        void setFriendly(bool friendly, lua_State* L);
        bool dontMove(lua_State* L) const;
        void setDontMove(bool dontMove, lua_State* L);
        void toIce(lua_State* L);
        void toCoin(lua_State* L);
        void harm(lua_State* L);
        void harm(short harmType, lua_State* L);
        void harm(short harmType, float damage, lua_State* L);
        bool collidesBlockBottom(lua_State* L) const;
        void setCollidesBlockBottom(bool collidesBlockBottom, lua_State* L);
        bool collidesBlockLeft(lua_State* L) const;
        void setCollidesBlockLeft(bool collidesBlockLeft, lua_State* L);
        bool collidesBlockUp(lua_State* L) const;
        void setCollidesBlockUp(bool collidesBlockUp, lua_State* L);
        bool collidesBlockRight(lua_State* L) const;
        void setCollidesBlockRight(bool collidesBlockRight, lua_State* L);
        bool isUnderwater(lua_State* L) const;
        void setIsUnderwater(bool isUnderwater, lua_State* L);
        short animationFrame(lua_State* L) const;
        void setAnimationFrame(short animationFrame, lua_State* L);
        float animationTimer(lua_State* L) const;
        void setAnimationTimer(float animationTimer, lua_State* L);
        short killFlag(lua_State* L) const;
        void setKillFlag(short killFlag, lua_State* L);


        bool isValid() const;
        bool isValid_throw(lua_State *L) const;
        //not bound functions
        void* getNativeAddr() const;


        int m_index;
    };

    class PlayerSettings {
    public:
        static PlayerSettings get(Characters character, PowerupID powerupID, lua_State* L);
        PlayerSettings(Characters character, PowerupID powerupID) : m_powerupID(powerupID), m_character(character) {}

        int getHitboxWidth(lua_State* L);
        void setHitboxWidth(int width, lua_State* L);
        int getHitboxHeight(lua_State* L);
        void setHitboxHeight(int height, lua_State* L);
        int getHitboxDuckHeight(lua_State* L);
        void setHitboxDuckHeight(int duckHeight, lua_State* L);
        int getGrabOffsetX(lua_State* L);
        void setGrabOffsetX(int grabOffsetX, lua_State* L);
        int getGrabOffsetY(lua_State* L);
        void setGrabOffsetY(int grabOffsetY, lua_State* L);

        int getSpriteOffsetX(int indexX, int indexY, lua_State* L);
        void setSpriteOffsetX(int indexX, int indexY, int value, lua_State* L);
        int getSpriteOffsetY(int indexX, int indexY, lua_State* L);
        void setSpriteOffsetY(int indexX, int indexY, int value, lua_State* L);


        PowerupID getPowerupID(lua_State* L) const;
        void setPowerupID(PowerupID val, lua_State* L);
        Characters getCharacter(lua_State* L) const;
        void setCharacter(Characters val, lua_State* L);
    
        bool isValid_throw(lua_State* L) const;
    private:
        PowerupID m_powerupID;
        Characters m_character;
        
    };

    class Player{
    public:
        static int count();
        static luabind::object get(lua_State* L);
        static luabind::object getTemplates(lua_State* L);
        
        Player ();
        Player (int index);
        int section(lua_State *L) const;
        Section sectionObj(lua_State *L) const;
        void kill(lua_State *L);
        void harm(lua_State *L);
        RECT screen(lua_State *L) const;
        double x(lua_State *L) const;
        void setX(double x, lua_State *L);
        double y(lua_State *L) const;
        void setY(double y, lua_State *L);
        double width(lua_State *L) const;
        void setWidth(double width, lua_State *L);
        double height(lua_State *L) const;
        void setHeight(double height, lua_State *L);
        double speedX(lua_State *L) const;
        void setSpeedX(double speedX, lua_State *L);
        double speedY(lua_State *L) const;
        void setSpeedY(double speedY, lua_State *L);
        PowerupID powerup(lua_State *L) const;
        void setPowerup(PowerupID powerup, lua_State *L);
        Characters character(lua_State *L) const;
        void setCharacter(Characters character, lua_State *L);
        int reservePowerup(lua_State *L) const;
        void setReservePowerup(int reservePowerup, lua_State *L);
        luabind::object holdingNPC(lua_State *L) const;
        bool upKeyPressing(lua_State *L) const;
        void setUpKeyPressing(bool upKeyPressing, lua_State *L);
        bool downKeyPressing(lua_State *L) const;
        void setDownKeyPressing(bool downKeyPressing, lua_State *L);
        bool leftKeyPressing(lua_State *L) const;
        void setLeftKeyPressing(bool leftKeyPressing, lua_State *L);
        bool rightKeyPressing(lua_State *L) const;
        void setRightKeyPressing(bool rightKeyPressing, lua_State *L);
        bool jumpKeyPressing(lua_State *L) const;
        void setJumpKeyPressing(bool jumpKeyPressing, lua_State *L);
        bool altJumpKeyPressing(lua_State *L) const;
        void setAltJumpKeyPressing(bool altJumpKeyPressing, lua_State *L);
        bool runKeyPressing(lua_State *L) const;
        void setRunKeyPressing(bool runKeyPressing, lua_State *L);
        bool altRunKeyPressing(lua_State *L) const;
        void setAltRunKeyPressing(bool altRunKeyPressing, lua_State *L);
        bool dropItemKeyPressing(lua_State *L) const;
        void setDropItemKeyPressing(bool dropItemKeyPressing, lua_State *L);
        bool pauseKeyPressing(lua_State *L) const;
        void setPauseKeyPressing(bool pauseKeyPressing, lua_State *L);
        luabind::object getCurrentPlayerSetting(lua_State* L);
        void getCurrentSpriteIndex(short& indexX, short& indexY, lua_State* L);
        void setCurrentSpriteIndex(short indexX, short indexY, bool forceDirection, lua_State* L);

        void mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State *L);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L) const;
        bool isValid() const;
        bool isValid_throw(lua_State *L) const;
        //Generated by code:
        short toadDoubleJReady(lua_State *L) const;
        void setToadDoubleJReady(short var_toadDoubleJReady, lua_State *L);
        short sparklingEffect(lua_State *L) const;
        void setSparklingEffect(short var_sparklingEffect, lua_State *L);
        short unknownCTRLLock1(lua_State *L) const;
        void setUnknownCTRLLock1(short var_unknownCTRLLock1, lua_State *L);
        short unknownCTRLLock2(lua_State *L) const;
        void setUnknownCTRLLock2(short var_unknownCTRLLock2, lua_State *L);
        short quicksandEffectTimer(lua_State *L) const;
        void setQuicksandEffectTimer(short var_quicksandEffectTimer, lua_State *L);
        short onSlipperyGround(lua_State *L) const;
        void setOnSlipperyGround(short var_onSlipperyGround, lua_State *L);
        short isAFairy(lua_State *L) const;
        void setIsAFairy(short var_isAFairy, lua_State *L);
        short fairyAlreadyInvoked(lua_State *L) const;
        void setFairyAlreadyInvoked(short var_fairyAlreadyInvoked, lua_State *L);
        short fairyFramesLeft(lua_State *L) const;
        void setFairyFramesLeft(short var_fairyFramesLeft, lua_State *L);
        short sheathHasKey(lua_State *L) const;
        void setSheathHasKey(short var_sheathHasKey, lua_State *L);
        short sheathAttackCooldown(lua_State *L) const;
        void setSheathAttackCooldown(short var_sheathAttackCooldown, lua_State *L);
        short hearts(lua_State *L) const;
        void setHearts(short var_hearts, lua_State *L);
        short peachHoverAvailable(lua_State *L) const;
        void setPeachHoverAvailable(short var_peachHoverAvailable, lua_State *L);
        short pressingHoverButton(lua_State *L) const;
        void setPressingHoverButton(short var_pressingHoverButton, lua_State *L);
        short peachHoverTimer(lua_State *L) const;
        void setPeachHoverTimer(short var_peachHoverTimer, lua_State *L);
        short unused1(lua_State *L) const;
        void setUnused1(short var_unused1, lua_State *L);
        float peachHoverTrembleSpeed(lua_State *L) const;
        void setPeachHoverTrembleSpeed(float var_peachHoverTrembleSpeed, lua_State *L);
        short peachHoverTrembleDir(lua_State *L) const;
        void setPeachHoverTrembleDir(short var_peachHoverTrembleDir, lua_State *L);
        short itemPullupTimer(lua_State *L) const;
        void setItemPullupTimer(short var_itemPullupTimer, lua_State *L);
        float itemPullupMomentumSave(lua_State *L) const;
        void setItemPullupMomentumSave(float var_itemPullupMomentumSave, lua_State *L);
        short unused2(lua_State *L) const;
        void setUnused2(short var_unused2, lua_State *L);
        short unkClimbing1(lua_State *L) const;
        void setUnkClimbing1(short var_unkClimbing1, lua_State *L);
        short unkClimbing2(lua_State *L) const;
        void setUnkClimbing2(short var_unkClimbing2, lua_State *L);
        short unkClimbing3(lua_State *L) const;
        void setUnkClimbing3(short var_unkClimbing3, lua_State *L);
        short waterOrQuicksandState(lua_State *L) const;
        void setWaterOrQuicksandState(short var_waterState, lua_State *L);
        short isInWater(lua_State *L) const;
        void setIsInWater(short var_isInWater, lua_State *L);
        short waterStrokeTimer(lua_State *L) const;
        void setWaterStrokeTimer(short var_waterStrokeTimer, lua_State *L);
        short unknownHoverTimer(lua_State *L) const;
        void setUnknownHoverTimer(short var_unknownHoverTimer, lua_State *L);
        short slidingState(lua_State *L) const;
        void setSlidingState(short var_slidingState, lua_State *L);
        short slidingGroundPuffs(lua_State *L) const;
        void setSlidingGroundPuffs(short var_slidingGroundPuffs, lua_State *L);
        short climbingState(lua_State *L) const;
        void setClimbingState(short var_climbingState, lua_State *L);
        short unknownTimer(lua_State *L) const;
        void setUnknownTimer(short var_unknownTimer, lua_State *L);
        short unknownFlag(lua_State *L) const;
        void setUnknownFlag(short var_unknownFlag, lua_State *L);
        short unknownPowerupState(lua_State *L) const;
        void setUnknownPowerupState(short var_unknownPowerupState, lua_State *L);
        short slopeRelated(lua_State *L) const;
        void setSlopeRelated(short var_slopeRelated, lua_State *L);
        short tanookiStatueActive(lua_State *L) const;
        void setTanookiStatueActive(short var_tanookiStatueActive, lua_State *L);
        short tanookiMorphCooldown(lua_State *L) const;
        void setTanookiMorphCooldown(short var_tanookiMorphCooldown, lua_State *L);
        short tanookiActiveFrameCount(lua_State *L) const;
        void setTanookiActiveFrameCount(short var_tanookiActiveFrameCount, lua_State *L);
        short isSpinjumping(lua_State *L) const;
        void setIsSpinjumping(short var_isSpinjumping, lua_State *L);
        short spinjumpStateCounter(lua_State *L) const;
        void setSpinjumpStateCounter(short var_spinjumpStateCounter, lua_State *L);
        short spinjumpLandDirection(lua_State *L) const;
        void setSpinjumpLandDirection(short var_spinjumpLandDirection, lua_State *L);
        short currentKillCombo(lua_State *L) const;
        void setCurrentKillCombo(short var_currentKillCombo, lua_State *L);
        short groundSlidingPuffsState(lua_State *L) const;
        void setGroundSlidingPuffsState(short var_groundSlidingPuffsState, lua_State *L);
        short nearbyWarpIndex(lua_State *L) const;
        void setNearbyWarpIndex(short var_warpNearby, lua_State *L);
        short unknown5C(lua_State *L) const;
        void setUnknown5C(short var_unknown5C, lua_State *L);
        short unknown5E(lua_State *L) const;
        void setUnknown5E(short var_unknown5E, lua_State *L);
        short hasJumped(lua_State *L) const;
        void setHasJumped(short var_hasJumped, lua_State *L);
        double curXPos(lua_State *L) const;
        void setCurXPos(double var_curXPos, lua_State *L);
        double curYPos(lua_State *L) const;
        void setCurYPos(double var_curYPos, lua_State *L);
        double curXSpeed(lua_State *L) const;
        void setCurXSpeed(double var_curXSpeed, lua_State *L);
        double curYSpeed(lua_State *L) const;
        void setCurYSpeed(double var_curYSpeed, lua_State *L);
        short identity(lua_State *L) const;
        void setIdentity(short var_identity, lua_State *L);
        short uKeyState(lua_State *L) const;
        void setUKeyState(short var_uKeyState, lua_State *L);
        short dKeyState(lua_State *L) const;
        void setDKeyState(short var_dKeyState, lua_State *L);
        short lKeyState(lua_State *L) const;
        void setLKeyState(short var_lKeyState, lua_State *L);
        short rKeyState(lua_State *L) const;
        void setRKeyState(short var_rKeyState, lua_State *L);
        short jKeyState(lua_State *L) const;
        void setJKeyState(short var_jKeyState, lua_State *L);
        short sJKeyState(lua_State *L) const;
        void setSJKeyState(short var_sJKeyState, lua_State *L);
        short xKeyState(lua_State *L) const;
        void setXKeyState(short var_xKeyState, lua_State *L);
        short rNKeyState(lua_State *L) const;
        void setRNKeyState(short var_rNKeyState, lua_State *L);
        short sELKeyState(lua_State *L) const;
        void setSELKeyState(short var_sELKeyState, lua_State *L);
        short sTRKeyState(lua_State *L) const;
        void setSTRKeyState(short var_sTRKeyState, lua_State *L);
        short facingDirection(lua_State *L) const;
        void setFacingDirection(short var_facingDirection, lua_State *L);
        short mountType(lua_State *L) const;
        void setMountType(short var_mountType, lua_State *L);
        short mountColor(lua_State *L) const;
        void setMountColor(short var_mountColor, lua_State *L);
        short mountState(lua_State *L) const;
        void setMountState(short var_mountState, lua_State *L);
        short mountHeightOffset(lua_State *L) const;
        void setMountHeightOffset(short var_mountHeightOffset, lua_State *L);
        short mountGfxIndex(lua_State *L) const;
        void setMountGfxIndex(short var_mountGfxIndex, lua_State *L);
        short currentPowerup(lua_State *L) const;
        void setCurrentPowerup(short var_currentPowerup, lua_State *L);
        short currentPlayerSprite(lua_State *L) const;
        void setCurrentPlayerSprite(short var_currentPlayerSprite, lua_State *L);
        short unused116(lua_State *L) const;
        void setUnused116(short var_unused116, lua_State *L);
        float gfxMirrorX(lua_State *L) const;
        void setGfxMirrorX(float var_gfxMirrorX, lua_State *L);
        short upwardJumpingForce(lua_State *L) const;
        void setUpwardJumpingForce(short var_upwardJumpingForce, lua_State *L);
        short jumpButtonHeld(lua_State *L) const;
        void setJumpButtonHeld(short var_jumpButtonHeld, lua_State *L);
        short spinjumpButtonHeld(lua_State *L) const;
        void setSpinjumpButtonHeld(short var_spinjumpButtonHeld, lua_State *L);
        short forcedAnimationState(lua_State *L) const;
        void setForcedAnimationState(short var_forcedAnimationState, lua_State *L);
        double forcedAnimationTimer(lua_State *L) const;
        void setForcedAnimationTimer(double var_forcedAnimationTimer, lua_State *L);
        short downButtonMirror(lua_State *L) const;
        void setDownButtonMirror(short var_downButtonMirror, lua_State *L);
        short inDuckingPosition(lua_State *L) const;
        void setInDuckingPosition(short var_inDuckingPosition, lua_State *L);
        short selectButtonMirror(lua_State *L) const;
        void setSelectButtonMirror(short var_selectButtonMirror, lua_State *L);
        short unknown132(lua_State *L) const;
        void setUnknown132(short var_unknown132, lua_State *L);
        short downButtonTapped(lua_State *L) const;
        void setDownButtonTapped(short var_downButtonTapped, lua_State *L);
        short unknown136(lua_State *L) const;
        void setUnknown136(short var_unknown136, lua_State *L);
        float xMomentumPush(lua_State *L) const;
        void setXMomentumPush(float var_xMomentumPush, lua_State *L);
        short deathState(lua_State *L) const;
        void setDeathState(short var_deathState, lua_State *L);
        short deathTimer(lua_State *L) const;
        void setDeathTimer(short var_deathTimer, lua_State *L);
        short blinkTimer(lua_State *L) const;
        void setBlinkTimer(short var_blinkTimer, lua_State *L);
        short blinkState(lua_State *L) const;
        void setBlinkState(short var_blinkState, lua_State *L);
        short unknown144(lua_State *L) const;
        void setUnknown144(short var_unknown144, lua_State *L);
        short layerStateStanding(lua_State *L) const;
        void setLayerStateStanding(short var_layerStateStanding, lua_State *L);
        short layerStateLeftContact(lua_State *L) const;
        void setLayerStateLeftContact(short var_layerStateLeftContact, lua_State *L);
        short layerStateTopContact(lua_State *L) const;
        void setLayerStateTopContact(short var_layerStateTopContact, lua_State *L);
        short layerStateRightContact(lua_State *L) const;
        void setLayerStateRightContact(short var_layerStateRightContact, lua_State *L);
        short pushedByMovingLayer(lua_State *L) const;
        void setPushedByMovingLayer(short var_pushedByMovingLayer, lua_State *L);
        short unused150(lua_State *L) const;
        void setUnused150(short var_unused150, lua_State *L);
        short unused152(lua_State *L) const;
        void setUnused152(short var_unused152, lua_State *L);
        short heldNPCIndex(lua_State *L) const;
        void setHeldNPCIndex(short var_heldNPCIndex, lua_State *L);
        short unknown156(lua_State *L) const;
        void setUnknown156(short var_unknown156, lua_State *L);
        short powerupBoxContents(lua_State *L) const;
        void setPowerupBoxContents(short var_powerupBoxContents, lua_State *L);
        short currentSection(lua_State *L) const;
        void setCurrentSection(short var_currentSection, lua_State *L);
        short warpCooldownTimer(lua_State *L) const;
        void setWarpCooldownTimer(short var_warpCooldownTimer, lua_State *L);
        short targetWarpIndex(lua_State *L) const;
        void setTargetWarpIndex(short var_targetWarpIndex, lua_State *L);
        short projectileTimer1(lua_State *L) const;
        void setProjectileTimer1(short var_projectileTimer1, lua_State *L);
        short projectileTimer2(lua_State *L) const;
        void setProjectileTimer2(short var_projectileTimer2, lua_State *L);
        short tailswipeTimer(lua_State *L) const;
        void setTailswipeTimer(short var_tailswipeTimer, lua_State *L);
        short unknown166(lua_State *L) const;
        void setUnknown166(short var_unknown166, lua_State *L);
        float takeoffSpeed(lua_State *L) const;
        void setTakeoffSpeed(float var_takeoffSpeed, lua_State *L);
        short canFly(lua_State *L) const;
        void setCanFly(short var_canFly, lua_State *L);
        short isFlying(lua_State *L) const;
        void setIsFlying(short var_isFlying, lua_State *L);
        short flightTimeRemaining(lua_State *L) const;
        void setFlightTimeRemaining(short var_flightTimeRemaining, lua_State *L);
        short holdingFlightRunButton(lua_State *L) const;
        void setHoldingFlightRunButton(short var_holdingFlightRunButton, lua_State *L);
        short holdingFlightButton(lua_State *L) const;
        void setHoldingFlightButton(short var_holdingFlightButton, lua_State *L);
        short nPCBeingStoodOnIndex(lua_State *L) const;
        void setNPCBeingStoodOnIndex(short var_nPCBeingStoodOnIndex, lua_State *L);
        short unknown178(lua_State *L) const;
        void setUnknown178(short var_unknown178, lua_State *L);
        short unknown17A(lua_State *L) const;
        void setUnknown17A(short var_unknown17A, lua_State *L);
        short unused17C(lua_State *L) const;
        void setUnused17C(short var_unused17C, lua_State *L);
        short unused17E(lua_State *L) const;
        void setUnused17E(short var_unused17E, lua_State *L);
        short unused180(lua_State *L) const;
        void setUnused180(short var_unused180, lua_State *L);
        short unused182(lua_State *L) const;
        void setUnused182(short var_unused182, lua_State *L);
        //internal use
        int getIndex() const;
        
        int m_index;
    };


    class Block
    {
    public:
        static int count();
        static luabind::object get(lua_State* L);
        static luabind::object get(luabind::object idFilter, lua_State* L);
        static luabind::object getIntersecting(double x1, double y1, double x2, double y2, lua_State* L);
        static LuaProxy::Block spawn(int npcid, double x, double y, lua_State* L);

        Block(int index);
        void mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L) const;
        double x() const;
        void setX(double x);
        double y() const;
        void setY(double y);
        double width() const;
        void setWidth(double width);
        double height() const;
        void setHeight(double height);
        double speedX() const;
        void setSpeedX(double speedX);
        double speedY() const;
        void setSpeedY(double speedY);
        short id() const;
        void setId(short id);
        short contentID() const;
        void setContentID(short contentID);
        bool slippery() const;
        void setSlippery(bool slippery);
        bool isHidden() const;
        void setIsHidden(bool isHidden);
        int collidesWith(const Player* player) const;
        VBStr layerName() const;
        luabind::object layerObj(lua_State* L) const;
        void remove();
        void remove(bool playSoundEffect);
        void hit();
        void hit(bool fromUpSide);
        void hit(bool fromUpSide, LuaProxy::Player player);
        void hit(bool fromUpSide, LuaProxy::Player player, int hittingCount);

        ::Block* getBlockPtr();
        bool isValid() const;
        int m_index;
    };

    //more functions:
    

    //For world exclusive:
    class World
    {
    public:
        World();
        void mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L) const;
        double playerX() const;
        void setPlayerX(double playerX);
        double playerY() const;
        void setPlayerY(double playerY);
        short currentWalkingDirection() const;
        void setCurrentWalkingDirection(short currentWalkingDirection);
        short currentWalkingFrame() const;
        void setCurrentWalkingFrame(short currentWalkingFrame);
        short currentWalkingFrameTimer() const;
        void setCurrentWalkingFrameTimer(short currentWalkingFrameTimer);
        short currentWalkingTimer() const;
        void setCurrentWalkingTimer(short currentWalkingTimer);
        bool playerIsCurrentWalking() const;
        luabind::object levelTitle(lua_State* L);
        luabind::object levelObj(lua_State* L);
        short getCurrentDirection() const;
        short playerPowerup() const;
        void setPlayerPowerup(short playerPowerup);
    };

    class Tile
    {
    public:
        static unsigned short count();
        static luabind::object get(lua_State* L);
        static luabind::object get(luabind::object idFilter, lua_State* L);
        static luabind::object getIntersecting(double x1, double y1, double x2, double y2, lua_State* L);

        Tile(unsigned short index);

        short id(lua_State* L) const;
        void setId(short id, lua_State* L);

        double x(lua_State* L) const;
        void setX(double x, lua_State* L) const;
        double y(lua_State*) const;
        void setY(double y, lua_State* L) const;

        double width(lua_State* L) const;
        void setWidth(double width, lua_State* L);
        double height(lua_State* L) const;
        void setHeight(double height, lua_State* L);

        bool isValid() const;
        bool isValid_throw(lua_State *L) const;

        unsigned short m_index;
    };

    class Scenery
    {
    public:
        static unsigned short count();
        static luabind::object get(lua_State* L);
        static luabind::object get(luabind::object idFilter, lua_State* L);
        static luabind::object getIntersecting(double x1, double y1, double x2, double y2, lua_State* L);

        Scenery(unsigned short index);

        short id(lua_State* L) const;
        void setId(short id, lua_State* L);

        double x(lua_State* L) const;
        void setX(double x, lua_State* L) const;
        double y(lua_State*) const;
        void setY(double y, lua_State* L) const;

        double width(lua_State* L) const;
        void setWidth(double width, lua_State* L);
        double height(lua_State* L) const;
        void setHeight(double height, lua_State* L);

        bool isValid() const;
        bool isValid_throw(lua_State *L) const;

        unsigned short m_index;
    };

    class Musicbox
    {
    public:
        static unsigned short count();
        static luabind::object get(lua_State* L);
        static luabind::object get(luabind::object idFilter, lua_State* L);
        static luabind::object getIntersecting(double x1, double y1, double x2, double y2, lua_State* L);

        Musicbox(unsigned short index);

        short id(lua_State* L) const;
        void setId(short id, lua_State* L);

        double x(lua_State* L) const;
        void setX(double x, lua_State* L) const;
        double y(lua_State*) const;
        void setY(double y, lua_State* L) const;

        double width(lua_State* L) const;
        void setWidth(double width, lua_State* L);
        double height(lua_State* L) const;
        void setHeight(double height, lua_State* L);

        bool isValid() const;
        bool isValid_throw(lua_State *L) const;

        unsigned short m_index;
    };

    class Path
    {
    public:
        static unsigned short count();
        static luabind::object get(lua_State* L);
        static luabind::object get(luabind::object idFilter, lua_State* L);
        static luabind::object getIntersecting(double x1, double y1, double x2, double y2, lua_State* L);

        Path(unsigned short index);

        short id(lua_State* L) const;
        void setId(short id, lua_State* L);

        double x(lua_State* L) const;
        void setX(double x, lua_State* L) const;
        double y(lua_State*) const;
        void setY(double y, lua_State* L) const;

        double width(lua_State* L) const;
        void setWidth(double width, lua_State* L);
        double height(lua_State* L) const;
        void setHeight(double height, lua_State* L);

        bool isValid() const;
        bool isValid_throw(lua_State *L) const;

        unsigned short m_index;
    };


    class LevelObject
    {
    public:
        static unsigned short count();
        static luabind::object get(lua_State* L);
        static luabind::object get(luabind::object idFilter, lua_State* L);

        static luabind::object getByName(const std::string& levelName, lua_State* L);
        static luabind::object getByFilename(const std::string& levelFilename, lua_State* L);
        static luabind::object findByName(const std::string& levelName, lua_State* L);
        static luabind::object findByFilename(const std::string& levelFilename, lua_State* L);


        LevelObject(int index);
        void mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L) const;
        double x() const;
        void setX(double x);
        double y() const;
        void setY(double y);
        double goToX();
        void setGoToX(double goToX);
        double goToY();
        void setGoToY(double goToY);
        ExitType topExitType() const;
        void setTopExitType(ExitType topExitType);
        ExitType leftExitType() const;
        void setLeftExitType(ExitType leftExitType);
        ExitType bottomExitType() const;
        void setBottomExitType(ExitType bottomExitType);
        ExitType rightExitType() const;
        void setRightExitType(ExitType rightExitType);
        short levelWarpNumber() const;
        void setLevelWarpNumber(short levelWarpNumber);
        bool isPathBackground() const;
        void setIsPathBackground(bool isPathBackground);
        bool isBigBackground() const;
        void setIsBigBackground(bool isBigBackground);
        bool isGameStartPoint() const;
        void setIsGameStartPoint(bool isGameStartPoint);
        bool isAlwaysVisible() const;
        void setIsAlwaysVisible(bool isAlwaysVisible);
        std::string title() const;
        std::string filename() const;

        bool isValid() const;
        int m_index;
    };

    class BGO
    {
    public:
        static unsigned short count();
        static luabind::object get(lua_State* L);
        static luabind::object get(luabind::object idFilter, lua_State* L);
        static luabind::object getIntersecting(double x1, double y1, double x2, double y2, lua_State* L);

        BGO(unsigned short index);

        bool isHidden(lua_State* L) const;
        void setIsHidden(bool isHidden, lua_State* L);
        short id(lua_State* L) const;
        void setId(short id, lua_State* L);

        double x(lua_State* L) const;
        void setX(double x, lua_State* L) const;
        double y(lua_State*) const;
        void setY(double y, lua_State* L) const;

        double width(lua_State* L) const;
        void setWidth(double width, lua_State* L);
        double height(lua_State* L) const;
        void setHeight(double height, lua_State* L);

        double speedX(lua_State* L) const;
        void setSpeedX(double speedX, lua_State* L);
        double speedY(lua_State* L) const;
        void setSpeedY(double speedY, lua_State* L);

        std::string layerName(lua_State* L) const;
        void setLayerName(const luabind::object& obj, lua_State* L);
        luabind::object layer(lua_State* L) const;
        void setLayer(const Layer& layer, lua_State* L);

        bool isValid() const;
        bool isValid_throw(lua_State *L) const;

        unsigned short m_index;
    };

    class InputConfig{
    public:
        InputConfig(unsigned short index);
       
        short inputType(lua_State* L);
        void setInputType(short inputType, lua_State* L);

        short up(lua_State* L);
        short down(lua_State* L);
        short left(lua_State* L);
        short right(lua_State* L);
        short run(lua_State* L);
        short altrun(lua_State* L);
        short jump(lua_State* L);
        short altjump(lua_State* L);
        short dropitem(lua_State* L);
        short pause(lua_State* L);

        void setUp(unsigned short keycode, lua_State* L);
        void setDown(unsigned short keycode, lua_State* L);
        void setLeft(unsigned short keycode, lua_State* L);
        void setRight(unsigned short keycode, lua_State* L);
        void setRun(unsigned short keycode, lua_State* L);
        void setAltRun(unsigned short keycode, lua_State* L);
        void setJump(unsigned short keycode, lua_State* L);
        void setAltJump(unsigned short keycode, lua_State* L);
        void setDropItem(unsigned short keycode, lua_State* L);
        void setPause(unsigned short keycode, lua_State* L);

        bool isValid() const;
        bool isValid_throw(lua_State *L) const;

        unsigned short m_index;
    };

    class Camera {
    public:
        static luabind::object get(lua_State *L);
        
        static double getX(unsigned short index);
        static double getY(unsigned short index);

        Camera(unsigned short index);

        void mem(int offset, LuaProxy::L_FIELDTYPE ftype, const luabind::object &value, lua_State *L);
        luabind::object mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State *L) const;
        double x();
        double y();
        void setX(double x);
        void setY(double y);
        double renderX();
        double renderY();
        void setRenderX(double renderX);
        void setRenderY(double renderY);
        double width();
        double height();
        void setWidth(double width);
        void setHeight(double height);
        
        unsigned short m_index;
    };

    class Logger {
    public:
        Logger(std::string filename);
        Logger(const Logger &lg) = default;
        ~Logger();
        void write(const std::string& line);
        std::string filePath;
        FILE* file;
    };


    //undocumented
    namespace Native{
        std::string getSMBXPath();
        void simulateError(short errcode);
    }
    //Debug/Text functions
    namespace Text{
        void windowDebug(const std::string& debugText, lua_State* L);
        void windowDebugSimple(const std::string& debugText);
        void print(const luabind::object& toPrint, int x, int y);
        void print(const luabind::object& toPrint, int type, int x, int y);
        void printWP(const luabind::object& toPrint, int x, int y, double priority);
        void printWP(const luabind::object& toPrint, int type, int x, int y, double priority);
        void showMessageBox(const std::string &text);
    }
    //General functions
    namespace Level{
        void exit();
        std::string filename();
        std::string name();
        unsigned short winState();
        void winState(unsigned short value);
    }

    namespace Misc{
        void npcToCoins();
        void doPOW();
        void doBombExplosion(double x, double y, short bombType);
        void doBombExplosion(double x, double y, short bombType, const LuaProxy::Player& playerObj);
        void doPSwitchRaw(bool activate);
        void doPSwitch();
        void doPSwitch(bool activate);
        std::string cheatBuffer();
        void cheatBuffer(const luabind::object &value, lua_State* L);
        luabind::object listFiles(const std::string& path, lua_State* L);
        luabind::object listLocalFiles(std::string path, lua_State* L);
        luabind::object resolveFile(const std::string& relativePath, lua_State* L);
        luabind::object resolveDirectory(const std::string& relativePath, lua_State* L);
        bool isSamePath(const std::string first, const std::string second);
        void openPauseMenu();
        void saveGame();
        void exitGame();
        bool loadEpisode(const std::string& episodeName);
        void pause();
        void unpause();
        bool isPausedByLua();
        void warning(const std::string& str);
        void registerCharacterId(const luabind::object& namedArgs, lua_State* L);
    }

    namespace Graphics{
        class LuaImageResource {
        public:
            std::shared_ptr<BMPBox> img;
            LuaImageResource(const std::shared_ptr<BMPBox>& img);
            ~LuaImageResource();
            int GetWidth() const;
            int GetHeight() const;
            uintptr_t __BMPBoxPtr();

        };
        enum RENDER_TYPE {
            RTYPE_IMAGE,
            RTYPE_TEXT
        };

        void activateHud(bool activateHud);
        bool isHudActivated();
        void activateOverworldHud(WORLD_HUD_CONTROL activateFlag);
        WORLD_HUD_CONTROL getOverworldHudState();
        //CSprite functions
        bool loadImage(const std::string&, int resNumber, int transColor);
        luabind::object loadAnimatedImage(const std::string& filename, int& smbxFrameTime, lua_State* L);
        LuaImageResource* loadImage(const std::string&, lua_State* L);
        void placeSprite(int type, int imgResource, int xPos, int yPos, const std::string& extra, int time);
        void placeSprite(int type, int imgResource, int xPos, int yPos, const std::string& extra);
        void placeSprite(int type, int imgResource, int xPos, int yPos);
        void placeSprite(int type, const LuaImageResource& img, int xPos, int yPos, const std::string& extra, int time);
        void placeSprite(int type, const LuaImageResource& img, int xPos, int yPos, const std::string& extra);
        void placeSprite(int type, const LuaImageResource& img, int xPos, int yPos);
        void unplaceSprites(const LuaImageResource& img, int xPos, int yPos);
        void unplaceSprites(const LuaImageResource& img);
        luabind::object getPixelData(const LuaImageResource& img, int& width, int& height, lua_State *L);

        void drawImage(const LuaImageResource& img, double xPos, double yPos, lua_State* L);
        void drawImage(const LuaImageResource& img, double xPos, double yPos, float opacity, lua_State* L);
        void drawImage(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, lua_State* L);
        void drawImage(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, lua_State* L);
        void drawImageWP(const LuaImageResource& img, double xPos, double yPos, double priority, lua_State* L);
        void drawImageWP(const LuaImageResource& img, double xPos, double yPos, float opacity, double priority, lua_State* L);
        void drawImageWP(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, double priority, lua_State* L);
        void drawImageWP(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, double priority, lua_State* L);
        void drawImageToScene(const LuaImageResource& img, double xPos, double yPos, lua_State* L);
        void drawImageToScene(const LuaImageResource& img, double xPos, double yPos, float opacity, lua_State* L);
        void drawImageToScene(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, lua_State* L);
        void drawImageToScene(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, lua_State* L);
        void drawImageToSceneWP(const LuaImageResource& img, double xPos, double yPos, double priority, lua_State* L);
        void drawImageToSceneWP(const LuaImageResource& img, double xPos, double yPos, float opacity, double priority, lua_State* L);
        void drawImageToSceneWP(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, double priority, lua_State* L);
        void drawImageToSceneWP(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, double priority, lua_State* L);
        void drawImageGeneric(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, bool sceneCoords, double priority, lua_State* L);
        void draw(const luabind::object& namedArgs, lua_State* L);

        bool isOpenGLEnabled();
        void glSetTexture(const LuaImageResource* img, uint32_t color);
        void glSetTextureRGBA(const LuaImageResource* img, uint32_t color);
        void __glInternalDraw(const luabind::object& namedArgs, lua_State* L);
        void __setSpriteOverride(const std::string& type, int index, const luabind::object& img, lua_State* L);
        void __setSimpleSpriteOverride(const std::string& name, const luabind::object& overrideImg, lua_State* L);
        luabind::object __getSpriteOverride(const std::string& type, int index, lua_State* L);
    }

    namespace Effects{
        void screenGlow(unsigned int color);
        void screenGlowNegative(unsigned int color);
        void flipX();
        void flipY();
        void flipXY();
    }
    
    //Non-Member-Constructors:
    RECT newRECT();
    RECTd newRECTd();


#ifdef _MSC_VER //DEPRECATED STUFF
#pragma region
#endif
    //Moved as static functions
    NPC spawnNPC(short npcid, double x, double y, short section, lua_State* L);
    NPC spawnNPC(short npcid, double x, double y, short section, bool respawn, lua_State* L);
    NPC spawnNPC(short npcid, double x, double y, short section, bool respawn, bool centered, lua_State* L);
    Animation spawnEffect(short effectID, double x, double y, lua_State* L);
    Animation spawnEffect(short effectID, double x, double y, float animationFrame, lua_State* L);

    void loadHitboxes(int _character, int _powerup, const std::string& ini_file, lua_State* L);

    VBStr getInput();

    //NPC functions [Moved as static function]
    int totalNPCs();
    luabind::object npcs(lua_State *L);
    luabind::object findNPCs(int ID, int section, lua_State *L);
    //Block functions [Moved as static function]
    luabind::object blocks(lua_State *L);
    luabind::object findblocks(int ID, lua_State *L);

    //Memory functions
    void mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L);
    luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);

    //SMBX trigger function [Moved as static function]
    void triggerEvent(const std::string& evName);
    //Music related
    void playSFX(int index);
    void playSFX(const std::string& filename);
    void playSFXSDL(const std::string& filename);
    void clearSFXBuffer();
    void MusicOpen(const std::string& filename);
    void MusicPlay();
    void MusicPlayFadeIn(int ms);
    void MusicStop();
    void MusicStopFadeOut(int ms);
    void MusicVolume(int vlm);
    bool MusicIsPlaying();
    bool MusicIsPaused();
    bool MusicIsFading();
    void playMusic(int section);

    //General global memory [Moved as Defines.*]
    unsigned short gravity();
    void gravity(unsigned short value);
    unsigned short earthquake();
    void earthquake(unsigned short value);
    unsigned short jumpheight();
    void jumpheight(unsigned short value);
    unsigned short jumpheightBounce();
    void jumpheightBounce(unsigned short value);

    //Layer functions [Moved as static function]
    luabind::object findlayer(const std::string& layername, lua_State *L);
    //Animation functions [Moved as static function]
    luabind::object animations(lua_State* L);
    //DEPRECATED
    void runAnimation(int id, double x, double y, double height, double width, double speedX, double speedY, int extraData);
    void runAnimation(int id, double x, double y, double height, double width, int extraData);
    void runAnimation(int id, double x, double y, int extraData);
    //Load hitbox values for playable character


    //World
    luabind::object levels(lua_State *L);
    luabind::object findlevels(const std::string &toFindName, lua_State* L);
    luabind::object findlevel(const std::string &toFindName, lua_State* L);

#ifdef _MSC_VER
#pragma endregion
#endif

}

#endif
