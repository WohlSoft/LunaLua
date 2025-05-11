//*** NPCs.h - Definition of known NPC structure and NPC-related framework functions ***
#ifndef NPCs_hhhh
#define NPCs_hhhh

#include <list>
#include <vector>
#include <string>
#include "../Defines.h"
#include "../Misc/VB6StrPtr.h"
#include "../Misc/CollisionMatrix.h"


enum NPCID : short
{
    NPCID_GOOMBA_SMB3 = 1,
    NPCID_REDGOOMBA = 2,
    NPCID_REDPARAGOOMBA = 3,
    NPCID_GRNTROOPA_SMB3 = 4,
    NPCID_GRNSHELL_SMB3 = 5,
    NPCID_REDTROOPA_SMB3 = 6,
    NPCID_REDSHELL_SMB3 = 7,
    NPCID_PIRHANA_SMB3 = 8,
    NPCID_SHROOM_SMB3 = 9,
    NPCID_COIN_SMB3 = 10,

    NPCID_ITEMGOAL = 11,
    NPCID_PODOBOO = 12,
    NPCID_PLAYERFIREBALL = 13,
    NPCID_FIREFLOWER_SMB3 = 14,
    NPCID_BOOMBOOM = 15,
    NPCID_GOALORB_SMB3 = 16,
    NPCID_BULLET_SMB3 = 17,
    NPCID_BULLET_SMW = 18,
    NPCID_BLUSHYGUY = 19,
    NPCID_REDSHYGUY = 20,

    NPCID_CANNONENEMY = 21,
    NPCID_CANNONITEM = 22,
    NPCID_BUZZYBEETLE = 23,
    NPCID_BUZZYSHELL = 24,
    NPCID_NINJI_SMB2 = 25,
    NPCID_SPRING = 26,
    NPCID_UNDERGOOMBA = 27,
    NPCID_GRNCHEEP = 28,
    NPCID_HAMBRO = 29,
    NPCID_ENEMYHAMMER = 30,

    NPCID_KEY = 31,
    NPCID_PSWITCH_SMW = 32,
    NPCID_COIN_SMW = 33,
    NPCID_LEAF = 34,
    NPCID_GRNBOOT = 35,
    NPCID_SPINY_SMB3 = 36,
    NPCID_THWOMP_SMB3 = 37,
    NPCID_BOO_SMB3 = 38,
    NPCID_BIRDO = 39,
    NPCID_BIRDOEGG = 40,

    NPCID_GOALORB_SMB2 = 41,
    NPCID_EERIE = 42,
    NPCID_BOO_SMW = 43,
    NPCID_BIGBOO = 44,
    NPCID_ICE_BLOCK = 45,       NPCID_THROWBLOCK = NPCID_ICE_BLOCK,
    NPCID_DONUTBLOCK_RED = 46,  NPCID_REDDONUT = NPCID_DONUTBLOCK_RED,
    NPCID_LAKITU_SMB3 = 47,
    NPCID_SPINYBALL_SMB3 = 48,
    NPCID_PIRHANAITEM = 49,
    NPCID_BLUPIRHANA = 50,

    NPCID_BOTTOMPIRHANA = 51,
    NPCID_SIDEPIRHANA = 52,
    NPCID_CRAB = 53,
    NPCID_FLY = 54,
    NPCID_EXTKOOPA = 55,
    NPCID_CLOWNCAR = 56,
    NPCID_CONVEYER = 57,
    NPCID_METALBARREL = 58,
    NPCID_YELSWITCHGOOMBA = 59,
    NPCID_YELBLOCKS = 60,

    NPCID_BLUSWITCHGOOMBA = 61,
    NPCID_BLUBLOCKS = 62,
    NPCID_GRNSWITCHGOOMBA = 63,
    NPCID_GRNBLOCKS = 64,
    NPCID_REDSWITCHGOOMBA = 65,
    NPCID_REDBLOCKS = 66,
    NPCID_HPIPE_SHORT = 67,
    NPCID_HPIPE_LONG = 68,
    NPCID_VPIPE_SHORT = 69,
    NPCID_VPIPE_LONG = 70,

    NPCID_BIGGOOMBA = 71,
    NPCID_BIGTROOPA = 72,
    NPCID_BIGSHELL = 73,
    NPCID_BIGPIRHANA = 74,
    NPCID_TOAD_A = 75,
    NPCID_GRNPARA_SMB3 = 76,
    NPCID_NINJI_SMW = 77,
    NPCID_TANKTREADS = 78,
    NPCID_SHORTWOOD = 79,
    NPCID_LONGWOOD = 80,

    NPCID_SLANTWOOD_L = 81,
    NPCID_SLANTWOOD_R = 82,
    NPCID_SLANTWOOD_M = 83,
    NPCID_STATUE_SMB3 = 84,
    NPCID_EXT_FIRE_B = 85,
    NPCID_BOWSER_SMB3 = 86,
    NPCID_EXT_FIRE_A = 87,
    NPCID_COIN_SMB = 88,
    NPCID_GOOMBA_SMB = 89,
    NPCID_LIFE_SMB3 = 90,

    NPCID_BURIEDPLANT = 91,
    NPCID_VEGGIE_TURNIP = 92,
    NPCID_PIRHANA_SMB = 93,
    NPCID_TOAD_B = 94,
    NPCID_YOSHI_GREEN = 95,
    NPCID_YOSHIEGG = 96,
    NPCID_STAR_SMB3 = 97,
    NPCID_YOSHI_BLUE = 98,
    NPCID_YOSHI_YELLOW = 99,
    NPCID_YOSHI_RED = 100,

    NPCID_LUIGI = 101,
    NPCID_LINK = 102,
    NPCID_RED_COIN = 103,       NPCID_REDCOIN = NPCID_RED_COIN,
    NPCID_PLATFORM_SMB3 = 104,
    NPCID_CHECKERPLATFORM = 105,
    NPCID_PLATFORM_SMB = 106,
    NPCID_PINKBOBOMB = 107,
    NPCID_YOSHIFIRE = 108,
    NPCID_GRNTROOPA_SMW = 109,
    NPCID_REDTROOPA_SMW = 110,

    NPCID_BLUTROOPA = 111,
    NPCID_YELTROOPA = 112,
    NPCID_GRNSHELL_SMW = 113,
    NPCID_REDSHELL_SMW = 114,
    NPCID_BLUSHELL = 115,
    NPCID_YELSHELL = 116,
    NPCID_GRNKOOPA = 117,
    NPCID_REDKOOPA = 118,
    NPCID_BLUKOOPA = 119,
    NPCID_YELKOOPA = 120,

    NPCID_GRNPARA_SMW = 121,
    NPCID_REDPARA_SMW = 122,
    NPCID_BLUPARA = 123,
    NPCID_YELPARA = 124,
    NPCID_WOSU = 125,       NPCID_KNIGHT = NPCID_WOSU,
    NPCID_BIT_BLUE = 126,   NPCID_SLIME_B = NPCID_BIT_BLUE,
    NPCID_BOT_CYAN = 127,   NPCID_SLIME_A = NPCID_BOT_CYAN,
    NPCID_BIT = 128,        NPCID_SLIME_C = NPCID_BIT, NPCID_BIT_RED = NPCID_BIT,
    NPCID_TWEETER = 129,
    NPCID_REDSNIFIT = 130,

    NPCID_BLUSNIFIT = 131,
    NPCID_GRYSNIFIT = 132,
    NPCID_CANNONBALL = 133,
    NPCID_BOMB = 134,
    NPCID_BOBOMB_SMB2 = 135,
    NPCID_BOBOMB_SMB3 = 136,
    NPCID_ACTIVEBOBOMB_SMB3 = 137,
    NPCID_COIN_SMB2 = 138,
    NPCID_VEGGIE_ONION = 139,
    NPCID_VEGGIE_TURNIP_2 = 140,

    NPCID_VEGGIE_SPROUT = 141,
    NPCID_VEGGIE_PUMPKIN = 142,
    NPCID_VEGGIE_RADISH_SPROUT = 143,
    NPCID_VEGGIE_TURNIP_SPROUT = 144,
    NPCID_VEGGIE_TURNIP_SPROUT_2 = 145,
    NPCID_VEGGIE_RADISH = 146,
    NPCID_VEGGIE_RANDOM = 147,
    NPCID_YOSHI_BLACK = 148,
    NPCID_YOSHI_PURPLE = 149,
    NPCID_YOSHI_PINK = 150,

    NPCID_SIGN = 151,
    NPCID_RING_MISC = 152,
    NPCID_SHROOMBLOCK_A = 154,
    NPCID_SHROOMBLOCK_B = 155,
    NPCID_SHROOMBLOCK_C = 156,
    NPCID_SHROOMBLOCK_D = 157,
    NPCID_SATURN = 158,
    NPCID_QUICKSAND = 159,
    NPCID_PSHROOM = 159,
    NPCID_ROCKETWOOD = 160,

    NPCID_REDPARA_SMB3 = 161,
    NPCID_REX_A = 162,
    NPCID_REX_B = 163,
    NPCID_BIGMOLE = 164,
    NPCID_GOOMBA_SMW = 165,
    NPCID_GOOMBA_SMW_HELD = 166,
    NPCID_PARAGOOMBA_SMW = 167,
    NPCID_BULLY = 168,
    NPCID_TANOOKISUIT = 169,
    NPCID_HAMMERSUIT = 170,

    NPCID_PLAYERHAMMER = 171,
    NPCID_GRNSHELL_SMB = 172,
    NPCID_GRNTROOPA_SMB = 173,
    NPCID_REDSHELL_SMB = 174,
    NPCID_REDTROOPA_SMB = 175,
    NPCID_GRNPARA_SMB = 176,
    NPCID_REDPARA_SMB = 177,
    NPCID_AXE = 178,
    NPCID_SAW = 179,
    NPCID_THWOMP_SMW = 180,

    NPCID_STATUE_SMW = 181,
    NPCID_FIREFLOWER_SMB = 182,
    NPCID_FIREFLOWER_SMW = 183,
    NPCID_SHROOM_SMB = 184,
    NPCID_SHROOM_SMW = 185,
    NPCID_LIFE_SMB = 186,
    NPCID_LIFE_SMW = 187,
    NPCID_MOON = 188,
    NPCID_DRYBONES = 189,
    NPCID_SKULL = 190,

    NPCID_REDBOOT = 191,
    NPCID_CHECKPOINT = 192,
    NPCID_BLUBOOT = 193,
    NPCID_DISCOSHELL = 194,
    NPCID_FLIPPEDDISCO = 195,
    NPCID_STAR_SMW = 196,
    NPCID_GOALTAPE = 197,
    NPCID_PRINCESS = 198,
    NPCID_BLARGG = 199,
    NPCID_BOWSER_SMB = 200,

    NPCID_WART = 201,
    NPCID_WARTBUBBLE = 202,
    NPCID_METROID_RIPPER = 203,        NPCID_METROID_A = NPCID_METROID_RIPPER,
    NPCID_METROID_ROCKET_RIPPER = 204, NPCID_METROID_B = NPCID_METROID_ROCKET_RIPPER,
    NPCID_METROID_ZOOMER = 205,        NPCID_METROID_C = NPCID_METROID_ZOOMER,
    NPCID_SPARK = 206,
    NPCID_SPIKE_TOP = 207, NPCID_SPIKEBEETLE = NPCID_SPIKE_TOP,
    NPCID_BOSSGLASS = 208,
    NPCID_MOTHERBRAIN = 209,
    NPCID_RINKA = 210,

    NPCID_RINKAGEN = 211,
    NPCID_DONUTBLOCK_BROWN = 212,       NPCID_BLUDONUT = NPCID_DONUTBLOCK_BROWN,
    NPCID_GRNVINE_SMB3 = 213,
    NPCID_REDVINE_SMB3 = 214,
    NPCID_GRNVINE_SMB2 = 215,
    NPCID_YELVINE = 216,
    NPCID_BLUVINE = 217,
    NPCID_GRNVINEBOTTOM = 218,
    NPCID_YELVINEBOTTOM = 219,
    NPCID_BLUVINEBOTTOM = 220,

    NPCID_LADDER = 221,
    NPCID_GRNVINE_SMB = 222,
    NPCID_GRNVINETOP_SMB = 223,
    NPCID_GRNVINE_SMW = 224,
    NPCID_REDVINETOP_SMB3 = 225,
    NPCID_GRNVINETOP_SMB3 = 226,
    NPCID_GRNVINETOP_SMW = 227,
    NPCID_YOSHI_CYAN = 228,
    NPCID_GREENCHEEPCHEEP = 229,
    NPCID_REDCHEEPCHEEP = 230,

    NPCID_BLOOPER_SMB3 = 231,
    NPCID_GOGGLEFISH = 232,
    NPCID_REDCHEEP = 233,
    NPCID_BONEFISH = 234,
    NPCID_BLOOPER = 235,
    NPCID_SMWCHEEP = 236,
    NPCID_ICEBLOCK = 237,
    NPCID_PSWITCH_SMB3 = 238,
    NPCID_DYNAMITE_PLUNGER = 239, NPCID_TNT = NPCID_DYNAMITE_PLUNGER,
    NPCID_TIMER_SMB2 = 240,

    NPCID_POW = 241,
    NPCID_GOOMBA_MISC = 242,
    NPCID_PARAGOOMBA_MISC = 243,
    NPCID_PARAGOOMBA_SMB3 = 244,
    NPCID_FIREPIRHANA = 245,
    NPCID_EXT_FIRE_D = 246,
    NPCID_POKEY = 247,
    NPCID_TIMER_SMB3 = 248,
    NPCID_SHROOM_SMB2 = 249,
    NPCID_HEART = 250,

    NPCID_GRNRUPEE = 251,
    NPCID_BLURUPEE = 252,
    NPCID_REDRUPEE = 253,
    NPCID_FAIRY_PENDANT = 254, NPCID_AMULET = NPCID_FAIRY_PENDANT,
    NPCID_LOCKDOOR = 255,
    NPCID_LONGPIRHANA_UP = 256,
    NPCID_LONGPIRHANA_DOWN = 257,
    NPCID_BLUECOIN = 258,
    NPCID_ROTODISK = 259,
    NPCID_FIREBAR = 260,

    NPCID_NIPPER_PLANT = 261, NPCID_MUNCHER = NPCID_NIPPER_PLANT,
    NPCID_MOUSER = 262,
    NPCID_ICE_CUBE = 263, NPCID_STATIC = NPCID_ICE_CUBE,
    NPCID_ICEFLOWER_SMB3 = 264,
    NPCID_PLAYERICEBALL = 265,
    NPCID_SWORDBEAM = 266,
    NPCID_LARRY = 267,
    NPCID_LARRYSHELL = 269,
    NPCID_RING_EXT = 269,
    NPCID_PIRHANAHEAD = 270,

    NPCID_SWOOPER = 271, NPCID_BAT = NPCID_SWOOPER,
    NPCID_HOOPSTER = 272,
    NPCID_Q_SHROOM = 273,
    NPCID_DRAGONCOIN = 274,
    NPCID_VOLCANO_LOTUS = 275,          NPCID_LAVALOTUS = NPCID_VOLCANO_LOTUS,
    NPCID_VOLCANO_LOTUS_FIREBALL = 276, NPCID_EXT_FIRE_E = NPCID_VOLCANO_LOTUS_FIREBALL,
    NPCID_ICEFLOWER_SMW = 277,
    NPCID_PROPELLERBLOCK = 278,
    NPCID_PROPELLERCANNON = 279,
    NPCID_LUDWIG = 280,

    NPCID_LUDWIG_SHELL = 281, NPCID_LUDWIGSHELL = NPCID_LUDWIG_SHELL,
    NPCID_LUDWIG_FIRE = 282, NPCID_EXT_FIRE_C = NPCID_LUDWIG_FIRE,
    NPCID_BUBBLE = 283,
    NPCID_LAKITU_SMW = 284,
    NPCID_SPINY_SMW = 286,
    NPCID_SPINYBALL_SMW = 286,
    NPCID_RANDOM_POWERUP = 287,
    NPCID_POTION = 288,
    NPCID_POTIONDOOR = 289,
    NPCID_COCKPIT = 290,

    NPCID_PEACHBOMB = 291,
    NPCID_BOOMERANG = 292,
};



// -- NPC structure -- ( 0x158 bytes )
// 0x+00	pt	= wchar_t* Attached layer name
// 0x+04	w	= Unknown
// 0x+06	w	= Unknown decrementing timer

// +0x28    w   = Should render mask only

// +0x2C	pt  = wchar_t* Activate event layer name
// +0x30	pt	= wchar_t* Death event layer name
// +0x34	pt	= wchar_t* Talk event name?
// +0x38	pt	= wchar_t* No More Objs event layer name
// +0x3C	pt	= wchar_t* Layer name

// 0x+40	w	= If the NPC is hidden. (Layer based)
// 0x+44	w	= Activated / interacted with player flag
// 0x+46	w	= Friendly (on = 0xFFFF)
// 0x+48	w	= Don't Move (on = 0xFFFF)

// 0x+4C	pt	= wchar_t* NPC Message text

// 0x+64	w	= Is a generator
// 0x+68	f	= Generator delay setting
// 0x+6A	w	- Generator firing rate
// 0x+6C	f	= Generator delay countdown
// 0x+70	w	= Direction to generate NPC?
// 0x+72	w	= The generator type (1 = Warp; 2 = Projectile)
// 0x+74	w	= Invalidity or offscreen flag?

// 0x+78	qw	= X position
// 0x+80	qw	= Y position
// 0x+88	qw	= Height
// 0x+90	qw	= Width
// 0x+98	qw	= X speed
// 0x+A0	qw	= Y speed
// 0x+AC	dw	= Spawn X
// 0x+B4	dw	= Spawn Y
// 0x+B8    dw  = GFX Height
// 0x+C0    dw  = GFX Width

// 0x+D8    f   = Spawn Direction
// 0x+DC    w   = Spawn ID

// 0x+E2	w	= Sprite GFX index/Identity   // x011C = lakitu
// 0x+E4	w	= Animation frame

// 0x+E8	f	= Animation timer
// 0x+EC    f   = Direciton faced

// 0x+F0    dq  = AI variable 1
// 0x+F8    dq  = AI variable 2
// 0x+100   dq  = AI variable 3
// 0x+108   dq  = AI variable 4
// 0x+110   dq  = AI variable 5

// 0x+118	f	= Direction faced
// 0x+122	w	= Kill/Kill effect (0 = No kill/ 1-? Kill effect with kill)
// 0x+124	w	= Unknown (same effect as 0x+128; -1 = offscreen)
// 0x+128	dw	= Ofscreen flag (0xFFFFFFFF = offscreen)
// 0x+12A	w	= Ofscreen countdown timer?
// 0x+12C	w	= Grabbing Player Index
// 0x+12E	w	= Grab timer

// 0x+136	w	= Collides with NPCs
// 0x+138   w   = Contained within
//
// 0x+146	w	= Current section this NPC is on
// 0x+148   f   = Hit Count
//
// 0x+14E   w   = ID of the block they were created from due to p-switch
//
// 0x+152	w	= NPC Collision (Seems to be set to -1 for a bullet when it destroys another NPC. May apply to all 0x136-related collisions but untested.)
//
// 0x+156	w	= Invincibility frames

#pragma pack(push, 4)
struct NPCMOB {
    VB6StrPtr attachedLayerName;            //+0x00
    short unknown_04;                       //+0x04
    short unknown_06;                       //+0x06 Unknown decrementing Timer
    short unknown_08;                       //+0x08 
    short collidesBelow;                    //+0x0A
    short collidesLeft;                     //+0x0C
    short collidesAbove;                    //+0x0E
    short collidesRight;                    //+0x10
    short crushPinched;                     //+0x12
    short unknown_14;                       //+0x14
    short unknown_16;                       //+0x16
    float unknown_18;                       //+0x18 NPC x-axis movement related, maybe temporary?
    short isUnderwater;                     //+0x1C
    short unknown_1E;                       //+0x1E
    short unknown_20;                       //+0x20
    short unknown_22;                       //+0x22
    short scoreMultiplier;                  //+0x24
    short invincibilityToSword;             //+0x26
    short isMaskOnly;                       //+0x28
    short unknown_2A;                       //+0x2A
    VB6StrPtr activateEventLayerName;       //+0x2C
    VB6StrPtr deathEventName;               //+0x30
    VB6StrPtr talkEventName;                //+0x34
    VB6StrPtr noMoreObjInLayerEventName;    //+0x38
    VB6StrPtr layerName;                    //+0x3C
    short isHidden;                         //+0x40
    short legacyBoss;                       //+0x42
    short activated;                        //+0x44
    short friendly;                         //+0x46
    short dontMove;                         //+0x48
    short dontMove2;                        //+0x4A
    VB6StrPtr talkMsg;                      //+0x4C
    short unknown_50;                       //+0x50
    short unknown_52;                       //+0x52
    short unknown_54;                       //+0x54
    short crushMovingPinched;               //+0x56
    short unknown_58;                       //+0x58
    short unknown_5A;                       //+0x5A
    short unknown_5C;                       //+0x5C
    short unknown_5E;                       //+0x5E
    short unknown_60;                       //+0x60
    short unknown_62;                       //+0x62
    short isGenerator;                      //+0x64
    short unknown_66;                       //+0x66
    float generatorDelaySetting;            //+0x68
    float generatorDelayCountdown;          //+0x6C
    short directionToGenerate;              //+0x70
    short generatorType;                    //+0x72
    short generatorActiveFlag;              //+0x74
    short unknown_76;                       //+0x76
    Momentum momentum;                      //+0x78
    Momentum spawnMomentum;                 //+0xA8
    float spawnDirection;                   //+0xD8
    short spawnID;                          //+0xDC
    short ai1_initial;                       //+0xDE
    short ai2_initial;                       //+0xE0
    short id;                               //+0xE2
    short animationFrame;                   //+0xE4
    short unknown_E6;                       //+0xE6
    float animationTimer;                   //+0xE8
    float directionFaced;                   //+0xEC
    double ai1;                             //+0xF0
    double ai2;                             //+0xF8
    double ai3;                             //+0x100
    double ai4;                             //+0x108
    double ai5;                             //+0x110
    float directionFaced2;                  //+0x118
    short unknown_11C;                      //+0x11C
    short unknown_11E;                      //+0x11E
    short bounceOffBlock;                   //+0x120
    short killFlag;                         //+0x122
    short activeFlag;                       //+0x124  Note: Must be set to -1 when spawning a new NPC
    short offscreenFlag1;                   //+0x126
    short offscreenFlag2;                   //+0x128
    short offscreenCountdownTimer;          //+0x12A
    short grabbingPlayerIndex;              //+0x12C
    short cantHurtTimer;                    //+0x12E
    short cantHurtPlayerIndex;              //+0x130
    short unknown_132;                      //+0x132
    short unknown_134;                      //+0x134
    short collidesWithNPC;                  //+0x136
    short containedWithin;                  //+0x138
    short unknown_13A;                      //+0x13A
    double effect2;                         //+0x13c-142
    //short unknown_13C;                      //+0x13C
    //short unknown_13E;                      //+0x13E
    //short unknown_140;                      //+0x140
    //short unknown_142;                      //+0x142
    short unknown_144;                      //+0x144
    short currentSection;                   //+0x146
    float hitCount;                         //+0x148
    short unknown_14C;                      //+0x14C
    short pSwitchTransformedBlockID;        //+0x14E
    short unknown_150;                      //+0x150
    short npcCollisionFlag;                 //+0x152
    short unknown_154;                      //+0x154
    short invincibilityFrames;              //+0x156
};
#pragma pack(pop)

/* Verify NPC struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(NPCMOB) == 0x158, "sizeof(NPCMOB) must be 0x158");
#endif

// Extra NPC fields
struct ExtendedNPCFields
{
    bool noblockcollision;
    bool nonpccollision;
    short fullyInsideSection;
    unsigned int collisionGroup;

    // Constructor
    ExtendedNPCFields()
    {
        Reset();
    }

    // Reset function
    void Reset()
    {
        noblockcollision = false;
        fullyInsideSection = -1;
        collisionGroup = 0u;
        nonpccollision = false;
    }
};

namespace NPC {
    /// Player functions ///

    // NPC ACCESS
    NPCMOB* Get(int index); //Get ptr to an NPC
    NPCMOB* GetRaw(int index);
    NPCMOB* GetDummyNPC();
    NPCMOB* GetFenceDummyNPC();
    NPCMOB* GetFirstMatch(int ID, int section); // Get first NPC found with the set ID and section, or 0 if no matches. -1 for ANY

    ExtendedNPCFields* GetRawExtended(int index);
    void ClearExtendedFields();

    void FindAll(int target, int internal_section, std::list<NPCMOB*>* return_list);

    short GetSection(NPCMOB* npc);

    void MemSet(int ID, int offset, double value, OPTYPE operation, FIELDTYPE ftype); // ID -1 for ALL

    // STATES
    void SetHits(NPCMOB* npc, float hits); // Set hits of passed NPC
    void FaceDirection(NPCMOB* npc, float direction);

    // ITERATORS
    void AllSetHits(int identity, int section, float hits);		// Set all specified NPC hits
    void AllFace(int identity, int section, double x);	// All specified NPCs face the supplied x/y point

    short HarmCombo(short npcIdx, short harmType, short multiplier);
    short HarmComboWithDamage(short npcIdx, short harmType, short multiplier, float damage);

    void Collect(short npcIdx, short playerIdx);

    void InitProperties();
    uint32_t GetVulnerableHarmTypes(int id);
    bool GetSpinjumpSafe(int id);
    bool GetNoWaterPhysics(int id);
    bool GetHarmlessGrab(int id);
    bool GetHarmlessThrown(int id);
    bool GetIgnoreThrownNPCs(int id);
    bool GetLinkShieldable(int id);
    bool GetNoShieldFireEffect(int id);
    bool GetNotCoinTransformable(int id);
    bool GetStaticDirection(int id);
    bool GetLuaHandlesSpeed(int id);
    double GetTerminalVelocity(int id);
    bool GetFallOffVineOnStomp(int id);

    uintptr_t GetPropertyTableAddress(const std::string& s);

    bool CheckSemisolidCollidingFlyType(unsigned int flyType);
    void SetSemisolidCollidingFlyType(unsigned int flyType, bool shouldCollide);

    // NOTE: This is the value we are patching it to. Originally 300
    static const short MAX_ID = 1000;
}

#endif

