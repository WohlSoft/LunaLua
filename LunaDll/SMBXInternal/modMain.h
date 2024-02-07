#if !defined(SMBXINTERNAL_MODMAIN_H)
#define SMBXINTERNAL_MODMAIN_H

#include <cstdint>
#include "../Misc/VB6StrPtr.h"

namespace SMBX13 {
    namespace Types {
        // Set packing settings (We've done all the padding ourselves)
        __pragma(pack(push,1))

        struct StdPicture_t;

        struct Controls_t {
            int16_t Up;      // 0x0
            int16_t Down;    // 0x2
            int16_t Left;    // 0x4
            int16_t Right;   // 0x6
            int16_t Jump;    // 0x8
            int16_t AltJump; // 0xA
            int16_t Run;     // 0xC
            int16_t AltRun;  // 0xE
            int16_t Drop;    // 0x10
            int16_t Start;   // 0x12
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Controls_t) == 0x14, "sizeof(SMBX13::Types::Controls_t) must be 0x14");
        #endif
        
        struct nPlayer_t {
            SMBX13::Types::Controls_t Controls;   // 0x0
            int16_t                   Cursor;     // 0x14
            int16_t                   IsMe;       // 0x16
            VB6StrPtr                 Nick;       // 0x18
            int16_t                   Active;     // 0x1C
            int16_t                   _padding1_; // 0x1E
            double                    ECurserX;   // 0x20
            double                    ECurserY;   // 0x28
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::nPlayer_t) == 0x30, "sizeof(SMBX13::Types::nPlayer_t) must be 0x30");
        #endif
        
        struct nPlay_t {
            int16_t                   Allow;              // 0x0
            int16_t                   Mode;               // 0x2
            VB6StrPtr                 ServerIP;           // 0x4
            int16_t                   ServerCon;          // 0x8
            int16_t                   _padding1_;         // 0xA
            VB6StrPtr                 ServerStr;          // 0xC
            int16_t                   ServerLocked;       // 0x10
            int16_t                   _padding2_;         // 0x12
            double                    ServerLoad1;        // 0x14
            int16_t                   ServerLoad;         // 0x1C
            int16_t                   ClientLocked[16];   // 0x1E
            int16_t                   _padding3_;         // 0x3E
            VB6StrPtr                 ClientIP[16];       // 0x40
            int16_t                   ClientCon[16];      // 0x80
            VB6StrPtr                 ClientName[16];     // 0xA0
            VB6StrPtr                 ClientStr[16];      // 0xE0
            int16_t                   ClientRelease[16];  // 0x120
            int16_t                   ClientPassword[16]; // 0x140
            double                    ClientLoad1[16];    // 0x160
            int16_t                   Online;             // 0x1E0
            int16_t                   MySlot;             // 0x1E2
            SMBX13::Types::Controls_t MyControls;         // 0x1E4
            SMBX13::Types::nPlayer_t  Player[16];         // 0x1F8
            int16_t                   PlayerWaitCount;    // 0x4F8
            int16_t                   _padding4_;         // 0x4FA
            float                     NPCWaitCount;       // 0x4FC
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::nPlay_t) == 0x500, "sizeof(SMBX13::Types::nPlay_t) must be 0x500");
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
            static_assert(sizeof(SMBX13::Types::Location_t) == 0x30, "sizeof(SMBX13::Types::Location_t) must be 0x30");
        #endif
        
        struct EditorControls_t {
            int16_t Up;         // 0x0
            int16_t Down;       // 0x2
            int16_t Left;       // 0x4
            int16_t Right;      // 0x6
            int16_t Mouse1;     // 0x8
            int16_t _padding1_; // 0xA
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::EditorControls_t) == 0xC, "sizeof(SMBX13::Types::EditorControls_t) must be 0xC");
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
            static_assert(sizeof(SMBX13::Types::conKeyboard_t) == 0x14, "sizeof(SMBX13::Types::conKeyboard_t) must be 0x14");
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
            static_assert(sizeof(SMBX13::Types::conJoystick_t) == 0xC, "sizeof(SMBX13::Types::conJoystick_t) must be 0xC");
        #endif
        
        struct NPC_t {
            VB6StrPtr                 AttLayer;           // 0x0
            int16_t                   Quicksand;          // 0x4
            int16_t                   RespawnDelay;       // 0x6
            int16_t                   Bouce;              // 0x8
            int16_t                   Pinched1;           // 0xA
            int16_t                   Pinched2;           // 0xC
            int16_t                   Pinched3;           // 0xE
            int16_t                   Pinched4;           // 0x10
            int16_t                   MovingPinched;      // 0x12
            int16_t                   NetTimeout;         // 0x14
            int16_t                   _padding1_;         // 0x16
            float                     RealSpeedX;         // 0x18
            int16_t                   Wet;                // 0x1C
            int16_t                   Settings;           // 0x1E
            int16_t                   NoLavaSplash;       // 0x20
            int16_t                   Slope;              // 0x22
            int16_t                   Multiplier;         // 0x24
            int16_t                   TailCD;             // 0x26
            int16_t                   Shadow;             // 0x28
            int16_t                   _padding2_;         // 0x2A
            VB6StrPtr                 TriggerActivate;    // 0x2C
            VB6StrPtr                 TriggerDeath;       // 0x30
            VB6StrPtr                 TriggerTalk;        // 0x34
            VB6StrPtr                 TriggerLast;        // 0x38
            VB6StrPtr                 Layer;              // 0x3C
            int16_t                   Hidden;             // 0x40
            int16_t                   Legacy;             // 0x42
            int16_t                   Chat;               // 0x44
            int16_t                   Inert;              // 0x46
            int16_t                   Stuck;              // 0x48
            int16_t                   DefaultStuck;       // 0x4A
            VB6StrPtr                 Text;               // 0x4C
            float                     oldAddBelt;         // 0x50
            int16_t                   PinchCount;         // 0x54
            int16_t                   Pinched;            // 0x56
            int16_t                   PinchedDirection;   // 0x58
            int16_t                   _padding3_;         // 0x5A
            float                     BeltSpeed;          // 0x5C
            int16_t                   standingOnPlayer;   // 0x60
            int16_t                   standingOnPlayerY;  // 0x62
            int16_t                   Generator;          // 0x64
            int16_t                   _padding4_;         // 0x66
            float                     GeneratorTimeMax;   // 0x68
            float                     GeneratorTime;      // 0x6C
            int16_t                   GeneratorDirection; // 0x70
            int16_t                   GeneratorEffect;    // 0x72
            int16_t                   GeneratorActive;    // 0x74
            int16_t                   playerTemp;         // 0x76
            SMBX13::Types::Location_t Location;           // 0x78
            SMBX13::Types::Location_t DefaultLocation;    // 0xA8
            float                     DefaultDirection;   // 0xD8
            int16_t                   DefaultType;        // 0xDC
            int16_t                   DefaultSpecial;     // 0xDE
            int16_t                   DefaultSpecial2;    // 0xE0
            int16_t                   Type;               // 0xE2
            int16_t                   Frame;              // 0xE4
            int16_t                   _padding5_;         // 0xE6
            float                     FrameCount;         // 0xE8
            float                     Direction;          // 0xEC
            double                    Special;            // 0xF0
            double                    Special2;           // 0xF8
            double                    Special3;           // 0x100
            double                    Special4;           // 0x108
            double                    Special5;           // 0x110
            double                    Special6;           // 0x118
            int16_t                   TurnAround;         // 0x120
            int16_t                   Killed;             // 0x122
            int16_t                   Active;             // 0x124
            int16_t                   Reset[2];           // 0x126
            int16_t                   TimeLeft;           // 0x12A
            int16_t                   HoldingPlayer;      // 0x12C
            int16_t                   CantHurt;           // 0x12E
            int16_t                   CantHurtPlayer;     // 0x130
            int16_t                   BattleOwner;        // 0x132
            int16_t                   WallDeath;          // 0x134
            int16_t                   Projectile;         // 0x136
            int16_t                   Effect;             // 0x138
            int16_t                   _padding6_;         // 0x13A
            double                    Effect2;            // 0x13C
            int16_t                   Effect3;            // 0x144
            int16_t                   Section;            // 0x146
            float                     Damage;             // 0x148
            int16_t                   JustActivated;      // 0x14C
            int16_t                   Block;              // 0x14E
            int16_t                   tempBlock;          // 0x150
            int16_t                   onWall;             // 0x152
            int16_t                   TurnBackWipe;       // 0x154
            int16_t                   Immune;             // 0x156
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::NPC_t) == 0x158, "sizeof(SMBX13::Types::NPC_t) must be 0x158");
        #endif
        
        struct Player_t {
            int16_t                   DoubleJump;           // 0x0
            int16_t                   FlySparks;            // 0x2
            int16_t                   Driving;              // 0x4
            int16_t                   Quicksand;            // 0x6
            int16_t                   Bombs;                // 0x8
            int16_t                   Slippy;               // 0xA
            int16_t                   Fairy;                // 0xC
            int16_t                   FairyCD;              // 0xE
            int16_t                   FairyTime;            // 0x10
            int16_t                   HasKey;               // 0x12
            int16_t                   SwordPoke;            // 0x14
            int16_t                   Hearts;               // 0x16
            int16_t                   CanFloat;             // 0x18
            int16_t                   FloatRelease;         // 0x1A
            int16_t                   FloatTime;            // 0x1C
            int16_t                   _padding1_;           // 0x1E
            float                     FloatSpeed;           // 0x20
            int16_t                   FloatDir;             // 0x24
            int16_t                   GrabTime;             // 0x26
            float                     GrabSpeed;            // 0x28
            double                    VineNPC;              // 0x2C
            int16_t                   Wet;                  // 0x34
            int16_t                   WetFrame;             // 0x36
            int16_t                   SwimCount;            // 0x38
            int16_t                   NoGravity;            // 0x3A
            int16_t                   Slide;                // 0x3C
            int16_t                   SlideKill;            // 0x3E
            int16_t                   Vine;                 // 0x40
            int16_t                   NoShellKick;          // 0x42
            int16_t                   ShellSurf;            // 0x44
            int16_t                   StateNPC;             // 0x46
            int16_t                   Slope;                // 0x48
            int16_t                   Stoned;               // 0x4A
            int16_t                   StonedCD;             // 0x4C
            int16_t                   StonedTime;           // 0x4E
            int16_t                   SpinJump;             // 0x50
            int16_t                   SpinFrame;            // 0x52
            int16_t                   SpinFireDir;          // 0x54
            int16_t                   Multiplier;           // 0x56
            int16_t                   SlideCounter;         // 0x58
            int16_t                   ShowWarp;             // 0x5A
            int16_t                   GroundPound;          // 0x5C
            int16_t                   GroundPound2;         // 0x5E
            int16_t                   CanPound;             // 0x60
            int16_t                   ForceHold;            // 0x62
            int16_t                   YoshiYellow;          // 0x64
            int16_t                   YoshiBlue;            // 0x66
            int16_t                   YoshiRed;             // 0x68
            int16_t                   YoshiWingsFrame;      // 0x6A
            int16_t                   YoshiWingsFrameCount; // 0x6C
            int16_t                   YoshiTX;              // 0x6E
            int16_t                   YoshiTY;              // 0x70
            int16_t                   YoshiTFrame;          // 0x72
            int16_t                   YoshiTFrameCount;     // 0x74
            int16_t                   YoshiBX;              // 0x76
            int16_t                   YoshiBY;              // 0x78
            int16_t                   YoshiBFrame;          // 0x7A
            int16_t                   YoshiBFrameCount;     // 0x7C
            int16_t                   _padding2_;           // 0x7E
            SMBX13::Types::Location_t YoshiTongue;          // 0x80
            float                     YoshiTongueX;         // 0xB0
            int16_t                   YoshiTongueLength;    // 0xB4
            int16_t                   YoshiTonugeBool;      // 0xB6
            int16_t                   YoshiNPC;             // 0xB8
            int16_t                   YoshiPlayer;          // 0xBA
            int16_t                   Dismount;             // 0xBC
            int16_t                   NoPlayerCol;          // 0xBE
            SMBX13::Types::Location_t Location;             // 0xC0
            int16_t                   Character;            // 0xF0
            int16_t                   _padding3_;           // 0xF2
            SMBX13::Types::Controls_t Controls;             // 0xF4
            int16_t                   Direction;            // 0x108
            int16_t                   Mount;                // 0x10A
            int16_t                   MountType;            // 0x10C
            int16_t                   MountSpecial;         // 0x10E
            int16_t                   MountOffsetY;         // 0x110
            int16_t                   MountFrame;           // 0x112
            int16_t                   State;                // 0x114
            int16_t                   Frame;                // 0x116
            float                     FrameCount;           // 0x118
            int16_t                   Jump;                 // 0x11C
            int16_t                   CanJump;              // 0x11E
            int16_t                   CanAltJump;           // 0x120
            int16_t                   Effect;               // 0x122
            double                    Effect2;              // 0x124
            int16_t                   DuckRelease;          // 0x12C
            int16_t                   Duck;                 // 0x12E
            int16_t                   DropRelease;          // 0x130
            int16_t                   StandUp;              // 0x132
            int16_t                   StandUp2;             // 0x134
            int16_t                   Bumped;               // 0x136
            float                     Bumped2;              // 0x138
            int16_t                   Dead;                 // 0x13C
            int16_t                   TimeToLive;           // 0x13E
            int16_t                   Immune;               // 0x140
            int16_t                   Immune2;              // 0x142
            int16_t                   ForceHitSpot3;        // 0x144
            int16_t                   Pinched1;             // 0x146
            int16_t                   Pinched2;             // 0x148
            int16_t                   Pinched3;             // 0x14A
            int16_t                   Pinched4;             // 0x14C
            int16_t                   NPCPinched;           // 0x14E
            float                     m2Speed;              // 0x150
            int16_t                   HoldingNPC;           // 0x154
            int16_t                   CanGrabNPCs;          // 0x156
            int16_t                   HeldBonus;            // 0x158
            int16_t                   Section;              // 0x15A
            int16_t                   WarpCD;               // 0x15C
            int16_t                   Warp;                 // 0x15E
            int16_t                   FireBallCD;           // 0x160
            int16_t                   FireBallCD2;          // 0x162
            int16_t                   TailCount;            // 0x164
            int16_t                   _padding4_;           // 0x166
            float                     RunCount;             // 0x168
            int16_t                   CanFly;               // 0x16C
            int16_t                   CanFly2;              // 0x16E
            int16_t                   FlyCount;             // 0x170
            int16_t                   RunRelease;           // 0x172
            int16_t                   JumpRelease;          // 0x174
            int16_t                   StandingOnNPC;        // 0x176
            int16_t                   StandingOnTempNPC;    // 0x178
            int16_t                   UnStart;              // 0x17A
            float                     mountBump;            // 0x17C
            float                     SpeedFixY;            // 0x180
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Player_t) == 0x184, "sizeof(SMBX13::Types::Player_t) must be 0x184");
        #endif
        
        struct Background_t {
            VB6StrPtr                 Layer;    // 0x0
            int16_t                   Hidden;   // 0x4
            int16_t                   Type;     // 0x6
            SMBX13::Types::Location_t Location; // 0x8
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Background_t) == 0x38, "sizeof(SMBX13::Types::Background_t) must be 0x38");
        #endif
        
        struct Water_t {
            VB6StrPtr                 Layer;      // 0x0
            int16_t                   Hidden;     // 0x4
            int16_t                   _padding1_; // 0x6
            float                     Buoy;       // 0x8
            int16_t                   Quicksand;  // 0xC
            int16_t                   _padding2_; // 0xE
            SMBX13::Types::Location_t Location;   // 0x10
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Water_t) == 0x40, "sizeof(SMBX13::Types::Water_t) must be 0x40");
        #endif
        
        struct Block_t {
            int16_t                   Slippy;            // 0x0
            int16_t                   RespawnDelay;      // 0x2
            int16_t                   RapidHit;          // 0x4
            int16_t                   DefaultType;       // 0x6
            int16_t                   DefaultSpecial;    // 0x8
            int16_t                   _padding1_;        // 0xA
            VB6StrPtr                 TriggerHit;        // 0xC
            VB6StrPtr                 TriggerDeath;      // 0x10
            VB6StrPtr                 TriggerLast;       // 0x14
            VB6StrPtr                 Layer;             // 0x18
            int16_t                   Hidden;            // 0x1C
            int16_t                   Type;              // 0x1E
            SMBX13::Types::Location_t Location;          // 0x20
            int16_t                   Special;           // 0x50
            int16_t                   ShakeY;            // 0x52
            int16_t                   ShakeY2;           // 0x54
            int16_t                   ShakeY3;           // 0x56
            int16_t                   Kill;              // 0x58
            int16_t                   Invis;             // 0x5A
            int16_t                   NPC;               // 0x5C
            int16_t                   IsPlayer;          // 0x5E
            int16_t                   IsNPC;             // 0x60
            int16_t                   standingOnPlayerY; // 0x62
            int16_t                   noProjClipping;    // 0x64
            int16_t                   IsReally;          // 0x66
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Block_t) == 0x68, "sizeof(SMBX13::Types::Block_t) must be 0x68");
        #endif
        
        struct Effect_t {
            int16_t                   Type;       // 0x0
            int16_t                   _padding1_; // 0x2
            SMBX13::Types::Location_t Location;   // 0x4
            int16_t                   Frame;      // 0x34
            int16_t                   _padding2_; // 0x36
            float                     FrameCount; // 0x38
            int16_t                   Life;       // 0x3C
            int16_t                   NewNpc;     // 0x3E
            int16_t                   Shadow;     // 0x40
            int16_t                   _padding3_; // 0x42
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Effect_t) == 0x44, "sizeof(SMBX13::Types::Effect_t) must be 0x44");
        #endif
        
        struct vScreen_t {
            double  Left;       // 0x0
            double  Top;        // 0x8
            double  Width;      // 0x10
            double  Height;     // 0x18
            int16_t Visible;    // 0x20
            int16_t _padding1_; // 0x22
            double  tempX;      // 0x24
            double  TempY;      // 0x2C
            int16_t TempDelay;  // 0x34
            int16_t _padding2_; // 0x36
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::vScreen_t) == 0x38, "sizeof(SMBX13::Types::vScreen_t) must be 0x38");
        #endif
        
        struct WorldLevel_t {
            SMBX13::Types::Location_t Location;     // 0x0
            int16_t                   Type;         // 0x30
            int16_t                   _padding1_;   // 0x32
            VB6StrPtr                 FileName;     // 0x34
            int16_t                   LevelExit[4]; // 0x38
            int16_t                   Active;       // 0x40
            int16_t                   _padding2_;   // 0x42
            VB6StrPtr                 LevelName;    // 0x44
            int16_t                   StartWarp;    // 0x48
            int16_t                   _padding3_;   // 0x4A
            double                    WarpX;        // 0x4C
            double                    WarpY;        // 0x54
            int16_t                   Path;         // 0x5C
            int16_t                   Path2;        // 0x5E
            int16_t                   Start;        // 0x60
            int16_t                   Visible;      // 0x62
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::WorldLevel_t) == 0x64, "sizeof(SMBX13::Types::WorldLevel_t) must be 0x64");
        #endif
        
        struct Warp_t {
            int16_t                   Locked;     // 0x0
            int16_t                   WarpNPC;    // 0x2
            int16_t                   NoYoshi;    // 0x4
            int16_t                   _padding1_; // 0x6
            VB6StrPtr                 Layer;      // 0x8
            int16_t                   Hidden;     // 0xC
            int16_t                   PlacedEnt;  // 0xE
            int16_t                   PlacedExit; // 0x10
            int16_t                   Stars;      // 0x12
            SMBX13::Types::Location_t Entrance;   // 0x14
            SMBX13::Types::Location_t Exit;       // 0x44
            int16_t                   Effect;     // 0x74
            int16_t                   _padding2_; // 0x76
            VB6StrPtr                 level;      // 0x78
            int16_t                   LevelWarp;  // 0x7C
            int16_t                   LevelEnt;   // 0x7E
            int16_t                   Direction;  // 0x80
            int16_t                   Direction2; // 0x82
            int16_t                   MapWarp;    // 0x84
            int16_t                   MapX;       // 0x86
            int16_t                   MapY;       // 0x88
            int16_t                   curStars;   // 0x8A
            int16_t                   maxStars;   // 0x8C
            int16_t                   _padding3_; // 0x8E
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Warp_t) == 0x90, "sizeof(SMBX13::Types::Warp_t) must be 0x90");
        #endif
        
         struct Tile_t {
            SMBX13::Types::Location_t Location;   // 0x0
            int16_t                   Type;       // 0x30
            int16_t                   _padding1_; // 0x32
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Tile_t) == 0x34, "sizeof(SMBX13::Types::Tile_t) must be 0x34");
        #endif
        
        struct Scene_t {
            SMBX13::Types::Location_t Location; // 0x0
            int16_t                   Type;     // 0x30
            int16_t                   Active;   // 0x32
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Scene_t) == 0x34, "sizeof(SMBX13::Types::Scene_t) must be 0x34");
        #endif
        
        struct WorldPath_t {
            SMBX13::Types::Location_t Location; // 0x0
            int16_t                   Active;   // 0x30
            int16_t                   Type;     // 0x32
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::WorldPath_t) == 0x34, "sizeof(SMBX13::Types::WorldPath_t) must be 0x34");
        #endif
        
        struct WorldMusic_t {
            SMBX13::Types::Location_t Location;   // 0x0
            int16_t                   Type;       // 0x30
            int16_t                   _padding1_; // 0x32
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::WorldMusic_t) == 0x34, "sizeof(SMBX13::Types::WorldMusic_t) must be 0x34");
        #endif
        
        struct EditorCursor_t {
            float                       X;            // 0x0
            float                       Y;            // 0x4
            int16_t                     SelectedMode; // 0x8
            int16_t                     Selected;     // 0xA
            SMBX13::Types::Location_t   Location;     // 0xC
            VB6StrPtr                   Layer;        // 0x3C
            int16_t                     Mode;         // 0x40
            int16_t                     _padding1_;   // 0x42
            SMBX13::Types::Block_t      Block;        // 0x44
            SMBX13::Types::Water_t      Water;        // 0xAC
            SMBX13::Types::Background_t Background;   // 0xEC
            SMBX13::Types::NPC_t        NPC;          // 0x124
            SMBX13::Types::Warp_t       Warp;         // 0x27C
            SMBX13::Types::Tile_t       Tile;         // 0x30C
            SMBX13::Types::Scene_t      Scene;        // 0x340
            SMBX13::Types::WorldLevel_t WorldLevel;   // 0x374
            SMBX13::Types::WorldPath_t  WorldPath;    // 0x3D8
            SMBX13::Types::WorldMusic_t WorldMusic;   // 0x40C
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::EditorCursor_t) == 0x440, "sizeof(SMBX13::Types::EditorCursor_t) must be 0x440");
        #endif
        
        struct WorldPlayer_t {
            SMBX13::Types::Location_t Location;  // 0x0
            int16_t                   Type;      // 0x30
            int16_t                   Frame;     // 0x32
            int16_t                   Frame2;    // 0x34
            int16_t                   Move;      // 0x36
            int16_t                   Move2;     // 0x38
            int16_t                   Move3;     // 0x3A
            VB6StrPtr                 LevelName; // 0x3C
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::WorldPlayer_t) == 0x40, "sizeof(SMBX13::Types::WorldPlayer_t) must be 0x40");
        #endif
        
        struct Layer_t {
            int16_t   EffectStop; // 0x0
            int16_t   _padding1_; // 0x2
            VB6StrPtr Name;       // 0x4
            int16_t   Hidden;     // 0x8
            int16_t   _padding2_; // 0xA
            float     SpeedX;     // 0xC
            float     SpeedY;     // 0x10
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Layer_t) == 0x14, "sizeof(SMBX13::Types::Layer_t) must be 0x14");
        #endif
        
        struct CreditLine_t {
            SMBX13::Types::Location_t Location; // 0x0
            VB6StrPtr                 Text;     // 0x30
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::CreditLine_t) == 0x34, "sizeof(SMBX13::Types::CreditLine_t) must be 0x34");
        #endif
        
        struct SelectWorld_t {
            VB6StrPtr WorldName;    // 0x0
            VB6StrPtr WorldPath;    // 0x4
            VB6StrPtr WorldFile;    // 0x8
            int16_t   blockChar[5]; // 0xC
            int16_t   _padding1_;   // 0x16
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::SelectWorld_t) == 0x18, "sizeof(SMBX13::Types::SelectWorld_t) must be 0x18");
        #endif
        
        struct Star_t {
            VB6StrPtr level;      // 0x0
            int16_t   Section;    // 0x4
            int16_t   _padding1_; // 0x6
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Star_t) == 0x8, "sizeof(SMBX13::Types::Star_t) must be 0x8");
        #endif
        
        struct NPCDefaults_t {
            int16_t NPCFrameOffsetX[301];    // 0x0
            int16_t NPCFrameOffsetY[301];    // 0x25A
            int16_t NPCWidth[301];           // 0x4B4
            int16_t NPCHeight[301];          // 0x70E
            int16_t NPCWidthGFX[301];        // 0x968
            int16_t NPCHeightGFX[301];       // 0xBC2
            int16_t NPCIsAShell[301];        // 0xE1C
            int16_t NPCIsABlock[301];        // 0x1076
            int16_t NPCIsAHit1Block[301];    // 0x12D0
            int16_t NPCIsABonus[301];        // 0x152A
            int16_t NPCIsACoin[301];         // 0x1784
            int16_t NPCIsAVine[301];         // 0x19DE
            int16_t NPCIsAnExit[301];        // 0x1C38
            int16_t NPCIsAParaTroopa[301];   // 0x1E92
            int16_t NPCIsCheep[301];         // 0x20EC
            int16_t NPCJumpHurt[301];        // 0x2346
            int16_t NPCNoClipping[301];      // 0x25A0
            int16_t NPCScore[301];           // 0x27FA
            int16_t NPCCanWalkOn[301];       // 0x2A54
            int16_t NPCGrabFromTop[301];     // 0x2CAE
            int16_t NPCTurnsAtCliffs[301];   // 0x2F08
            int16_t NPCWontHurt[301];        // 0x3162
            int16_t NPCMovesPlayer[301];     // 0x33BC
            int16_t NPCStandsOnPlayer[301];  // 0x3616
            int16_t NPCIsGrabbable[301];     // 0x3870
            int16_t NPCIsBoot[301];          // 0x3ACA
            int16_t NPCIsYoshi[301];         // 0x3D24
            int16_t NPCIsToad[301];          // 0x3F7E
            int16_t NPCNoYoshi[301];         // 0x41D8
            int16_t NPCForeground[301];      // 0x4432
            int16_t NPCIsABot[301];          // 0x468C
            int16_t NPCDefaultMovement[301]; // 0x48E6
            int16_t NPCIsVeggie[301];        // 0x4B40
            int16_t _padding1_;              // 0x4D9A
            float   NPCSpeedvar[301];        // 0x4D9C
            int16_t NPCNoFireBall[301];      // 0x5250
            int16_t NPCNoIceBall[301];       // 0x54AA
            int16_t NPCNoGravity[301];       // 0x5704
            int16_t _padding2_;              // 0x595E
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::NPCDefaults_t) == 0x5960, "sizeof(SMBX13::Types::NPCDefaults_t) must be 0x5960");
        #endif
        
        struct EffectDefaults_t {
            int16_t EffectWidth[200];  // 0x0
            int16_t EffectHeight[200]; // 0x190
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::EffectDefaults_t) == 0x320, "sizeof(SMBX13::Types::EffectDefaults_t) must be 0x320");
        #endif
        
        struct Physics_t {
            int16_t PlayerJumpHeight;       // 0x0
            int16_t PlayerBlockJumpHeight;  // 0x2
            int16_t PlayerHeadJumpHeight;   // 0x4
            int16_t PlayerNPCJumpHeight;    // 0x6
            int16_t PlayerSpringJumpHeight; // 0x8
            int16_t _padding1_;             // 0xA
            float   PlayerJumpVelocity;     // 0xC
            float   PlayerRunSpeed;         // 0x10
            float   PlayerWalkSpeed;        // 0x14
            int16_t PlayerTerminalVelocity; // 0x18
            int16_t _padding2_;             // 0x1A
            float   PlayerGravity;          // 0x1C
            int16_t PlayerHeight[5][7];     // 0x20
            int16_t PlayerDuckHeight[5][7]; // 0x66
            int16_t PlayerWidth[5][7];      // 0xAC
            int16_t PlayerGrabSpotX[5][7];  // 0xF2
            int16_t PlayerGrabSpotY[5][7];  // 0x138
            int16_t NPCTimeOffScreen;       // 0x17E
            int16_t NPCCanHurtWait;         // 0x180
            int16_t _padding3_;             // 0x182
            float   NPCShellSpeed;          // 0x184
            float   NPCShellSpeedY;         // 0x188
            float   NPCWalkingSpeed;        // 0x18C
            float   NPCWalkingOnSpeed;      // 0x190
            float   NPCMushroomSpeed;       // 0x194
            float   NPCGravity;             // 0x198
            float   NPCGravityReal;         // 0x19C
            int16_t NPCPSwitch;             // 0x1A0
            int16_t _padding4_;             // 0x1A2
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Physics_t) == 0x1A4, "sizeof(SMBX13::Types::Physics_t) must be 0x1A4");
        #endif
        
        struct Events_t {
            VB6StrPtr                 addSavedEvent;    // 0x0
            VB6StrPtr                 RemoveSavedEvent; // 0x4
            int16_t                   LayerSmoke;       // 0x8
            int16_t                   Sound;            // 0xA
            VB6StrPtr                 Name;             // 0xC
            VB6StrPtr                 Text;             // 0x10
            VB6StrPtr                 HideLayer[21];    // 0x14
            VB6StrPtr                 ShowLayer[21];    // 0x68
            VB6StrPtr                 ToggleLayer[21];  // 0xBC
            int16_t                   Music[21];        // 0x110
            int16_t                   Background[21];   // 0x13A
            SMBX13::Types::Location_t level[21];        // 0x164
            int16_t                   EndGame;          // 0x554
            int16_t                   _padding1_;       // 0x556
            VB6StrPtr                 TriggerEvent;     // 0x558
            double                    TriggerDelay;     // 0x55C
            SMBX13::Types::Controls_t Controls;         // 0x564
            VB6StrPtr                 MoveLayer;        // 0x578
            float                     SpeedX;           // 0x57C
            float                     SpeedY;           // 0x580
            float                     AutoX;            // 0x584
            float                     AutoY;            // 0x588
            int16_t                   AutoSection;      // 0x58C
            int16_t                   AutoStart;        // 0x58E
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::Events_t) == 0x590, "sizeof(SMBX13::Types::Events_t) must be 0x590");
        #endif
        
        struct modMain_t {
            int32_t                         myBackBuffer;                              // 0xB25028
            int32_t                         myBufferBMP;                               // 0xB2502C
            int16_t                         AllCharBlock;                              // 0xB25030
            int16_t                         _padding1_;                                // 0xB25032
            VB6StrPtr                       LocalNick;                                 // 0xB25034
            int16_t                         LocalCursor;                               // 0xB25038
            int16_t                         _padding2_;                                // 0xB2503A
            VB6StrPtr                       ClientPassword;                            // 0xB2503C
            VB6StrPtr                       ServerPassword;                            // 0xB25040
            int16_t                         ServerClear;                               // 0xB25044
            int16_t                         StartMenu;                                 // 0xB25046
            int16_t                         BlockFlash;                                // 0xB25048
            int16_t                         ScrollRelease;                             // 0xB2504A
            int16_t                         TakeScreen;                                // 0xB2504C
            int16_t                         _padding3_;                                // 0xB2504E
            VB6StrPtr                       LB;                                        // 0xB25050
            VB6StrPtr                       EoT;                                       // 0xB25054
            uint8_t                         _reserved_before_conKeyboard[16];          // 0xB25058
            SMBX13::Types::conKeyboard_t    (&conKeyboard)[2];                         // 0xB25068
            uint8_t                         _reserved_after_conKeyboard[8];            // 0xB2506C
            uint8_t                         _reserved_before_conJoystick[16];          // 0xB25074
            SMBX13::Types::conJoystick_t    (&conJoystick)[2];                         // 0xB25084
            uint8_t                         _reserved_after_conJoystick[8];            // 0xB25088
            uint8_t                         _reserved_before_useJoystick[16];          // 0xB25090
            int16_t                         (&useJoystick)[2];                         // 0xB250A0
            uint8_t                         _reserved_after_useJoystick[8];            // 0xB250A4
            int16_t                         ScreenShake;                               // 0xB250AC
            int16_t                         _padding4_;                                // 0xB250AE
            VB6StrPtr                       Checkpoint;                                // 0xB250B0
            int16_t                         MagicHand;                                 // 0xB250B4
            int16_t                         _padding5_;                                // 0xB250B6
            uint8_t                         _reserved_before_testPlayer[16];           // 0xB250B8
            SMBX13::Types::Player_t         (&testPlayer)[2];                          // 0xB250C8
            uint8_t                         _reserved_after_testPlayer[8];             // 0xB250CC
            int16_t                         ClearBuffer;                               // 0xB250D4
            int16_t                         numLocked;                                 // 0xB250D6
            int16_t                         resChanged;                                // 0xB250D8
            int16_t                         inputKey;                                  // 0xB250DA
            int16_t                         getNewKeyboard;                            // 0xB250DC
            int16_t                         getNewJoystick;                            // 0xB250DE
            int16_t                         lastJoyButton;                             // 0xB250E0
            int16_t                         GamePaused;                                // 0xB250E2
            VB6StrPtr                       MessageText;                               // 0xB250E4
            int16_t                         NumSelectWorld;                            // 0xB250E8
            int16_t                         _padding6_;                                // 0xB250EA
            uint8_t                         _reserved_before_SelectWorld[16];          // 0xB250EC
            SMBX13::Types::SelectWorld_t    (&SelectWorld)[100];                       // 0xB250FC
            uint8_t                         _reserved_after_SelectWorld[8];            // 0xB25100
            int16_t                         ShowFPS;                                   // 0xB25108
            int16_t                         _padding7_;                                // 0xB2510A
            double                          PrintFPS;                                  // 0xB2510C
            uint8_t                         _reserved_before_vScreen[16];              // 0xB25114
            SMBX13::Types::vScreen_t        (&vScreen)[3];                             // 0xB25124
            uint8_t                         _reserved_after_vScreen[8];                // 0xB25128
            int16_t                         ScreenType;                                // 0xB25130
            int16_t                         DScreenType;                               // 0xB25132
            int16_t                         LevelEditor;                               // 0xB25134
            int16_t                         WorldEditor;                               // 0xB25136
            uint8_t                         _reserved_before_PlayerStart[16];          // 0xB25138
            SMBX13::Types::Location_t       (&PlayerStart)[2];                         // 0xB25148
            uint8_t                         _reserved_after_PlayerStart[8];            // 0xB2514C
            uint8_t                         _reserved_before_blockCharacter[16];       // 0xB25154
            int16_t                         (&blockCharacter)[21];                     // 0xB25164
            uint8_t                         _reserved_after_blockCharacter[8];         // 0xB25168
            uint8_t                         _reserved_before_OwedMount[16];            // 0xB25170
            int16_t                         (&OwedMount)[201];                         // 0xB25180
            uint8_t                         _reserved_after_OwedMount[8];              // 0xB25184
            uint8_t                         _reserved_before_OwedMountType[16];        // 0xB2518C
            int16_t                         (&OwedMountType)[201];                     // 0xB2519C
            uint8_t                         _reserved_after_OwedMountType[8];          // 0xB251A0
            uint8_t                         _reserved_before_AutoX[16];                // 0xB251A8
            float                           (&AutoX)[21];                              // 0xB251B8
            uint8_t                         _reserved_after_AutoX[8];                  // 0xB251BC
            uint8_t                         _reserved_before_AutoY[16];                // 0xB251C4
            float                           (&AutoY)[21];                              // 0xB251D4
            uint8_t                         _reserved_after_AutoY[8];                  // 0xB251D8
            int16_t                         numStars;                                  // 0xB251E0
            int16_t                         _padding8_;                                // 0xB251E2
            SMBX13::Types::nPlay_t          nPlay;                                     // 0xB251E4
            uint8_t                         _reserved_before_Water[16];                // 0xB256E4
            SMBX13::Types::Water_t          (&Water)[1001];                            // 0xB256F4
            uint8_t                         _reserved_after_Water[8];                  // 0xB256F8
            int16_t                         numWater;                                  // 0xB25700
            int16_t                         _padding9_;                                // 0xB25702
            uint8_t                         _reserved_before_Star[16];                 // 0xB25704
            SMBX13::Types::Star_t           (&Star)[1000];                             // 0xB25714
            uint8_t                         _reserved_after_Star[8];                   // 0xB25718
            VB6StrPtr                       GoToLevel;                                 // 0xB25720
            VB6StrPtr                       StartLevel;                                // 0xB25724
            int16_t                         NoMap;                                     // 0xB25728
            int16_t                         RestartLevel;                              // 0xB2572A
            uint8_t                         _reserved_before_LevelChop[16];            // 0xB2572C
            float                           (&LevelChop)[21];                          // 0xB2573C
            uint8_t                         _reserved_after_LevelChop[8];              // 0xB25740
            uint8_t                         _reserved_before_FirstBlock[16];           // 0xB25748
            int16_t                         (&FirstBlock)[16001];                      // 0xB25758
            uint8_t                         _reserved_after_FirstBlock[8];             // 0xB2575C
            uint8_t                         _reserved_before_LastBlock[16];            // 0xB25764
            int16_t                         (&LastBlock)[16001];                       // 0xB25774
            uint8_t                         _reserved_after_LastBlock[8];              // 0xB25778
            int16_t                         MidBackground;                             // 0xB25780
            int16_t                         LastBackground;                            // 0xB25782
            int16_t                         iBlocks;                                   // 0xB25784
            int16_t                         _padding10_;                               // 0xB25786
            uint8_t                         _reserved_before_iBlock[16];               // 0xB25788
            int16_t                         (&iBlock)[20001];                          // 0xB25798
            uint8_t                         _reserved_after_iBlock[8];                 // 0xB2579C
            int16_t                         numTiles;                                  // 0xB257A4
            int16_t                         numScenes;                                 // 0xB257A6
            uint8_t                         _reserved_before_CustomMusic[16];          // 0xB257A8
            VB6StrPtr                       (&CustomMusic)[21];                        // 0xB257B8
            uint8_t                         _reserved_after_CustomMusic[8];            // 0xB257BC
            uint8_t                         _reserved_before_level[16];                // 0xB257C4
            SMBX13::Types::Location_t       (&level)[21];                              // 0xB257D4
            uint8_t                         _reserved_after_level[8];                  // 0xB257D8
            uint8_t                         _reserved_before_LevelWrap[16];            // 0xB257E0
            int16_t                         (&LevelWrap)[21];                          // 0xB257F0
            uint8_t                         _reserved_after_LevelWrap[8];              // 0xB257F4
            uint8_t                         _reserved_before_OffScreenExit[16];        // 0xB257FC
            int16_t                         (&OffScreenExit)[21];                      // 0xB2580C
            uint8_t                         _reserved_after_OffScreenExit[8];          // 0xB25810
            uint8_t                         _reserved_before_bgMusic[16];              // 0xB25818
            int16_t                         (&bgMusic)[21];                            // 0xB25828
            uint8_t                         _reserved_after_bgMusic[8];                // 0xB2582C
            uint8_t                         _reserved_before_bgMusicREAL[16];          // 0xB25834
            int16_t                         (&bgMusicREAL)[21];                        // 0xB25844
            uint8_t                         _reserved_after_bgMusicREAL[8];            // 0xB25848
            uint8_t                         _reserved_before_Background2REAL[16];      // 0xB25850
            int16_t                         (&Background2REAL)[21];                    // 0xB25860
            uint8_t                         _reserved_after_Background2REAL[8];        // 0xB25864
            uint8_t                         _reserved_before_LevelREAL[16];            // 0xB2586C
            SMBX13::Types::Location_t       (&LevelREAL)[21];                          // 0xB2587C
            uint8_t                         _reserved_after_LevelREAL[8];              // 0xB25880
            int16_t                         curMusic;                                  // 0xB25888
            int16_t                         _padding11_;                               // 0xB2588A
            uint8_t                         _reserved_before_bgColor[16];              // 0xB2588C
            int32_t                         (&bgColor)[21];                            // 0xB2589C
            uint8_t                         _reserved_after_bgColor[8];                // 0xB258A0
            uint8_t                         _reserved_before_Background2[16];          // 0xB258A8
            int16_t                         (&Background2)[21];                        // 0xB258B8
            uint8_t                         _reserved_after_Background2[8];            // 0xB258BC
            uint8_t                         _reserved_before_WorldPath[16];            // 0xB258C4
            SMBX13::Types::WorldPath_t      (&WorldPath)[2000];                        // 0xB258D4
            uint8_t                         _reserved_after_WorldPath[8];              // 0xB258D8
            int16_t                         numWorldPaths;                             // 0xB258E0
            int16_t                         numWarps;                                  // 0xB258E2
            uint8_t                         _reserved_before_Warp[16];                 // 0xB258E4
            SMBX13::Types::Warp_t           (&Warp)[200];                              // 0xB258F4
            uint8_t                         _reserved_after_Warp[8];                   // 0xB258F8
            uint8_t                         _reserved_before_Tile[16];                 // 0xB25900
            SMBX13::Types::Tile_t           (&Tile)[20000];                            // 0xB25910
            uint8_t                         _reserved_after_Tile[8];                   // 0xB25914
            uint8_t                         _reserved_before_Scene[16];                // 0xB2591C
            SMBX13::Types::Scene_t          (&Scene)[5000];                            // 0xB2592C
            uint8_t                         _reserved_after_Scene[8];                  // 0xB25930
            uint8_t                         _reserved_before_Credit[16];               // 0xB25938
            SMBX13::Types::CreditLine_t     (&Credit)[200];                            // 0xB25948
            uint8_t                         _reserved_after_Credit[8];                 // 0xB2594C
            int16_t                         numCredits;                                // 0xB25954
            int16_t                         numBlock;                                  // 0xB25956
            int16_t                         numBackground;                             // 0xB25958
            int16_t                         numNPCs;                                   // 0xB2595A
            int16_t                         numEffects;                                // 0xB2595C
            int16_t                         numPlayers;                                // 0xB2595E
            int16_t                         numWorldLevels;                            // 0xB25960
            int16_t                         _padding12_;                               // 0xB25962
            uint8_t                         _reserved_before_WorldMusic[16];           // 0xB25964
            SMBX13::Types::WorldMusic_t     (&WorldMusic)[1000];                       // 0xB25974
            uint8_t                         _reserved_after_WorldMusic[8];             // 0xB25978
            int16_t                         numWorldMusic;                             // 0xB25980
            int16_t                         _padding13_;                               // 0xB25982
            uint8_t                         _reserved_before_WorldLevel[16];           // 0xB25984
            SMBX13::Types::WorldLevel_t     (&WorldLevel)[400];                        // 0xB25994
            uint8_t                         _reserved_after_WorldLevel[8];             // 0xB25998
            uint8_t                         _reserved_before_Background[16];           // 0xB259A0
            SMBX13::Types::Background_t     (&Background)[8000];                       // 0xB259B0
            uint8_t                         _reserved_after_Background[8];             // 0xB259B4
            uint8_t                         _reserved_before_Effect[16];               // 0xB259BC
            SMBX13::Types::Effect_t         (&Effect)[1000];                           // 0xB259CC
            uint8_t                         _reserved_after_Effect[8];                 // 0xB259D0
            uint8_t                         _reserved_before_NPC[16];                  // 0xB259D8
            SMBX13::Types::NPC_t            (&NPC)[5129];                              // 0xB259E8
            uint8_t                         _reserved_after_NPC[8];                    // 0xB259EC
            uint8_t                         _reserved_before_Block[16];                // 0xB259F4
            SMBX13::Types::Block_t          (&Block)[20001];                           // 0xB25A04
            uint8_t                         _reserved_after_Block[8];                  // 0xB25A08
            uint8_t                         _reserved_before_Player[16];               // 0xB25A10
            SMBX13::Types::Player_t         (&Player)[201];                            // 0xB25A20
            uint8_t                         _reserved_after_Player[8];                 // 0xB25A24
            uint8_t                         _reserved_before_MarioFrameX[16];          // 0xB25A2C
            int16_t                         (&MarioFrameX)[751];                       // 0xB25A3C
            uint8_t                         _reserved_after_MarioFrameX[8];            // 0xB25A40
            uint8_t                         _reserved_before_MarioFrameY[16];          // 0xB25A48
            int16_t                         (&MarioFrameY)[751];                       // 0xB25A58
            uint8_t                         _reserved_after_MarioFrameY[8];            // 0xB25A5C
            uint8_t                         _reserved_before_LuigiFrameX[16];          // 0xB25A64
            int16_t                         (&LuigiFrameX)[751];                       // 0xB25A74
            uint8_t                         _reserved_after_LuigiFrameX[8];            // 0xB25A78
            uint8_t                         _reserved_before_LuigiFrameY[16];          // 0xB25A80
            int16_t                         (&LuigiFrameY)[751];                       // 0xB25A90
            uint8_t                         _reserved_after_LuigiFrameY[8];            // 0xB25A94
            uint8_t                         _reserved_before_PeachFrameX[16];          // 0xB25A9C
            int16_t                         (&PeachFrameX)[751];                       // 0xB25AAC
            uint8_t                         _reserved_after_PeachFrameX[8];            // 0xB25AB0
            uint8_t                         _reserved_before_PeachFrameY[16];          // 0xB25AB8
            int16_t                         (&PeachFrameY)[751];                       // 0xB25AC8
            uint8_t                         _reserved_after_PeachFrameY[8];            // 0xB25ACC
            uint8_t                         _reserved_before_ToadFrameX[16];           // 0xB25AD4
            int16_t                         (&ToadFrameX)[751];                        // 0xB25AE4
            uint8_t                         _reserved_after_ToadFrameX[8];             // 0xB25AE8
            uint8_t                         _reserved_before_ToadFrameY[16];           // 0xB25AF0
            int16_t                         (&ToadFrameY)[751];                        // 0xB25B00
            uint8_t                         _reserved_after_ToadFrameY[8];             // 0xB25B04
            uint8_t                         _reserved_before_LinkFrameX[16];           // 0xB25B0C
            int16_t                         (&LinkFrameX)[751];                        // 0xB25B1C
            uint8_t                         _reserved_after_LinkFrameX[8];             // 0xB25B20
            uint8_t                         _reserved_before_LinkFrameY[16];           // 0xB25B28
            int16_t                         (&LinkFrameY)[751];                        // 0xB25B38
            uint8_t                         _reserved_after_LinkFrameY[8];             // 0xB25B3C
            uint8_t                         _reserved_before_BackgroundFence[16];      // 0xB25B44
            int16_t                         (&BackgroundFence)[201];                   // 0xB25B54
            uint8_t                         _reserved_after_BackgroundFence[8];        // 0xB25B58
            uint8_t                         _reserved_before_NPCFrameOffsetX[16];      // 0xB25B60
            int16_t                         (&NPCFrameOffsetX)[301];                   // 0xB25B70
            uint8_t                         _reserved_after_NPCFrameOffsetX[8];        // 0xB25B74
            uint8_t                         _reserved_before_NPCFrameOffsetY[16];      // 0xB25B7C
            int16_t                         (&NPCFrameOffsetY)[301];                   // 0xB25B8C
            uint8_t                         _reserved_after_NPCFrameOffsetY[8];        // 0xB25B90
            uint8_t                         _reserved_before_NPCWidth[16];             // 0xB25B98
            int16_t                         (&NPCWidth)[301];                          // 0xB25BA8
            uint8_t                         _reserved_after_NPCWidth[8];               // 0xB25BAC
            uint8_t                         _reserved_before_NPCHeight[16];            // 0xB25BB4
            int16_t                         (&NPCHeight)[301];                         // 0xB25BC4
            uint8_t                         _reserved_after_NPCHeight[8];              // 0xB25BC8
            uint8_t                         _reserved_before_NPCWidthGFX[16];          // 0xB25BD0
            int16_t                         (&NPCWidthGFX)[301];                       // 0xB25BE0
            uint8_t                         _reserved_after_NPCWidthGFX[8];            // 0xB25BE4
            uint8_t                         _reserved_before_NPCHeightGFX[16];         // 0xB25BEC
            int16_t                         (&NPCHeightGFX)[301];                      // 0xB25BFC
            uint8_t                         _reserved_after_NPCHeightGFX[8];           // 0xB25C00
            uint8_t                         _reserved_before_NPCSpeedvar[16];          // 0xB25C08
            float                           (&NPCSpeedvar)[301];                       // 0xB25C18
            uint8_t                         _reserved_after_NPCSpeedvar[8];            // 0xB25C1C
            uint8_t                         _reserved_before_NPCIsAShell[16];          // 0xB25C24
            int16_t                         (&NPCIsAShell)[301];                       // 0xB25C34
            uint8_t                         _reserved_after_NPCIsAShell[8];            // 0xB25C38
            uint8_t                         _reserved_before_NPCIsABlock[16];          // 0xB25C40
            int16_t                         (&NPCIsABlock)[301];                       // 0xB25C50
            uint8_t                         _reserved_after_NPCIsABlock[8];            // 0xB25C54
            uint8_t                         _reserved_before_NPCIsAHit1Block[16];      // 0xB25C5C
            int16_t                         (&NPCIsAHit1Block)[301];                   // 0xB25C6C
            uint8_t                         _reserved_after_NPCIsAHit1Block[8];        // 0xB25C70
            uint8_t                         _reserved_before_NPCIsABonus[16];          // 0xB25C78
            int16_t                         (&NPCIsABonus)[301];                       // 0xB25C88
            uint8_t                         _reserved_after_NPCIsABonus[8];            // 0xB25C8C
            uint8_t                         _reserved_before_NPCIsACoin[16];           // 0xB25C94
            int16_t                         (&NPCIsACoin)[301];                        // 0xB25CA4
            uint8_t                         _reserved_after_NPCIsACoin[8];             // 0xB25CA8
            uint8_t                         _reserved_before_NPCIsAVine[16];           // 0xB25CB0
            int16_t                         (&NPCIsAVine)[301];                        // 0xB25CC0
            uint8_t                         _reserved_after_NPCIsAVine[8];             // 0xB25CC4
            uint8_t                         _reserved_before_NPCIsAnExit[16];          // 0xB25CCC
            int16_t                         (&NPCIsAnExit)[301];                       // 0xB25CDC
            uint8_t                         _reserved_after_NPCIsAnExit[8];            // 0xB25CE0
            uint8_t                         _reserved_before_NPCIsAParaTroopa[16];     // 0xB25CE8
            int16_t                         (&NPCIsAParaTroopa)[301];                  // 0xB25CF8
            uint8_t                         _reserved_after_NPCIsAParaTroopa[8];       // 0xB25CFC
            uint8_t                         _reserved_before_NPCIsCheep[16];           // 0xB25D04
            int16_t                         (&NPCIsCheep)[301];                        // 0xB25D14
            uint8_t                         _reserved_after_NPCIsCheep[8];             // 0xB25D18
            uint8_t                         _reserved_before_NPCJumpHurt[16];          // 0xB25D20
            int16_t                         (&NPCJumpHurt)[301];                       // 0xB25D30
            uint8_t                         _reserved_after_NPCJumpHurt[8];            // 0xB25D34
            uint8_t                         _reserved_before_NPCNoClipping[16];        // 0xB25D3C
            int16_t                         (&NPCNoClipping)[301];                     // 0xB25D4C
            uint8_t                         _reserved_after_NPCNoClipping[8];          // 0xB25D50
            uint8_t                         _reserved_before_NPCScore[16];             // 0xB25D58
            int16_t                         (&NPCScore)[301];                          // 0xB25D68
            uint8_t                         _reserved_after_NPCScore[8];               // 0xB25D6C
            uint8_t                         _reserved_before_NPCCanWalkOn[16];         // 0xB25D74
            int16_t                         (&NPCCanWalkOn)[301];                      // 0xB25D84
            uint8_t                         _reserved_after_NPCCanWalkOn[8];           // 0xB25D88
            uint8_t                         _reserved_before_NPCGrabFromTop[16];       // 0xB25D90
            int16_t                         (&NPCGrabFromTop)[301];                    // 0xB25DA0
            uint8_t                         _reserved_after_NPCGrabFromTop[8];         // 0xB25DA4
            uint8_t                         _reserved_before_NPCTurnsAtCliffs[16];     // 0xB25DAC
            int16_t                         (&NPCTurnsAtCliffs)[301];                  // 0xB25DBC
            uint8_t                         _reserved_after_NPCTurnsAtCliffs[8];       // 0xB25DC0
            uint8_t                         _reserved_before_NPCWontHurt[16];          // 0xB25DC8
            int16_t                         (&NPCWontHurt)[301];                       // 0xB25DD8
            uint8_t                         _reserved_after_NPCWontHurt[8];            // 0xB25DDC
            uint8_t                         _reserved_before_NPCMovesPlayer[16];       // 0xB25DE4
            int16_t                         (&NPCMovesPlayer)[301];                    // 0xB25DF4
            uint8_t                         _reserved_after_NPCMovesPlayer[8];         // 0xB25DF8
            uint8_t                         _reserved_before_NPCStandsOnPlayer[16];    // 0xB25E00
            int16_t                         (&NPCStandsOnPlayer)[301];                 // 0xB25E10
            uint8_t                         _reserved_after_NPCStandsOnPlayer[8];      // 0xB25E14
            uint8_t                         _reserved_before_NPCIsGrabbable[16];       // 0xB25E1C
            int16_t                         (&NPCIsGrabbable)[301];                    // 0xB25E2C
            uint8_t                         _reserved_after_NPCIsGrabbable[8];         // 0xB25E30
            uint8_t                         _reserved_before_NPCIsBoot[16];            // 0xB25E38
            int16_t                         (&NPCIsBoot)[301];                         // 0xB25E48
            uint8_t                         _reserved_after_NPCIsBoot[8];              // 0xB25E4C
            uint8_t                         _reserved_before_NPCIsYoshi[16];           // 0xB25E54
            int16_t                         (&NPCIsYoshi)[301];                        // 0xB25E64
            uint8_t                         _reserved_after_NPCIsYoshi[8];             // 0xB25E68
            uint8_t                         _reserved_before_NPCIsToad[16];            // 0xB25E70
            int16_t                         (&NPCIsToad)[301];                         // 0xB25E80
            uint8_t                         _reserved_after_NPCIsToad[8];              // 0xB25E84
            uint8_t                         _reserved_before_NPCNoYoshi[16];           // 0xB25E8C
            int16_t                         (&NPCNoYoshi)[301];                        // 0xB25E9C
            uint8_t                         _reserved_after_NPCNoYoshi[8];             // 0xB25EA0
            uint8_t                         _reserved_before_NPCForeground[16];        // 0xB25EA8
            int16_t                         (&NPCForeground)[301];                     // 0xB25EB8
            uint8_t                         _reserved_after_NPCForeground[8];          // 0xB25EBC
            uint8_t                         _reserved_before_NPCIsABot[16];            // 0xB25EC4
            int16_t                         (&NPCIsABot)[301];                         // 0xB25ED4
            uint8_t                         _reserved_after_NPCIsABot[8];              // 0xB25ED8
            uint8_t                         _reserved_before_NPCDefaultMovement[16];   // 0xB25EE0
            int16_t                         (&NPCDefaultMovement)[301];                // 0xB25EF0
            uint8_t                         _reserved_after_NPCDefaultMovement[8];     // 0xB25EF4
            uint8_t                         _reserved_before_NPCIsVeggie[16];          // 0xB25EFC
            int16_t                         (&NPCIsVeggie)[301];                       // 0xB25F0C
            uint8_t                         _reserved_after_NPCIsVeggie[8];            // 0xB25F10
            uint8_t                         _reserved_before_NPCNoFireBall[16];        // 0xB25F18
            int16_t                         (&NPCNoFireBall)[301];                     // 0xB25F28
            uint8_t                         _reserved_after_NPCNoFireBall[8];          // 0xB25F2C
            uint8_t                         _reserved_before_NPCNoIceBall[16];         // 0xB25F34
            int16_t                         (&NPCNoIceBall)[301];                      // 0xB25F44
            uint8_t                         _reserved_after_NPCNoIceBall[8];           // 0xB25F48
            uint8_t                         _reserved_before_NPCNoGravity[16];         // 0xB25F50
            int16_t                         (&NPCNoGravity)[301];                      // 0xB25F60
            uint8_t                         _reserved_after_NPCNoGravity[8];           // 0xB25F64
            uint8_t                         _reserved_before_NPCFrame[16];             // 0xB25F6C
            int16_t                         (&NPCFrame)[301];                          // 0xB25F7C
            uint8_t                         _reserved_after_NPCFrame[8];               // 0xB25F80
            uint8_t                         _reserved_before_NPCFrameSpeed[16];        // 0xB25F88
            int16_t                         (&NPCFrameSpeed)[301];                     // 0xB25F98
            uint8_t                         _reserved_after_NPCFrameSpeed[8];          // 0xB25F9C
            uint8_t                         _reserved_before_NPCFrameStyle[16];        // 0xB25FA4
            int16_t                         (&NPCFrameStyle)[301];                     // 0xB25FB4
            uint8_t                         _reserved_after_NPCFrameStyle[8];          // 0xB25FB8
            SMBX13::Types::NPCDefaults_t    NPCDefaults;                               // 0xB25FC0
            uint8_t                         _reserved_before_BlockIsSizable[16];       // 0xB2B920
            int16_t                         (&BlockIsSizable)[701];                    // 0xB2B930
            uint8_t                         _reserved_after_BlockIsSizable[8];         // 0xB2B934
            uint8_t                         _reserved_before_BlockSlope[16];           // 0xB2B93C
            int16_t                         (&BlockSlope)[701];                        // 0xB2B94C
            uint8_t                         _reserved_after_BlockSlope[8];             // 0xB2B950
            uint8_t                         _reserved_before_BlockSlope2[16];          // 0xB2B958
            int16_t                         (&BlockSlope2)[701];                       // 0xB2B968
            uint8_t                         _reserved_after_BlockSlope2[8];            // 0xB2B96C
            uint8_t                         _reserved_before_vScreenX[16];             // 0xB2B974
            double                          (&vScreenX)[201];                          // 0xB2B984
            uint8_t                         _reserved_after_vScreenX[8];               // 0xB2B988
            uint8_t                         _reserved_before_vScreenY[16];             // 0xB2B990
            double                          (&vScreenY)[201];                          // 0xB2B9A0
            uint8_t                         _reserved_after_vScreenY[8];               // 0xB2B9A4
            uint8_t                         _reserved_before_qScreenX[16];             // 0xB2B9AC
            double                          (&qScreenX)[200];                          // 0xB2B9BC
            uint8_t                         _reserved_after_qScreenX[8];               // 0xB2B9C0
            uint8_t                         _reserved_before_qScreenY[16];             // 0xB2B9C8
            double                          (&qScreenY)[200];                          // 0xB2B9D8
            uint8_t                         _reserved_after_qScreenY[8];               // 0xB2B9DC
            int16_t                         qScreen;                                   // 0xB2B9E4
            int16_t                         _padding14_;                               // 0xB2B9E6
            uint8_t                         _reserved_before_BlockWidth[16];           // 0xB2B9E8
            int16_t                         (&BlockWidth)[701];                        // 0xB2B9F8
            uint8_t                         _reserved_after_BlockWidth[8];             // 0xB2B9FC
            uint8_t                         _reserved_before_BlockHeight[16];          // 0xB2BA04
            int16_t                         (&BlockHeight)[701];                       // 0xB2BA14
            uint8_t                         _reserved_after_BlockHeight[8];            // 0xB2BA18
            uint8_t                         _reserved_before_BonusWidth[16];           // 0xB2BA20
            int16_t                         (&BonusWidth)[100];                        // 0xB2BA30
            uint8_t                         _reserved_after_BonusWidth[8];             // 0xB2BA34
            uint8_t                         _reserved_before_BonusHeight[16];          // 0xB2BA3C
            int16_t                         (&BonusHeight)[100];                       // 0xB2BA4C
            uint8_t                         _reserved_after_BonusHeight[8];            // 0xB2BA50
            uint8_t                         _reserved_before_EffectWidth[16];          // 0xB2BA58
            int16_t                         (&EffectWidth)[200];                       // 0xB2BA68
            uint8_t                         _reserved_after_EffectWidth[8];            // 0xB2BA6C
            uint8_t                         _reserved_before_EffectHeight[16];         // 0xB2BA74
            int16_t                         (&EffectHeight)[200];                      // 0xB2BA84
            uint8_t                         _reserved_after_EffectHeight[8];           // 0xB2BA88
            SMBX13::Types::EffectDefaults_t EffectDefaults;                            // 0xB2BA90
            uint8_t                         _reserved_before_SceneWidth[16];           // 0xB2BDB0
            int16_t                         (&SceneWidth)[100];                        // 0xB2BDC0
            uint8_t                         _reserved_after_SceneWidth[8];             // 0xB2BDC4
            uint8_t                         _reserved_before_SceneHeight[16];          // 0xB2BDCC
            int16_t                         (&SceneHeight)[100];                       // 0xB2BDDC
            uint8_t                         _reserved_after_SceneHeight[8];            // 0xB2BDE0
            uint8_t                         _reserved_before_BackgroundHasNoMask[16];  // 0xB2BDE8
            int16_t                         (&BackgroundHasNoMask)[200];               // 0xB2BDF8
            uint8_t                         _reserved_after_BackgroundHasNoMask[8];    // 0xB2BDFC
            uint8_t                         _reserved_before_Foreground[16];           // 0xB2BE04
            int16_t                         (&Foreground)[201];                        // 0xB2BE14
            uint8_t                         _reserved_after_Foreground[8];             // 0xB2BE18
            uint8_t                         _reserved_before_BackgroundWidth[16];      // 0xB2BE20
            int16_t                         (&BackgroundWidth)[200];                   // 0xB2BE30
            uint8_t                         _reserved_after_BackgroundWidth[8];        // 0xB2BE34
            uint8_t                         _reserved_before_BackgroundHeight[16];     // 0xB2BE3C
            int16_t                         (&BackgroundHeight)[200];                  // 0xB2BE4C
            uint8_t                         _reserved_after_BackgroundHeight[8];       // 0xB2BE50
            uint8_t                         _reserved_before_BackgroundFrame[16];      // 0xB2BE58
            int16_t                         (&BackgroundFrame)[200];                   // 0xB2BE68
            uint8_t                         _reserved_after_BackgroundFrame[8];        // 0xB2BE6C
            uint8_t                         _reserved_before_BackgroundFrameCount[16]; // 0xB2BE74
            int16_t                         (&BackgroundFrameCount)[200];              // 0xB2BE84
            uint8_t                         _reserved_after_BackgroundFrameCount[8];   // 0xB2BE88
            uint8_t                         _reserved_before_BlockFrame[16];           // 0xB2BE90
            int16_t                         (&BlockFrame)[700];                        // 0xB2BEA0
            uint8_t                         _reserved_after_BlockFrame[8];             // 0xB2BEA4
            uint8_t                         _reserved_before_BlockFrame2[16];          // 0xB2BEAC
            int16_t                         (&BlockFrame2)[700];                       // 0xB2BEBC
            uint8_t                         _reserved_after_BlockFrame2[8];            // 0xB2BEC0
            uint8_t                         _reserved_before_sBlockArray[16];          // 0xB2BEC8
            int16_t                         (&sBlockArray)[1000];                      // 0xB2BED8
            uint8_t                         _reserved_after_sBlockArray[8];            // 0xB2BEDC
            int16_t                         sBlockNum;                                 // 0xB2BEE4
            int16_t                         _padding15_;                               // 0xB2BEE6
            uint8_t                         _reserved_before_SceneFrame[16];           // 0xB2BEE8
            int16_t                         (&SceneFrame)[100];                        // 0xB2BEF8
            uint8_t                         _reserved_after_SceneFrame[8];             // 0xB2BEFC
            uint8_t                         _reserved_before_SceneFrame2[16];          // 0xB2BF04
            int16_t                         (&SceneFrame2)[100];                       // 0xB2BF14
            uint8_t                         _reserved_after_SceneFrame2[8];            // 0xB2BF18
            uint8_t                         _reserved_before_SpecialFrame[16];         // 0xB2BF20
            int16_t                         (&SpecialFrame)[101];                      // 0xB2BF30
            uint8_t                         _reserved_after_SpecialFrame[8];           // 0xB2BF34
            uint8_t                         _reserved_before_SpecialFrameCount[16];    // 0xB2BF3C
            float                           (&SpecialFrameCount)[101];                 // 0xB2BF4C
            uint8_t                         _reserved_after_SpecialFrameCount[8];      // 0xB2BF50
            uint8_t                         _reserved_before_TileWidth[16];            // 0xB2BF58
            int16_t                         (&TileWidth)[400];                         // 0xB2BF68
            uint8_t                         _reserved_after_TileWidth[8];              // 0xB2BF6C
            uint8_t                         _reserved_before_TileHeight[16];           // 0xB2BF74
            int16_t                         (&TileHeight)[400];                        // 0xB2BF84
            uint8_t                         _reserved_after_TileHeight[8];             // 0xB2BF88
            uint8_t                         _reserved_before_TileFrame[16];            // 0xB2BF90
            int16_t                         (&TileFrame)[400];                         // 0xB2BFA0
            uint8_t                         _reserved_after_TileFrame[8];              // 0xB2BFA4
            uint8_t                         _reserved_before_TileFrame2[16];           // 0xB2BFAC
            int16_t                         (&TileFrame2)[400];                        // 0xB2BFBC
            uint8_t                         _reserved_after_TileFrame2[8];             // 0xB2BFC0
            uint8_t                         _reserved_before_LevelFrame[16];           // 0xB2BFC8
            int16_t                         (&LevelFrame)[100];                        // 0xB2BFD8
            uint8_t                         _reserved_after_LevelFrame[8];             // 0xB2BFDC
            uint8_t                         _reserved_before_LevelFrame2[16];          // 0xB2BFE4
            int16_t                         (&LevelFrame2)[100];                       // 0xB2BFF4
            uint8_t                         _reserved_after_LevelFrame2[8];            // 0xB2BFF8
            uint8_t                         _reserved_before_BlockHasNoMask[16];       // 0xB2C000
            int16_t                         (&BlockHasNoMask)[700];                    // 0xB2C010
            uint8_t                         _reserved_after_BlockHasNoMask[8];         // 0xB2C014
            uint8_t                         _reserved_before_LevelHasNoMask[16];       // 0xB2C01C
            int16_t                         (&LevelHasNoMask)[100];                    // 0xB2C02C
            uint8_t                         _reserved_after_LevelHasNoMask[8];         // 0xB2C030
            uint8_t                         _reserved_before_BlockOnlyHitspot1[16];    // 0xB2C038
            int16_t                         (&BlockOnlyHitspot1)[701];                 // 0xB2C048
            uint8_t                         _reserved_after_BlockOnlyHitspot1[8];      // 0xB2C04C
            uint8_t                         _reserved_before_BlockKills[16];           // 0xB2C054
            int16_t                         (&BlockKills)[701];                        // 0xB2C064
            uint8_t                         _reserved_after_BlockKills[8];             // 0xB2C068
            uint8_t                         _reserved_before_BlockKills2[16];          // 0xB2C070
            int16_t                         (&BlockKills2)[701];                       // 0xB2C080
            uint8_t                         _reserved_after_BlockKills2[8];            // 0xB2C084
            uint8_t                         _reserved_before_BlockHurts[16];           // 0xB2C08C
            int16_t                         (&BlockHurts)[701];                        // 0xB2C09C
            uint8_t                         _reserved_after_BlockHurts[8];             // 0xB2C0A0
            uint8_t                         _reserved_before_BlockPSwitch[16];         // 0xB2C0A8
            int16_t                         (&BlockPSwitch)[701];                      // 0xB2C0B8
            uint8_t                         _reserved_after_BlockPSwitch[8];           // 0xB2C0BC
            uint8_t                         _reserved_before_BlockNoClipping[16];      // 0xB2C0C4
            int16_t                         (&BlockNoClipping)[701];                   // 0xB2C0D4
            uint8_t                         _reserved_after_BlockNoClipping[8];        // 0xB2C0D8
            uint8_t                         _reserved_before_CoinFrame[16];            // 0xB2C0E0
            int16_t                         (&CoinFrame)[10];                          // 0xB2C0F0
            uint8_t                         _reserved_after_CoinFrame[8];              // 0xB2C0F4
            uint8_t                         _reserved_before_CoinFrame2[16];           // 0xB2C0FC
            int16_t                         (&CoinFrame2)[10];                         // 0xB2C10C
            uint8_t                         _reserved_after_CoinFrame2[8];             // 0xB2C110
            SMBX13::Types::EditorCursor_t   EditorCursor;                              // 0xB2C118
            SMBX13::Types::EditorControls_t EditorControls;                            // 0xB2C558
            uint8_t                         _reserved_before_Sound[16];                // 0xB2C564
            int16_t                         (&Sound)[100];                             // 0xB2C574
            uint8_t                         _reserved_after_Sound[8];                  // 0xB2C578
            uint8_t                         _reserved_before_SoundPause[16];           // 0xB2C580
            int16_t                         (&SoundPause)[100];                        // 0xB2C590
            uint8_t                         _reserved_after_SoundPause[8];             // 0xB2C594
            int16_t                         EndLevel;                                  // 0xB2C59C
            int16_t                         LevelMacro;                                // 0xB2C59E
            int16_t                         LevelMacroCounter;                         // 0xB2C5A0
            int16_t                         numJoysticks;                              // 0xB2C5A2
            VB6StrPtr                       FileName;                                  // 0xB2C5A4
            int16_t                         Coins;                                     // 0xB2C5A8
            int16_t                         _padding16_;                               // 0xB2C5AA
            float                           Lives;                                     // 0xB2C5AC
            int16_t                         EndIntro;                                  // 0xB2C5B0
            int16_t                         ExitMenu;                                  // 0xB2C5B2
            int16_t                         LevelSelect;                               // 0xB2C5B4
            int16_t                         _padding17_;                               // 0xB2C5B6
            uint8_t                         _reserved_before_WorldPlayer[16];          // 0xB2C5B8
            SMBX13::Types::WorldPlayer_t    (&WorldPlayer)[2];                         // 0xB2C5C8
            uint8_t                         _reserved_after_WorldPlayer[8];            // 0xB2C5CC
            int16_t                         LevelBeatCode;                             // 0xB2C5D4
            int16_t                         curWorldLevel;                             // 0xB2C5D6
            int16_t                         curWorldMusic;                             // 0xB2C5D8
            int16_t                         _padding18_;                               // 0xB2C5DA
            uint8_t                         _reserved_before_NoTurnBack[16];           // 0xB2C5DC
            int16_t                         (&NoTurnBack)[21];                         // 0xB2C5EC
            uint8_t                         _reserved_after_NoTurnBack[8];             // 0xB2C5F0
            uint8_t                         _reserved_before_UnderWater[16];           // 0xB2C5F8
            int16_t                         (&UnderWater)[21];                         // 0xB2C608
            uint8_t                         _reserved_after_UnderWater[8];             // 0xB2C60C
            int16_t                         TestLevel;                                 // 0xB2C614
            int16_t                         _padding19_;                               // 0xB2C616
            VB6StrPtr                       FullFileName;                              // 0xB2C618
            VB6StrPtr                       FileNamePath;                              // 0xB2C61C
            int16_t                         GameMenu;                                  // 0xB2C620
            int16_t                         _padding20_;                               // 0xB2C622
            VB6StrPtr                       WorldName;                                 // 0xB2C624
            int16_t                         selWorld;                                  // 0xB2C628
            int16_t                         selSave;                                   // 0xB2C62A
            int16_t                         PSwitchTime;                               // 0xB2C62C
            int16_t                         PSwitchStop;                               // 0xB2C62E
            int16_t                         PSwitchPlayer;                             // 0xB2C630
            int16_t                         _padding21_;                               // 0xB2C632
            uint8_t                         _reserved_before_SaveSlot[16];             // 0xB2C634
            int16_t                         (&SaveSlot)[3];                            // 0xB2C644
            uint8_t                         _reserved_after_SaveSlot[8];               // 0xB2C648
            uint8_t                         _reserved_before_SaveStars[16];            // 0xB2C650
            int16_t                         (&SaveStars)[3];                           // 0xB2C660
            uint8_t                         _reserved_after_SaveStars[8];              // 0xB2C664
            int16_t                         BeltDirection;                             // 0xB2C66C
            int16_t                         BeatTheGame;                               // 0xB2C66E
            int16_t                         cycleCount;                                // 0xB2C670
            int16_t                         _padding22_;                               // 0xB2C672
            double                          fpsTime;                                   // 0xB2C674
            double                          fpsCount;                                  // 0xB2C67C
            int16_t                         FrameSkip;                                 // 0xB2C684
            int16_t                         _padding23_;                               // 0xB2C686
            double                          GoalTime;                                  // 0xB2C688
            double                          overTime;                                  // 0xB2C690
            int16_t                         worldCurs;                                 // 0xB2C698
            int16_t                         minShow;                                   // 0xB2C69A
            int16_t                         maxShow;                                   // 0xB2C69C
            int16_t                         _padding24_;                               // 0xB2C69E
            uint8_t                         _reserved_before_Layer[16];                // 0xB2C6A0
            SMBX13::Types::Layer_t          (&Layer)[101];                             // 0xB2C6B0
            uint8_t                         _reserved_after_Layer[8];                  // 0xB2C6B4
            uint8_t                         _reserved_before_Events[16];               // 0xB2C6BC
            SMBX13::Types::Events_t         (&Events)[101];                            // 0xB2C6CC
            uint8_t                         _reserved_after_Events[8];                 // 0xB2C6D0
            int16_t                         ReturnWarp;                                // 0xB2C6D8
            int16_t                         StartWarp;                                 // 0xB2C6DA
            SMBX13::Types::Physics_t        Physics;                                   // 0xB2C6DC
            int16_t                         MenuCursor;                                // 0xB2C880
            int16_t                         MenuMode;                                  // 0xB2C882
            int16_t                         MenuCursorCanMove;                         // 0xB2C884
            int16_t                         MenuCursorCanMove2;                        // 0xB2C886
            int16_t                         NextFrame;                                 // 0xB2C888
            int16_t                         StopHit;                                   // 0xB2C88A
            int16_t                         MouseRelease;                              // 0xB2C88C
            int16_t                         TestFullscreen;                            // 0xB2C88E
            int16_t                         keyDownAlt;                                // 0xB2C890
            int16_t                         keyDownEnter;                              // 0xB2C892
            int16_t                         BlocksSorted;                              // 0xB2C894
            int16_t                         SingleCoop;                                // 0xB2C896
            VB6StrPtr                       CheatString;                               // 0xB2C898
            int16_t                         GameOutro;                                 // 0xB2C89C
            int16_t                         _padding25_;                               // 0xB2C89E
            float                           CreditChop;                                // 0xB2C8A0
            int16_t                         EndCredits;                                // 0xB2C8A4
            int16_t                         curStars;                                  // 0xB2C8A6
            int16_t                         maxStars;                                  // 0xB2C8A8
            int16_t                         ShadowMode;                                // 0xB2C8AA
            int16_t                         MultiHop;                                  // 0xB2C8AC
            int16_t                         SuperSpeed;                                // 0xB2C8AE
            int16_t                         WalkAnywhere;                              // 0xB2C8B0
            int16_t                         FlyForever;                                // 0xB2C8B2
            int16_t                         FreezeNPCs;                                // 0xB2C8B4
            int16_t                         CaptainN;                                  // 0xB2C8B6
            int16_t                         FlameThrower;                              // 0xB2C8B8
            int16_t                         CoinMode;                                  // 0xB2C8BA
            int16_t                         WorldUnlock;                               // 0xB2C8BC
            int16_t                         MaxFPS;                                    // 0xB2C8BE
            int16_t                         GodMode;                                   // 0xB2C8C0
            int16_t                         GrabAll;                                   // 0xB2C8C2
            int16_t                         Cheater;                                   // 0xB2C8C4
            int16_t                         _padding26_;                               // 0xB2C8C6
            uint8_t                         _reserved_before_WorldCredits[16];         // 0xB2C8C8
            VB6StrPtr                       (&WorldCredits)[5];                        // 0xB2C8D8
            uint8_t                         _reserved_after_WorldCredits[8];           // 0xB2C8DC
            int32_t                         Score;                                     // 0xB2C8E4
            uint8_t                         _reserved_before_Points[16];               // 0xB2C8E8
            int16_t                         (&Points)[13];                             // 0xB2C8F8
            uint8_t                         _reserved_after_Points[8];                 // 0xB2C8FC
            int16_t                         oldJumpJoy;                                // 0xB2C904
            int16_t                         MaxWorldStars;                             // 0xB2C906
            int16_t                         Debugger;                                  // 0xB2C908
            int16_t                         _padding27_;                               // 0xB2C90A
            uint8_t                         _reserved_before_SavedChar[16];            // 0xB2C90C
            SMBX13::Types::Player_t         (&SavedChar)[11];                          // 0xB2C91C
            uint8_t                         _reserved_after_SavedChar[8];              // 0xB2C920
            int16_t                         LoadCoins;                                 // 0xB2C928
            int16_t                         _padding28_;                               // 0xB2C92A
            float                           LoadCoinsT;                                // 0xB2C92C
            uint8_t                         _reserved_before_GFXBlockCustom[16];       // 0xB2C930
            int16_t                         (&GFXBlockCustom)[700];                    // 0xB2C940
            uint8_t                         _reserved_after_GFXBlockCustom[8];         // 0xB2C944
            uint8_t                         _reserved_before_GFXBlock[16];             // 0xB2C94C
            int32_t                         (&GFXBlock)[700];                          // 0xB2C95C
            uint8_t                         _reserved_after_GFXBlock[8];               // 0xB2C960
            uint8_t                         _reserved_before_GFXBlockMask[16];         // 0xB2C968
            int32_t                         (&GFXBlockMask)[700];                      // 0xB2C978
            uint8_t                         _reserved_after_GFXBlockMask[8];           // 0xB2C97C
            uint8_t                         _reserved_before_GFXBlockBMP[28];          // 0xB2C984
            SMBX13::Types::StdPicture_t     (&GFXBlockBMP)[700];                       // 0xB2C9A0
            uint8_t                         _reserved_after_GFXBlockBMP[8];            // 0xB2C9A4
            uint8_t                         _reserved_before_GFXBlockMaskBMP[28];      // 0xB2C9AC
            SMBX13::Types::StdPicture_t     (&GFXBlockMaskBMP)[700];                   // 0xB2C9C8
            uint8_t                         _reserved_after_GFXBlockMaskBMP[8];        // 0xB2C9CC
            uint8_t                         _reserved_before_GFXBackground2Custom[16]; // 0xB2C9D4
            int16_t                         (&GFXBackground2Custom)[100];              // 0xB2C9E4
            uint8_t                         _reserved_after_GFXBackground2Custom[8];   // 0xB2C9E8
            uint8_t                         _reserved_before_GFXBackground2[16];       // 0xB2C9F0
            int32_t                         (&GFXBackground2)[100];                    // 0xB2CA00
            uint8_t                         _reserved_after_GFXBackground2[8];         // 0xB2CA04
            uint8_t                         _reserved_before_GFXBackground2BMP[28];    // 0xB2CA0C
            SMBX13::Types::StdPicture_t     (&GFXBackground2BMP)[100];                 // 0xB2CA28
            uint8_t                         _reserved_after_GFXBackground2BMP[8];      // 0xB2CA2C
            uint8_t                         _reserved_before_GFXBackground2Height[16]; // 0xB2CA34
            int16_t                         (&GFXBackground2Height)[100];              // 0xB2CA44
            uint8_t                         _reserved_after_GFXBackground2Height[8];   // 0xB2CA48
            uint8_t                         _reserved_before_GFXBackground2Width[16];  // 0xB2CA50
            int16_t                         (&GFXBackground2Width)[100];               // 0xB2CA60
            uint8_t                         _reserved_after_GFXBackground2Width[8];    // 0xB2CA64
            uint8_t                         _reserved_before_GFXNPCCustom[16];         // 0xB2CA6C
            int16_t                         (&GFXNPCCustom)[300];                      // 0xB2CA7C
            uint8_t                         _reserved_after_GFXNPCCustom[8];           // 0xB2CA80
            uint8_t                         _reserved_before_GFXNPC[16];               // 0xB2CA88
            int32_t                         (&GFXNPC)[300];                            // 0xB2CA98
            uint8_t                         _reserved_after_GFXNPC[8];                 // 0xB2CA9C
            uint8_t                         _reserved_before_GFXNPCMask[16];           // 0xB2CAA4
            int32_t                         (&GFXNPCMask)[300];                        // 0xB2CAB4
            uint8_t                         _reserved_after_GFXNPCMask[8];             // 0xB2CAB8
            uint8_t                         _reserved_before_GFXNPCBMP[28];            // 0xB2CAC0
            SMBX13::Types::StdPicture_t     (&GFXNPCBMP)[300];                         // 0xB2CADC
            uint8_t                         _reserved_after_GFXNPCBMP[8];              // 0xB2CAE0
            uint8_t                         _reserved_before_GFXNPCMaskBMP[28];        // 0xB2CAE8
            SMBX13::Types::StdPicture_t     (&GFXNPCMaskBMP)[300];                     // 0xB2CB04
            uint8_t                         _reserved_after_GFXNPCMaskBMP[8];          // 0xB2CB08
            uint8_t                         _reserved_before_GFXNPCHeight[16];         // 0xB2CB10
            int16_t                         (&GFXNPCHeight)[300];                      // 0xB2CB20
            uint8_t                         _reserved_after_GFXNPCHeight[8];           // 0xB2CB24
            uint8_t                         _reserved_before_GFXNPCWidth[16];          // 0xB2CB2C
            int16_t                         (&GFXNPCWidth)[300];                       // 0xB2CB3C
            uint8_t                         _reserved_after_GFXNPCWidth[8];            // 0xB2CB40
            uint8_t                         _reserved_before_GFXEffectCustom[16];      // 0xB2CB48
            int16_t                         (&GFXEffectCustom)[200];                   // 0xB2CB58
            uint8_t                         _reserved_after_GFXEffectCustom[8];        // 0xB2CB5C
            uint8_t                         _reserved_before_GFXEffect[16];            // 0xB2CB64
            int32_t                         (&GFXEffect)[200];                         // 0xB2CB74
            uint8_t                         _reserved_after_GFXEffect[8];              // 0xB2CB78
            uint8_t                         _reserved_before_GFXEffectMask[16];        // 0xB2CB80
            int32_t                         (&GFXEffectMask)[200];                     // 0xB2CB90
            uint8_t                         _reserved_after_GFXEffectMask[8];          // 0xB2CB94
            uint8_t                         _reserved_before_GFXEffectBMP[28];         // 0xB2CB9C
            SMBX13::Types::StdPicture_t     (&GFXEffectBMP)[200];                      // 0xB2CBB8
            uint8_t                         _reserved_after_GFXEffectBMP[8];           // 0xB2CBBC
            uint8_t                         _reserved_before_GFXEffectMaskBMP[28];     // 0xB2CBC4
            SMBX13::Types::StdPicture_t     (&GFXEffectMaskBMP)[200];                  // 0xB2CBE0
            uint8_t                         _reserved_after_GFXEffectMaskBMP[8];       // 0xB2CBE4
            uint8_t                         _reserved_before_GFXEffectHeight[16];      // 0xB2CBEC
            int16_t                         (&GFXEffectHeight)[200];                   // 0xB2CBFC
            uint8_t                         _reserved_after_GFXEffectHeight[8];        // 0xB2CC00
            uint8_t                         _reserved_before_GFXEffectWidth[16];       // 0xB2CC08
            int16_t                         (&GFXEffectWidth)[200];                    // 0xB2CC18
            uint8_t                         _reserved_after_GFXEffectWidth[8];         // 0xB2CC1C
            uint8_t                         _reserved_before_GFXBackgroundCustom[16];  // 0xB2CC24
            int16_t                         (&GFXBackgroundCustom)[200];               // 0xB2CC34
            uint8_t                         _reserved_after_GFXBackgroundCustom[8];    // 0xB2CC38
            uint8_t                         _reserved_before_GFXBackground[16];        // 0xB2CC40
            int32_t                         (&GFXBackground)[200];                     // 0xB2CC50
            uint8_t                         _reserved_after_GFXBackground[8];          // 0xB2CC54
            uint8_t                         _reserved_before_GFXBackgroundMask[16];    // 0xB2CC5C
            int32_t                         (&GFXBackgroundMask)[200];                 // 0xB2CC6C
            uint8_t                         _reserved_after_GFXBackgroundMask[8];      // 0xB2CC70
            uint8_t                         _reserved_before_GFXBackgroundBMP[28];     // 0xB2CC78
            SMBX13::Types::StdPicture_t     (&GFXBackgroundBMP)[200];                  // 0xB2CC94
            uint8_t                         _reserved_after_GFXBackgroundBMP[8];       // 0xB2CC98
            uint8_t                         _reserved_before_GFXBackgroundMaskBMP[28]; // 0xB2CCA0
            SMBX13::Types::StdPicture_t     (&GFXBackgroundMaskBMP)[200];              // 0xB2CCBC
            uint8_t                         _reserved_after_GFXBackgroundMaskBMP[8];   // 0xB2CCC0
            uint8_t                         _reserved_before_GFXBackgroundHeight[16];  // 0xB2CCC8
            int16_t                         (&GFXBackgroundHeight)[200];               // 0xB2CCD8
            uint8_t                         _reserved_after_GFXBackgroundHeight[8];    // 0xB2CCDC
            uint8_t                         _reserved_before_GFXBackgroundWidth[16];   // 0xB2CCE4
            int16_t                         (&GFXBackgroundWidth)[200];                // 0xB2CCF4
            uint8_t                         _reserved_after_GFXBackgroundWidth[8];     // 0xB2CCF8
            uint8_t                         _reserved_before_GFXMarioCustom[16];       // 0xB2CD00
            int16_t                         (&GFXMarioCustom)[10];                     // 0xB2CD10
            uint8_t                         _reserved_after_GFXMarioCustom[8];         // 0xB2CD14
            uint8_t                         _reserved_before_GFXMario[16];             // 0xB2CD1C
            int32_t                         (&GFXMario)[10];                           // 0xB2CD2C
            uint8_t                         _reserved_after_GFXMario[8];               // 0xB2CD30
            uint8_t                         _reserved_before_GFXMarioMask[16];         // 0xB2CD38
            int32_t                         (&GFXMarioMask)[10];                       // 0xB2CD48
            uint8_t                         _reserved_after_GFXMarioMask[8];           // 0xB2CD4C
            uint8_t                         _reserved_before_GFXMarioBMP[28];          // 0xB2CD54
            SMBX13::Types::StdPicture_t     (&GFXMarioBMP)[10];                        // 0xB2CD70
            uint8_t                         _reserved_after_GFXMarioBMP[8];            // 0xB2CD74
            uint8_t                         _reserved_before_GFXMarioMaskBMP[28];      // 0xB2CD7C
            SMBX13::Types::StdPicture_t     (&GFXMarioMaskBMP)[10];                    // 0xB2CD98
            uint8_t                         _reserved_after_GFXMarioMaskBMP[8];        // 0xB2CD9C
            uint8_t                         _reserved_before_GFXMarioHeight[16];       // 0xB2CDA4
            int16_t                         (&GFXMarioHeight)[10];                     // 0xB2CDB4
            uint8_t                         _reserved_after_GFXMarioHeight[8];         // 0xB2CDB8
            uint8_t                         _reserved_before_GFXMarioWidth[16];        // 0xB2CDC0
            int16_t                         (&GFXMarioWidth)[10];                      // 0xB2CDD0
            uint8_t                         _reserved_after_GFXMarioWidth[8];          // 0xB2CDD4
            uint8_t                         _reserved_before_GFXLuigiCustom[16];       // 0xB2CDDC
            int16_t                         (&GFXLuigiCustom)[10];                     // 0xB2CDEC
            uint8_t                         _reserved_after_GFXLuigiCustom[8];         // 0xB2CDF0
            uint8_t                         _reserved_before_GFXLuigi[16];             // 0xB2CDF8
            int32_t                         (&GFXLuigi)[10];                           // 0xB2CE08
            uint8_t                         _reserved_after_GFXLuigi[8];               // 0xB2CE0C
            uint8_t                         _reserved_before_GFXLuigiMask[16];         // 0xB2CE14
            int32_t                         (&GFXLuigiMask)[10];                       // 0xB2CE24
            uint8_t                         _reserved_after_GFXLuigiMask[8];           // 0xB2CE28
            uint8_t                         _reserved_before_GFXLuigiBMP[28];          // 0xB2CE30
            SMBX13::Types::StdPicture_t     (&GFXLuigiBMP)[10];                        // 0xB2CE4C
            uint8_t                         _reserved_after_GFXLuigiBMP[8];            // 0xB2CE50
            uint8_t                         _reserved_before_GFXLuigiMaskBMP[28];      // 0xB2CE58
            SMBX13::Types::StdPicture_t     (&GFXLuigiMaskBMP)[10];                    // 0xB2CE74
            uint8_t                         _reserved_after_GFXLuigiMaskBMP[8];        // 0xB2CE78
            uint8_t                         _reserved_before_GFXLuigiHeight[16];       // 0xB2CE80
            int16_t                         (&GFXLuigiHeight)[10];                     // 0xB2CE90
            uint8_t                         _reserved_after_GFXLuigiHeight[8];         // 0xB2CE94
            uint8_t                         _reserved_before_GFXLuigiWidth[16];        // 0xB2CE9C
            int16_t                         (&GFXLuigiWidth)[10];                      // 0xB2CEAC
            uint8_t                         _reserved_after_GFXLuigiWidth[8];          // 0xB2CEB0
            uint8_t                         _reserved_before_GFXPeachCustom[16];       // 0xB2CEB8
            int16_t                         (&GFXPeachCustom)[10];                     // 0xB2CEC8
            uint8_t                         _reserved_after_GFXPeachCustom[8];         // 0xB2CECC
            uint8_t                         _reserved_before_GFXPeach[16];             // 0xB2CED4
            int32_t                         (&GFXPeach)[10];                           // 0xB2CEE4
            uint8_t                         _reserved_after_GFXPeach[8];               // 0xB2CEE8
            uint8_t                         _reserved_before_GFXPeachMask[16];         // 0xB2CEF0
            int32_t                         (&GFXPeachMask)[10];                       // 0xB2CF00
            uint8_t                         _reserved_after_GFXPeachMask[8];           // 0xB2CF04
            uint8_t                         _reserved_before_GFXPeachBMP[28];          // 0xB2CF0C
            SMBX13::Types::StdPicture_t     (&GFXPeachBMP)[10];                        // 0xB2CF28
            uint8_t                         _reserved_after_GFXPeachBMP[8];            // 0xB2CF2C
            uint8_t                         _reserved_before_GFXPeachMaskBMP[28];      // 0xB2CF34
            SMBX13::Types::StdPicture_t     (&GFXPeachMaskBMP)[10];                    // 0xB2CF50
            uint8_t                         _reserved_after_GFXPeachMaskBMP[8];        // 0xB2CF54
            uint8_t                         _reserved_before_GFXPeachHeight[16];       // 0xB2CF5C
            int16_t                         (&GFXPeachHeight)[10];                     // 0xB2CF6C
            uint8_t                         _reserved_after_GFXPeachHeight[8];         // 0xB2CF70
            uint8_t                         _reserved_before_GFXPeachWidth[16];        // 0xB2CF78
            int16_t                         (&GFXPeachWidth)[10];                      // 0xB2CF88
            uint8_t                         _reserved_after_GFXPeachWidth[8];          // 0xB2CF8C
            uint8_t                         _reserved_before_GFXToadCustom[16];        // 0xB2CF94
            int16_t                         (&GFXToadCustom)[10];                      // 0xB2CFA4
            uint8_t                         _reserved_after_GFXToadCustom[8];          // 0xB2CFA8
            uint8_t                         _reserved_before_GFXToad[16];              // 0xB2CFB0
            int32_t                         (&GFXToad)[10];                            // 0xB2CFC0
            uint8_t                         _reserved_after_GFXToad[8];                // 0xB2CFC4
            uint8_t                         _reserved_before_GFXToadMask[16];          // 0xB2CFCC
            int32_t                         (&GFXToadMask)[10];                        // 0xB2CFDC
            uint8_t                         _reserved_after_GFXToadMask[8];            // 0xB2CFE0
            uint8_t                         _reserved_before_GFXToadBMP[28];           // 0xB2CFE8
            SMBX13::Types::StdPicture_t     (&GFXToadBMP)[10];                         // 0xB2D004
            uint8_t                         _reserved_after_GFXToadBMP[8];             // 0xB2D008
            uint8_t                         _reserved_before_GFXToadMaskBMP[28];       // 0xB2D010
            SMBX13::Types::StdPicture_t     (&GFXToadMaskBMP)[10];                     // 0xB2D02C
            uint8_t                         _reserved_after_GFXToadMaskBMP[8];         // 0xB2D030
            uint8_t                         _reserved_before_GFXToadHeight[16];        // 0xB2D038
            int16_t                         (&GFXToadHeight)[10];                      // 0xB2D048
            uint8_t                         _reserved_after_GFXToadHeight[8];          // 0xB2D04C
            uint8_t                         _reserved_before_GFXToadWidth[16];         // 0xB2D054
            int16_t                         (&GFXToadWidth)[10];                       // 0xB2D064
            uint8_t                         _reserved_after_GFXToadWidth[8];           // 0xB2D068
            uint8_t                         _reserved_before_GFXLinkCustom[16];        // 0xB2D070
            int16_t                         (&GFXLinkCustom)[10];                      // 0xB2D080
            uint8_t                         _reserved_after_GFXLinkCustom[8];          // 0xB2D084
            uint8_t                         _reserved_before_GFXLink[16];              // 0xB2D08C
            int32_t                         (&GFXLink)[10];                            // 0xB2D09C
            uint8_t                         _reserved_after_GFXLink[8];                // 0xB2D0A0
            uint8_t                         _reserved_before_GFXLinkMask[16];          // 0xB2D0A8
            int32_t                         (&GFXLinkMask)[10];                        // 0xB2D0B8
            uint8_t                         _reserved_after_GFXLinkMask[8];            // 0xB2D0BC
            uint8_t                         _reserved_before_GFXLinkBMP[28];           // 0xB2D0C4
            SMBX13::Types::StdPicture_t     (&GFXLinkBMP)[10];                         // 0xB2D0E0
            uint8_t                         _reserved_after_GFXLinkBMP[8];             // 0xB2D0E4
            uint8_t                         _reserved_before_GFXLinkMaskBMP[28];       // 0xB2D0EC
            SMBX13::Types::StdPicture_t     (&GFXLinkMaskBMP)[10];                     // 0xB2D108
            uint8_t                         _reserved_after_GFXLinkMaskBMP[8];         // 0xB2D10C
            uint8_t                         _reserved_before_GFXLinkHeight[16];        // 0xB2D114
            int16_t                         (&GFXLinkHeight)[10];                      // 0xB2D124
            uint8_t                         _reserved_after_GFXLinkHeight[8];          // 0xB2D128
            uint8_t                         _reserved_before_GFXLinkWidth[16];         // 0xB2D130
            int16_t                         (&GFXLinkWidth)[10];                       // 0xB2D140
            uint8_t                         _reserved_after_GFXLinkWidth[8];           // 0xB2D144
            uint8_t                         _reserved_before_GFXYoshiBCustom[16];      // 0xB2D14C
            int16_t                         (&GFXYoshiBCustom)[10];                    // 0xB2D15C
            uint8_t                         _reserved_after_GFXYoshiBCustom[8];        // 0xB2D160
            uint8_t                         _reserved_before_GFXYoshiB[16];            // 0xB2D168
            int32_t                         (&GFXYoshiB)[10];                          // 0xB2D178
            uint8_t                         _reserved_after_GFXYoshiB[8];              // 0xB2D17C
            uint8_t                         _reserved_before_GFXYoshiBMask[16];        // 0xB2D184
            int32_t                         (&GFXYoshiBMask)[10];                      // 0xB2D194
            uint8_t                         _reserved_after_GFXYoshiBMask[8];          // 0xB2D198
            uint8_t                         _reserved_before_GFXYoshiBBMP[28];         // 0xB2D1A0
            SMBX13::Types::StdPicture_t     (&GFXYoshiBBMP)[10];                       // 0xB2D1BC
            uint8_t                         _reserved_after_GFXYoshiBBMP[8];           // 0xB2D1C0
            uint8_t                         _reserved_before_GFXYoshiBMaskBMP[28];     // 0xB2D1C8
            SMBX13::Types::StdPicture_t     (&GFXYoshiBMaskBMP)[10];                   // 0xB2D1E4
            uint8_t                         _reserved_after_GFXYoshiBMaskBMP[8];       // 0xB2D1E8
            uint8_t                         _reserved_before_GFXYoshiTCustom[16];      // 0xB2D1F0
            int16_t                         (&GFXYoshiTCustom)[10];                    // 0xB2D200
            uint8_t                         _reserved_after_GFXYoshiTCustom[8];        // 0xB2D204
            uint8_t                         _reserved_before_GFXYoshiT[16];            // 0xB2D20C
            int32_t                         (&GFXYoshiT)[10];                          // 0xB2D21C
            uint8_t                         _reserved_after_GFXYoshiT[8];              // 0xB2D220
            uint8_t                         _reserved_before_GFXYoshiTMask[16];        // 0xB2D228
            int32_t                         (&GFXYoshiTMask)[10];                      // 0xB2D238
            uint8_t                         _reserved_after_GFXYoshiTMask[8];          // 0xB2D23C
            uint8_t                         _reserved_before_GFXYoshiTBMP[28];         // 0xB2D244
            SMBX13::Types::StdPicture_t     (&GFXYoshiTBMP)[10];                       // 0xB2D260
            uint8_t                         _reserved_after_GFXYoshiTBMP[8];           // 0xB2D264
            uint8_t                         _reserved_before_GFXYoshiTMaskBMP[28];     // 0xB2D26C
            SMBX13::Types::StdPicture_t     (&GFXYoshiTMaskBMP)[10];                   // 0xB2D288
            uint8_t                         _reserved_after_GFXYoshiTMaskBMP[8];       // 0xB2D28C
            uint8_t                         _reserved_before_GFXTileCustom[16];        // 0xB2D294
            int32_t                         (&GFXTileCustom)[400];                     // 0xB2D2A4
            uint8_t                         _reserved_after_GFXTileCustom[8];          // 0xB2D2A8
            uint8_t                         _reserved_before_GFXTile[16];              // 0xB2D2B0
            int32_t                         (&GFXTile)[400];                           // 0xB2D2C0
            uint8_t                         _reserved_after_GFXTile[8];                // 0xB2D2C4
            uint8_t                         _reserved_before_GFXTileBMP[28];           // 0xB2D2CC
            SMBX13::Types::StdPicture_t     (&GFXTileBMP)[400];                        // 0xB2D2E8
            uint8_t                         _reserved_after_GFXTileBMP[8];             // 0xB2D2EC
            uint8_t                         _reserved_before_GFXTileHeight[16];        // 0xB2D2F4
            int16_t                         (&GFXTileHeight)[400];                     // 0xB2D304
            uint8_t                         _reserved_after_GFXTileHeight[8];          // 0xB2D308
            uint8_t                         _reserved_before_GFXTileWidth[16];         // 0xB2D310
            int16_t                         (&GFXTileWidth)[400];                      // 0xB2D320
            uint8_t                         _reserved_after_GFXTileWidth[8];           // 0xB2D324
            uint8_t                         _reserved_before_GFXLevelCustom[16];       // 0xB2D32C
            int32_t                         (&GFXLevelCustom)[101];                    // 0xB2D33C
            uint8_t                         _reserved_after_GFXLevelCustom[8];         // 0xB2D340
            uint8_t                         _reserved_before_GFXLevel[16];             // 0xB2D348
            int32_t                         (&GFXLevel)[101];                          // 0xB2D358
            uint8_t                         _reserved_after_GFXLevel[8];               // 0xB2D35C
            uint8_t                         _reserved_before_GFXLevelMask[16];         // 0xB2D364
            int32_t                         (&GFXLevelMask)[101];                      // 0xB2D374
            uint8_t                         _reserved_after_GFXLevelMask[8];           // 0xB2D378
            uint8_t                         _reserved_before_GFXLevelBMP[28];          // 0xB2D380
            SMBX13::Types::StdPicture_t     (&GFXLevelBMP)[101];                       // 0xB2D39C
            uint8_t                         _reserved_after_GFXLevelBMP[8];            // 0xB2D3A0
            uint8_t                         _reserved_before_GFXLevelMaskBMP[28];      // 0xB2D3A8
            SMBX13::Types::StdPicture_t     (&GFXLevelMaskBMP)[101];                   // 0xB2D3C4
            uint8_t                         _reserved_after_GFXLevelMaskBMP[8];        // 0xB2D3C8
            uint8_t                         _reserved_before_GFXLevelHeight[16];       // 0xB2D3D0
            int16_t                         (&GFXLevelHeight)[101];                    // 0xB2D3E0
            uint8_t                         _reserved_after_GFXLevelHeight[8];         // 0xB2D3E4
            uint8_t                         _reserved_before_GFXLevelWidth[16];        // 0xB2D3EC
            int16_t                         (&GFXLevelWidth)[101];                     // 0xB2D3FC
            uint8_t                         _reserved_after_GFXLevelWidth[8];          // 0xB2D400
            uint8_t                         _reserved_before_GFXLevelBig[16];          // 0xB2D408
            int16_t                         (&GFXLevelBig)[101];                       // 0xB2D418
            uint8_t                         _reserved_after_GFXLevelBig[8];            // 0xB2D41C
            uint8_t                         _reserved_before_GFXSceneCustom[16];       // 0xB2D424
            int32_t                         (&GFXSceneCustom)[100];                    // 0xB2D434
            uint8_t                         _reserved_after_GFXSceneCustom[8];         // 0xB2D438
            uint8_t                         _reserved_before_GFXScene[16];             // 0xB2D440
            int32_t                         (&GFXScene)[100];                          // 0xB2D450
            uint8_t                         _reserved_after_GFXScene[8];               // 0xB2D454
            uint8_t                         _reserved_before_GFXSceneMask[16];         // 0xB2D45C
            int32_t                         (&GFXSceneMask)[100];                      // 0xB2D46C
            uint8_t                         _reserved_after_GFXSceneMask[8];           // 0xB2D470
            uint8_t                         _reserved_before_GFXSceneBMP[28];          // 0xB2D478
            SMBX13::Types::StdPicture_t     (&GFXSceneBMP)[100];                       // 0xB2D494
            uint8_t                         _reserved_after_GFXSceneBMP[8];            // 0xB2D498
            uint8_t                         _reserved_before_GFXSceneMaskBMP[28];      // 0xB2D4A0
            SMBX13::Types::StdPicture_t     (&GFXSceneMaskBMP)[100];                   // 0xB2D4BC
            uint8_t                         _reserved_after_GFXSceneMaskBMP[8];        // 0xB2D4C0
            uint8_t                         _reserved_before_GFXSceneHeight[16];       // 0xB2D4C8
            int16_t                         (&GFXSceneHeight)[100];                    // 0xB2D4D8
            uint8_t                         _reserved_after_GFXSceneHeight[8];         // 0xB2D4DC
            uint8_t                         _reserved_before_GFXSceneWidth[16];        // 0xB2D4E4
            int16_t                         (&GFXSceneWidth)[100];                     // 0xB2D4F4
            uint8_t                         _reserved_after_GFXSceneWidth[8];          // 0xB2D4F8
            uint8_t                         _reserved_before_GFXPathCustom[16];        // 0xB2D500
            int32_t                         (&GFXPathCustom)[100];                     // 0xB2D510
            uint8_t                         _reserved_after_GFXPathCustom[8];          // 0xB2D514
            uint8_t                         _reserved_before_GFXPath[16];              // 0xB2D51C
            int32_t                         (&GFXPath)[100];                           // 0xB2D52C
            uint8_t                         _reserved_after_GFXPath[8];                // 0xB2D530
            uint8_t                         _reserved_before_GFXPathMask[16];          // 0xB2D538
            int32_t                         (&GFXPathMask)[100];                       // 0xB2D548
            uint8_t                         _reserved_after_GFXPathMask[8];            // 0xB2D54C
            uint8_t                         _reserved_before_GFXPathBMP[28];           // 0xB2D554
            SMBX13::Types::StdPicture_t     (&GFXPathBMP)[100];                        // 0xB2D570
            uint8_t                         _reserved_after_GFXPathBMP[8];             // 0xB2D574
            uint8_t                         _reserved_before_GFXPathMaskBMP[28];       // 0xB2D57C
            SMBX13::Types::StdPicture_t     (&GFXPathMaskBMP)[100];                    // 0xB2D598
            uint8_t                         _reserved_after_GFXPathMaskBMP[8];         // 0xB2D59C
            uint8_t                         _reserved_before_GFXPathHeight[16];        // 0xB2D5A4
            int16_t                         (&GFXPathHeight)[100];                     // 0xB2D5B4
            uint8_t                         _reserved_after_GFXPathHeight[8];          // 0xB2D5B8
            uint8_t                         _reserved_before_GFXPathWidth[16];         // 0xB2D5C0
            int16_t                         (&GFXPathWidth)[100];                      // 0xB2D5D0
            uint8_t                         _reserved_after_GFXPathWidth[8];           // 0xB2D5D4
            uint8_t                         _reserved_before_GFXPlayerCustom[16];      // 0xB2D5DC
            int32_t                         (&GFXPlayerCustom)[5];                     // 0xB2D5EC
            uint8_t                         _reserved_after_GFXPlayerCustom[8];        // 0xB2D5F0
            uint8_t                         _reserved_before_GFXPlayer[16];            // 0xB2D5F8
            int32_t                         (&GFXPlayer)[5];                           // 0xB2D608
            uint8_t                         _reserved_after_GFXPlayer[8];              // 0xB2D60C
            uint8_t                         _reserved_before_GFXPlayerMask[16];        // 0xB2D614
            int32_t                         (&GFXPlayerMask)[5];                       // 0xB2D624
            uint8_t                         _reserved_after_GFXPlayerMask[8];          // 0xB2D628
            uint8_t                         _reserved_before_GFXPlayerBMP[28];         // 0xB2D630
            SMBX13::Types::StdPicture_t     (&GFXPlayerBMP)[5];                        // 0xB2D64C
            uint8_t                         _reserved_after_GFXPlayerBMP[8];           // 0xB2D650
            uint8_t                         _reserved_before_GFXPlayerMaskBMP[28];     // 0xB2D658
            SMBX13::Types::StdPicture_t     (&GFXPlayerMaskBMP)[5];                    // 0xB2D674
            uint8_t                         _reserved_after_GFXPlayerMaskBMP[8];       // 0xB2D678
            uint8_t                         _reserved_before_GFXPlayerHeight[16];      // 0xB2D680
            int16_t                         (&GFXPlayerHeight)[5];                     // 0xB2D690
            uint8_t                         _reserved_after_GFXPlayerHeight[8];        // 0xB2D694
            uint8_t                         _reserved_before_GFXPlayerWidth[16];       // 0xB2D69C
            int16_t                         (&GFXPlayerWidth)[5];                      // 0xB2D6AC
            uint8_t                         _reserved_after_GFXPlayerWidth[8];         // 0xB2D6B0
            int16_t                         PlayerCharacter;                           // 0xB2D6B8
            int16_t                         PlayerCharacter2;                          // 0xB2D6BA
            double                          MenuMouseX;                                // 0xB2D6BC
            double                          MenuMouseY;                                // 0xB2D6C4
            int16_t                         MenuMouseDown;                             // 0xB2D6CC
            int16_t                         MenuMouseBack;                             // 0xB2D6CE
            int16_t                         MenuMouseRelease;                          // 0xB2D6D0
            int16_t                         MenuMouseMove;                             // 0xB2D6D2
            int16_t                         MenuMouseClick;                            // 0xB2D6D4
            int16_t                         _padding29_;                               // 0xB2D6D6
            uint8_t                         _reserved_before_NewEvent[16];             // 0xB2D6D8
            VB6StrPtr                       (&NewEvent)[100];                          // 0xB2D6E8
            uint8_t                         _reserved_after_NewEvent[8];               // 0xB2D6EC
            uint8_t                         _reserved_before_newEventDelay[16];        // 0xB2D6F4
            int16_t                         (&newEventDelay)[100];                     // 0xB2D704
            uint8_t                         _reserved_after_newEventDelay[8];          // 0xB2D708
            int16_t                         newEventNum;                               // 0xB2D710
            int16_t                         ForcedControls;                            // 0xB2D712
            SMBX13::Types::Controls_t       ForcedControl;                             // 0xB2D714
            int16_t                         SyncCount;                                 // 0xB2D728
            int16_t                         noUpdate;                                  // 0xB2D72A
            double                          gameTime;                                  // 0xB2D72C
            int16_t                         noSound;                                   // 0xB2D734
            int16_t                         _padding30_;                               // 0xB2D736
            double                          tempTime;                                  // 0xB2D738
            int16_t                         BattleMode;                                // 0xB2D740
            int16_t                         BattleWinner;                              // 0xB2D742
            uint8_t                         _reserved_before_BattleLives[16];          // 0xB2D744
            int16_t                         (&BattleLives)[200];                       // 0xB2D754
            uint8_t                         _reserved_after_BattleLives[8];            // 0xB2D758
            int16_t                         BattleIntro;                               // 0xB2D760
            int16_t                         BattleOutro;                               // 0xB2D762
            VB6StrPtr                       LevelName;                                 // 0xB2D764
        };
        #if !defined(__INTELLISENSE__)
            static_assert(sizeof(SMBX13::Types::modMain_t) == 0x8740, "sizeof(SMBX13::Types::modMain_t) must be 0x8740");
        #endif
        
    }

    // Pop packing settings
    __pragma(pack(pop))

    static SMBX13::Types::modMain_t& modMain = *reinterpret_cast<SMBX13::Types::modMain_t*>(0xB25028);
}

#endif // !defined(SMBXINTERNAL_FUNCTIONS_H)

