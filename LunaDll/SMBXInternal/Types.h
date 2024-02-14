#if !defined(SMBXINTERNAL_TYPES_H)
#define SMBXINTERNAL_TYPES_H

#include <cstdint>
#include "../Misc/VB6StrPtr.h"
#include "../Misc/VB6Bool.h"
#include "../Misc/VB6Array.h"

#pragma pack(push, 1)
namespace SMBX13 {
    namespace Types {
        // Type related constants
        static constexpr ptrdiff_t maxBlocks         = 20000;
        static constexpr ptrdiff_t maxPlayers        = 200;
        static constexpr ptrdiff_t maxEffects        = 1000;
        static constexpr ptrdiff_t maxNPCs           = 5000;
        static constexpr ptrdiff_t maxBackgrounds    = 8000;
        static constexpr ptrdiff_t maxPlayerFrames   = 750;
        static constexpr ptrdiff_t maxBlockType      = 2000; // Patched by NpcIdExtender.cpp, was 700.
        static constexpr ptrdiff_t maxBackgroundType = 1000; // Patched by NpcIdExtender.cpp, was 200.
        static constexpr ptrdiff_t maxSceneType      = 100;
        static constexpr ptrdiff_t maxNPCType        = 1000; // Patched by NpcIdExtender.cpp, was 300.
        static constexpr ptrdiff_t maxEffectType     = 200;
        static constexpr ptrdiff_t maxWarps          = 2000; // Patched by RuntimeHookFixups.cpp, was 200.
        static constexpr ptrdiff_t numBackground2    = 100;
        static constexpr ptrdiff_t numCharacters     = 5;
        static constexpr ptrdiff_t numStates         = 7;
        static constexpr ptrdiff_t maxWater          = 1000;
        static constexpr ptrdiff_t maxWorldLevels    = 400;
        static constexpr ptrdiff_t maxWorldPaths     = 2000;
        static constexpr ptrdiff_t maxWorldMusic     = 1000;
        static constexpr ptrdiff_t numSounds         = 100;
        static constexpr ptrdiff_t maxSections       = 20;
        static constexpr ptrdiff_t maxTileType       = 400;
        static constexpr ptrdiff_t maxLevelType      = 100;
        static constexpr ptrdiff_t maxPathType       = 100;
        static constexpr ptrdiff_t maxTiles          = 20000;
        static constexpr ptrdiff_t maxScenes         = 5000;
        static constexpr ptrdiff_t FLBlocks          = 8000;
        static constexpr ptrdiff_t maxLayers         = 255;  // Patched by RuntimeHookFixups.cpp, was 101. Note that the array is 0-indexed and this is the count. Note that "maxLayers" wasn't in the VB code.
        static constexpr ptrdiff_t maxEvents         = 255;  // Patched by RuntimeHookFixups.cpp, was 101. Note that the array is 0-indexed and this is the count. Note that "maxEvents" wasn't in the VB code.

        struct StdPicture_t;

        struct Controls_t {
            VB6Bool Up;      // 0x0
            VB6Bool Down;    // 0x2
            VB6Bool Left;    // 0x4
            VB6Bool Right;   // 0x6
            VB6Bool Jump;    // 0x8
            VB6Bool AltJump; // 0xA
            VB6Bool Run;     // 0xC
            VB6Bool AltRun;  // 0xE
            VB6Bool Drop;    // 0x10
            VB6Bool Start;   // 0x12
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Controls_t) == 0x14, "sizeof(Controls_t) must be 0x14");
        #endif
        
        struct nPlayer_t {
            Controls_t Controls;   // 0x0
            int16_t    Cursor;     // 0x14
            VB6Bool    IsMe;       // 0x16
            VB6StrPtr  Nick;       // 0x18
            VB6Bool    Active;     // 0x1C
            int16_t    _padding1_; // 0x1E
            double     ECurserX;   // 0x20
            double     ECurserY;   // 0x28
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(nPlayer_t) == 0x30, "sizeof(nPlayer_t) must be 0x30");
        #endif
        
        struct nPlay_t {
            VB6Bool                    Allow;           // 0x0
            int16_t                    Mode;            // 0x2
            VB6StrPtr                  ServerIP;        // 0x4
            VB6Bool                    ServerCon;       // 0x8
            int16_t                    _padding1_;      // 0xA
            VB6StrPtr                  ServerStr;       // 0xC
            VB6Bool                    ServerLocked;    // 0x10
            int16_t                    _padding2_;      // 0x12
            double                     ServerLoad1;     // 0x14
            VB6Bool                    ServerLoad;      // 0x1C
            VB6Array<VB6Bool, 0, 15>   ClientLocked;    // 0x1E
            int16_t                    _padding3_;      // 0x3E
            VB6Array<VB6StrPtr, 0, 15> ClientIP;        // 0x40
            VB6Array<VB6Bool, 0, 15>   ClientCon;       // 0x80
            VB6Array<VB6StrPtr, 0, 15> ClientName;      // 0xA0
            VB6Array<VB6StrPtr, 0, 15> ClientStr;       // 0xE0
            VB6Array<int16_t, 0, 15>   ClientRelease;   // 0x120
            VB6Array<VB6Bool, 0, 15>   ClientPassword;  // 0x140
            VB6Array<double, 0, 15>    ClientLoad1;     // 0x160
            VB6Bool                    Online;          // 0x1E0
            int16_t                    MySlot;          // 0x1E2
            Controls_t                 MyControls;      // 0x1E4
            VB6Array<nPlayer_t, 0, 15> Player;          // 0x1F8
            int16_t                    PlayerWaitCount; // 0x4F8
            int16_t                    _padding4_;      // 0x4FA
            float                      NPCWaitCount;    // 0x4FC
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(nPlay_t) == 0x500, "sizeof(nPlay_t) must be 0x500");
        #endif
        
        struct Location_t {
            double X;      // 0x0
            double Y;      // 0x8
            double Height; // 0x10
            double Width;  // 0x18
            double SpeedX; // 0x20
            double SpeedY; // 0x28
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Location_t) == 0x30, "sizeof(Location_t) must be 0x30");
        #endif
        
        struct EditorControls_t {
            VB6Bool Up;         // 0x0
            VB6Bool Down;       // 0x2
            VB6Bool Left;       // 0x4
            VB6Bool Right;      // 0x6
            VB6Bool Mouse1;     // 0x8
            int16_t _padding1_; // 0xA
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(EditorControls_t) == 0xC, "sizeof(EditorControls_t) must be 0xC");
        #endif
        
        struct conKeyboard_t {
            int16_t Up;      // 0x0
            int16_t Down;    // 0x2
            int16_t Left;    // 0x4
            int16_t Right;   // 0x6
            int16_t Jump;    // 0x8
            int16_t Run;     // 0xA
            int16_t Drop;    // 0xC
            int16_t Start;   // 0xE
            int16_t AltJump; // 0x10
            int16_t AltRun;  // 0x12
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(conKeyboard_t) == 0x14, "sizeof(conKeyboard_t) must be 0x14");
        #endif
        
        struct conJoystick_t {
            int16_t Jump;    // 0x0
            int16_t Run;     // 0x2
            int16_t Drop;    // 0x4
            int16_t Start;   // 0x6
            int16_t AltJump; // 0x8
            int16_t AltRun;  // 0xA
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(conJoystick_t) == 0xC, "sizeof(conJoystick_t) must be 0xC");
        #endif
        
        struct NPC_t {
            VB6StrPtr               AttLayer;           // 0x0
            int16_t                 Quicksand;          // 0x4
            int16_t                 RespawnDelay;       // 0x6
            VB6Bool                 Bouce;              // 0x8
            int16_t                 Pinched1;           // 0xA
            int16_t                 Pinched2;           // 0xC
            int16_t                 Pinched3;           // 0xE
            int16_t                 Pinched4;           // 0x10
            int16_t                 MovingPinched;      // 0x12
            int16_t                 NetTimeout;         // 0x14
            int16_t                 _padding1_;         // 0x16
            float                   RealSpeedX;         // 0x18
            int16_t                 Wet;                // 0x1C
            int16_t                 Settings;           // 0x1E
            VB6Bool                 NoLavaSplash;       // 0x20
            int16_t                 Slope;              // 0x22
            int16_t                 Multiplier;         // 0x24
            int16_t                 TailCD;             // 0x26
            VB6Bool                 Shadow;             // 0x28
            int16_t                 _padding2_;         // 0x2A
            VB6StrPtr               TriggerActivate;    // 0x2C
            VB6StrPtr               TriggerDeath;       // 0x30
            VB6StrPtr               TriggerTalk;        // 0x34
            VB6StrPtr               TriggerLast;        // 0x38
            VB6StrPtr               Layer;              // 0x3C
            VB6Bool                 Hidden;             // 0x40
            VB6Bool                 Legacy;             // 0x42
            VB6Bool                 Chat;               // 0x44
            VB6Bool                 Inert;              // 0x46
            VB6Bool                 Stuck;              // 0x48
            VB6Bool                 DefaultStuck;       // 0x4A
            VB6StrPtr               Text;               // 0x4C
            float                   oldAddBelt;         // 0x50
            int16_t                 PinchCount;         // 0x54
            VB6Bool                 Pinched;            // 0x56
            int16_t                 PinchedDirection;   // 0x58
            int16_t                 _padding3_;         // 0x5A
            float                   BeltSpeed;          // 0x5C
            int16_t                 standingOnPlayer;   // 0x60
            int16_t                 standingOnPlayerY;  // 0x62
            VB6Bool                 Generator;          // 0x64
            int16_t                 _padding4_;         // 0x66
            float                   GeneratorTimeMax;   // 0x68
            float                   GeneratorTime;      // 0x6C
            int16_t                 GeneratorDirection; // 0x70
            int16_t                 GeneratorEffect;    // 0x72
            VB6Bool                 GeneratorActive;    // 0x74
            VB6Bool                 playerTemp;         // 0x76
            Location_t              Location;           // 0x78
            Location_t              DefaultLocation;    // 0xA8
            float                   DefaultDirection;   // 0xD8
            int16_t                 DefaultType;        // 0xDC
            int16_t                 DefaultSpecial;     // 0xDE
            int16_t                 DefaultSpecial2;    // 0xE0
            int16_t                 Type;               // 0xE2
            int16_t                 Frame;              // 0xE4
            int16_t                 _padding5_;         // 0xE6
            float                   FrameCount;         // 0xE8
            float                   Direction;          // 0xEC
            double                  Special;            // 0xF0
            double                  Special2;           // 0xF8
            double                  Special3;           // 0x100
            double                  Special4;           // 0x108
            double                  Special5;           // 0x110
            double                  Special6;           // 0x118
            VB6Bool                 TurnAround;         // 0x120
            int16_t                 Killed;             // 0x122
            VB6Bool                 Active;             // 0x124
            VB6Array<VB6Bool, 1, 2> Reset;              // 0x126
            int16_t                 TimeLeft;           // 0x12A
            int16_t                 HoldingPlayer;      // 0x12C
            int16_t                 CantHurt;           // 0x12E
            int16_t                 CantHurtPlayer;     // 0x130
            int16_t                 BattleOwner;        // 0x132
            int16_t                 WallDeath;          // 0x134
            VB6Bool                 Projectile;         // 0x136
            int16_t                 Effect;             // 0x138
            int16_t                 _padding6_;         // 0x13A
            double                  Effect2;            // 0x13C
            int16_t                 Effect3;            // 0x144
            int16_t                 Section;            // 0x146
            float                   Damage;             // 0x148
            int16_t                 JustActivated;      // 0x14C
            int16_t                 Block;              // 0x14E
            int16_t                 tempBlock;          // 0x150
            VB6Bool                 onWall;             // 0x152
            VB6Bool                 TurnBackWipe;       // 0x154
            int16_t                 Immune;             // 0x156
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(NPC_t) == 0x158, "sizeof(NPC_t) must be 0x158");
        #endif
        
        struct Player_t {
            VB6Bool    DoubleJump;           // 0x0
            VB6Bool    FlySparks;            // 0x2
            VB6Bool    Driving;              // 0x4
            int16_t    Quicksand;            // 0x6
            int16_t    Bombs;                // 0x8
            VB6Bool    Slippy;               // 0xA
            VB6Bool    Fairy;                // 0xC
            int16_t    FairyCD;              // 0xE
            int16_t    FairyTime;            // 0x10
            VB6Bool    HasKey;               // 0x12
            int16_t    SwordPoke;            // 0x14
            int16_t    Hearts;               // 0x16
            VB6Bool    CanFloat;             // 0x18
            VB6Bool    FloatRelease;         // 0x1A
            int16_t    FloatTime;            // 0x1C
            int16_t    _padding1_;           // 0x1E
            float      FloatSpeed;           // 0x20
            int16_t    FloatDir;             // 0x24
            int16_t    GrabTime;             // 0x26
            float      GrabSpeed;            // 0x28
            double     VineNPC;              // 0x2C
            int16_t    Wet;                  // 0x34
            VB6Bool    WetFrame;             // 0x36
            int16_t    SwimCount;            // 0x38
            int16_t    NoGravity;            // 0x3A
            VB6Bool    Slide;                // 0x3C
            VB6Bool    SlideKill;            // 0x3E
            int16_t    Vine;                 // 0x40
            int16_t    NoShellKick;          // 0x42
            VB6Bool    ShellSurf;            // 0x44
            int16_t    StateNPC;             // 0x46
            int16_t    Slope;                // 0x48
            VB6Bool    Stoned;               // 0x4A
            int16_t    StonedCD;             // 0x4C
            int16_t    StonedTime;           // 0x4E
            VB6Bool    SpinJump;             // 0x50
            int16_t    SpinFrame;            // 0x52
            int16_t    SpinFireDir;          // 0x54
            int16_t    Multiplier;           // 0x56
            int16_t    SlideCounter;         // 0x58
            int16_t    ShowWarp;             // 0x5A
            VB6Bool    GroundPound;          // 0x5C
            VB6Bool    GroundPound2;         // 0x5E
            VB6Bool    CanPound;             // 0x60
            int16_t    ForceHold;            // 0x62
            VB6Bool    YoshiYellow;          // 0x64
            VB6Bool    YoshiBlue;            // 0x66
            VB6Bool    YoshiRed;             // 0x68
            int16_t    YoshiWingsFrame;      // 0x6A
            int16_t    YoshiWingsFrameCount; // 0x6C
            int16_t    YoshiTX;              // 0x6E
            int16_t    YoshiTY;              // 0x70
            int16_t    YoshiTFrame;          // 0x72
            int16_t    YoshiTFrameCount;     // 0x74
            int16_t    YoshiBX;              // 0x76
            int16_t    YoshiBY;              // 0x78
            int16_t    YoshiBFrame;          // 0x7A
            int16_t    YoshiBFrameCount;     // 0x7C
            int16_t    _padding2_;           // 0x7E
            Location_t YoshiTongue;          // 0x80
            float      YoshiTongueX;         // 0xB0
            int16_t    YoshiTongueLength;    // 0xB4
            VB6Bool    YoshiTonugeBool;      // 0xB6
            int16_t    YoshiNPC;             // 0xB8
            int16_t    YoshiPlayer;          // 0xBA
            int16_t    Dismount;             // 0xBC
            int16_t    NoPlayerCol;          // 0xBE
            Location_t Location;             // 0xC0
            int16_t    Character;            // 0xF0
            Controls_t Controls;             // 0xF2
            int16_t    Direction;            // 0x106
            int16_t    Mount;                // 0x108
            int16_t    MountType;            // 0x10A
            int16_t    MountSpecial;         // 0x10C
            int16_t    MountOffsetY;         // 0x10E
            int16_t    MountFrame;           // 0x110
            int16_t    State;                // 0x112
            int16_t    Frame;                // 0x114
            int16_t    _padding3_;           // 0x116
            float      FrameCount;           // 0x118
            int16_t    Jump;                 // 0x11C
            VB6Bool    CanJump;              // 0x11E
            VB6Bool    CanAltJump;           // 0x120
            int16_t    Effect;               // 0x122
            double     Effect2;              // 0x124
            VB6Bool    DuckRelease;          // 0x12C
            VB6Bool    Duck;                 // 0x12E
            VB6Bool    DropRelease;          // 0x130
            VB6Bool    StandUp;              // 0x132
            VB6Bool    StandUp2;             // 0x134
            VB6Bool    Bumped;               // 0x136
            float      Bumped2;              // 0x138
            VB6Bool    Dead;                 // 0x13C
            int16_t    TimeToLive;           // 0x13E
            int16_t    Immune;               // 0x140
            VB6Bool    Immune2;              // 0x142
            VB6Bool    ForceHitSpot3;        // 0x144
            int16_t    Pinched1;             // 0x146
            int16_t    Pinched2;             // 0x148
            int16_t    Pinched3;             // 0x14A
            int16_t    Pinched4;             // 0x14C
            int16_t    NPCPinched;           // 0x14E
            float      m2Speed;              // 0x150
            int16_t    HoldingNPC;           // 0x154
            VB6Bool    CanGrabNPCs;          // 0x156
            int16_t    HeldBonus;            // 0x158
            int16_t    Section;              // 0x15A
            int16_t    WarpCD;               // 0x15C
            int16_t    Warp;                 // 0x15E
            int16_t    FireBallCD;           // 0x160
            int16_t    FireBallCD2;          // 0x162
            int16_t    TailCount;            // 0x164
            int16_t    _padding4_;           // 0x166
            float      RunCount;             // 0x168
            VB6Bool    CanFly;               // 0x16C
            VB6Bool    CanFly2;              // 0x16E
            int16_t    FlyCount;             // 0x170
            VB6Bool    RunRelease;           // 0x172
            VB6Bool    JumpRelease;          // 0x174
            int16_t    StandingOnNPC;        // 0x176
            int16_t    StandingOnTempNPC;    // 0x178
            VB6Bool    UnStart;              // 0x17A
            float      mountBump;            // 0x17C
            float      SpeedFixY;            // 0x180
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Player_t) == 0x184, "sizeof(Player_t) must be 0x184");
        #endif
        
        struct Background_t {
            VB6StrPtr  Layer;    // 0x0
            VB6Bool    Hidden;   // 0x4
            int16_t    Type;     // 0x6
            Location_t Location; // 0x8
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Background_t) == 0x38, "sizeof(Background_t) must be 0x38");
        #endif
        
        struct Water_t {
            VB6StrPtr  Layer;      // 0x0
            VB6Bool    Hidden;     // 0x4
            int16_t    _padding1_; // 0x6
            float      Buoy;       // 0x8
            VB6Bool    Quicksand;  // 0xC
            int16_t    _padding2_; // 0xE
            Location_t Location;   // 0x10
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Water_t) == 0x40, "sizeof(Water_t) must be 0x40");
        #endif
        
        struct Block_t {
            VB6Bool    Slippy;            // 0x0
            int16_t    RespawnDelay;      // 0x2
            int16_t    RapidHit;          // 0x4
            int16_t    DefaultType;       // 0x6
            int16_t    DefaultSpecial;    // 0x8
            int16_t    _padding1_;        // 0xA
            VB6StrPtr  TriggerHit;        // 0xC
            VB6StrPtr  TriggerDeath;      // 0x10
            VB6StrPtr  TriggerLast;       // 0x14
            VB6StrPtr  Layer;             // 0x18
            VB6Bool    Hidden;            // 0x1C
            int16_t    Type;              // 0x1E
            Location_t Location;          // 0x20
            int16_t    Special;           // 0x50
            int16_t    ShakeY;            // 0x52
            int16_t    ShakeY2;           // 0x54
            int16_t    ShakeY3;           // 0x56
            VB6Bool    Kill;              // 0x58
            VB6Bool    Invis;             // 0x5A
            int16_t    NPC;               // 0x5C
            int16_t    IsPlayer;          // 0x5E
            int16_t    IsNPC;             // 0x60
            int16_t    standingOnPlayerY; // 0x62
            VB6Bool    noProjClipping;    // 0x64
            int16_t    IsReally;          // 0x66
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Block_t) == 0x68, "sizeof(Block_t) must be 0x68");
        #endif
        
        struct Effect_t {
            int16_t    Type;       // 0x0
            int16_t    _padding1_; // 0x2
            Location_t Location;   // 0x4
            int16_t    Frame;      // 0x34
            int16_t    _padding2_; // 0x36
            float      FrameCount; // 0x38
            int16_t    Life;       // 0x3C
            int16_t    NewNpc;     // 0x3E
            VB6Bool    Shadow;     // 0x40
            int16_t    _padding3_; // 0x42
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Effect_t) == 0x44, "sizeof(Effect_t) must be 0x44");
        #endif
        
        struct vScreen_t {
            double  Left;       // 0x0
            double  Top;        // 0x8
            double  Width;      // 0x10
            double  Height;     // 0x18
            VB6Bool Visible;    // 0x20
            int16_t _padding1_; // 0x22
            double  tempX;      // 0x24
            double  TempY;      // 0x2C
            int16_t TempDelay;  // 0x34
            int16_t _padding2_; // 0x36
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(vScreen_t) == 0x38, "sizeof(vScreen_t) must be 0x38");
        #endif
        
        struct WorldLevel_t {
            Location_t              Location;   // 0x0
            int16_t                 Type;       // 0x30
            int16_t                 _padding1_; // 0x32
            VB6StrPtr               FileName;   // 0x34
            VB6Array<int16_t, 1, 4> LevelExit;  // 0x38
            VB6Bool                 Active;     // 0x40
            int16_t                 _padding2_; // 0x42
            VB6StrPtr               LevelName;  // 0x44
            int16_t                 StartWarp;  // 0x48
            int16_t                 _padding3_; // 0x4A
            double                  WarpX;      // 0x4C
            double                  WarpY;      // 0x54
            VB6Bool                 Path;       // 0x5C
            VB6Bool                 Path2;      // 0x5E
            VB6Bool                 Start;      // 0x60
            VB6Bool                 Visible;    // 0x62
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(WorldLevel_t) == 0x64, "sizeof(WorldLevel_t) must be 0x64");
        #endif
        
        struct Warp_t {
            VB6Bool    Locked;     // 0x0
            VB6Bool    WarpNPC;    // 0x2
            VB6Bool    NoYoshi;    // 0x4
            int16_t    _padding1_; // 0x6
            VB6StrPtr  Layer;      // 0x8
            VB6Bool    Hidden;     // 0xC
            VB6Bool    PlacedEnt;  // 0xE
            VB6Bool    PlacedExit; // 0x10
            int16_t    Stars;      // 0x12
            Location_t Entrance;   // 0x14
            Location_t Exit;       // 0x44
            int16_t    Effect;     // 0x74
            int16_t    _padding2_; // 0x76
            VB6StrPtr  level;      // 0x78
            int16_t    LevelWarp;  // 0x7C
            VB6Bool    LevelEnt;   // 0x7E
            int16_t    Direction;  // 0x80
            int16_t    Direction2; // 0x82
            VB6Bool    MapWarp;    // 0x84
            int16_t    MapX;       // 0x86
            int16_t    MapY;       // 0x88
            int16_t    curStars;   // 0x8A
            int16_t    maxStars;   // 0x8C
            int16_t    _padding3_; // 0x8E
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Warp_t) == 0x90, "sizeof(Warp_t) must be 0x90");
        #endif
        
        struct Tile_t {
            Location_t Location;   // 0x0
            int16_t    Type;       // 0x30
            int16_t    _padding1_; // 0x32
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Tile_t) == 0x34, "sizeof(Tile_t) must be 0x34");
        #endif
        
        struct Scene_t {
            Location_t Location; // 0x0
            int16_t    Type;     // 0x30
            VB6Bool    Active;   // 0x32
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Scene_t) == 0x34, "sizeof(Scene_t) must be 0x34");
        #endif
        
        struct WorldPath_t {
            Location_t Location; // 0x0
            VB6Bool    Active;   // 0x30
            int16_t    Type;     // 0x32
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(WorldPath_t) == 0x34, "sizeof(WorldPath_t) must be 0x34");
        #endif
        
        struct WorldMusic_t {
            Location_t Location;   // 0x0
            int16_t    Type;       // 0x30
            int16_t    _padding1_; // 0x32
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(WorldMusic_t) == 0x34, "sizeof(WorldMusic_t) must be 0x34");
        #endif
        
        struct EditorCursor_t {
            float        X;            // 0x0
            float        Y;            // 0x4
            int16_t      SelectedMode; // 0x8
            int16_t      Selected;     // 0xA
            Location_t   Location;     // 0xC
            VB6StrPtr    Layer;        // 0x3C
            int16_t      Mode;         // 0x40
            int16_t      _padding1_;   // 0x42
            Block_t      Block;        // 0x44
            Water_t      Water;        // 0xAC
            Background_t Background;   // 0xEC
            NPC_t        NPC;          // 0x124
            Warp_t       Warp;         // 0x27C
            Tile_t       Tile;         // 0x30C
            Scene_t      Scene;        // 0x340
            WorldLevel_t WorldLevel;   // 0x374
            WorldPath_t  WorldPath;    // 0x3D8
            WorldMusic_t WorldMusic;   // 0x40C
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(EditorCursor_t) == 0x440, "sizeof(EditorCursor_t) must be 0x440");
        #endif
        
        struct WorldPlayer_t {
            Location_t Location;  // 0x0
            int16_t    Type;      // 0x30
            int16_t    Frame;     // 0x32
            int16_t    Frame2;    // 0x34
            int16_t    Move;      // 0x36
            int16_t    Move2;     // 0x38
            VB6Bool    Move3;     // 0x3A
            VB6StrPtr  LevelName; // 0x3C
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(WorldPlayer_t) == 0x40, "sizeof(WorldPlayer_t) must be 0x40");
        #endif
        
        struct Layer_t {
            VB6Bool   EffectStop; // 0x0
            int16_t   _padding1_; // 0x2
            VB6StrPtr Name;       // 0x4
            VB6Bool   Hidden;     // 0x8
            int16_t   _padding2_; // 0xA
            float     SpeedX;     // 0xC
            float     SpeedY;     // 0x10
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Layer_t) == 0x14, "sizeof(Layer_t) must be 0x14");
        #endif
        
        struct CreditLine_t {
            Location_t Location; // 0x0
            VB6StrPtr  Text;     // 0x30
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(CreditLine_t) == 0x34, "sizeof(CreditLine_t) must be 0x34");
        #endif
        
        struct SelectWorld_t {
            VB6StrPtr                           WorldName;  // 0x0
            VB6StrPtr                           WorldPath;  // 0x4
            VB6StrPtr                           WorldFile;  // 0x8
            VB6Array<VB6Bool, 1, numCharacters> blockChar;  // 0xC
            int16_t                             _padding1_; // 0x16
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SelectWorld_t) == 0x18, "sizeof(SelectWorld_t) must be 0x18");
        #endif
        
        struct Star_t {
            VB6StrPtr level;      // 0x0
            int16_t   Section;    // 0x4
            int16_t   _padding1_; // 0x6
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Star_t) == 0x8, "sizeof(Star_t) must be 0x8");
        #endif
        
        struct NPCDefaults_t {
            VB6Array<int16_t, 0, 300> NPCFrameOffsetX;    // 0x0
            VB6Array<int16_t, 0, 300> NPCFrameOffsetY;    // 0x25A
            VB6Array<int16_t, 0, 300> NPCWidth;           // 0x4B4
            VB6Array<int16_t, 0, 300> NPCHeight;          // 0x70E
            VB6Array<int16_t, 0, 300> NPCWidthGFX;        // 0x968
            VB6Array<int16_t, 0, 300> NPCHeightGFX;       // 0xBC2
            VB6Array<VB6Bool, 0, 300> NPCIsAShell;        // 0xE1C
            VB6Array<VB6Bool, 0, 300> NPCIsABlock;        // 0x1076
            VB6Array<VB6Bool, 0, 300> NPCIsAHit1Block;    // 0x12D0
            VB6Array<VB6Bool, 0, 300> NPCIsABonus;        // 0x152A
            VB6Array<VB6Bool, 0, 300> NPCIsACoin;         // 0x1784
            VB6Array<VB6Bool, 0, 300> NPCIsAVine;         // 0x19DE
            VB6Array<VB6Bool, 0, 300> NPCIsAnExit;        // 0x1C38
            VB6Array<VB6Bool, 0, 300> NPCIsAParaTroopa;   // 0x1E92
            VB6Array<VB6Bool, 0, 300> NPCIsCheep;         // 0x20EC
            VB6Array<VB6Bool, 0, 300> NPCJumpHurt;        // 0x2346
            VB6Array<VB6Bool, 0, 300> NPCNoClipping;      // 0x25A0
            VB6Array<int16_t, 0, 300> NPCScore;           // 0x27FA
            VB6Array<VB6Bool, 0, 300> NPCCanWalkOn;       // 0x2A54
            VB6Array<VB6Bool, 0, 300> NPCGrabFromTop;     // 0x2CAE
            VB6Array<VB6Bool, 0, 300> NPCTurnsAtCliffs;   // 0x2F08
            VB6Array<VB6Bool, 0, 300> NPCWontHurt;        // 0x3162
            VB6Array<VB6Bool, 0, 300> NPCMovesPlayer;     // 0x33BC
            VB6Array<VB6Bool, 0, 300> NPCStandsOnPlayer;  // 0x3616
            VB6Array<VB6Bool, 0, 300> NPCIsGrabbable;     // 0x3870
            VB6Array<VB6Bool, 0, 300> NPCIsBoot;          // 0x3ACA
            VB6Array<VB6Bool, 0, 300> NPCIsYoshi;         // 0x3D24
            VB6Array<VB6Bool, 0, 300> NPCIsToad;          // 0x3F7E
            VB6Array<VB6Bool, 0, 300> NPCNoYoshi;         // 0x41D8
            VB6Array<VB6Bool, 0, 300> NPCForeground;      // 0x4432
            VB6Array<VB6Bool, 0, 300> NPCIsABot;          // 0x468C
            VB6Array<VB6Bool, 0, 300> NPCDefaultMovement; // 0x48E6
            VB6Array<VB6Bool, 0, 300> NPCIsVeggie;        // 0x4B40
            int16_t                   _padding1_;         // 0x4D9A
            VB6Array<float, 0, 300>   NPCSpeedvar;        // 0x4D9C
            VB6Array<VB6Bool, 0, 300> NPCNoFireBall;      // 0x5250
            VB6Array<VB6Bool, 0, 300> NPCNoIceBall;       // 0x54AA
            VB6Array<VB6Bool, 0, 300> NPCNoGravity;       // 0x5704
            int16_t                   _padding2_;         // 0x595E
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(NPCDefaults_t) == 0x5960, "sizeof(NPCDefaults_t) must be 0x5960");
        #endif
        
        struct EffectDefaults_t {
            VB6Array<int16_t, 1, maxEffectType> EffectWidth;  // 0x0
            VB6Array<int16_t, 1, maxEffectType> EffectHeight; // 0x190
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(EffectDefaults_t) == 0x320, "sizeof(EffectDefaults_t) must be 0x320");
        #endif
        
        struct Physics_t {
            int16_t                                             PlayerJumpHeight;       // 0x0
            int16_t                                             PlayerBlockJumpHeight;  // 0x2
            int16_t                                             PlayerHeadJumpHeight;   // 0x4
            int16_t                                             PlayerNPCJumpHeight;    // 0x6
            int16_t                                             PlayerSpringJumpHeight; // 0x8
            int16_t                                             _padding1_;             // 0xA
            float                                               PlayerJumpVelocity;     // 0xC
            float                                               PlayerRunSpeed;         // 0x10
            float                                               PlayerWalkSpeed;        // 0x14
            int16_t                                             PlayerTerminalVelocity; // 0x18
            int16_t                                             _padding2_;             // 0x1A
            float                                               PlayerGravity;          // 0x1C
            VB6Array2D<int16_t, 1, numStates, 1, numCharacters> PlayerHeight;           // 0x20
            VB6Array2D<int16_t, 1, numStates, 1, numCharacters> PlayerDuckHeight;       // 0x66
            VB6Array2D<int16_t, 1, numStates, 1, numCharacters> PlayerWidth;            // 0xAC
            VB6Array2D<int16_t, 1, numStates, 1, numCharacters> PlayerGrabSpotX;        // 0xF2
            VB6Array2D<int16_t, 1, numStates, 1, numCharacters> PlayerGrabSpotY;        // 0x138
            int16_t                                             NPCTimeOffScreen;       // 0x17E
            int16_t                                             NPCCanHurtWait;         // 0x180
            int16_t                                             _padding3_;             // 0x182
            float                                               NPCShellSpeed;          // 0x184
            float                                               NPCShellSpeedY;         // 0x188
            float                                               NPCWalkingSpeed;        // 0x18C
            float                                               NPCWalkingOnSpeed;      // 0x190
            float                                               NPCMushroomSpeed;       // 0x194
            float                                               NPCGravity;             // 0x198
            float                                               NPCGravityReal;         // 0x19C
            int16_t                                             NPCPSwitch;             // 0x1A0
            int16_t                                             _padding4_;             // 0x1A2
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Physics_t) == 0x1A4, "sizeof(Physics_t) must be 0x1A4");
        #endif
        
        struct Events_t {
            VB6StrPtr                            addSavedEvent;    // 0x0
            VB6StrPtr                            RemoveSavedEvent; // 0x4
            VB6Bool                              LayerSmoke;       // 0x8
            int16_t                              Sound;            // 0xA
            VB6StrPtr                            Name;             // 0xC
            VB6StrPtr                            Text;             // 0x10
            VB6Array<VB6StrPtr, 0, 20>           HideLayer;        // 0x14
            VB6Array<VB6StrPtr, 0, 20>           ShowLayer;        // 0x68
            VB6Array<VB6StrPtr, 0, 20>           ToggleLayer;      // 0xBC
            VB6Array<int16_t, 0, maxSections>    Music;            // 0x110
            VB6Array<int16_t, 0, maxSections>    Background;       // 0x13A
            VB6Array<Location_t, 0, maxSections> level;            // 0x164
            int16_t                              EndGame;          // 0x554
            int16_t                              _padding1_;       // 0x556
            VB6StrPtr                            TriggerEvent;     // 0x558
            double                               TriggerDelay;     // 0x55C
            Controls_t                           Controls;         // 0x564
            VB6StrPtr                            MoveLayer;        // 0x578
            float                                SpeedX;           // 0x57C
            float                                SpeedY;           // 0x580
            float                                AutoX;            // 0x584
            float                                AutoY;            // 0x588
            int16_t                              AutoSection;      // 0x58C
            VB6Bool                              AutoStart;        // 0x58E
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(Events_t) == 0x590, "sizeof(Events_t) must be 0x590");
        #endif
        
        struct modMain_t {
            int32_t                                         myBackBuffer;         // 0xB25028
            int32_t                                         myBufferBMP;          // 0xB2502C
            int16_t                                         AllCharBlock;         // 0xB25030
            int16_t                                         _padding1_;           // 0xB25032
            VB6StrPtr                                       LocalNick;            // 0xB25034
            int16_t                                         LocalCursor;          // 0xB25038
            int16_t                                         _padding2_;           // 0xB2503A
            VB6StrPtr                                       ClientPassword;       // 0xB2503C
            VB6StrPtr                                       ServerPassword;       // 0xB25040
            VB6Bool                                         ServerClear;          // 0xB25044
            VB6Bool                                         StartMenu;            // 0xB25046
            int16_t                                         BlockFlash;           // 0xB25048
            VB6Bool                                         ScrollRelease;        // 0xB2504A
            VB6Bool                                         TakeScreen;           // 0xB2504C
            int16_t                                         _padding3_;           // 0xB2504E
            VB6StrPtr                                       LB;                   // 0xB25050
            VB6StrPtr                                       EoT;                  // 0xB25054
            VB6ArrayRef<conKeyboard_t, 1, 2>                conKeyboard;          // 0xB25068
            VB6ArrayRef<conJoystick_t, 1, 2>                conJoystick;          // 0xB25084
            VB6ArrayRef<int16_t, 1, 2>                      useJoystick;          // 0xB250A0
            int16_t                                         ScreenShake;          // 0xB250AC
            int16_t                                         _padding4_;           // 0xB250AE
            VB6StrPtr                                       Checkpoint;           // 0xB250B0
            VB6Bool                                         MagicHand;            // 0xB250B4
            int16_t                                         _padding5_;           // 0xB250B6
            VB6ArrayRef<Player_t, 1, 2>                     testPlayer;           // 0xB250C8
            VB6Bool                                         ClearBuffer;          // 0xB250D4
            int16_t                                         numLocked;            // 0xB250D6
            VB6Bool                                         resChanged;           // 0xB250D8
            int16_t                                         inputKey;             // 0xB250DA
            VB6Bool                                         getNewKeyboard;       // 0xB250DC
            VB6Bool                                         getNewJoystick;       // 0xB250DE
            int16_t                                         lastJoyButton;        // 0xB250E0
            VB6Bool                                         GamePaused;           // 0xB250E2
            VB6StrPtr                                       MessageText;          // 0xB250E4
            int16_t                                         NumSelectWorld;       // 0xB250E8
            int16_t                                         _padding6_;           // 0xB250EA
            VB6ArrayRef<SelectWorld_t, 1, 100>              SelectWorld;          // 0xB250FC
            VB6Bool                                         ShowFPS;              // 0xB25108
            int16_t                                         _padding7_;           // 0xB2510A
            double                                          PrintFPS;             // 0xB2510C
            VB6ArrayRef<vScreen_t, 0, 2>                    vScreen;              // 0xB25124
            int16_t                                         ScreenType;           // 0xB25130
            int16_t                                         DScreenType;          // 0xB25132
            VB6Bool                                         LevelEditor;          // 0xB25134
            VB6Bool                                         WorldEditor;          // 0xB25136
            VB6ArrayRef<Location_t, 1, 2>                   PlayerStart;          // 0xB25148
            VB6ArrayRef<VB6Bool, 0, 20>                     blockCharacter;       // 0xB25164
            VB6ArrayRef<int16_t, 0, maxPlayers>             OwedMount;            // 0xB25180
            VB6ArrayRef<int16_t, 0, maxPlayers>             OwedMountType;        // 0xB2519C
            VB6ArrayRef<float, 0, maxSections>              AutoX;                // 0xB251B8
            VB6ArrayRef<float, 0, maxSections>              AutoY;                // 0xB251D4
            int16_t                                         numStars;             // 0xB251E0
            int16_t                                         _padding8_;           // 0xB251E2
            nPlay_t                                         nPlay;                // 0xB251E4
            VB6ArrayRef<Water_t, 0, maxWater>               Water;                // 0xB256F4
            int16_t                                         numWater;             // 0xB25700
            int16_t                                         _padding9_;           // 0xB25702
            VB6ArrayRef<Star_t, 1, 1000>                    Star;                 // 0xB25714
            VB6StrPtr                                       GoToLevel;            // 0xB25720
            VB6StrPtr                                       StartLevel;           // 0xB25724
            VB6Bool                                         NoMap;                // 0xB25728
            VB6Bool                                         RestartLevel;         // 0xB2572A
            VB6ArrayRef<float, 0, maxSections>              LevelChop;            // 0xB2573C
            VB6ArrayRef<int16_t, -FLBlocks, FLBlocks>       FirstBlock;           // 0xB25758
            VB6ArrayRef<int16_t, -FLBlocks, FLBlocks>       LastBlock;            // 0xB25774
            int16_t                                         MidBackground;        // 0xB25780
            int16_t                                         LastBackground;       // 0xB25782
            int16_t                                         iBlocks;              // 0xB25784
            int16_t                                         _padding10_;          // 0xB25786
            VB6ArrayRef<int16_t, 0, maxBlocks>              iBlock;               // 0xB25798
            int16_t                                         numTiles;             // 0xB257A4
            int16_t                                         numScenes;            // 0xB257A6
            VB6ArrayRef<VB6StrPtr, 0, maxSections>          CustomMusic;          // 0xB257B8
            VB6ArrayRef<Location_t, 0, maxSections>         level;                // 0xB257D4
            VB6ArrayRef<VB6Bool, 0, maxSections>            LevelWrap;            // 0xB257F0
            VB6ArrayRef<VB6Bool, 0, maxSections>            OffScreenExit;        // 0xB2580C
            VB6ArrayRef<int16_t, 0, maxSections>            bgMusic;              // 0xB25828
            VB6ArrayRef<int16_t, 0, maxSections>            bgMusicREAL;          // 0xB25844
            VB6ArrayRef<int16_t, 0, maxSections>            Background2REAL;      // 0xB25860
            VB6ArrayRef<Location_t, 0, maxSections>         LevelREAL;            // 0xB2587C
            int16_t                                         curMusic;             // 0xB25888
            int16_t                                         _padding11_;          // 0xB2588A
            VB6ArrayRef<int32_t, 0, maxSections>            bgColor;              // 0xB2589C
            VB6ArrayRef<int16_t, 0, maxSections>            Background2;          // 0xB258B8
            VB6ArrayRef<WorldPath_t, 1, maxWorldPaths>      WorldPath;            // 0xB258D4
            int16_t                                         numWorldPaths;        // 0xB258E0
            int16_t                                         numWarps;             // 0xB258E2
            VB6ArrayRef<Warp_t, 1, maxWarps>                Warp;                 // 0xB258F4
            VB6ArrayRef<Tile_t, 1, maxTiles>                Tile;                 // 0xB25910
            VB6ArrayRef<Scene_t, 1, maxScenes>              Scene;                // 0xB2592C
            VB6ArrayRef<CreditLine_t, 1, 200>               Credit;               // 0xB25948
            int16_t                                         numCredits;           // 0xB25954
            int16_t                                         numBlock;             // 0xB25956
            int16_t                                         numBackground;        // 0xB25958
            int16_t                                         numNPCs;              // 0xB2595A
            int16_t                                         numEffects;           // 0xB2595C
            int16_t                                         numPlayers;           // 0xB2595E
            int16_t                                         numWorldLevels;       // 0xB25960
            int16_t                                         _padding12_;          // 0xB25962
            VB6ArrayRef<WorldMusic_t, 1, maxWorldMusic>     WorldMusic;           // 0xB25974
            int16_t                                         numWorldMusic;        // 0xB25980
            int16_t                                         _padding13_;          // 0xB25982
            VB6ArrayRef<WorldLevel_t, 1, maxWorldLevels>    WorldLevel;           // 0xB25994
            VB6ArrayRef<Background_t, 1, maxBackgrounds>    Background;           // 0xB259B0
            VB6ArrayRef<Effect_t, 1, maxEffects>            Effect;               // 0xB259CC
            VB6ArrayRef<NPC_t, -128, maxNPCs>               NPC;                  // 0xB259E8
            VB6ArrayRef<Block_t, 0, maxBlocks>              Block;                // 0xB25A04
            VB6ArrayRef<Player_t, 0, maxPlayers>            Player;               // 0xB25A20
            VB6ArrayRef<int16_t, 0, maxPlayerFrames>        MarioFrameX;          // 0xB25A3C
            VB6ArrayRef<int16_t, 0, maxPlayerFrames>        MarioFrameY;          // 0xB25A58
            VB6ArrayRef<int16_t, 0, maxPlayerFrames>        LuigiFrameX;          // 0xB25A74
            VB6ArrayRef<int16_t, 0, maxPlayerFrames>        LuigiFrameY;          // 0xB25A90
            VB6ArrayRef<int16_t, 0, maxPlayerFrames>        PeachFrameX;          // 0xB25AAC
            VB6ArrayRef<int16_t, 0, maxPlayerFrames>        PeachFrameY;          // 0xB25AC8
            VB6ArrayRef<int16_t, 0, maxPlayerFrames>        ToadFrameX;           // 0xB25AE4
            VB6ArrayRef<int16_t, 0, maxPlayerFrames>        ToadFrameY;           // 0xB25B00
            VB6ArrayRef<int16_t, 0, maxPlayerFrames>        LinkFrameX;           // 0xB25B1C
            VB6ArrayRef<int16_t, 0, maxPlayerFrames>        LinkFrameY;           // 0xB25B38
            VB6ArrayRef<VB6Bool, 0, maxBackgroundType>      BackgroundFence;      // 0xB25B54
            VB6ArrayRef<int16_t, 0, maxNPCType>             NPCFrameOffsetX;      // 0xB25B70
            VB6ArrayRef<int16_t, 0, maxNPCType>             NPCFrameOffsetY;      // 0xB25B8C
            VB6ArrayRef<int16_t, 0, maxNPCType>             NPCWidth;             // 0xB25BA8
            VB6ArrayRef<int16_t, 0, maxNPCType>             NPCHeight;            // 0xB25BC4
            VB6ArrayRef<int16_t, 0, maxNPCType>             NPCWidthGFX;          // 0xB25BE0
            VB6ArrayRef<int16_t, 0, maxNPCType>             NPCHeightGFX;         // 0xB25BFC
            VB6ArrayRef<float, 0, maxNPCType>               NPCSpeedvar;          // 0xB25C18
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsAShell;          // 0xB25C34
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsABlock;          // 0xB25C50
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsAHit1Block;      // 0xB25C6C
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsABonus;          // 0xB25C88
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsACoin;           // 0xB25CA4
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsAVine;           // 0xB25CC0
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsAnExit;          // 0xB25CDC
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsAParaTroopa;     // 0xB25CF8
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsCheep;           // 0xB25D14
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCJumpHurt;          // 0xB25D30
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCNoClipping;        // 0xB25D4C
            VB6ArrayRef<int16_t, 0, maxNPCType>             NPCScore;             // 0xB25D68
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCCanWalkOn;         // 0xB25D84
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCGrabFromTop;       // 0xB25DA0
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCTurnsAtCliffs;     // 0xB25DBC
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCWontHurt;          // 0xB25DD8
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCMovesPlayer;       // 0xB25DF4
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCStandsOnPlayer;    // 0xB25E10
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsGrabbable;       // 0xB25E2C
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsBoot;            // 0xB25E48
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsYoshi;           // 0xB25E64
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsToad;            // 0xB25E80
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCNoYoshi;           // 0xB25E9C
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCForeground;        // 0xB25EB8
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsABot;            // 0xB25ED4
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCDefaultMovement;   // 0xB25EF0
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCIsVeggie;          // 0xB25F0C
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCNoFireBall;        // 0xB25F28
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCNoIceBall;         // 0xB25F44
            VB6ArrayRef<VB6Bool, 0, maxNPCType>             NPCNoGravity;         // 0xB25F60
            VB6ArrayRef<int16_t, 0, maxNPCType>             NPCFrame;             // 0xB25F7C
            VB6ArrayRef<int16_t, 0, maxNPCType>             NPCFrameSpeed;        // 0xB25F98
            VB6ArrayRef<int16_t, 0, maxNPCType>             NPCFrameStyle;        // 0xB25FB4
            NPCDefaults_t                                   _DONTUSE_NPCDefaults; // 0xB25FC0 NOTE: Do not use this. This is fixed-size, and the arrays from it get replaced in NpcIdExtender.cpp
            VB6ArrayRef<VB6Bool, 0, maxBlockType>           BlockIsSizable;       // 0xB2B930
            VB6ArrayRef<int16_t, 0, maxBlockType>           BlockSlope;           // 0xB2B94C
            VB6ArrayRef<int16_t, 0, maxBlockType>           BlockSlope2;          // 0xB2B968
            VB6ArrayRef<double, 0, maxPlayers>              vScreenX;             // 0xB2B984
            VB6ArrayRef<double, 0, maxPlayers>              vScreenY;             // 0xB2B9A0
            VB6ArrayRef<double, 1, maxPlayers>              qScreenX;             // 0xB2B9BC
            VB6ArrayRef<double, 1, maxPlayers>              qScreenY;             // 0xB2B9D8
            VB6Bool                                         qScreen;              // 0xB2B9E4
            int16_t                                         _padding14_;          // 0xB2B9E6
            VB6ArrayRef<int16_t, 0, maxBlockType>           BlockWidth;           // 0xB2B9F8
            VB6ArrayRef<int16_t, 0, maxBlockType>           BlockHeight;          // 0xB2BA14
            VB6ArrayRef<int16_t, 1, 100>                    BonusWidth;           // 0xB2BA30
            VB6ArrayRef<int16_t, 1, 100>                    BonusHeight;          // 0xB2BA4C
            VB6ArrayRef<int16_t, 1, maxEffectType>          EffectWidth;          // 0xB2BA68
            VB6ArrayRef<int16_t, 1, maxEffectType>          EffectHeight;         // 0xB2BA84
            EffectDefaults_t                                EffectDefaults;       // 0xB2BA90
            VB6ArrayRef<int16_t, 1, 100>                    SceneWidth;           // 0xB2BDC0
            VB6ArrayRef<int16_t, 1, 100>                    SceneHeight;          // 0xB2BDDC
            VB6ArrayRef<VB6Bool, 1, 200>                    BackgroundHasNoMask;  // 0xB2BDF8 (NOTE: using 'maxBackgroundType' because rsizing missed by NpcIdExtender. Doesn't seem to matter though?)
            VB6ArrayRef<VB6Bool, 0, maxBackgroundType>      Foreground;           // 0xB2BE14
            VB6ArrayRef<int16_t, 1, maxBackgroundType>      BackgroundWidth;      // 0xB2BE30
            VB6ArrayRef<int16_t, 1, maxBackgroundType>      BackgroundHeight;     // 0xB2BE4C
            VB6ArrayRef<int16_t, 1, 200>                    BackgroundFrame;      // 0xB2BE68 (NOTE: using 'maxBackgroundType' because rsizing missed by NpcIdExtender. Doesn't seem to matter though?)
            VB6ArrayRef<int16_t, 1, 200>                    BackgroundFrameCount; // 0xB2BE84 (NOTE: using 'maxBackgroundType' because rsizing missed by NpcIdExtender. Doesn't seem to matter though?)
            VB6ArrayRef<int16_t, 1, maxBlockType>           BlockFrame;           // 0xB2BEA0
            VB6ArrayRef<int16_t, 1, maxBlockType>           BlockFrame2;          // 0xB2BEBC
            VB6ArrayRef<int16_t, 1, 1000>                   sBlockArray;          // 0xB2BED8
            int16_t                                         sBlockNum;            // 0xB2BEE4
            int16_t                                         _padding15_;          // 0xB2BEE6
            VB6ArrayRef<int16_t, 1, maxSceneType>           SceneFrame;           // 0xB2BEF8
            VB6ArrayRef<int16_t, 1, maxSceneType>           SceneFrame2;          // 0xB2BF14
            VB6ArrayRef<int16_t, 0, 100>                    SpecialFrame;         // 0xB2BF30
            VB6ArrayRef<float, 0, 100>                      SpecialFrameCount;    // 0xB2BF4C
            VB6ArrayRef<int16_t, 1, maxTileType>            TileWidth;            // 0xB2BF68
            VB6ArrayRef<int16_t, 1, maxTileType>            TileHeight;           // 0xB2BF84
            VB6ArrayRef<int16_t, 1, maxTileType>            TileFrame;            // 0xB2BFA0
            VB6ArrayRef<int16_t, 1, maxTileType>            TileFrame2;           // 0xB2BFBC
            VB6ArrayRef<int16_t, 1, 100>                    LevelFrame;           // 0xB2BFD8
            VB6ArrayRef<int16_t, 1, 100>                    LevelFrame2;          // 0xB2BFF4
            VB6ArrayRef<VB6Bool, 1, maxBlockType>           BlockHasNoMask;       // 0xB2C010
            VB6ArrayRef<VB6Bool, 1, 100>                    LevelHasNoMask;       // 0xB2C02C
            VB6ArrayRef<VB6Bool, 0, maxBlockType>           BlockOnlyHitspot1;    // 0xB2C048
            VB6ArrayRef<VB6Bool, 0, maxBlockType>           BlockKills;           // 0xB2C064
            VB6ArrayRef<VB6Bool, 0, maxBlockType>           BlockKills2;          // 0xB2C080
            VB6ArrayRef<VB6Bool, 0, maxBlockType>           BlockHurts;           // 0xB2C09C
            VB6ArrayRef<VB6Bool, 0, maxBlockType>           BlockPSwitch;         // 0xB2C0B8
            VB6ArrayRef<VB6Bool, 0, maxBlockType>           BlockNoClipping;      // 0xB2C0D4
            VB6ArrayRef<int16_t, 1, 10>                     CoinFrame;            // 0xB2C0F0
            VB6ArrayRef<int16_t, 1, 10>                     CoinFrame2;           // 0xB2C10C
            EditorCursor_t                                  EditorCursor;         // 0xB2C118
            EditorControls_t                                EditorControls;       // 0xB2C558
            VB6ArrayRef<int16_t, 1, numSounds>              Sound;                // 0xB2C574
            VB6ArrayRef<int16_t, 1, numSounds>              SoundPause;           // 0xB2C590
            VB6Bool                                         EndLevel;             // 0xB2C59C
            int16_t                                         LevelMacro;           // 0xB2C59E
            int16_t                                         LevelMacroCounter;    // 0xB2C5A0
            int16_t                                         numJoysticks;         // 0xB2C5A2
            VB6StrPtr                                       FileName;             // 0xB2C5A4
            int16_t                                         Coins;                // 0xB2C5A8
            int16_t                                         _padding16_;          // 0xB2C5AA
            float                                           Lives;                // 0xB2C5AC
            VB6Bool                                         EndIntro;             // 0xB2C5B0
            VB6Bool                                         ExitMenu;             // 0xB2C5B2
            VB6Bool                                         LevelSelect;          // 0xB2C5B4
            int16_t                                         _padding17_;          // 0xB2C5B6
            VB6ArrayRef<WorldPlayer_t, 0, 1>                WorldPlayer;          // 0xB2C5C8
            int16_t                                         LevelBeatCode;        // 0xB2C5D4
            int16_t                                         curWorldLevel;        // 0xB2C5D6
            int16_t                                         curWorldMusic;        // 0xB2C5D8
            int16_t                                         _padding18_;          // 0xB2C5DA
            VB6ArrayRef<VB6Bool, 0, maxSections>            NoTurnBack;           // 0xB2C5EC
            VB6ArrayRef<VB6Bool, 0, maxSections>            UnderWater;           // 0xB2C608
            VB6Bool                                         TestLevel;            // 0xB2C614
            int16_t                                         _padding19_;          // 0xB2C616
            VB6StrPtr                                       FullFileName;         // 0xB2C618
            VB6StrPtr                                       FileNamePath;         // 0xB2C61C
            VB6Bool                                         GameMenu;             // 0xB2C620
            int16_t                                         _padding20_;          // 0xB2C622
            VB6StrPtr                                       WorldName;            // 0xB2C624
            int16_t                                         selWorld;             // 0xB2C628
            int16_t                                         selSave;              // 0xB2C62A
            int16_t                                         PSwitchTime;          // 0xB2C62C
            int16_t                                         PSwitchStop;          // 0xB2C62E
            int16_t                                         PSwitchPlayer;        // 0xB2C630
            int16_t                                         _padding21_;          // 0xB2C632
            VB6ArrayRef<int16_t, 1, 3>                      SaveSlot;             // 0xB2C644
            VB6ArrayRef<int16_t, 1, 3>                      SaveStars;            // 0xB2C660
            int16_t                                         BeltDirection;        // 0xB2C66C
            VB6Bool                                         BeatTheGame;          // 0xB2C66E
            int16_t                                         cycleCount;           // 0xB2C670
            int16_t                                         _padding22_;          // 0xB2C672
            double                                          fpsTime;              // 0xB2C674
            double                                          fpsCount;             // 0xB2C67C
            VB6Bool                                         FrameSkip;            // 0xB2C684
            int16_t                                         _padding23_;          // 0xB2C686
            double                                          GoalTime;             // 0xB2C688
            double                                          overTime;             // 0xB2C690
            int16_t                                         worldCurs;            // 0xB2C698
            int16_t                                         minShow;              // 0xB2C69A
            int16_t                                         maxShow;              // 0xB2C69C
            int16_t                                         _padding24_;          // 0xB2C69E
            VB6ArrayRef<Layer_t, 0, maxLayers-1>            Layer;                // 0xB2C6B0
            VB6ArrayRef<Events_t, 0, maxEvents-1>           Events;               // 0xB2C6CC
            int16_t                                         ReturnWarp;           // 0xB2C6D8
            int16_t                                         StartWarp;            // 0xB2C6DA
            Physics_t                                       Physics;              // 0xB2C6DC
            int16_t                                         MenuCursor;           // 0xB2C880
            int16_t                                         MenuMode;             // 0xB2C882
            VB6Bool                                         MenuCursorCanMove;    // 0xB2C884
            VB6Bool                                         MenuCursorCanMove2;   // 0xB2C886
            VB6Bool                                         NextFrame;            // 0xB2C888
            int16_t                                         StopHit;              // 0xB2C88A
            VB6Bool                                         MouseRelease;         // 0xB2C88C
            VB6Bool                                         TestFullscreen;       // 0xB2C88E
            VB6Bool                                         keyDownAlt;           // 0xB2C890
            VB6Bool                                         keyDownEnter;         // 0xB2C892
            VB6Bool                                         BlocksSorted;         // 0xB2C894
            int16_t                                         SingleCoop;           // 0xB2C896
            VB6StrPtr                                       CheatString;          // 0xB2C898
            VB6Bool                                         GameOutro;            // 0xB2C89C
            int16_t                                         _padding25_;          // 0xB2C89E
            float                                           CreditChop;           // 0xB2C8A0
            int16_t                                         EndCredits;           // 0xB2C8A4
            int16_t                                         curStars;             // 0xB2C8A6
            int16_t                                         maxStars;             // 0xB2C8A8
            VB6Bool                                         ShadowMode;           // 0xB2C8AA
            VB6Bool                                         MultiHop;             // 0xB2C8AC
            VB6Bool                                         SuperSpeed;           // 0xB2C8AE
            VB6Bool                                         WalkAnywhere;         // 0xB2C8B0
            VB6Bool                                         FlyForever;           // 0xB2C8B2
            VB6Bool                                         FreezeNPCs;           // 0xB2C8B4
            VB6Bool                                         CaptainN;             // 0xB2C8B6
            VB6Bool                                         FlameThrower;         // 0xB2C8B8
            VB6Bool                                         CoinMode;             // 0xB2C8BA
            VB6Bool                                         WorldUnlock;          // 0xB2C8BC
            VB6Bool                                         MaxFPS;               // 0xB2C8BE
            VB6Bool                                         GodMode;              // 0xB2C8C0
            VB6Bool                                         GrabAll;              // 0xB2C8C2
            VB6Bool                                         Cheater;              // 0xB2C8C4
            int16_t                                         _padding26_;          // 0xB2C8C6
            VB6ArrayRef<VB6StrPtr, 1, 5>                    WorldCredits;         // 0xB2C8D8
            int32_t                                         Score;                // 0xB2C8E4
            VB6ArrayRef<int16_t, 1, 13>                     Points;               // 0xB2C8F8
            int16_t                                         oldJumpJoy;           // 0xB2C904
            int16_t                                         MaxWorldStars;        // 0xB2C906
            VB6Bool                                         Debugger;             // 0xB2C908
            int16_t                                         _padding27_;          // 0xB2C90A
            VB6ArrayRef<Player_t, 0, 10>                    SavedChar;            // 0xB2C91C
            int16_t                                         LoadCoins;            // 0xB2C928
            int16_t                                         _padding28_;          // 0xB2C92A
            float                                           LoadCoinsT;           // 0xB2C92C
            VB6ArrayRef<VB6Bool, 1, maxBlockType>           GFXBlockCustom;       // 0xB2C940
            VB6ArrayRef<int32_t, 1, maxBlockType>           GFXBlock;             // 0xB2C95C
            VB6ArrayRef<int32_t, 1, maxBlockType>           GFXBlockMask;         // 0xB2C978
            VB6ArrayRef<StdPicture_t, 1, maxBlockType>      GFXBlockBMP;          // 0xB2C9A0
            VB6ArrayRef<StdPicture_t, 1, maxBlockType>      GFXBlockMaskBMP;      // 0xB2C9C8
            VB6ArrayRef<VB6Bool, 1, numBackground2>         GFXBackground2Custom; // 0xB2C9E4
            VB6ArrayRef<int32_t, 1, numBackground2>         GFXBackground2;       // 0xB2CA00
            VB6ArrayRef<StdPicture_t, 1, numBackground2>    GFXBackground2BMP;    // 0xB2CA28
            VB6ArrayRef<int16_t, 1, numBackground2>         GFXBackground2Height; // 0xB2CA44
            VB6ArrayRef<int16_t, 1, numBackground2>         GFXBackground2Width;  // 0xB2CA60
            VB6ArrayRef<VB6Bool, 1, maxNPCType>             GFXNPCCustom;         // 0xB2CA7C
            VB6ArrayRef<int32_t, 1, maxNPCType>             GFXNPC;               // 0xB2CA98
            VB6ArrayRef<int32_t, 1, maxNPCType>             GFXNPCMask;           // 0xB2CAB4
            VB6ArrayRef<StdPicture_t, 1, maxNPCType>        GFXNPCBMP;            // 0xB2CADC
            VB6ArrayRef<StdPicture_t, 1, maxNPCType>        GFXNPCMaskBMP;        // 0xB2CB04
            VB6ArrayRef<int16_t, 1, maxNPCType>             GFXNPCHeight;         // 0xB2CB20
            VB6ArrayRef<int16_t, 1, maxNPCType>             GFXNPCWidth;          // 0xB2CB3C
            VB6ArrayRef<VB6Bool, 1, maxEffectType>          GFXEffectCustom;      // 0xB2CB58
            VB6ArrayRef<int32_t, 1, maxEffectType>          GFXEffect;            // 0xB2CB74
            VB6ArrayRef<int32_t, 1, maxEffectType>          GFXEffectMask;        // 0xB2CB90
            VB6ArrayRef<StdPicture_t, 1, maxEffectType>     GFXEffectBMP;         // 0xB2CBB8
            VB6ArrayRef<StdPicture_t, 1, maxEffectType>     GFXEffectMaskBMP;     // 0xB2CBE0
            VB6ArrayRef<int16_t, 1, maxEffectType>          GFXEffectHeight;      // 0xB2CBFC
            VB6ArrayRef<int16_t, 1, maxEffectType>          GFXEffectWidth;       // 0xB2CC18
            VB6ArrayRef<VB6Bool, 1, maxBackgroundType>      GFXBackgroundCustom;  // 0xB2CC34
            VB6ArrayRef<int32_t, 1, maxBackgroundType>      GFXBackground;        // 0xB2CC50
            VB6ArrayRef<int32_t, 1, maxBackgroundType>      GFXBackgroundMask;    // 0xB2CC6C
            VB6ArrayRef<StdPicture_t, 1, maxBackgroundType> GFXBackgroundBMP;     // 0xB2CC94
            VB6ArrayRef<StdPicture_t, 1, maxBackgroundType> GFXBackgroundMaskBMP; // 0xB2CCBC
            VB6ArrayRef<int16_t, 1, maxBackgroundType>      GFXBackgroundHeight;  // 0xB2CCD8
            VB6ArrayRef<int16_t, 1, maxBackgroundType>      GFXBackgroundWidth;   // 0xB2CCF4
            VB6ArrayRef<VB6Bool, 1, 10>                     GFXMarioCustom;       // 0xB2CD10
            VB6ArrayRef<int32_t, 1, 10>                     GFXMario;             // 0xB2CD2C
            VB6ArrayRef<int32_t, 1, 10>                     GFXMarioMask;         // 0xB2CD48
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXMarioBMP;          // 0xB2CD70
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXMarioMaskBMP;      // 0xB2CD98
            VB6ArrayRef<int16_t, 1, 10>                     GFXMarioHeight;       // 0xB2CDB4
            VB6ArrayRef<int16_t, 1, 10>                     GFXMarioWidth;        // 0xB2CDD0
            VB6ArrayRef<VB6Bool, 1, 10>                     GFXLuigiCustom;       // 0xB2CDEC
            VB6ArrayRef<int32_t, 1, 10>                     GFXLuigi;             // 0xB2CE08
            VB6ArrayRef<int32_t, 1, 10>                     GFXLuigiMask;         // 0xB2CE24
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXLuigiBMP;          // 0xB2CE4C
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXLuigiMaskBMP;      // 0xB2CE74
            VB6ArrayRef<int16_t, 1, 10>                     GFXLuigiHeight;       // 0xB2CE90
            VB6ArrayRef<int16_t, 1, 10>                     GFXLuigiWidth;        // 0xB2CEAC
            VB6ArrayRef<VB6Bool, 1, 10>                     GFXPeachCustom;       // 0xB2CEC8
            VB6ArrayRef<int32_t, 1, 10>                     GFXPeach;             // 0xB2CEE4
            VB6ArrayRef<int32_t, 1, 10>                     GFXPeachMask;         // 0xB2CF00
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXPeachBMP;          // 0xB2CF28
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXPeachMaskBMP;      // 0xB2CF50
            VB6ArrayRef<int16_t, 1, 10>                     GFXPeachHeight;       // 0xB2CF6C
            VB6ArrayRef<int16_t, 1, 10>                     GFXPeachWidth;        // 0xB2CF88
            VB6ArrayRef<VB6Bool, 1, 10>                     GFXToadCustom;        // 0xB2CFA4
            VB6ArrayRef<int32_t, 1, 10>                     GFXToad;              // 0xB2CFC0
            VB6ArrayRef<int32_t, 1, 10>                     GFXToadMask;          // 0xB2CFDC
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXToadBMP;           // 0xB2D004
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXToadMaskBMP;       // 0xB2D02C
            VB6ArrayRef<int16_t, 1, 10>                     GFXToadHeight;        // 0xB2D048
            VB6ArrayRef<int16_t, 1, 10>                     GFXToadWidth;         // 0xB2D064
            VB6ArrayRef<VB6Bool, 1, 10>                     GFXLinkCustom;        // 0xB2D080
            VB6ArrayRef<int32_t, 1, 10>                     GFXLink;              // 0xB2D09C
            VB6ArrayRef<int32_t, 1, 10>                     GFXLinkMask;          // 0xB2D0B8
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXLinkBMP;           // 0xB2D0E0
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXLinkMaskBMP;       // 0xB2D108
            VB6ArrayRef<int16_t, 1, 10>                     GFXLinkHeight;        // 0xB2D124
            VB6ArrayRef<int16_t, 1, 10>                     GFXLinkWidth;         // 0xB2D140
            VB6ArrayRef<VB6Bool, 1, 10>                     GFXYoshiBCustom;      // 0xB2D15C
            VB6ArrayRef<int32_t, 1, 10>                     GFXYoshiB;            // 0xB2D178
            VB6ArrayRef<int32_t, 1, 10>                     GFXYoshiBMask;        // 0xB2D194
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXYoshiBBMP;         // 0xB2D1BC
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXYoshiBMaskBMP;     // 0xB2D1E4
            VB6ArrayRef<VB6Bool, 1, 10>                     GFXYoshiTCustom;      // 0xB2D200
            VB6ArrayRef<int32_t, 1, 10>                     GFXYoshiT;            // 0xB2D21C
            VB6ArrayRef<int32_t, 1, 10>                     GFXYoshiTMask;        // 0xB2D238
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXYoshiTBMP;         // 0xB2D260
            VB6ArrayRef<StdPicture_t, 1, 10>                GFXYoshiTMaskBMP;     // 0xB2D288
            VB6ArrayRef<int32_t, 1, maxTileType>            GFXTileCustom;        // 0xB2D2A4
            VB6ArrayRef<int32_t, 1, maxTileType>            GFXTile;              // 0xB2D2C0
            VB6ArrayRef<StdPicture_t, 1, maxTileType>       GFXTileBMP;           // 0xB2D2E8
            VB6ArrayRef<int16_t, 1, maxTileType>            GFXTileHeight;        // 0xB2D304
            VB6ArrayRef<int16_t, 1, maxTileType>            GFXTileWidth;         // 0xB2D320
            VB6ArrayRef<int32_t, 0, maxLevelType>           GFXLevelCustom;       // 0xB2D33C
            VB6ArrayRef<int32_t, 0, maxLevelType>           GFXLevel;             // 0xB2D358
            VB6ArrayRef<int32_t, 0, maxLevelType>           GFXLevelMask;         // 0xB2D374
            VB6ArrayRef<StdPicture_t, 0, maxLevelType>      GFXLevelBMP;          // 0xB2D39C
            VB6ArrayRef<StdPicture_t, 0, maxLevelType>      GFXLevelMaskBMP;      // 0xB2D3C4
            VB6ArrayRef<int16_t, 0, maxLevelType>           GFXLevelHeight;       // 0xB2D3E0
            VB6ArrayRef<int16_t, 0, maxLevelType>           GFXLevelWidth;        // 0xB2D3FC
            VB6ArrayRef<VB6Bool, 0, maxLevelType>           GFXLevelBig;          // 0xB2D418
            VB6ArrayRef<int32_t, 1, maxSceneType>           GFXSceneCustom;       // 0xB2D434
            VB6ArrayRef<int32_t, 1, maxSceneType>           GFXScene;             // 0xB2D450
            VB6ArrayRef<int32_t, 1, maxSceneType>           GFXSceneMask;         // 0xB2D46C
            VB6ArrayRef<StdPicture_t, 1, maxSceneType>      GFXSceneBMP;          // 0xB2D494
            VB6ArrayRef<StdPicture_t, 1, maxSceneType>      GFXSceneMaskBMP;      // 0xB2D4BC
            VB6ArrayRef<int16_t, 1, maxSceneType>           GFXSceneHeight;       // 0xB2D4D8
            VB6ArrayRef<int16_t, 1, maxSceneType>           GFXSceneWidth;        // 0xB2D4F4
            VB6ArrayRef<int32_t, 1, maxPathType>            GFXPathCustom;        // 0xB2D510
            VB6ArrayRef<int32_t, 1, maxPathType>            GFXPath;              // 0xB2D52C
            VB6ArrayRef<int32_t, 1, maxPathType>            GFXPathMask;          // 0xB2D548
            VB6ArrayRef<StdPicture_t, 1, maxPathType>       GFXPathBMP;           // 0xB2D570
            VB6ArrayRef<StdPicture_t, 1, maxPathType>       GFXPathMaskBMP;       // 0xB2D598
            VB6ArrayRef<int16_t, 1, maxPathType>            GFXPathHeight;        // 0xB2D5B4
            VB6ArrayRef<int16_t, 1, maxPathType>            GFXPathWidth;         // 0xB2D5D0
            VB6ArrayRef<int32_t, 1, numCharacters>          GFXPlayerCustom;      // 0xB2D5EC
            VB6ArrayRef<int32_t, 1, numCharacters>          GFXPlayer;            // 0xB2D608
            VB6ArrayRef<int32_t, 1, numCharacters>          GFXPlayerMask;        // 0xB2D624
            VB6ArrayRef<StdPicture_t, 1, numCharacters>     GFXPlayerBMP;         // 0xB2D64C
            VB6ArrayRef<StdPicture_t, 1, numCharacters>     GFXPlayerMaskBMP;     // 0xB2D674
            VB6ArrayRef<int16_t, 1, numCharacters>          GFXPlayerHeight;      // 0xB2D690
            VB6ArrayRef<int16_t, 1, numCharacters>          GFXPlayerWidth;       // 0xB2D6AC
            int16_t                                         PlayerCharacter;      // 0xB2D6B8
            int16_t                                         PlayerCharacter2;     // 0xB2D6BA
            double                                          MenuMouseX;           // 0xB2D6BC
            double                                          MenuMouseY;           // 0xB2D6C4
            VB6Bool                                         MenuMouseDown;        // 0xB2D6CC
            VB6Bool                                         MenuMouseBack;        // 0xB2D6CE
            VB6Bool                                         MenuMouseRelease;     // 0xB2D6D0
            VB6Bool                                         MenuMouseMove;        // 0xB2D6D2
            VB6Bool                                         MenuMouseClick;       // 0xB2D6D4
            int16_t                                         _padding29_;          // 0xB2D6D6
            VB6ArrayRef<VB6StrPtr, 1, 100>                  NewEvent;             // 0xB2D6E8
            VB6ArrayRef<int16_t, 1, 100>                    newEventDelay;        // 0xB2D704
            int16_t                                         newEventNum;          // 0xB2D710
            VB6Bool                                         ForcedControls;       // 0xB2D712
            Controls_t                                      ForcedControl;        // 0xB2D714
            int16_t                                         SyncCount;            // 0xB2D728
            VB6Bool                                         noUpdate;             // 0xB2D72A
            double                                          gameTime;             // 0xB2D72C
            VB6Bool                                         noSound;              // 0xB2D734
            int16_t                                         _padding30_;          // 0xB2D736
            double                                          tempTime;             // 0xB2D738
            VB6Bool                                         BattleMode;           // 0xB2D740
            int16_t                                         BattleWinner;         // 0xB2D742
            VB6ArrayRef<int16_t, 1, maxPlayers>             BattleLives;          // 0xB2D754
            int16_t                                         BattleIntro;          // 0xB2D760
            int16_t                                         BattleOutro;          // 0xB2D762
            VB6StrPtr                                       LevelName;            // 0xB2D764
        };
        #if !defined(__INTELLISENSE__)
                static_assert(sizeof(SMBX13::Types::modMain_t) == 0x8740, "sizeof(SMBX13::Types::modMain_t) must be 0x8740");
        #endif

    }

}
#pragma pack(pop)

#endif // !defined(SMBXINTERNAL_TYPES_H)
