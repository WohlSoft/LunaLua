// LuaHelper.h Helper Functions
#ifndef LuaProxy_Hhh
#define LuaProxy_Hhh

#include <Windows.h>
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>
#include "LuaHelper.h"

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

#ifdef _MSC_VER //DEPRECATED STUFF
#pragma region
#endif

    void loadHitboxes(int _character, int _powerup, const char *ini_file);
	
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
    void triggerEvent(const char* evName);
	//Music related
    void playSFX(int index);
    void playSFX(const char* filename);
	void playSFXSDL(const char* filename);
	void clearSFXBuffer();
	void MusicOpen(const char *filename);
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
    luabind::object findlayer(const char* layername, lua_State *L);
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

    //for runAnimation
    struct coorStruct{
        double x;
        double y;
        double Height;
        double Width;
        double XSpeed;
        double YSpeed;
    };
    

    

    //Deprecated
    namespace SaveBankProxy {
        void setValue(const char* key, double value);
        luabind::object getValue(const char* key, lua_State *L);
        bool isValueSet(const char* key);
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

    class Animation{
    public:
        Animation (int animationIndex);
		void mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L) const;
		short id() const;
        void setId(short id);
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
		short timer() const;
        void setTimer(short timer);
        short npcID() const;
        void setNpcID(short npcID);
        bool drawOnlyMask() const;
        void setDrawOnlyMask(bool drawOnlyMask);
        short animationFrame() const;
        void setAnimationFrame(short animationFrame);
        bool isValid() const;
    private:
        int m_animationIndex;
    };

    class Layer{
    public:
        Layer (int layerIndex);
		VBStr layerName() const;
		float speedX() const;
        void setSpeedX(float speedX);
		float speedY() const;
        void setSpeedY(float speedY);
        void stop();

		int layerIndex() const;
    private:
        int m_layerIndex;
    };

    class Section{
    public:
        Section (int sectionNum);
        RECTd boundary() const;
        void setBoundary(const RECTd &boundary);
    private:
        int m_secNum;
    };

    class NPC{
    public:
        static int count();
        static luabind::object get(lua_State* L);
        static luabind::object get(luabind::object idFilter, luabind::object sectionFilter, lua_State* L);

        NPC (int index);
		int id(lua_State* L) const;
		float direction(lua_State* L) const;
        void setDirection(float direction, lua_State* L);
		double x(lua_State* L) const;
        void setX(double x, lua_State* L);
		double y(lua_State* L) const;
        void setY(double y, lua_State* L);
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
        bool isValid() const;
		bool isValid_throw(lua_State *L) const;
		//not bound functions
		void* getNativeAddr() const;
	private:
        int m_index;
    };

    class Player{
    public:
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
		double speedX(lua_State *L) const;
        void setSpeedX(double speedX, lua_State *L);
		double speedY(lua_State *L) const;
        void setSpeedY(double speedY, lua_State *L);
		int powerup(lua_State *L) const;
        void setPowerup(int powerup, lua_State *L);
		int reservePowerup(lua_State *L) const;
        void setReservePowerup(int reservePowerup, lua_State *L);
		luabind::object holdingNPC(lua_State *L) const;
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
		short peachHoverTrembleSpeed(lua_State *L) const;
        void setPeachHoverTrembleSpeed(short var_peachHoverTrembleSpeed, lua_State *L);
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
		short waterState(lua_State *L) const;
        void setWaterState(short var_waterState, lua_State *L);
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
		short warpNearby(lua_State *L) const;
        void setWarpNearby(short var_warpNearby, lua_State *L);
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
		double height(lua_State *L) const;
        void setHeight(double var_height, lua_State *L);
		double width(lua_State *L) const;
        void setWidth(double var_width, lua_State *L);
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
		float unknown124(lua_State *L) const;
        void setUnknown124(float var_unknown124, lua_State *L);
		float unknown128(lua_State *L) const;
        void setUnknown128(float var_unknown128, lua_State *L);
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
		short warpTimer(lua_State *L) const;
        void setWarpTimer(short var_warpTimer, lua_State *L);
		short unknown15E(lua_State *L) const;
        void setUnknown15E(short var_unknown15E, lua_State *L);
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
		
    private:
        int m_index;
    };


    class Block
    {
    public:
        Block(int index);
		void mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L);
		luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L) const;
		double x() const;
        void setX(double x);
		double y() const;
        void setY(double y);
		double speedX() const;
        void setSpeedX(double speedX);
		double speedY() const;
        void setSpeedY(double speedY);
		short id() const;
        void setId(short id);
		bool slippery() const;
        void setSlippery(bool slippery);
		bool invisible() const;
        void setInvisible(bool invisible);
		int collidesWith(const Player* player) const;
		VBStr layerName() const;
		luabind::object layerObj(lua_State* L) const;
    private:
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
		short getCurrentDirection() const;
	};

	class LevelObject
	{
	public:
		LevelObject(int index);
		void mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L);
		luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L) const;
		double x() const;
		void setX(double x);
		double y() const;
		void setY(double y);
		VBStr levelTitle() const;
	private:
		bool isValid() const;
		int m_index;
	};



    //undocumented
    namespace Native{
        std::string getSMBXPath();
        void simulateError(short errcode);
    }
    //Debug/Text functions
    namespace Text{
        void windowDebug(const char* debugText);
        void print(const char *text, int x, int y);
        void print(const char *text, int type, int x, int y);
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
        VBStr getInput();
    }

    namespace Hud{
        void activate(bool activate);
        //CSprite functions
        bool loadImage(const char* filename, int resNumber, int transColor);
        void placeSprite(int type, int imgResource, int xPos, int yPos, const char* extra, int time);
        void placeSprite(int type, int imgResource, int xPos, int yPos, const char* extra);
        void placeSprite(int type, int imgResource, int xPos, int yPos);
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
#ifdef _MSC_VER
#pragma endregion
#endif
}

#endif
