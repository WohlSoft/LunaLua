// LuaHelper.h Helper Functions
#ifndef LuaProxy_Hhh
#define LuaProxy_Hhh

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

	//undocumented
	std::string getSMBXPath();

    void windowDebug(const char* debugText);
    void print(const char *text, int x, int y);
    void print(const char *text, int type, int x, int y);
    int totalNPCs();
    luabind::object npcs(lua_State *L);
    luabind::object findNPCs(int ID, int section, lua_State *L);
    luabind::object blocks(lua_State *L);
    luabind::object findblocks(int ID, lua_State *L);
    void mem(int offset, L_FIELDTYPE ftype, luabind::object value);
    luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);
    void triggerEvent(const char* evName);
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
    void loadImage(const char* filename, int resNumber, int transColor);
    void placeSprite(int type, int imgResource, int xPos, int yPos, const char* extra, int time);
    void placeSprite(int type, int imgResource, int xPos, int yPos, const char* extra);
    void placeSprite(int type, int imgResource, int xPos, int yPos);
    unsigned short gravity();
    void gravity(unsigned short value);
    unsigned short earthquake();
    void earthquake(unsigned short value);
    unsigned short jumpheight();
    void jumpheight(unsigned short value);
    unsigned short jumpheightBounce();
    void jumpheightBounce(unsigned short value);
    luabind::object findlayer(const char* layername, lua_State *L);
    void exitLevel();
    unsigned short winState();
    void winState(unsigned short value);
    luabind::object animations(lua_State* L);
	std::string getLevelFilename();
	std::string getLevelName();
	//Load hitbox values for playable character
	void loadHitboxes(int _character, int _powerup, const char *ini_file);

	//World
	luabind::object levels(lua_State *L);
	luabind::object findlevels(std::string toFindName, lua_State* L);
	luabind::object findlevel(std::string toFindName, lua_State* L);

    //for runAnimation
    struct coorStruct{
        double x;
        double y;
        double Height;
        double Width;
        double XSpeed;
        double YSpeed;
    };
    void runAnimation(int id, double x, double y, double height, double width, double speedX, double speedY, int extraData);
    void runAnimation(int id, double x, double y, double height, double width, int extraData);
    void runAnimation(int id, double x, double y, int extraData);

    void npcToCoins();
	void hud(bool activate);


    namespace SaveBankProxy {
        void setValue(const char* key, double value);
        luabind::object getValue(const char* key, lua_State *L);
        bool isValueSet(const char* key);
        luabind::object values(lua_State* L);
        void save();
    }

    struct RECTd{
        double left;
        double top;
        double right;
        double bottom;
    };

    RECT newRECT();
    RECTd newRECTd();

    class VBStr{
    public:
        VBStr (wchar_t* ptr);
        std::string str() const;
        void setStr(std::string str);
        int length();
        void setLength(int len);
        void clear();
		bool isValid() const;
		friend std::ostream& operator<<(std::ostream& os, const VBStr& wStr);

    private:
        wchar_t* m_wcharptr;
    };

	std::ostream& operator<<(std::ostream& os, const VBStr& wStr);

    class Animation{
    public:
        Animation (int animationIndex);
        void mem(int offset, L_FIELDTYPE ftype, luabind::object value);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);
        short id();
        void setId(short id);
        double x();
        void setX(double x);
        double y();
        void setY(double y);
        double speedX();
        void setSpeedX(double speedX);
        double speedY();
        void setSpeedY(double speedY);
        double height();
        void setHeight(double height);
        double width();
        void setWidth(double width);
        short timer();
        void setTimer(short timer);
    private:
        bool isValid();
        int m_animationIndex;
    };

    class Layer{
    public:
        Layer (int layerIndex);
        VBStr layerName();
        float speedX();
        void setSpeedX(float speedX);
        float speedY();
        void setSpeedY(float speedY);
        void stop();
    private:
        int m_layerIndex;
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
        int id(lua_State* L);
        float direction(lua_State* L);
        void setDirection(float direction, lua_State* L);
        double x(lua_State* L);
        void setX(double x, lua_State* L);
        double y(lua_State* L);
        void setY(double y, lua_State* L);
        double speedX(lua_State* L);
        void setSpeedX(double speedX, lua_State* L);
        double speedY(lua_State* L);
        void setSpeedY(double speedY, lua_State* L);
        void mem(int offset, L_FIELDTYPE ftype, luabind::object value, lua_State* L);
        void kill(lua_State* L);
        void kill(int killEffectID, lua_State* L);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);
        VBStr attachedLayerName(lua_State* L);
        luabind::object attachedLayerObj(lua_State *L);
        VBStr activateEventName(lua_State* L);
        VBStr deathEventName(lua_State* L);
        VBStr talkEventName(lua_State* L);
        VBStr noMoreObjInLayer(lua_State* L);
        VBStr msg(lua_State* L);
        VBStr layerName(lua_State* L);
        luabind::object layerObj(lua_State *L);
        bool isValid();
		bool isValid_throw(lua_State *L);
	private:
        int m_index;
    };

    class Player{
    public:
        Player ();
        Player (int index);
        int section(lua_State *L);
        Section sectionObj(lua_State *L);
        void kill(lua_State *L);
        void harm(lua_State *L);
        RECT screen(lua_State *L);
        double x(lua_State *L);
        void setX(double x, lua_State *L);
        double y(lua_State *L);
        void setY(double y, lua_State *L);
        double speedX(lua_State *L);
        void setSpeedX(double speedX, lua_State *L);
        double speedY(lua_State *L);
        void setSpeedY(double speedY, lua_State *L);
        int powerup(lua_State *L);
        void setPowerup(int powerup, lua_State *L);
        int reservePowerup(lua_State *L);
        void setReservePowerup(int reservePowerup, lua_State *L);
        luabind::object holdingNPC(lua_State *L);
        void mem(int offset, L_FIELDTYPE ftype, luabind::object value, lua_State *L);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);
        bool isValid();
		bool isValid_throw(lua_State *L);
        //Generated by code:
        short toadDoubleJReady(lua_State *L);
        void setToadDoubleJReady(short var_toadDoubleJReady, lua_State *L);
        short sparklingEffect(lua_State *L);
        void setSparklingEffect(short var_sparklingEffect, lua_State *L);
        short unknownCTRLLock1(lua_State *L);
        void setUnknownCTRLLock1(short var_unknownCTRLLock1, lua_State *L);
        short unknownCTRLLock2(lua_State *L);
        void setUnknownCTRLLock2(short var_unknownCTRLLock2, lua_State *L);
        short quicksandEffectTimer(lua_State *L);
        void setQuicksandEffectTimer(short var_quicksandEffectTimer, lua_State *L);
        short onSlipperyGround(lua_State *L);
        void setOnSlipperyGround(short var_onSlipperyGround, lua_State *L);
        short isAFairy(lua_State *L);
        void setIsAFairy(short var_isAFairy, lua_State *L);
        short fairyAlreadyInvoked(lua_State *L);
        void setFairyAlreadyInvoked(short var_fairyAlreadyInvoked, lua_State *L);
        short fairyFramesLeft(lua_State *L);
        void setFairyFramesLeft(short var_fairyFramesLeft, lua_State *L);
        short sheathHasKey(lua_State *L);
        void setSheathHasKey(short var_sheathHasKey, lua_State *L);
        short sheathAttackCooldown(lua_State *L);
        void setSheathAttackCooldown(short var_sheathAttackCooldown, lua_State *L);
        short hearts(lua_State *L);
        void setHearts(short var_hearts, lua_State *L);
        short peachHoverAvailable(lua_State *L);
        void setPeachHoverAvailable(short var_peachHoverAvailable, lua_State *L);
        short pressingHoverButton(lua_State *L);
        void setPressingHoverButton(short var_pressingHoverButton, lua_State *L);
        short peachHoverTimer(lua_State *L);
        void setPeachHoverTimer(short var_peachHoverTimer, lua_State *L);
        short unused1(lua_State *L);
        void setUnused1(short var_unused1, lua_State *L);
        short peachHoverTrembleSpeed(lua_State *L);
        void setPeachHoverTrembleSpeed(short var_peachHoverTrembleSpeed, lua_State *L);
        short peachHoverTrembleDir(lua_State *L);
        void setPeachHoverTrembleDir(short var_peachHoverTrembleDir, lua_State *L);
        short itemPullupTimer(lua_State *L);
        void setItemPullupTimer(short var_itemPullupTimer, lua_State *L);
        float itemPullupMomentumSave(lua_State *L);
        void setItemPullupMomentumSave(float var_itemPullupMomentumSave, lua_State *L);
        short unused2(lua_State *L);
        void setUnused2(short var_unused2, lua_State *L);
        short unkClimbing1(lua_State *L);
        void setUnkClimbing1(short var_unkClimbing1, lua_State *L);
        short unkClimbing2(lua_State *L);
        void setUnkClimbing2(short var_unkClimbing2, lua_State *L);
        short unkClimbing3(lua_State *L);
        void setUnkClimbing3(short var_unkClimbing3, lua_State *L);
        short waterState(lua_State *L);
        void setWaterState(short var_waterState, lua_State *L);
        short isInWater(lua_State *L);
        void setIsInWater(short var_isInWater, lua_State *L);
        short waterStrokeTimer(lua_State *L);
        void setWaterStrokeTimer(short var_waterStrokeTimer, lua_State *L);
        short unknownHoverTimer(lua_State *L);
        void setUnknownHoverTimer(short var_unknownHoverTimer, lua_State *L);
        short slidingState(lua_State *L);
        void setSlidingState(short var_slidingState, lua_State *L);
        short slidingGroundPuffs(lua_State *L);
        void setSlidingGroundPuffs(short var_slidingGroundPuffs, lua_State *L);
        short climbingState(lua_State *L);
        void setClimbingState(short var_climbingState, lua_State *L);
        short unknownTimer(lua_State *L);
        void setUnknownTimer(short var_unknownTimer, lua_State *L);
        short unknownFlag(lua_State *L);
        void setUnknownFlag(short var_unknownFlag, lua_State *L);
        short unknownPowerupState(lua_State *L);
        void setUnknownPowerupState(short var_unknownPowerupState, lua_State *L);
        short slopeRelated(lua_State *L);
        void setSlopeRelated(short var_slopeRelated, lua_State *L);
        short tanookiStatueActive(lua_State *L);
        void setTanookiStatueActive(short var_tanookiStatueActive, lua_State *L);
        short tanookiMorphCooldown(lua_State *L);
        void setTanookiMorphCooldown(short var_tanookiMorphCooldown, lua_State *L);
        short tanookiActiveFrameCount(lua_State *L);
        void setTanookiActiveFrameCount(short var_tanookiActiveFrameCount, lua_State *L);
        short isSpinjumping(lua_State *L);
        void setIsSpinjumping(short var_isSpinjumping, lua_State *L);
        short spinjumpStateCounter(lua_State *L);
        void setSpinjumpStateCounter(short var_spinjumpStateCounter, lua_State *L);
        short spinjumpLandDirection(lua_State *L);
        void setSpinjumpLandDirection(short var_spinjumpLandDirection, lua_State *L);
        short currentKillCombo(lua_State *L);
        void setCurrentKillCombo(short var_currentKillCombo, lua_State *L);
        short groundSlidingPuffsState(lua_State *L);
        void setGroundSlidingPuffsState(short var_groundSlidingPuffsState, lua_State *L);
        short warpNearby(lua_State *L);
        void setWarpNearby(short var_warpNearby, lua_State *L);
        short unknown5C(lua_State *L);
        void setUnknown5C(short var_unknown5C, lua_State *L);
        short unknown5E(lua_State *L);
        void setUnknown5E(short var_unknown5E, lua_State *L);
        short hasJumped(lua_State *L);
        void setHasJumped(short var_hasJumped, lua_State *L);
        double curXPos(lua_State *L);
        void setCurXPos(double var_curXPos, lua_State *L);
        double curYPos(lua_State *L);
        void setCurYPos(double var_curYPos, lua_State *L);
        double height(lua_State *L);
        void setHeight(double var_height, lua_State *L);
        double width(lua_State *L);
        void setWidth(double var_width, lua_State *L);
        double curXSpeed(lua_State *L);
        void setCurXSpeed(double var_curXSpeed, lua_State *L);
        double curYSpeed(lua_State *L);
        void setCurYSpeed(double var_curYSpeed, lua_State *L);
        short identity(lua_State *L);
        void setIdentity(short var_identity, lua_State *L);
        short uKeyState(lua_State *L);
        void setUKeyState(short var_uKeyState, lua_State *L);
        short dKeyState(lua_State *L);
        void setDKeyState(short var_dKeyState, lua_State *L);
        short lKeyState(lua_State *L);
        void setLKeyState(short var_lKeyState, lua_State *L);
        short rKeyState(lua_State *L);
        void setRKeyState(short var_rKeyState, lua_State *L);
        short jKeyState(lua_State *L);
        void setJKeyState(short var_jKeyState, lua_State *L);
        short sJKeyState(lua_State *L);
        void setSJKeyState(short var_sJKeyState, lua_State *L);
        short xKeyState(lua_State *L);
        void setXKeyState(short var_xKeyState, lua_State *L);
        short rNKeyState(lua_State *L);
        void setRNKeyState(short var_rNKeyState, lua_State *L);
        short sELKeyState(lua_State *L);
        void setSELKeyState(short var_sELKeyState, lua_State *L);
        short sTRKeyState(lua_State *L);
        void setSTRKeyState(short var_sTRKeyState, lua_State *L);
        short facingDirection(lua_State *L);
        void setFacingDirection(short var_facingDirection, lua_State *L);
        short mountType(lua_State *L);
        void setMountType(short var_mountType, lua_State *L);
        short mountColor(lua_State *L);
        void setMountColor(short var_mountColor, lua_State *L);
        short mountState(lua_State *L);
        void setMountState(short var_mountState, lua_State *L);
        short mountHeightOffset(lua_State *L);
        void setMountHeightOffset(short var_mountHeightOffset, lua_State *L);
        short mountGfxIndex(lua_State *L);
        void setMountGfxIndex(short var_mountGfxIndex, lua_State *L);
        short currentPowerup(lua_State *L);
        void setCurrentPowerup(short var_currentPowerup, lua_State *L);
        short currentPlayerSprite(lua_State *L);
        void setCurrentPlayerSprite(short var_currentPlayerSprite, lua_State *L);
        short unused116(lua_State *L);
        void setUnused116(short var_unused116, lua_State *L);
        float gfxMirrorX(lua_State *L);
        void setGfxMirrorX(float var_gfxMirrorX, lua_State *L);
        short upwardJumpingForce(lua_State *L);
        void setUpwardJumpingForce(short var_upwardJumpingForce, lua_State *L);
        short jumpButtonHeld(lua_State *L);
        void setJumpButtonHeld(short var_jumpButtonHeld, lua_State *L);
        short spinjumpButtonHeld(lua_State *L);
        void setSpinjumpButtonHeld(short var_spinjumpButtonHeld, lua_State *L);
        short forcedAnimationState(lua_State *L);
        void setForcedAnimationState(short var_forcedAnimationState, lua_State *L);
        float unknown124(lua_State *L);
        void setUnknown124(float var_unknown124, lua_State *L);
        float unknown128(lua_State *L);
        void setUnknown128(float var_unknown128, lua_State *L);
        short downButtonMirror(lua_State *L);
        void setDownButtonMirror(short var_downButtonMirror, lua_State *L);
        short inDuckingPosition(lua_State *L);
        void setInDuckingPosition(short var_inDuckingPosition, lua_State *L);
        short selectButtonMirror(lua_State *L);
        void setSelectButtonMirror(short var_selectButtonMirror, lua_State *L);
        short unknown132(lua_State *L);
        void setUnknown132(short var_unknown132, lua_State *L);
        short downButtonTapped(lua_State *L);
        void setDownButtonTapped(short var_downButtonTapped, lua_State *L);
        short unknown136(lua_State *L);
        void setUnknown136(short var_unknown136, lua_State *L);
        float xMomentumPush(lua_State *L);
        void setXMomentumPush(float var_xMomentumPush, lua_State *L);
        short deathState(lua_State *L);
        void setDeathState(short var_deathState, lua_State *L);
        short deathTimer(lua_State *L);
        void setDeathTimer(short var_deathTimer, lua_State *L);
        short blinkTimer(lua_State *L);
        void setBlinkTimer(short var_blinkTimer, lua_State *L);
        short blinkState(lua_State *L);
        void setBlinkState(short var_blinkState, lua_State *L);
        short unknown144(lua_State *L);
        void setUnknown144(short var_unknown144, lua_State *L);
        short layerStateStanding(lua_State *L);
        void setLayerStateStanding(short var_layerStateStanding, lua_State *L);
        short layerStateLeftContact(lua_State *L);
        void setLayerStateLeftContact(short var_layerStateLeftContact, lua_State *L);
        short layerStateTopContact(lua_State *L);
        void setLayerStateTopContact(short var_layerStateTopContact, lua_State *L);
        short layerStateRightContact(lua_State *L);
        void setLayerStateRightContact(short var_layerStateRightContact, lua_State *L);
        short pushedByMovingLayer(lua_State *L);
        void setPushedByMovingLayer(short var_pushedByMovingLayer, lua_State *L);
        short unused150(lua_State *L);
        void setUnused150(short var_unused150, lua_State *L);
        short unused152(lua_State *L);
        void setUnused152(short var_unused152, lua_State *L);
        short heldNPCIndex(lua_State *L);
        void setHeldNPCIndex(short var_heldNPCIndex, lua_State *L);
        short unknown156(lua_State *L);
        void setUnknown156(short var_unknown156, lua_State *L);
        short powerupBoxContents(lua_State *L);
        void setPowerupBoxContents(short var_powerupBoxContents, lua_State *L);
        short currentSection(lua_State *L);
        void setCurrentSection(short var_currentSection, lua_State *L);
        short warpTimer(lua_State *L);
        void setWarpTimer(short var_warpTimer, lua_State *L);
        short unknown15E(lua_State *L);
        void setUnknown15E(short var_unknown15E, lua_State *L);
        short projectileTimer1(lua_State *L);
        void setProjectileTimer1(short var_projectileTimer1, lua_State *L);
        short projectileTimer2(lua_State *L);
        void setProjectileTimer2(short var_projectileTimer2, lua_State *L);
        short tailswipeTimer(lua_State *L);
        void setTailswipeTimer(short var_tailswipeTimer, lua_State *L);
        short unknown166(lua_State *L);
        void setUnknown166(short var_unknown166, lua_State *L);
        float takeoffSpeed(lua_State *L);
        void setTakeoffSpeed(float var_takeoffSpeed, lua_State *L);
        short canFly(lua_State *L);
        void setCanFly(short var_canFly, lua_State *L);
        short isFlying(lua_State *L);
        void setIsFlying(short var_isFlying, lua_State *L);
        short flightTimeRemaining(lua_State *L);
        void setFlightTimeRemaining(short var_flightTimeRemaining, lua_State *L);
        short holdingFlightRunButton(lua_State *L);
        void setHoldingFlightRunButton(short var_holdingFlightRunButton, lua_State *L);
        short holdingFlightButton(lua_State *L);
        void setHoldingFlightButton(short var_holdingFlightButton, lua_State *L);
        short nPCBeingStoodOnIndex(lua_State *L);
        void setNPCBeingStoodOnIndex(short var_nPCBeingStoodOnIndex, lua_State *L);
        short unknown178(lua_State *L);
        void setUnknown178(short var_unknown178, lua_State *L);
        short unknown17A(lua_State *L);
        void setUnknown17A(short var_unknown17A, lua_State *L);
        short unused17C(lua_State *L);
        void setUnused17C(short var_unused17C, lua_State *L);
        short unused17E(lua_State *L);
        void setUnused17E(short var_unused17E, lua_State *L);
        short unused180(lua_State *L);
        void setUnused180(short var_unused180, lua_State *L);
        short unused182(lua_State *L);
        void setUnused182(short var_unused182, lua_State *L);
        short unused184(lua_State *L);
        void setUnused184(short var_unused184, lua_State *L);
        //internal use
        int getIndex();
		
    private:
        int m_index;
    };


    class Block
    {
    public:
        Block(int index);
        void mem(int offset, L_FIELDTYPE ftype, luabind::object value);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);
        double x();
        void setX(double x);
        double y();
        void setY(double y);
        double speedX();
        void setSpeedX(double speedX);
        double speedY();
        void setSpeedY(double speedY);
        short id();
        void setId(short id);
        bool slippery();
        void setSlippery(bool slippery);
        bool invisible();
        void setInvisible(bool invisible);
        int collidesWith(Player* player);
        VBStr layerName();
        luabind::object layerObj(lua_State* L);
    private:
        bool isValid();
        int m_index;
    };

    //more functions:
    VBStr getInput();

	//For world exclusive:
	class World
	{
	public:
		World();
		void mem(int offset, L_FIELDTYPE ftype, luabind::object value);
		luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);
		double playerX();
		void setPlayerX(double playerX);
		double playerY();
		void setPlayerY(double playerY);
		short currentWalkingDirection();
		void setCurrentWalkingDirection(short currentWalkingDirection);
		short currentWalkingFrame();
		void setCurrentWalkingFrame(short currentWalkingFrame);
		short currentWalkingFrameTimer();
		void setCurrentWalkingFrameTimer(short currentWalkingFrameTimer);
		short currentWalkingTimer();
		void setCurrentWalkingTimer(short currentWalkingTimer);
		bool playerIsCurrentWalking();
		luabind::object levelTitle(lua_State* L);
		short getCurrentDirection();
	};

	class LevelObject
	{
	public:
		LevelObject(int index);
		void mem(int offset, L_FIELDTYPE ftype, luabind::object value);
		luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);
		double x();
		void setX(double x);
		double y();
		void setY(double y);
		VBStr levelTitle();
	private:
		bool isValid();
		int m_index;
	};
}

#endif
