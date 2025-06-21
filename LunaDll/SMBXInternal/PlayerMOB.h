//*** PlayerMOB.h - Definition of known Player structure and Player-related framework functions ***
#ifndef PlayerMob_hhhhh
#define PlayerMob_hhhhh
#include "../Defines.h"
#include "BaseItemArray.h"
#include <cstddef>

#ifndef __MINGW32__
#pragma region Lookup
// - Player MOB Struct. size = 0x184 (388 bytes)
//
//+0x00		w	= Toad doublejump ready (FFFF = true)
//+0x02		w	= Star sparkling effect on player
//+0x04		w	= Horizontal and ducking disabled (?)
//+0x06		w	= Ducking enabled?
//+0x08		w	= Water or quicksand timer
//+0x0A		w	= Is on slippery ground

// - SHEATH
//+0x0C		w	= Is a fairy
//+0x0E		w	= Fairy already used this jump (1 = true)
//+0x10		w	= Frames until fairy runs out
//+0x12		w	= Sheath has a key
//+0x14		w	= Sheath slash cooldown timer
//+0x16		w	= # of hearts

// - PRINCESS
//+0x18		w	= Princess hover is available
//+0x1A		w	= Holding jump button
//+0x1C		w	= Princess hover timer
//+0x1E		w	= Unused (not cleared on level load)
//+0x20		f	= Princess hover Y tremble speed
//+0x24		w	= Princess hover Y tremble direction

//+0x26		w	= Ground item pull-up timer
//+0x28		f	= Ground item pull-up momentum save
//+0x2A		w	= Unused (not cleared on level load)

//+0x2C		w	= Climbing related
//+0x30		w	= Climbing related
//+0x32		w	= Climbing related

//- WATER
//+0x34		w	= 2 when in water or quicksand
//+0x36		w	= 0xFFFF when in water
//+0x38		w	= Water stroke timer (can't stroke again until 0)

//- MISC
//+0x3A		w	= Unknown hover timer
//+0x3C		w	= Is sliding
//+0x3E		w	= Is generating sliding smoke puffs
//+0x40		w	= Climbing state (3 = climbing, 2 = pushed up against edge of climbable area)
//+0x42		w	= Unknown timer42
//+0x44		w	= Unknown flag44
//+0x46		w	= Unknown46 (powerup pickup related)
//+0x48		w	= Slope modifier

//- TANOOKI SUIT
//+0x4A		w	= Tanooki suit statue flag
//+0x4C		w	= Statue transform cooldown frame timer
//+0x4E		w	= Frames spent as statue

//- SPINJUMP
//+0x50		w	= Spinjump flag (-1 = true)
//+0x52		w	= Spinjump state counter
//+0x54		w	= Spinjump land direction (will face this direction when landing)

// - STATES
//+0x56		w	= Current enemy kill combo count
//+0x58		w	= Ground sliding smoke puffs state
//+0x5A		w	= Index/id of nearby warp entrance

//+0x60		w	= Has jumped
//+0x62		w	= Unknown62

// - MOUNT 
//+0x64		w	= Yoshi has earthquake (yellow shell)
//+0x66		w	= Yoshi has flight (blue shell)
//+0x68		w	= Yoshi has fire breath (red shell)

//+0x7A		w	= Yoshi's animation frame (0-6 is left, 7-13 is right)
//+0x7C		w	= Yoshi animation frame timer (rapidly increments from 0 to 31, freezes in the air, resets to 0 when pressing down; probably used to to move body and head with the animation)
//+0x7E		w	= Mount upper X offset
//+0x80		w	= Mount upper Y offset
//+0x82		w	= Mount upper GFX index
//+0x84		w	= Mount item in mouth swallow timer
//+0x86		w	= Mount lower X offset
//+0x88		w	= Mount upper Y offset (related to yoshi tongue)
//+0x8A		w	= Mount lower GFX index
//+0x8C		w	= Unknown8C
//+0x8E		w	= Unknown8E
//+0x90		qw	= Tongue X position
//+0x98		qw	= Tongue Y position
//+0xA0		qw	= Tongue height or hitbox related
//+0xA8		qw	= Tongue height or hitbox related

// - POSITION
//+0xC0		qw	= Player X position (absolute coordinates within level)
//+0xC8		qw	= Player Y position (absolute coordinates within level)
//+0xD0		qw	= Player height or hitbox related
//+0xD8		qw	= Player width or hitbox related
//+0xE0		qw	= Player X speed
//+0xE8		qw	= Player Y speed

//+0xF0		w	= Player identity index (0 = nothing! don't use, 1 = demo, 2 = iris, 3 = princess, 5 = sheath)

/// - KEYS - 
//+0xF2		w	= U key pressing
//+0xF4		w	= D key pressing
//+0xF6		w	= L key pressing
//+0xF8		w	= R key pressing
//+0xFA		w	= J key pressing
//+0xFC		w	= SJ key pressing
//+0xFE		w	= X key pressing
//+0x100	w	= RN key pressing
//+0x102	w	= SEL key pressing
//+0x104	w	= STR key pressing

//+0x106	w	= Direction faced (-1 = left)

/// - MOUNT
//+0x108	w	= Mount identity (0 = no mount,1 = boot, 2 = clowcar, 3 = yoshi)
//+0x10A	w	= Mount color
//+0x10C	w	= Mount state
//+0x10E	w	= Mount sprite Y offset
//+0x110	w	= Mount gfx index

/// - STATES
//+0x112	w	= Current powerup
//+0x114	w	= Current player sprite index being displayed
//+0x116	w	= Unused
//+0x118	f	= X momentum assumption (used when determining how to draw the sprite)
//+0x11C	w	= Current upward jumping force (2 byte integer representation)
//+0x11E	w	= Holding jump button
//+0x120	w	= Holding spinjump button
//+0x122	w	= Forced animation state	(1 = powerup, 2 = powerdown, 3 = entering pipe, 4 = getting fire flower,
//                                           7 = entering door, 8 = invisible/immobile/intangible state (during fairy or character change poof),
//                                           500 = tanooki statue poof state)
//+0x124	qw	= Forced Animation Timer
//+0x12C	w	= Down button mirror (redundant?)
//+0x12E	w	= In ducking state
//+0x130	w	= Select button mirror (redundant?)
//+0x132	w	= Unknown powerup change related
//+0x134	w	= Down button pressed this frame (reset next frame)
//+0x136	w	= Unknown136
//+0x138	f	= X momentum push (eg. pushed by a bully)
//
//+0x13C	w	= Player death state
//+0x13E	w	= Player death animation timer
//
//+0x140	w	= Powerup blinking timer
//+0x142	w	= Powerup blinking state
//+0x144	w	= Unknown144
//
// - LAYER INTERACTION
//+0x146	w	= Bottom state (0 = not on the ground or standing on sprite, 2 = foot contact with a solid layer)
//+0x148	w	= Left state (0 = no left contact, 1 = half or pushed back by a solid layer, 2 = pushing against layer)
//+0x14A	w	= Top state (0 = no top contact, 1 = half or pushed back by a solid layer, 2 = pushing against layer)
//+0x14C	w	= Right state (0 = no right contact, 1 = half or pushed back by a solid layer, 2 = pushing against layer)
//+0x14E	w	= Pushed by a moving layer (0 = not pushed by any, 2 = being pushed to the left or right)
//+0x150	w	= Unused150
//+0x152	w	= Unused152

//+0x154	w	= Index of sprite being held (index to a specific sprite object that was generated only, -1 = can't carry anything)
//+0x156	w	= Player can grab anything setting (0xFFFF = can grab anything)
//+0x158	w	= Powerup box contents (0 = no item)
//
// - SECTIONS
//+0x15A	w	= Current section
//+0x15C	w	= Warp cooldown timer (can't warp / pipe until 0)
//+0x15E	w	= Target warp exit index/id (not reset after finished warping)
//
// - PROJECTILES / ATTACKS
//+0x160	w	= Projectile timer (fireballs, hammers, link slash..)
//+0x162	w	= Projectile timer 2 (link projectiles)
//+0x164	w	= Tail swipe timer
//+0x166	w	= Unknown166
//
// - FLIGHT
//+0x168	f	= Run speed aggregate until flight achieveable
//+0x16C	w	= Can fly
//+0x16E	w	= Is flying
//+0x170	w	= Flight time remaining
//+0x172	w	= Holding flight run button
//+0x174	w	= Holding flight button

//+0x176	w	= Index of sprite being stood on
//+0x178	w	= Unknown X momentum with sprites

//+0x17A	w	= Usually forced to -1	
//+0x17C	w	= Unused17C
//+0x17E	w	= Unused17E
//+0x180	w	= Unused180
//+0x182	w	= Unused182
//+0x184	w	= Unused184
#pragma endregion
#endif

// SMBX player structure (INCOMPLETE)
#pragma pack(push, 4)
struct PlayerMOB {
    short ToadDoubleJReady;             // +0x00
    short SparklingEffect;              // +0x02
    short UnknownCTRLLock1;             // +0x04
    short UnknownCTRLLock2;             // +0x06
    short QuicksandEffectTimer;         // +0x08
    short OnSlipperyGround;             // +0x0A

    short IsAFairy;                     // +0x0C
    short FairyAlreadyInvoked;          // +0x0E
    short FairyFramesLeft;              // +0x10
    short SheathHasKey;                 // +0x12
    short SheathAttackCooldown;         // +0x14
    short Hearts;                       // +0x16

    short PeachHoverAvailable;          // +0x18
    short PressingHoverButton;          // +0x1A
    short PeachHoverTimer;              // +0x1C
    short Unused1;                      // +0x1E
    float PeachHoverTrembleSpeed;       // +0x20
    short PeachHoverTrembleDir;         // +0x24

    short ItemPullupTimer;              // +0x26
    float ItemPullupMomentumSave;       // +0x28

    double ClimbingNPCOrBGO;            // +0x2C

    short WaterOrQuicksandState;
    short IsInWater;
    short WaterStrokeTimer;

    short UnknownHoverTimer;
    short SlidingState;
    short SlidingGroundPuffs;

    short ClimbingState;                // +0x40

    short UnknownTimer;
    short UnknownFlag;
    short UnknownPowerupState;
    short SlopeRelated;

    short TanookiStatueActive;
    short TanookiMorphCooldown;
    short TanookiActiveFrameCount;
    
    short IsSpinjumping;                // +0x50
    short SpinjumpStateCounter;
    short SpinjumpLandDirection;        // (will face this direction when landing)

    short CurrentKillCombo;
    short GroundSlidingPuffsState;
    short NearbyWarpIndex;              // Index of intersecting warp entrance
    short Unknown5C;
    short Unknown5E;
    
    short HasJumped;                    // +0x60

    short Unknown62;
    short YoshiHasEarthquake;
    short YoshiHasFlight;
    short YoshiHasFireBreath;
    short Unknown6A;
    short Unknown6C;
    short Unknown6E;
    short Unknown70;                    // +0x70
    short Unknown72;
    short YoshiTFrameCount;
    short Unknown76;
    short Unknown78;
    short MountAnimationFrame;
    short MountFrameTimer;
    short MountUpperXOffset;
    short MountUpperYOffset;            // +0x80
    short MountUpperGfxIndex;
    short MountItemInMouthSwallowTimer;
    short MountLowerXOffset;
    short MountLowerYOffset;
    short MountLowerGfxIndex;
    short Unknown8C;
    short Unknown8E;
    Momentum MountTongueMomentum;       // +0x90

    Momentum momentum;

    Characters Identity;

    KeyMap keymap;

    short FacingDirection;            // offset x106

    short MountType;
    short MountColor;
    short MountState;
    short MountHeightOffset;
    short MountGfxIndex;

    short CurrentPowerup;
    short CurrentPlayerSprite;
    short Unused116;
    float GfxMirrorX;
    short UpwardJumpingForce;
    short JumpButtonHeld;
    short SpinjumpButtonHeld;
    short ForcedAnimationState;
    double ForcedAnimationTimer;

    short DownButtonMirror;
    short InDuckingPosition;
    short SelectButtonMirror;
    short Unknown132;
    short DownButtonTapped;
    short Unknown136;
    float XMomentumPush;

    short DeathState;
    short DeathTimer;

    short BlinkTimer;
    short BlinkState;
    short Unknown144;

    short LayerStateStanding;
    short LayerStateLeftContact;
    short LayerStateTopContact;
    short LayerStateRightContact;
    short PushedByMovingLayer;
    short Unused150;
    short Unused152;

    short HeldNPCIndex;
    short Unknown156;
    short PowerupBoxContents;

    short CurrentSection;
    short WarpCooldownTimer;
    short TargetWarpIndex;

    short ProjectileTimer1;
    short ProjectileTimer2;
    short TailswipeTimer;
    short Unknown166;

    float TakeoffSpeed;
    short CanFly;
    short IsFlying;
    short FlightTimeRemaining;
    short HoldingFlightRunButton;
    short HoldingFlightButton;

    short NPCBeingStoodOnIndex;
    short Unknown178;

    short Unknown17A;
    short Unused17C;
    short Unused17E;
    short Unused180;
    short Unused182;

    static PlayerMOB* Get(int index) {
        if (index >= 1000 && index <= 1010) {
            int templateIndex = index - 1000;
            return &((PlayerMOB*)GM_PLAYERS_TEMPLATE)[templateIndex];
        }

        if (index > GM_PLAYERS_COUNT || index < 0)
            return 0;
        return &((PlayerMOB*)GM_PLAYERS_PTR)[index];
    }


};
#pragma pack(pop)

/* Verify struct is correctly sized, and also verify that a sampling of fields
 * that errors in would indicate a problem */
#ifndef __INTELLISENSE__
static_assert(offsetof(PlayerMOB, ItemPullupTimer) == 0x26, "ItemPullupTimer must be at at 0x26");
static_assert(offsetof(PlayerMOB, HasJumped) == 0x60, "HasJumped must be at at 0x60");
static_assert(offsetof(PlayerMOB, momentum.x) == 0xC0, "momentum.x must be at at 0xC0");
static_assert(offsetof(PlayerMOB, Unknown166) == 0x166, "Unknown166 must be at at 0x166");
static_assert(sizeof(PlayerMOB) == 0x184, "sizeof(PlayerMOB) must be 0x184");
#endif

// Extra player fields
struct ExtendedPlayerFields
{
    bool noblockcollision;
    bool nonpcinteraction;
    bool noplayerinteraction;
    unsigned int collisionGroup;
    // used for fixing a 1.3 slope bug
    int slidingTimeSinceOnSlope;

    // Constructor
    ExtendedPlayerFields()
    {
        Reset();
    }

    // Reset function
    void Reset()
    {
        noblockcollision = false;
        nonpcinteraction = false;
        noplayerinteraction = false;
        collisionGroup = 0u;

        slidingTimeSinceOnSlope = 0;
    }
};

namespace Player {

    /// Player functions ///

    // PLAYER ACCESS -- (Currently only returns the ptr to the main player)
    PlayerMOB* Get(int player);

    ExtendedPlayerFields* GetExtended(int index);
    void ClearExtendedFields();

    // PLAYER MANAGEMENT
    bool InternalSwap(int player1, int player2); // swaps position of two players in the object list
    bool InternalSwap(PlayerMOB* player1, PlayerMOB* player2);
    void MemSet(int offset, double value, OPTYPE operation, FIELDTYPE ftype);

    // PLAYER BUTTONS
    bool PressingDown(PlayerMOB* player);
    bool PressingUp(PlayerMOB* player);
    bool PressingLeft(PlayerMOB* player);
    bool PressingRight(PlayerMOB* player);
    bool PressingJump(PlayerMOB* player);
    bool PressingRun(PlayerMOB* player);
    bool PressingSEL(PlayerMOB* pPlayer);

    void ConsumeAllKeys(PlayerMOB* pPlayer);

    // CYCLE PLAYER
    void CycleRight(PlayerMOB* player);	// Changes player identity to the next character, or around to Demo
    void CycleLeft(PlayerMOB* player);	// Changes player identity to the previous character, or around to Sheath

    // PLAYER STATES
    bool UsesHearts(PlayerMOB* players);
    bool IsSpinjumping(PlayerMOB* player);
    bool IsInForcedAnimation(PlayerMOB* player);
    bool IsInScreenFreezingForcedState(PlayerMOB* player);
    bool IsHoldingSpriteType(PlayerMOB* player, int NPC_ID);
    int	IsStandingOnNPC(PlayerMOB* player); // Returns the index of the NPC being stood on, or 0 if not standing on one

    // FILTERS
    void FilterToFire(PlayerMOB* player);	// Lowers player's powerup state so they have, at most, a fire flower
    void FilterToBig(PlayerMOB* player);	// Lowers player's powerup state so they have, at most, mushroom big-ness
    void FilterToSmall(PlayerMOB* player);	// Lowers player's powerup state to small demo
    void FilterReservePowerup(PlayerMOB* player);
    void FilterMount(PlayerMOB* player);

    // HEARTS
    void SetHearts(PlayerMOB* player, int new_hearts);

    // ACTIONS
    void Harm(short index);
    void Kill(short index);

    RECT GetScreenPosition(PlayerMOB* player);
};

#endif
