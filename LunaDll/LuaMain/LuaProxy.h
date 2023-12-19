// LuaHelper.h Helper Functions
#ifndef LuaProxy_Hhh
#define LuaProxy_Hhh

#include <algorithm>
using std::min;
using std::max;
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
#include "../SMBXInternal/Warp.h"
#include "../Rendering/Shaders/GLShader.h"
#include "../libs/PGE_File_Formats/lvl_filedata.h"
#include "../libs/PGE_File_Formats/wld_filedata.h"
#include "../libs/PGE_File_Formats/npc_filedata.h"

class BMPBox;

namespace LuaProxy {

    template<typename T>
    bool luaUserdataCompare(const luabind::object& arg1, const luabind::object& arg2, lua_State* L) {
        T* rawArg1 = luabind::object_cast_nothrow<T*>(arg1, static_cast<T*>(nullptr));
        T* rawArg2 = luabind::object_cast_nothrow<T*>(arg2, static_cast<T*>(nullptr));

        if (rawArg1 == nullptr)
        {
            luaL_error(L, "Failed to compare %s == %s:\nLeft compare argument is not %s", typeid(T).name(), typeid(T).name(), typeid(T).name());
            return false;
        }
        if (rawArg2 == nullptr)
        {
            luaL_error(L, "Failed to compare %s == %s:\nRight compare argument is not %s", typeid(T).name(), typeid(T).name(), typeid(T).name());
            return false;
        }

        return rawArg1 == rawArg2;
    };

    template<typename C, typename IndexType, IndexType C::* Index>
    bool luaUserdataIndexCompare(const luabind::object& arg1, const luabind::object& arg2, lua_State* L){
        static_assert(std::is_integral<IndexType>::value, "Data member type must be integral!");

        C* rawArg1 = luabind::object_cast_nothrow<C*>(arg1, static_cast<C*>(nullptr));
        C* rawArg2 = luabind::object_cast_nothrow<C*>(arg2, static_cast<C*>(nullptr));

        if (rawArg1 == nullptr)
        {
            luaL_error(L, "Failed to compare %s == %s:\nLeft compare argument is not %s", typeid(C).name(), typeid(C).name(), typeid(C).name());
            return false;
        }
        if (rawArg2 == nullptr)
        {
            luaL_error(L, "Failed to compare %s == %s:\nRight compare argument is not %s", typeid(C).name(), typeid(C).name(), typeid(C).name());
            return false;
        }

        return rawArg1->*Index == rawArg2->*Index;
    };

#define LUAPROXY_DEFUSERDATAINEDXCOMPARE(def_class, def_datamember) &LuaProxy::luaUserdataIndexCompare<def_class, decltype( def_class :: def_datamember ), & def_class :: def_datamember >


    template<typename ConvertedType>
    struct vb6_converter {
        typedef ConvertedType from_type; // vb6
        typedef ConvertedType to_type; // C++

        static to_type ConvertForGetter(const from_type& data) {
            return data;
        }

        static from_type ConvertForSetter(const to_type& data) {
            return data;
        }
    };

    template<>
    struct vb6_converter<bool> {
        typedef short from_type; // vb6
        typedef bool to_type; // C++

        static to_type ConvertForGetter(const from_type& data) {
            return data == -1;
        }

        static from_type ConvertForSetter(const to_type& data) {
            return COMBOOL(data);
        }
    };

    template<>
    struct vb6_converter<std::string> {
        typedef VB6StrPtr from_type; // vb6
        typedef std::string to_type; // C++

        static to_type ConvertForGetter(const from_type& data) {
            return data;
        }

        static from_type ConvertForSetter(const to_type& data) {
            return data;
        }
    };


    // NOTE: This class is currently not actually used.
    //       m_index must be accessed somehow properly.
    template<class WrapperClass, class InternalClass>
    class CommonVB6Wrapper {
    public:
        typedef InternalClass internal_class;

        template<typename DataType, DataType InternalClass::* Ptr, typename InputType = DataType>
        void Setter(InputType data)
        {
            typedef vb6_converter<InputType> converter;

            InternalClass::Get(static_cast<WrapperClass*>(this)->m_index)->*Ptr = converter::ConvertForSetter(data);
        }

        template<typename DataType, DataType InternalClass::* Ptr, typename OutputType = DataType>
        OutputType Getter() const
        {
            typedef vb6_converter<OutputType> converter;

            return converter::ConvertForGetter(InternalClass::Get(static_cast<const WrapperClass*>(this)->m_index)->*Ptr);
        }

        template<Momentum InternalClass::* Ptr, double Momentum::* SubPtr>
        void MomentumSetter(double data)
        {
            (&(InternalClass::Get(static_cast<WrapperClass*>(this)->m_index)->*Ptr))->*SubPtr = data;
        }

        template<Momentum InternalClass::* Ptr, double Momentum::* SubPtr>
        double MomentumGetter() const
        {
            return (&(InternalClass::Get(static_cast<const WrapperClass*>(this)->m_index)->*Ptr))->*SubPtr;
        }
    };

    // &LuaProxy::Warp::MomentumGetter<&LuaProxy::Warp::internal_class::exit, &Momentum::x>, &LuaProxy::Warp::MomentumSetter<&LuaProxy::Warp::internal_class::exit, &Momentum::x>
#define LUAPROXY_REG_R_MOMENTUM(WrapperCls, MomentumField, MomentumSubField) \
    &WrapperCls::MomentumGetter<&WrapperCls::internal_class::MomentumField, &Momentum:: MomentumSubField>
#define LUAPROXY_REG_W_MOMENTUM(WrapperCls, MomentumField, MomentumSubField) \
    &WrapperCls::MomentumSetter<&WrapperCls::internal_class::MomentumField, &Momentum:: MomentumSubField>

#define LUAPROXY_REG_RW_MOMENTUM(WrapperCls, MomentumField, MomentumSubField) \
    LUAPROXY_REG_R_MOMENTUM(WrapperCls, MomentumField, MomentumSubField), \
    LUAPROXY_REG_W_MOMENTUM(WrapperCls, MomentumField, MomentumSubField)

    // &LuaProxy::Warp::Getter<decltype(SMBX_Warp::isHidden), &SMBX_Warp::isHidden>, &LuaProxy::Warp::Setter<decltype(SMBX_Warp::isHidden), &SMBX_Warp::isHidden>
#define LUAPROXY_REG_R(WrapperCls, StructField) \
    &WrapperCls::Getter<decltype(WrapperCls::internal_class::StructField), &WrapperCls::internal_class::StructField>
#define LUAPROXY_REG_W(WrapperCls, StructField) \
    &WrapperCls::Setter<decltype(WrapperCls::internal_class::StructField), &WrapperCls::internal_class::StructField>
#define LUAPROXY_REG_RW(WrapperCls, StructField) \
    LUAPROXY_REG_R(WrapperCls, StructField), \
    LUAPROXY_REG_W(WrapperCls, StructField)

#define LUAPROXY_REG_R_CUSTOM(WrapperCls, StructField, CustomField) \
    &WrapperCls::Getter<decltype(WrapperCls::internal_class::StructField), &WrapperCls::internal_class::StructField, CustomField>
#define LUAPROXY_REG_W_CUSTOM(WrapperCls, StructField, CustomField) \
    &WrapperCls::Setter<decltype(WrapperCls::internal_class::StructField), &WrapperCls::internal_class::StructField, CustomField>
#define LUAPROXY_REG_RW_CUSTOM(WrapperCls, StructField, CustomField) \
    LUAPROXY_REG_R_CUSTOM(WrapperCls, StructField, CustomField), \
    LUAPROXY_REG_W_CUSTOM(WrapperCls, StructField, CustomField)


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

    class Console {
    public:
        Console();

        void print(const std::string& outText);
        void println(const std::string& outText);
    private:
        bool m_isDefaultConsole;
    };



    class Warp : public CommonVB6Wrapper<Warp, SMBX_Warp> {
    public:
        static int count();
        static luabind::object get(lua_State* L);
        static luabind::object getIntersectingEntrance(double x1, double y1, double x2, double y2, lua_State* L);
        static luabind::object getIntersectingExit(double x1, double y1, double x2, double y2, lua_State* L);

        Warp(int warpIndex);
        int idx() const;
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
        int idx() const;
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
        int idx() const;
        std::string layerName() const;
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
        int idx() const;
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
        std::string attachedLayerName(lua_State* L) const;
        void setAttachedLayerName(const luabind::object &value, lua_State* L);
        luabind::object attachedLayerObj(lua_State *L) const;
        void setAttachedLayerObj(const LuaProxy::Layer &value, lua_State *L);
        std::string activateEventName(lua_State* L) const;
        void setActivateEventName(const luabind::object &value, lua_State* L);
        std::string deathEventName(lua_State* L) const;
        void setDeathEventName(const luabind::object &value, lua_State* L);
        std::string talkEventName(lua_State* L) const;
        void setTalkEventName(const luabind::object &value, lua_State* L);
        std::string noMoreObjInLayer(lua_State* L) const;
        void setNoMoreObjInLayer(const luabind::object &value, lua_State* L);
        std::string msg(lua_State* L) const;
        void setMsg(const luabind::object &value, lua_State* L);
        std::string layerName(lua_State* L) const;
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
        short harmCombo(short harmType, short multiplier, lua_State* L);
        short harmCombo(short harmType, short multiplier, float damage, lua_State* L);
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

    class Block
    {
    public:
        static int count();
        static luabind::object get(lua_State* L);
        static luabind::object get(luabind::object idFilter, lua_State* L);
        static luabind::object getIntersecting(double x1, double y1, double x2, double y2, lua_State* L);
        static LuaProxy::Block spawn(int npcid, double x, double y, lua_State* L);
        static bool _getBumpable(int id);
        static void _setBumpable(int id, bool bumpable);
        static void _rawHitBlock(unsigned int blockIdx, short fromUpSide, unsigned short playerIdx, int hittingCount);

        Block(int index);
        int idx() const;
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
        std::string layerName() const;
        luabind::object layerObj(lua_State* L) const;
        void remove();
        void remove(bool playSoundEffect);

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
        std::string levelTitle(lua_State* L);
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

        int idx() const;
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

        int idx() const;
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

        int idx() const;
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

        int idx() const;
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

        bool visible(lua_State* L) const;
        void setVisible(bool visible, lua_State* L);

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
        int idx() const;
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
        bool visible() const;
        void setVisible(bool isHidden);
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

    class InputConfig{
    public:
        InputConfig(unsigned short index);

        int idx() const;
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

    class Logger {
    public:
        Logger(std::string filename);
        Logger(const Logger &lg) = default;
        ~Logger();
        void write(const std::string& line);
    private:
        std::string filePath;
        FILE* file;
    };

    class Shader {
    public:
        Shader();
        void compileFromSource(const std::string& vertexSource, const std::string& fragmentSource, lua_State* L);
        void compileFromFile(const std::string& fileNameVertex, const std::string& fileNameFragment, lua_State* L);

        bool isCompiled() const;
        luabind::object getAttributeInfo(lua_State* L) const;
        luabind::object getUniformInfo(lua_State* L) const;


        std::shared_ptr<GLShader> getInternalShader() const;
    private:
        std::vector<GLShaderAttributeInfo> m_cachedAttributeInfo;
        std::vector<GLShaderUniformInfo> m_cachedUniformInfo;
        std::shared_ptr<GLShader> m_internalShader;
    };


    //undocumented
    namespace Native{
        std::string getSMBXPath();
        std::string getEpisodePath();
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
        std::string filename();
        std::string name();
    }

    namespace Misc{
        void npcToCoins();
        void doPOW();
        void doBombExplosion(double x, double y, short bombType);
        void doPSwitchRaw(bool activate);
        void doPSwitch();
        void doPSwitch(bool activate);
        std::string cheatBuffer();
        void cheatBuffer(const luabind::object &value, lua_State* L);
        luabind::object listFiles(const std::string& path, lua_State* L);
        luabind::object listDirectories(const std::string& path, lua_State* L);
        luabind::object listLocalFiles(const std::string& path, lua_State* L);
        luabind::object listLocalDirectories(const std::string& path, lua_State* L);
        luabind::object resolveFile(const std::string& relativePath, lua_State* L);
        luabind::object resolveDirectory(const std::string& relativePath, lua_State* L);
        luabind::object resolveGraphicsFile(const std::string& file, lua_State* L);
        bool isSamePath(const std::string first, const std::string second);
        void openPauseMenu();
        void saveGame();
        void exitGame();
        void exitEngine();
        bool didGameOver();
        bool loadEpisode(const std::string& episodeName);
        void pause();
        void pause(bool atFrameEnd);
        void unpause();
        bool isPausedByLua();
        void warning(const std::string& str);
        void registerCharacterId(const luabind::object& namedArgs, lua_State* L);
        std::string showRichDialog(const std::string& title, const std::string& rtfText, bool isReadOnly);

        // Internal use profiler functions
        void __enablePerfTracker();
        void __disablePerfTracker();
        luabind::object __getPerfTrackerData(lua_State* L);
    }

    namespace Graphics{
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
        std::shared_ptr<LunaImage> loadImage(const std::string&, lua_State* L);
        void placeSprite(int type, int imgResource, int xPos, int yPos, const std::string& extra, int time);
        void placeSprite(int type, int imgResource, int xPos, int yPos, const std::string& extra);
        void placeSprite(int type, int imgResource, int xPos, int yPos);
        void placeSprite(int type, const std::shared_ptr<LunaImage>& img, int xPos, int yPos, const std::string& extra, int time);
        void placeSprite(int type, const std::shared_ptr<LunaImage>& img, int xPos, int yPos, const std::string& extra);
        void placeSprite(int type, const std::shared_ptr<LunaImage>& img, int xPos, int yPos);
        void unplaceSprites(const std::shared_ptr<LunaImage>& img, int xPos, int yPos);
        void unplaceSprites(const std::shared_ptr<LunaImage>& img);
        luabind::object getPixelData(const std::shared_ptr<LunaImage>& img, int& width, int& height, lua_State *L);

        void drawImage(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, lua_State* L);
        void drawImage(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, float opacity, lua_State* L);
        void drawImage(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, lua_State* L);
        void drawImage(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, lua_State* L);
        void drawImageWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double priority, lua_State* L);
        void drawImageWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, float opacity, double priority, lua_State* L);
        void drawImageWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, double priority, lua_State* L);
        void drawImageWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, double priority, lua_State* L);
        void drawImageToScene(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, lua_State* L);
        void drawImageToScene(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, float opacity, lua_State* L);
        void drawImageToScene(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, lua_State* L);
        void drawImageToScene(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, lua_State* L);
        void drawImageToSceneWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double priority, lua_State* L);
        void drawImageToSceneWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, float opacity, double priority, lua_State* L);
        void drawImageToSceneWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, double priority, lua_State* L);
        void drawImageToSceneWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, double priority, lua_State* L);
        void drawImageGeneric(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, bool sceneCoords, double priority, lua_State* L);
        void draw(const luabind::object& namedArgs, lua_State* L);

        bool isOpenGLEnabled();
        void glSetTexture(const std::shared_ptr<LunaImage>& img, uint32_t color);
        void glSetTextureRGBA(const std::shared_ptr<LunaImage>& img, uint32_t color);
        void __glInternalDraw(const luabind::object& namedArgs, lua_State* L);
        void __setSpriteOverride(const std::string& type, int index, std::shared_ptr<LunaImage>* pimg, lua_State* L);
        void __setHardcodedSpriteOverride(const std::string& name, std::shared_ptr<LunaImage>* poverrideImg, lua_State* L);
        std::shared_ptr<LunaImage> __getHardcodedSpriteOverride(const std::string& name, lua_State* L);
        std::shared_ptr<LunaImage> __getSpriteOverride(const std::string& type, int index, lua_State* L);
    }

    namespace Effects{
        void screenGlow(unsigned int color);
        void screenGlowNegative(unsigned int color);
        void flipX();
        void flipY();
        void flipXY();
    }

    namespace Formats{
        luabind::object openLevelHeader(const std::string &filePath, lua_State *L);
        luabind::object openLevel(const std::string &filePath, lua_State *L);
        luabind::object getLevelData(lua_State *L);
        luabind::object openWorldHeader(const std::string &filePath, lua_State *L);
        luabind::object openWorld(const std::string &filePath, lua_State *L);
        //luabind::object getWorldData(lua_State *L); // TODO: Implement this once PGEFL will handle WorldMap reading
        luabind::object openNpcConfig(const std::string &filePath, lua_State *L);
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

    std::string getInput();

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
    void playSFX(const std::string& filename, lua_State* L);
    void playSFXSDL(const std::string& filename, lua_State* L);
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
