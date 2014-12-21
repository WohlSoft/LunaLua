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
	//Load hitbox values for playable character
	void loadHitboxes(int _character, int _powerup, const char *ini_file);

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

    RECT newRECT(){
        RECT r;
        r.bottom = 0;
        r.left = 0;
        r.right = 0;
        r.top = 0;
        return r;
    }

    RECTd newRECTd(){
        RECTd r;
        r.bottom = 0.0;
        r.left = 0.0;
        r.right = 0.0;
        r.top = 0.0;
        return r;
    }

    class VBStr{
    public:
        VBStr (wchar_t* ptr);
        std::string str();
        void setStr(std::string str);
        int length();
        void setLength(int len);
        void clear();
    private:
        wchar_t* m_wcharptr;
    };

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
        void kill();
        void kill(int killEffectID);
        luabind::object mem(int offset, L_FIELDTYPE ftype, lua_State* L);
        VBStr attachedLayerName();
        luabind::object attachedLayerObj(lua_State *L);
        VBStr activateEventName();
        VBStr deathEventName();
        VBStr talkEventName();
        VBStr noMoreObjInLayer();
        VBStr msg();
        VBStr layerName();
        luabind::object layerObj(lua_State *L);
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
        //Generated by code:
        short toadDoubleJReady();
        void setToadDoubleJReady(short var_toadDoubleJReady);
        short sparklingEffect();
        void setSparklingEffect(short var_sparklingEffect);
        short unknownCTRLLock1();
        void setUnknownCTRLLock1(short var_unknownCTRLLock1);
        short unknownCTRLLock2();
        void setUnknownCTRLLock2(short var_unknownCTRLLock2);
        short quicksandEffectTimer();
        void setQuicksandEffectTimer(short var_quicksandEffectTimer);
        short onSlipperyGround();
        void setOnSlipperyGround(short var_onSlipperyGround);
        short isAFairy();
        void setIsAFairy(short var_isAFairy);
        short fairyAlreadyInvoked();
        void setFairyAlreadyInvoked(short var_fairyAlreadyInvoked);
        short fairyFramesLeft();
        void setFairyFramesLeft(short var_fairyFramesLeft);
        short sheathHasKey();
        void setSheathHasKey(short var_sheathHasKey);
        short sheathAttackCooldown();
        void setSheathAttackCooldown(short var_sheathAttackCooldown);
        short hearts();
        void setHearts(short var_hearts);
        short peachHoverAvailable();
        void setPeachHoverAvailable(short var_peachHoverAvailable);
        short pressingHoverButton();
        void setPressingHoverButton(short var_pressingHoverButton);
        short peachHoverTimer();
        void setPeachHoverTimer(short var_peachHoverTimer);
        short unused1();
        void setUnused1(short var_unused1);
        short peachHoverTrembleSpeed();
        void setPeachHoverTrembleSpeed(short var_peachHoverTrembleSpeed);
        short peachHoverTrembleDir();
        void setPeachHoverTrembleDir(short var_peachHoverTrembleDir);
        short itemPullupTimer();
        void setItemPullupTimer(short var_itemPullupTimer);
        float itemPullupMomentumSave();
        void setItemPullupMomentumSave(float var_itemPullupMomentumSave);
        short unused2();
        void setUnused2(short var_unused2);
        short unkClimbing1();
        void setUnkClimbing1(short var_unkClimbing1);
        short unkClimbing2();
        void setUnkClimbing2(short var_unkClimbing2);
        short unkClimbing3();
        void setUnkClimbing3(short var_unkClimbing3);
        short waterState();
        void setWaterState(short var_waterState);
        short isInWater();
        void setIsInWater(short var_isInWater);
        short waterStrokeTimer();
        void setWaterStrokeTimer(short var_waterStrokeTimer);
        short unknownHoverTimer();
        void setUnknownHoverTimer(short var_unknownHoverTimer);
        short slidingState();
        void setSlidingState(short var_slidingState);
        short slidingGroundPuffs();
        void setSlidingGroundPuffs(short var_slidingGroundPuffs);
        short climbingState();
        void setClimbingState(short var_climbingState);
        short unknownTimer();
        void setUnknownTimer(short var_unknownTimer);
        short unknownFlag();
        void setUnknownFlag(short var_unknownFlag);
        short unknownPowerupState();
        void setUnknownPowerupState(short var_unknownPowerupState);
        short slopeRelated();
        void setSlopeRelated(short var_slopeRelated);
        short tanookiStatueActive();
        void setTanookiStatueActive(short var_tanookiStatueActive);
        short tanookiMorphCooldown();
        void setTanookiMorphCooldown(short var_tanookiMorphCooldown);
        short tanookiActiveFrameCount();
        void setTanookiActiveFrameCount(short var_tanookiActiveFrameCount);
        short isSpinjumping();
        void setIsSpinjumping(short var_isSpinjumping);
        short spinjumpStateCounter();
        void setSpinjumpStateCounter(short var_spinjumpStateCounter);
        short spinjumpLandDirection();
        void setSpinjumpLandDirection(short var_spinjumpLandDirection);
        short currentKillCombo();
        void setCurrentKillCombo(short var_currentKillCombo);
        short groundSlidingPuffsState();
        void setGroundSlidingPuffsState(short var_groundSlidingPuffsState);
        short warpNearby();
        void setWarpNearby(short var_warpNearby);
        short unknown5C();
        void setUnknown5C(short var_unknown5C);
        short unknown5E();
        void setUnknown5E(short var_unknown5E);
        short hasJumped();
        void setHasJumped(short var_hasJumped);
        double curXPos();
        void setCurXPos(double var_curXPos);
        double curYPos();
        void setCurYPos(double var_curYPos);
        double height();
        void setHeight(double var_height);
        double width();
        void setWidth(double var_width);
        double curXSpeed();
        void setCurXSpeed(double var_curXSpeed);
        double curYSpeed();
        void setCurYSpeed(double var_curYSpeed);
        short identity();
        void setIdentity(short var_identity);
        short uKeyState();
        void setUKeyState(short var_uKeyState);
        short dKeyState();
        void setDKeyState(short var_dKeyState);
        short lKeyState();
        void setLKeyState(short var_lKeyState);
        short rKeyState();
        void setRKeyState(short var_rKeyState);
        short jKeyState();
        void setJKeyState(short var_jKeyState);
        short sJKeyState();
        void setSJKeyState(short var_sJKeyState);
        short xKeyState();
        void setXKeyState(short var_xKeyState);
        short rNKeyState();
        void setRNKeyState(short var_rNKeyState);
        short sELKeyState();
        void setSELKeyState(short var_sELKeyState);
        short sTRKeyState();
        void setSTRKeyState(short var_sTRKeyState);
        short facingDirection();
        void setFacingDirection(short var_facingDirection);
        short mountType();
        void setMountType(short var_mountType);
        short mountColor();
        void setMountColor(short var_mountColor);
        short mountState();
        void setMountState(short var_mountState);
        short mountHeightOffset();
        void setMountHeightOffset(short var_mountHeightOffset);
        short mountGfxIndex();
        void setMountGfxIndex(short var_mountGfxIndex);
        short currentPowerup();
        void setCurrentPowerup(short var_currentPowerup);
        short currentPlayerSprite();
        void setCurrentPlayerSprite(short var_currentPlayerSprite);
        short unused116();
        void setUnused116(short var_unused116);
        float gfxMirrorX();
        void setGfxMirrorX(float var_gfxMirrorX);
        short upwardJumpingForce();
        void setUpwardJumpingForce(short var_upwardJumpingForce);
        short jumpButtonHeld();
        void setJumpButtonHeld(short var_jumpButtonHeld);
        short spinjumpButtonHeld();
        void setSpinjumpButtonHeld(short var_spinjumpButtonHeld);
        short forcedAnimationState();
        void setForcedAnimationState(short var_forcedAnimationState);
        float unknown124();
        void setUnknown124(float var_unknown124);
        float unknown128();
        void setUnknown128(float var_unknown128);
        short downButtonMirror();
        void setDownButtonMirror(short var_downButtonMirror);
        short inDuckingPosition();
        void setInDuckingPosition(short var_inDuckingPosition);
        short selectButtonMirror();
        void setSelectButtonMirror(short var_selectButtonMirror);
        short unknown132();
        void setUnknown132(short var_unknown132);
        short downButtonTapped();
        void setDownButtonTapped(short var_downButtonTapped);
        short unknown136();
        void setUnknown136(short var_unknown136);
        float xMomentumPush();
        void setXMomentumPush(float var_xMomentumPush);
        short deathState();
        void setDeathState(short var_deathState);
        short deathTimer();
        void setDeathTimer(short var_deathTimer);
        short blinkTimer();
        void setBlinkTimer(short var_blinkTimer);
        short blinkState();
        void setBlinkState(short var_blinkState);
        short unknown144();
        void setUnknown144(short var_unknown144);
        short layerStateStanding();
        void setLayerStateStanding(short var_layerStateStanding);
        short layerStateLeftContact();
        void setLayerStateLeftContact(short var_layerStateLeftContact);
        short layerStateTopContact();
        void setLayerStateTopContact(short var_layerStateTopContact);
        short layerStateRightContact();
        void setLayerStateRightContact(short var_layerStateRightContact);
        short pushedByMovingLayer();
        void setPushedByMovingLayer(short var_pushedByMovingLayer);
        short unused150();
        void setUnused150(short var_unused150);
        short unused152();
        void setUnused152(short var_unused152);
        short heldNPCIndex();
        void setHeldNPCIndex(short var_heldNPCIndex);
        short unknown156();
        void setUnknown156(short var_unknown156);
        short powerupBoxContents();
        void setPowerupBoxContents(short var_powerupBoxContents);
        short currentSection();
        void setCurrentSection(short var_currentSection);
        short warpTimer();
        void setWarpTimer(short var_warpTimer);
        short unknown15E();
        void setUnknown15E(short var_unknown15E);
        short projectileTimer1();
        void setProjectileTimer1(short var_projectileTimer1);
        short projectileTimer2();
        void setProjectileTimer2(short var_projectileTimer2);
        short tailswipeTimer();
        void setTailswipeTimer(short var_tailswipeTimer);
        short unknown166();
        void setUnknown166(short var_unknown166);
        float takeoffSpeed();
        void setTakeoffSpeed(float var_takeoffSpeed);
        short canFly();
        void setCanFly(short var_canFly);
        short isFlying();
        void setIsFlying(short var_isFlying);
        short flightTimeRemaining();
        void setFlightTimeRemaining(short var_flightTimeRemaining);
        short holdingFlightRunButton();
        void setHoldingFlightRunButton(short var_holdingFlightRunButton);
        short holdingFlightButton();
        void setHoldingFlightButton(short var_holdingFlightButton);
        short nPCBeingStoodOnIndex();
        void setNPCBeingStoodOnIndex(short var_nPCBeingStoodOnIndex);
        short unknown178();
        void setUnknown178(short var_unknown178);
        short unknown17A();
        void setUnknown17A(short var_unknown17A);
        short unused17C();
        void setUnused17C(short var_unused17C);
        short unused17E();
        void setUnused17E(short var_unused17E);
        short unused180();
        void setUnused180(short var_unused180);
        short unused182();
        void setUnused182(short var_unused182);
        short unused184();
        void setUnused184(short var_unused184);
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

}
