#if !defined(SMBXINTERNAL_FUNCTIONS_H)
#define SMBXINTERNAL_FUNCTIONS_H

#include <cstdint>
#include "../Misc/VB6StrPtr.h"

namespace SMBX13 {
    namespace Types {
        struct Location_t;
    }

    namespace frmMain { namespace Private {
        static const auto Form_DblClick = reinterpret_cast<void(__stdcall *)()>(0x008BD770);
        static const auto Form_KeyDown = reinterpret_cast<void(__stdcall *)(int16_t& KeyCode, int16_t& Shift)>(0x008BD900);
        static const auto Form_KeyPress = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x008BD9E0);
        static const auto Form_KeyUp = reinterpret_cast<void(__stdcall *)(int16_t& KeyCode, int16_t& Shift)>(0x008BDAB0);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x008BDB40);
        static const auto Form_MouseDown = reinterpret_cast<void(__stdcall *)(int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x008BDE80);
        static const auto Form_MouseMove = reinterpret_cast<void(__stdcall *)(int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x008BDF10);
        static const auto Form_MouseUp = reinterpret_cast<void(__stdcall *)(int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x008BE080);
        static const auto Form_QueryUnload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel, int16_t& UnloadMode)>(0x008BE100);
        static const auto Form_Resize = reinterpret_cast<void(__stdcall *)()>(0x008BE1A0);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x008BE360);
    }}

    namespace modMain {
        static const auto SetupPhysics = reinterpret_cast<void(__stdcall *)()>(0x008BE410);
        static const auto Main = reinterpret_cast<void(__stdcall *)()>(0x008BE9C0);
        static const auto SetupVars = reinterpret_cast<void(__stdcall *)()>(0x008C2720);
        static const auto GameLoop = reinterpret_cast<void(__stdcall *)()>(0x008CA210);
        static const auto MenuLoop = reinterpret_cast<void(__stdcall *)()>(0x008CA650);
        static const auto EditorLoop = reinterpret_cast<void(__stdcall *)()>(0x008D6B80);
        static const auto KillIt = reinterpret_cast<void(__stdcall *)()>(0x008D6BB0);
        static const auto SaveLevel = reinterpret_cast<void(__stdcall *)(VB6StrPtr& FilePath)>(0x008D6CF0);
        static const auto OpenLevel = reinterpret_cast<void(__stdcall *)(VB6StrPtr& FilePath)>(0x008D8F40);
        static const auto ClearLevel = reinterpret_cast<void(__stdcall *)()>(0x008DC6E0);
        static const auto NextLevel = reinterpret_cast<void(__stdcall *)()>(0x008DDE30);
        static const auto UpdateMacro = reinterpret_cast<void(__stdcall *)()>(0x008DE080);
        static const auto SaveWorld = reinterpret_cast<void(__stdcall *)(VB6StrPtr& FilePath)>(0x008DEB60);
        static const auto OpenWorld = reinterpret_cast<void(__stdcall *)(VB6StrPtr& FilePath)>(0x008DF5B0);
        static const auto WorldLoop = reinterpret_cast<void(__stdcall *)()>(0x008E06B0);
        static const auto LevelPath = reinterpret_cast<void(__stdcall *)(int16_t& Lvl, int16_t& Direction, int16_t& Skp)>(0x008E1DF0);
        static const auto PathPath = reinterpret_cast<void(__stdcall *)(int16_t& Pth, int16_t& Skp)>(0x008E2350);
        static const auto PathWait = reinterpret_cast<void(__stdcall *)()>(0x008E2A40);
        static const auto ClearWorld = reinterpret_cast<void(__stdcall *)()>(0x008E2E40);
        static const auto FindWorlds = reinterpret_cast<void(__stdcall *)()>(0x008E35E0);
        static const auto FindLevels = reinterpret_cast<void(__stdcall *)()>(0x008E3D10);
        static const auto FindSaves = reinterpret_cast<void(__stdcall *)()>(0x008E41D0);
        static const auto SaveGame = reinterpret_cast<void(__stdcall *)()>(0x008E47D0);
        static const auto LoadGame = reinterpret_cast<void(__stdcall *)()>(0x008E4E00);
        static const auto PauseGame = reinterpret_cast<void(__stdcall *)(int16_t& plr)>(0x008E54C0);
        static const auto InitControls = reinterpret_cast<void(__stdcall *)()>(0x008E6700);
        static const auto OpenConfig = reinterpret_cast<void(__stdcall *)()>(0x008E6910);
        static const auto SaveConfig = reinterpret_cast<void(__stdcall *)()>(0x008E6CC0);
    }

    namespace modMain { namespace Private {
        static const auto CheckActive = reinterpret_cast<void(__stdcall *)()>(0x008E6F70);
    }}

    namespace modMain {
        static const auto CheatCode = reinterpret_cast<void(__stdcall *)(VB6StrPtr& NewKey)>(0x008E7490);
        static const auto OutroLoop = reinterpret_cast<void(__stdcall *)()>(0x008F6D20);
        static const auto SetupCredits = reinterpret_cast<void(__stdcall *)()>(0x008F7200);
        static const auto FindStars = reinterpret_cast<void(__stdcall *)()>(0x008F7D70);
        static const auto AddCredit = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newCredit)>(0x008F8000);
        static const auto newLoc = reinterpret_cast<SMBX13::Types::Location_t(__stdcall *)(double& X, double& Y, double& Width, double& Height)>(0x008F8050);
        static const auto MoreScore = reinterpret_cast<void(__stdcall *)(int16_t& addScore, SMBX13::Types::Location_t& Loc, int16_t& Multiplier)>(0x008F80C0);
        static const auto SetupPlayerFrames = reinterpret_cast<void(__stdcall *)()>(0x008F8260);
    }

    namespace modMain { namespace Private {
        static const auto SizableBlocks = reinterpret_cast<void(__stdcall *)()>(0x008FE2A0);
    }}

    namespace modMain {
        static const auto StartBattleMode = reinterpret_cast<void(__stdcall *)()>(0x008FE440);
        static const auto FixComma = reinterpret_cast<VB6StrPtr(__stdcall *)(VB6StrPtr& newStr)>(0x008FE930);
    }

    namespace modGraphics {
        static const auto UpdateGraphics2 = reinterpret_cast<void(__stdcall *)()>(0x008FEB10);
        static const auto UpdateGraphics = reinterpret_cast<void(__stdcall *)()>(0x00909290);
        static const auto GetvScreen = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x0094DF20);
        static const auto GetvScreenAverage = reinterpret_cast<void(__stdcall *)()>(0x0094EEE0);
        static const auto GetvScreenAverage2 = reinterpret_cast<void(__stdcall *)()>(0x0094F440);
        static const auto SetupGraphics = reinterpret_cast<void(__stdcall *)()>(0x0094F680);
        static const auto SetupEditorGraphics = reinterpret_cast<void(__stdcall *)()>(0x0094F8D0);
        static const auto SetupScreens = reinterpret_cast<void(__stdcall *)()>(0x009502E0);
        static const auto DynamicScreen = reinterpret_cast<void(__stdcall *)()>(0x00950770);
        static const auto SuperPrint = reinterpret_cast<void(__stdcall *)(VB6StrPtr& SuperWords, int16_t& Font, float& X, float& Y)>(0x00951F50);
        static const auto SetRes = reinterpret_cast<void(__stdcall *)()>(0x00954240);
        static const auto CheckKey = reinterpret_cast<VB6StrPtr(__stdcall *)(VB6StrPtr& newStrizzle)>(0x009542C0);
    }

    namespace modGraphics { namespace Private {
        static const auto SpecialFrames = reinterpret_cast<void(__stdcall *)()>(0x00954AC0);
    }}

    namespace modGraphics {
        static const auto DrawBackground = reinterpret_cast<void(__stdcall *)(int16_t& S, int16_t& Z)>(0x00954F50);
        static const auto PlayerWarpGFX = reinterpret_cast<void(__stdcall *)(int16_t& A, SMBX13::Types::Location_t& tempLocation, float& X2, float& Y2)>(0x00969C00);
        static const auto NPCWarpGFX = reinterpret_cast<void(__stdcall *)(int16_t& A, SMBX13::Types::Location_t& tempLocation, float& X2, float& Y2)>(0x0096A2F0);
        static const auto ChangeScreen = reinterpret_cast<void(__stdcall *)()>(0x0096AD80);
        static const auto GetvScreenCredits = reinterpret_cast<void(__stdcall *)()>(0x0096B640);
        static const auto DoCredits = reinterpret_cast<void(__stdcall *)()>(0x0096BAF0);
        static const auto DrawInterface = reinterpret_cast<void(__stdcall *)(int16_t& Z, void* numScreens)>(0x0096BF20);
        static const auto pfrX = reinterpret_cast<int16_t(__stdcall *)(int16_t& plrFrame)>(0x00987CE0);
        static const auto pfrY = reinterpret_cast<int16_t(__stdcall *)(int16_t& plrFrame)>(0x00987D90);
        static const auto GameThing = reinterpret_cast<void(__stdcall *)()>(0x00987DE0);
        static const auto DrawPlayer = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& Z)>(0x00988E10);
        static const auto ScreenShot = reinterpret_cast<void(__stdcall *)()>(0x009918C0);
        static const auto DrawFrozenNPC = reinterpret_cast<void(__stdcall *)(int16_t& Z, int16_t& A)>(0x009920D0);
    }

    namespace modCollision {
        static const auto CheckCollision = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00992F50);
        static const auto n00bCollision = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00992FE0);
        static const auto WarpCollision = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, int16_t& A)>(0x009931D0);
        static const auto FindCollision = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00993330);
        static const auto FindCollisionBelt = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, float& BeltSpeed)>(0x00993470);
        static const auto NPCFindCollision = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x009935C0);
        static const auto EasyModeCollision = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, int16_t& StandOn)>(0x00993700);
        static const auto BootCollision = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, int16_t& StandOn)>(0x00993970);
        static const auto CursorCollision = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00993BE0);
        static const auto ShakeCollision = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, int16_t& ShakeY3)>(0x00993CD0);
        static const auto vScreenCollision = reinterpret_cast<int16_t(__stdcall *)(int16_t& A, SMBX13::Types::Location_t& Loc2)>(0x00993DE0);
        static const auto vScreenCollision2 = reinterpret_cast<int16_t(__stdcall *)(int16_t& A, SMBX13::Types::Location_t& Loc2)>(0x00993F90);
        static const auto WalkingCollision = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00994130);
        static const auto WalkingCollision3 = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, float& BeltSpeed)>(0x009941B0);
        static const auto FindRunningCollision = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00994250);
        static const auto CanComeOut = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00994390);
        static const auto CheckHitSpot1 = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00994480);
    }

    namespace modPlayer {
        static const auto SetupPlayers = reinterpret_cast<void(__stdcall *)()>(0x009944F0);
        static const auto UpdatePlayer = reinterpret_cast<void(__stdcall *)()>(0x00995A20);
        static const auto PlayerHurt = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B51E0);
        static const auto PlayerDead = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B66D0);
        static const auto KillPlayer = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B6EC0);
        static const auto CheckDead = reinterpret_cast<int16_t(__stdcall *)()>(0x009B7590);
        static const auto CheckLiving = reinterpret_cast<int16_t(__stdcall *)()>(0x009B7680);
        static const auto LivingPlayers = reinterpret_cast<int16_t(__stdcall *)()>(0x009B7710);
        static const auto EveryonesDead = reinterpret_cast<void(__stdcall *)()>(0x009B7780);
        static const auto UnDuck = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B7BF0);
        static const auto CheckSection = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B7E10);
        static const auto PlayerFrame = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B8620);
        static const auto UpdatePlayerBonus = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& B)>(0x009BB2B0);
        static const auto TailSwipe = reinterpret_cast<void(__stdcall *)(int16_t& plr, int16_t& bool_, int16_t& Stab, int16_t& StabDir)>(0x009BB490);
        static const auto YoshiHeight = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009BD560);
        static const auto YoshiEat = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009BD610);
        static const auto YoshiSpit = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009BE6D0);
        static const auto YoshiPound = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& C, int16_t& BreakBlocks)>(0x009C0040);
        static const auto SwapCoop = reinterpret_cast<void(__stdcall *)()>(0x009C06A0);
        static const auto PlayerPush = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& HitSpot)>(0x009C0800);
        static const auto SizeCheck = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C0C50);
        static const auto YoshiEatCode = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C1D90);
        static const auto StealBonus = reinterpret_cast<void(__stdcall *)()>(0x009C46C0);
        static const auto ClownCar = reinterpret_cast<void(__stdcall *)()>(0x009C4A70);
    }

    namespace modPlayer { namespace Private {
        static const auto WaterCheck = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C5AC0);
        static const auto Tanooki = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C6430);
        static const auto PowerUps = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C6600);
        static const auto SuperWarp = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C9740);
        static const auto PlayerCollide = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009CAEC0);
    }}

    namespace modPlayer {
        static const auto PlayerGrabCode = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& DontResetGrabTime)>(0x009CC2B0);
        static const auto LinkFrame = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009D1480);
    }

    namespace modPlayer { namespace Private {
        static const auto PlayerEffects = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009D1E80);
    }}

    namespace modBlocks {
        static const auto BlockHit = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& HitDown, int16_t& whatPlayer)>(0x009DA620);
        static const auto BlockShakeUp = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009E0940);
        static const auto BlockShakeUpPow = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009E0A00);
        static const auto BlockShakeDown = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009E0AC0);
        static const auto BlockHitHard = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009E0B80);
        static const auto KillBlock = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& Splode)>(0x009E0D50);
        static const auto BlockFrames = reinterpret_cast<void(__stdcall *)()>(0x009E14B0);
        static const auto UpdateBlocks = reinterpret_cast<void(__stdcall *)()>(0x009E2090);
        static const auto PSwitch = reinterpret_cast<void(__stdcall *)(int16_t& bool_)>(0x009E33B0);
        static const auto PowBlock = reinterpret_cast<void(__stdcall *)()>(0x009E4600);
    }

    namespace modEffect {
        static const auto UpdateEffects = reinterpret_cast<void(__stdcall *)()>(0x009E4900);
        static const auto NewEffect = reinterpret_cast<void(__stdcall *)(int16_t& A, SMBX13::Types::Location_t& Location, float& Direction, int16_t& NewNpc, int16_t& Shadow)>(0x009E7380);
        static const auto KillEffect = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009EC150);
    }

    namespace modEditor {
        static const auto UpdateEditor = reinterpret_cast<void(__stdcall *)()>(0x009EC200);
        static const auto GetEditorControls = reinterpret_cast<void(__stdcall *)()>(0x009FCD10);
        static const auto SetCursor = reinterpret_cast<void(__stdcall *)()>(0x009FCDA0);
        static const auto PositionCursor = reinterpret_cast<void(__stdcall *)()>(0x00A00C50);
        static const auto HideCursor = reinterpret_cast<void(__stdcall *)()>(0x00A02090);
        static const auto KillWarp = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A02120);
        static const auto zTestLevel = reinterpret_cast<void(__stdcall *)()>(0x00A02220);
        static const auto EditorNPCFrame = reinterpret_cast<int16_t(__stdcall *)(int16_t& A, float& C, int16_t& N)>(0x00A03630);
        static const auto MouseMove = reinterpret_cast<void(__stdcall *)(float& X, float& Y, int16_t& nCur)>(0x00A041D0);
        static const auto ResetNPC = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A05C40);
        static const auto BlockFill = reinterpret_cast<void(__stdcall *)(SMBX13::Types::Location_t& Loc)>(0x00A05D20);
    }

    namespace modNPC {
        static const auto UpdateNPCs = reinterpret_cast<void(__stdcall *)()>(0x00A06090);
        static const auto DropBonus = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A24310);
        static const auto TouchBonus = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& B)>(0x00A24CD0);
        static const auto NPCHit = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& B, int16_t& C)>(0x00A281B0);
        static const auto KillNPC = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& B)>(0x00A315A0);
        static const auto CheckSectionNPC = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A3B680);
        static const auto Deactivate = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A3B880);
        static const auto Bomb = reinterpret_cast<void(__stdcall *)(SMBX13::Types::Location_t& Location, int16_t& Game, int16_t& ImmunePlayer)>(0x00A3BA90);
        static const auto TurnNPCsIntoCoins = reinterpret_cast<void(__stdcall *)()>(0x00A3C580);
        static const auto NPCFrames = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A3C990);
        static const auto SkullRide = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A446A0);
        static const auto NPCSpecial = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A448D0);
        static const auto SpecialNPC = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A52BB0);
        static const auto CharStuff = reinterpret_cast<void(__stdcall *)(int16_t& WhatNPC, int16_t& CheckEggs)>(0x00A60AF0);
        static const auto RandomBonus = reinterpret_cast<void(__stdcall *)()>(0x00A61A00);
    }

    namespace modSound {
        static const auto StartMusic = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A61B40);
        static const auto StopMusic = reinterpret_cast<void(__stdcall *)()>(0x00A621A0);
        static const auto InitSound = reinterpret_cast<void(__stdcall *)()>(0x00A624D0);
        static const auto PlaySound = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A73FD0);
        static const auto BlockSound = reinterpret_cast<void(__stdcall *)()>(0x00A74420);
        static const auto UpdateSound = reinterpret_cast<void(__stdcall *)()>(0x00A74460);
    }

    namespace modJoystick {
        static const auto UpdateControls = reinterpret_cast<void(__stdcall *)()>(0x00A74910);
        static const auto StartJoystick = reinterpret_cast<int16_t(__stdcall *)(int16_t JoystickNumber)>(0x00A75680);
        static const auto PollJoystick = reinterpret_cast<void(__stdcall *)()>(0x00A75820);
    }

    namespace frmOpen { namespace Private {
        static const auto cmdCancel_Click = reinterpret_cast<void(__stdcall *)()>(0x00A75970);
        static const auto cmdOpen_Click = reinterpret_cast<void(__stdcall *)()>(0x00A75A40);
        static const auto Dir1_Change = reinterpret_cast<void(__stdcall *)()>(0x00A75AB0);
        static const auto Drive1_Change = reinterpret_cast<void(__stdcall *)()>(0x00A75BE0);
        static const auto File1_DblClick = reinterpret_cast<void(__stdcall *)()>(0x00A75D10);
        static const auto File1_KeyPress = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x00A75D80);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00A75E00);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A76100);
        static const auto OpenIt = reinterpret_cast<void(__stdcall *)()>(0x00A761B0);
    }}

    namespace frmSave { namespace Private {
        static const auto cmdCancel_Click = reinterpret_cast<void(__stdcall *)()>(0x00A76A80);
        static const auto cmdSave_Click = reinterpret_cast<void(__stdcall *)()>(0x00A76B50);
        static const auto Dir1_Change = reinterpret_cast<void(__stdcall *)()>(0x00A76BC0);
        static const auto Drive1_Change = reinterpret_cast<void(__stdcall *)()>(0x00A76CF0);
        static const auto File1_Click = reinterpret_cast<void(__stdcall *)()>(0x00A76E90);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00A77060);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A773D0);
        static const auto txtFileName_KeyDown = reinterpret_cast<void(__stdcall *)(int16_t& KeyCode, int16_t& Shift)>(0x00A77480);
        static const auto SaveIt = reinterpret_cast<void(__stdcall *)()>(0x00A77500);
    }}

    namespace frmLevelEditor { namespace Private {
        static const auto chkAlign_Click = reinterpret_cast<void(__stdcall *)()>(0x00A77F30);
        static const auto chkAlign2_Click = reinterpret_cast<void(__stdcall *)()>(0x00A78050);
        static const auto MDIForm_Load = reinterpret_cast<void(__stdcall *)()>(0x00A78170);
        static const auto MDIForm_QueryUnload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel, int16_t& UnloadMode)>(0x00A7C7B0);
        static const auto MDIForm_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A7C820);
        static const auto menHelp_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7C890);
        static const auto menuGameplay_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7CB40);
        static const auto menuLevelDebugger_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7CDF0);
        static const auto menuEvents_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7CEE0);
        static const auto menuFileExit_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7D0D0);
        static const auto menuFileNew_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7D140);
        static const auto menuFileOpen_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7D570);
        static const auto menuFileSave_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7D680);
        static const auto menuLayers_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7D790);
        static const auto menuNetplay_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7D980);
        static const auto menureset_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7DBE0);
        static const auto menuStatus_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7DC70);
        static const auto menuTestLevel_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7DCE0);
        static const auto menuTestLevel128_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7DE00);
        static const auto menuTestLevel16_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7DE80);
        static const auto menuTestLevel2_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7DF00);
        static const auto menuTestLevel32_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7E020);
        static const auto menuTestLevel4_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7E0A0);
        static const auto menuTestLevel64_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7E120);
        static const auto menuTestLevel8_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7E1A0);
        static const auto MenuTestLevelBattle_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7E220);
        static const auto menuTestLevelTwo_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7E2B0);
        static const auto mnuChat_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7E330);
        static const auto mnuLevelEditor_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7E420);
        static const auto mnuTestSetting_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7E8C0);
        static const auto mnuWorldEditor_Click = reinterpret_cast<void(__stdcall *)()>(0x00A7E9B0);
        static const auto optCursor_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A7EF10);
    }}

    namespace frmLevelWindow { namespace Private {
        static const auto Form_KeyPress = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x00A814C0);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00A81530);
        static const auto Form_LostFocus = reinterpret_cast<void(__stdcall *)()>(0x00A81E20);
        static const auto Form_Resize = reinterpret_cast<void(__stdcall *)()>(0x00A81E90);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A82050);
        static const auto vScreen_KeyDown = reinterpret_cast<void(__stdcall *)(int16_t& Index, int16_t& KeyCode, int16_t& Shift)>(0x00A820C0);
        static const auto vScreen_KeyPress = reinterpret_cast<void(__stdcall *)(int16_t& Index, int16_t& KeyAscii)>(0x00A82140);
        static const auto vScreen_LostFocus = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A82210);
        static const auto vScreen_MouseDown = reinterpret_cast<void(__stdcall *)(int16_t& Index, int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x00A82280);
        static const auto vScreen_MouseMove = reinterpret_cast<void(__stdcall *)(int16_t& Index, int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x00A824E0);
        static const auto vScreen_MouseUp = reinterpret_cast<void(__stdcall *)(int16_t& Index, int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x00A829E0);
    }}

    namespace frmLevelSettings { namespace Private {
        static const auto cmdAdvanced_Click = reinterpret_cast<void(__stdcall *)()>(0x00A82A60);
        static const auto cmdExit_Click = reinterpret_cast<void(__stdcall *)()>(0x00A82EC0);
        static const auto cmdNoTurnBack_Click = reinterpret_cast<void(__stdcall *)()>(0x00A83300);
        static const auto cmdWater_Click = reinterpret_cast<void(__stdcall *)()>(0x00A83740);
        static const auto cmdWrap_Click = reinterpret_cast<void(__stdcall *)()>(0x00A83B80);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00A83FC0);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A84280);
        static const auto optBackground_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A843A0);
        static const auto optBackgroundColor_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A84840);
        static const auto optGame_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A84A00);
        static const auto optLevel_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A84DC0);
        static const auto optMusic_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A850C0);
        static const auto optSection_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A85590);
        static const auto txtMusic_Change = reinterpret_cast<void(__stdcall *)()>(0x00A86D30);
    }}

    namespace frmBlocks { namespace Private {
        static const auto Block_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A86F50);
        static const auto BlockH_Change = reinterpret_cast<void(__stdcall *)()>(0x00A87790);
        static const auto BlockR_Click = reinterpret_cast<void(__stdcall *)()>(0x00A87900);
        static const auto BlockW_Change = reinterpret_cast<void(__stdcall *)()>(0x00A87B40);
        static const auto cmdAdvanced_Click = reinterpret_cast<void(__stdcall *)()>(0x00A87CB0);
        static const auto cmdInvis_Click = reinterpret_cast<void(__stdcall *)()>(0x00A87EA0);
        static const auto cmdSlip_Click = reinterpret_cast<void(__stdcall *)()>(0x00A87FF0);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00A88140);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A89250);
        static const auto optBlockS_Change = reinterpret_cast<void(__stdcall *)()>(0x00A89370);
        static const auto optBlockS_Scroll = reinterpret_cast<void(__stdcall *)()>(0x00A89520);
        static const auto optBlockSpecial_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A896D0);
        static const auto optGame_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A89840);
        static const auto optMisc_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A89C00);
        static const auto optSMW_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A89FC0);
        static const auto optSMB3_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A8A380);
        static const auto optSMB2_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A8A740);
        static const auto optSMB1_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A8AB00);
        static const auto qBlockS_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A8AEC0);
        static const auto qCoin_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A8B0A0);
    }}

    namespace frmBlocks {
        static const auto CheckBlock = reinterpret_cast<void(__stdcall *)()>(0x00A8B150);
    }

    namespace frmNPCs { namespace Private {
        static const auto Bubble_Click = reinterpret_cast<void(__stdcall *)()>(0x00A8D850);
        static const auto Buried_Click = reinterpret_cast<void(__stdcall *)()>(0x00A8DD40);
        static const auto chkMessage_Click = reinterpret_cast<void(__stdcall *)()>(0x00A8E300);
        static const auto cmdAdvanced_Click = reinterpret_cast<void(__stdcall *)()>(0x00A8E5B0);
        static const auto cmdEvents_Click = reinterpret_cast<void(__stdcall *)()>(0x00A8E7A0);
        static const auto cmdGenerator_Click = reinterpret_cast<void(__stdcall *)()>(0x00A8E990);
        static const auto DontMove_Click = reinterpret_cast<void(__stdcall *)()>(0x00A8EB80);
        static const auto Egg_Click = reinterpret_cast<void(__stdcall *)()>(0x00A8EE60);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00A8F420);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A8F950);
        static const auto Friendly_Click = reinterpret_cast<void(__stdcall *)()>(0x00A8FA70);
        static const auto Lakitu_Click = reinterpret_cast<void(__stdcall *)()>(0x00A8FD50);
        static const auto NPC_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A90310);
        static const auto NPCText_Change = reinterpret_cast<void(__stdcall *)()>(0x00A93770);
        static const auto optEffect_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A93830);
        static const auto optGame_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A93970);
        static const auto optNPCDirection_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A94050);
    }}

    namespace frmNPCs {
        static const auto ShowLak = reinterpret_cast<void(__stdcall *)()>(0x00A941D0);
    }

    namespace frmBackgrounds { namespace Private {
        static const auto Background_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A943F0);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00A946F0);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A94AF0);
        static const auto optGame_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A94C10);
    }}

    namespace frmWarp { namespace Private {
        static const auto chkEntrance_Click = reinterpret_cast<void(__stdcall *)()>(0x00A94FD0);
        static const auto chkMapWarp_Click = reinterpret_cast<void(__stdcall *)()>(0x00A95140);
        static const auto cmdLocked_Click = reinterpret_cast<void(__stdcall *)()>(0x00A952B0);
        static const auto cmdNPC_Click = reinterpret_cast<void(__stdcall *)()>(0x00A95590);
        static const auto cmdYoshi_Click = reinterpret_cast<void(__stdcall *)()>(0x00A95870);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A95B50);
        static const auto optDirection_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A95C70);
        static const auto optDirection2_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A95DE0);
        static const auto optE_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A95F50);
        static const auto optEffect_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A960C0);
        static const auto scrWarp_Change = reinterpret_cast<void(__stdcall *)()>(0x00A966B0);
    }}

    namespace frmTiles { namespace Private {
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00A96A40);
        static const auto optGame_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A96D60);
        static const auto Tile_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97140);
    }}

    namespace frmScene { namespace Private {
        static const auto Scene_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97440);
    }}

    namespace frmLevels { namespace Private {
        static const auto cmbExit_Change = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A975B0);
        static const auto cmbExit_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97720);
        static const auto scrWarp_Change = reinterpret_cast<void(__stdcall *)()>(0x00A97890);
        static const auto WorldLevel_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97C20);
    }}

    namespace frmPaths { namespace Private {
        static const auto WorldPath_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97D90);
    }}

    namespace frmMusic { namespace Private {
        static const auto optMusic_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97F00);
    }}

    namespace modChangeRes {
        static const auto GetCurrentRes = reinterpret_cast<void(__stdcall *)()>(0x00A98070);
        static const auto SetOrigRes = reinterpret_cast<void(__stdcall *)()>(0x00A98150);
        static const auto ChangeRes = reinterpret_cast<void(__stdcall *)(int16_t& ScreenX, int16_t& ScreenY, int16_t& ScreenColor, int16_t& ScreenFreq)>(0x00A98190);
    }

    namespace modChangeRes { namespace Private {
        static const auto SaveIt = reinterpret_cast<void(__stdcall *)(int16_t& ScX, int16_t& ScY, int16_t& ScC, int16_t& ScF, VB6StrPtr& ScreenChanged)>(0x00A984A0);
    }}

    namespace modSorting {
        static const auto qSortBlocksY = reinterpret_cast<void(__stdcall *)(int16_t& min, int16_t& max)>(0x00A985A0);
        static const auto qSortBlocksX = reinterpret_cast<void(__stdcall *)(int16_t& min, int16_t& max)>(0x00A988E0);
        static const auto qSortBackgrounds = reinterpret_cast<void(__stdcall *)(int16_t& min, int16_t& max)>(0x00A98C20);
        static const auto FindBlocks = reinterpret_cast<void(__stdcall *)()>(0x00A98FE0);
        static const auto BackGroundPri = reinterpret_cast<double(__stdcall *)(int16_t& A)>(0x00A992C0);
        static const auto NPCSort = reinterpret_cast<void(__stdcall *)()>(0x00A99620);
        static const auto FindSBlocks = reinterpret_cast<void(__stdcall *)()>(0x00A99870);
        static const auto qSortSBlocks = reinterpret_cast<void(__stdcall *)(int16_t& min, int16_t& max)>(0x00A99930);
        static const auto qSortNPCsY = reinterpret_cast<void(__stdcall *)(int16_t& min, int16_t& max)>(0x00A99C10);
        static const auto UpdateBackgrounds = reinterpret_cast<void(__stdcall *)()>(0x00A9A000);
    }

    namespace frmWorld { namespace Private {
        static const auto chkChar_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9A1E0);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A9A430);
        static const auto txtStars_Change = reinterpret_cast<void(__stdcall *)()>(0x00A9A4C0);
    }}

    namespace frmTestSettings { namespace Private {
        static const auto chkFullscreen_Click = reinterpret_cast<void(__stdcall *)()>(0x00A9A680);
        static const auto chkGodMode_Click = reinterpret_cast<void(__stdcall *)()>(0x00A9A760);
        static const auto chkGrabAll_Click = reinterpret_cast<void(__stdcall *)()>(0x00A9A840);
        static const auto chkMaxFPS_Click = reinterpret_cast<void(__stdcall *)()>(0x00A9A920);
        static const auto chkShowFPS_Click = reinterpret_cast<void(__stdcall *)()>(0x00A9AA00);
        static const auto cmdClose_Click = reinterpret_cast<void(__stdcall *)()>(0x00A9AAE0);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A9AB70);
        static const auto optBlackYoshi_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9AC00);
        static const auto optBlueBoot_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9ACC0);
        static const auto optBlueYoshi_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9AD80);
        static const auto optBoot_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9AE40);
        static const auto optChrLink_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9AF00);
        static const auto optChrLuigi_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9B850);
        static const auto optChrMario_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9BCF0);
        static const auto optChrPeach_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9C190);
        static const auto optChrToad_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9C980);
        static const auto optFireMario_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D170);
        static const auto optGreenYoshi_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D200);
        static const auto optKoopaClownCar_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D2C0);
        static const auto optHammerMario_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D300);
        static const auto optIceMario_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D390);
        static const auto optIceYoshi_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D420);
        static const auto optMario_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D4E0);
        static const auto optMarioC_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D570);
        static const auto optNoMount_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D590);
        static const auto optPinkYoshi_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D620);
        static const auto optPurpleYoshi_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D6E0);
        static const auto optRacoonMario_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D7A0);
        static const auto optRedBoot_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D830);
        static const auto optRedYoshi_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D8F0);
        static const auto optSuperMario_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D9B0);
        static const auto optTanookiMario_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9DA40);
        static const auto optYellowYoshi_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9DAD0);
        static const auto ShowAll = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9DB90);
    }}

    namespace frmAdvanced { namespace Private {
        static const auto cmbPara_Click = reinterpret_cast<void(__stdcall *)()>(0x00A9E440);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00A9E7D0);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A9E910);
    }}

    namespace frmLayers { namespace Private {
        static const auto cmdAdd_Click = reinterpret_cast<void(__stdcall *)()>(0x00A9E9A0);
        static const auto cmdDelete_Click = reinterpret_cast<void(__stdcall *)()>(0x00A9F260);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00A9FE00);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A9FF10);
        static const auto lstLayer_Click = reinterpret_cast<void(__stdcall *)()>(0x00A9FFA0);
        static const auto lstLayer_DblClick = reinterpret_cast<void(__stdcall *)()>(0x00AA0F20);
        static const auto txtLayer_KeyPress = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x00AA1250);
    }}

    namespace frmLayers {
        static const auto UpdateLayers = reinterpret_cast<void(__stdcall *)()>(0x00AA2160);
    }

    namespace modLayers {
        static const auto ShowLayer = reinterpret_cast<void(__stdcall *)(VB6StrPtr& LayerName, int16_t& NoEffect)>(0x00AA2760);
        static const auto HideLayer = reinterpret_cast<void(__stdcall *)(VB6StrPtr& LayerName, int16_t& NoEffect)>(0x00AA3730);
        static const auto ProcEvent = reinterpret_cast<void(__stdcall *)(VB6StrPtr& EventName, int16_t& NoEffect)>(0x00AA42D0);
        static const auto UpdateEvents = reinterpret_cast<void(__stdcall *)()>(0x00AA60E0);
        static const auto UpdateLayers = reinterpret_cast<void(__stdcall *)()>(0x00AA6710);
    }

    namespace frmEvents { namespace Private {
        static const auto AutoSection_Change = reinterpret_cast<void(__stdcall *)()>(0x00AA7F30);
        static const auto cAltJump_Click = reinterpret_cast<void(__stdcall *)()>(0x00AA8410);
        static const auto cAltRun_Click = reinterpret_cast<void(__stdcall *)()>(0x00AA87A0);
        static const auto cDown_Click = reinterpret_cast<void(__stdcall *)()>(0x00AA8B30);
        static const auto cDrop_Click = reinterpret_cast<void(__stdcall *)()>(0x00AA8EC0);
        static const auto chkSmoke_Click = reinterpret_cast<void(__stdcall *)()>(0x00AA9250);
        static const auto chkStart_Click = reinterpret_cast<void(__stdcall *)()>(0x00AA96A0);
        static const auto cJump_Click = reinterpret_cast<void(__stdcall *)()>(0x00AA9AF0);
        static const auto cLeft_Click = reinterpret_cast<void(__stdcall *)()>(0x00AA9E80);
        static const auto cmbEndGame_Click = reinterpret_cast<void(__stdcall *)()>(0x00AAA210);
        static const auto cmbLayerMove_Click = reinterpret_cast<void(__stdcall *)()>(0x00AAA560);
        static const auto cmbSound_Click = reinterpret_cast<void(__stdcall *)()>(0x00AAA970);
        static const auto cmdAdd_Click = reinterpret_cast<void(__stdcall *)()>(0x00AAADE0);
        static const auto cmdBackground_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AAB770);
        static const auto cmdDelete_Click = reinterpret_cast<void(__stdcall *)()>(0x00AABDD0);
        static const auto cmdDupe_Click = reinterpret_cast<void(__stdcall *)()>(0x00AAD210);
        static const auto cmdHideAdd_Click = reinterpret_cast<void(__stdcall *)()>(0x00AADD30);
        static const auto cmdHideRemove_Click = reinterpret_cast<void(__stdcall *)()>(0x00AAE1F0);
        static const auto cmdMusic_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AAE660);
        static const auto cmdPosition_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AAECC0);
        static const auto cmdShowAdd_Click = reinterpret_cast<void(__stdcall *)()>(0x00AAF5C0);
        static const auto cmdShowRemove_Click = reinterpret_cast<void(__stdcall *)()>(0x00AAFA80);
        static const auto cmdToggleAdd_Click = reinterpret_cast<void(__stdcall *)()>(0x00AAFEF0);
        static const auto cmdToggleRemove_Click = reinterpret_cast<void(__stdcall *)()>(0x00AB03C0);
        static const auto cRight_Click = reinterpret_cast<void(__stdcall *)()>(0x00AB0840);
        static const auto cRun_Click = reinterpret_cast<void(__stdcall *)()>(0x00AB0BD0);
        static const auto cStart_Click = reinterpret_cast<void(__stdcall *)()>(0x00AB0F60);
        static const auto cUp_Click = reinterpret_cast<void(__stdcall *)()>(0x00AB12F0);
        static const auto EventText_Change = reinterpret_cast<void(__stdcall *)()>(0x00AB1680);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00AB1D30);
        static const auto lstEvent_Click = reinterpret_cast<void(__stdcall *)()>(0x00AB1DC0);
        static const auto lstLayers_Click = reinterpret_cast<void(__stdcall *)()>(0x00AB2110);
        static const auto scrDelay_Change = reinterpret_cast<void(__stdcall *)()>(0x00AB2250);
        static const auto scrX_Change = reinterpret_cast<void(__stdcall *)()>(0x00AB28E0);
        static const auto scrY_Change = reinterpret_cast<void(__stdcall *)()>(0x00AB2D00);
        static const auto TriggerEvent_Click = reinterpret_cast<void(__stdcall *)()>(0x00AB3120);
        static const auto txtEvent_KeyPress = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x00AB3490);
    }}

    namespace frmEvents {
        static const auto RefreshEvents = reinterpret_cast<void(__stdcall *)()>(0x00AB4A70);
    }

    namespace frmEvents { namespace Private {
        static const auto txtX_Change = reinterpret_cast<void(__stdcall *)()>(0x00AB92B0);
        static const auto txtY_Change = reinterpret_cast<void(__stdcall *)()>(0x00AB96D0);
    }}

    namespace frmAdvancedBlock { namespace Private {
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00AB9AF0);
    }}

    namespace frmSplash { namespace Private {
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00AB9B80);
    }}

    namespace modLoadGFX {
        static const auto LoadGFX = reinterpret_cast<void(__stdcall *)()>(0x00ABA390);
        static const auto UnloadGFX = reinterpret_cast<void(__stdcall *)()>(0x00ACB440);
        static const auto LoadCustomGFX = reinterpret_cast<void(__stdcall *)()>(0x00ACD220);
        static const auto UnloadCustomGFX = reinterpret_cast<void(__stdcall *)()>(0x00AD3420);
        static const auto LoadCustomGFX2 = reinterpret_cast<void(__stdcall *)(VB6StrPtr& GFXFilePath)>(0x00AD9DC0);
        static const auto LoadWorldCustomGFX = reinterpret_cast<void(__stdcall *)()>(0x00ADFF90);
        static const auto UnloadWorldCustomGFX = reinterpret_cast<void(__stdcall *)()>(0x00AE2630);
    }

    namespace modLoadGFX { namespace Private {
        static const auto cBlockGFX = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE5250);
        static const auto cNPCGFX = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE57E0);
        static const auto cBackgroundGFX = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE65E0);
        static const auto cTileGFX = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE6B70);
        static const auto cSceneGFX = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE7100);
        static const auto cLevelGFX = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE7680);
        static const auto cPathGFX = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE7D20);
    }}

    namespace modLoadGFX {
        static const auto UpdateLoad = reinterpret_cast<void(__stdcall *)()>(0x00AE8110);
    }

    namespace frmLevelDebugger { namespace Private {
        static const auto cmdSortBlocks_Click = reinterpret_cast<void(__stdcall *)()>(0x00AE8B30);
        static const auto cmdSwap_Click = reinterpret_cast<void(__stdcall *)()>(0x00AE8C70);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00AE90D0);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00AE91D0);
    }}

    namespace frmLevelDebugger {
        static const auto UpdateDisplay = reinterpret_cast<void(__stdcall *)()>(0x00AE9240);
    }

    namespace frmLevelDebugger { namespace Private {
        static const auto plrSprites_Click = reinterpret_cast<void(__stdcall *)()>(0x00AEA740);
    }}

    namespace frmGenerator { namespace Private {
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00AEAB70);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00AEAC00);
        static const auto optEffect_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AEACF0);
        static const auto Spawn_Click = reinterpret_cast<void(__stdcall *)()>(0x00AEAE60);
    }}

    namespace frmGenerator {
        static const auto CheckSpawn = reinterpret_cast<void(__stdcall *)()>(0x00AEB180);
    }

    namespace frmGenerator { namespace Private {
        static const auto optSpawnDirection_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AEB9E0);
        static const auto scrDelay_Change = reinterpret_cast<void(__stdcall *)()>(0x00AEBB50);
    }}

    namespace frmNPCAdvanced { namespace Private {
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00AEC010);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00AEC1C0);
        static const auto Legacy_Click = reinterpret_cast<void(__stdcall *)()>(0x00AEC250);
        static const auto scrFire_Change = reinterpret_cast<void(__stdcall *)()>(0x00AEC530);
    }}

    namespace frmWater { namespace Private {
        static const auto Quicksand_Click = reinterpret_cast<void(__stdcall *)()>(0x00AEC690);
        static const auto WaterH_Change = reinterpret_cast<void(__stdcall *)()>(0x00AEC970);
        static const auto WaterR_Click = reinterpret_cast<void(__stdcall *)()>(0x00AECAE0);
        static const auto WaterW_Change = reinterpret_cast<void(__stdcall *)()>(0x00AECD20);
    }}

    namespace frmSprites { namespace Private {
        static const auto Form_Resize = reinterpret_cast<void(__stdcall *)()>(0x00AECE90);
    }}

    namespace Netplay { namespace Private {
        static const auto nClient_Close = reinterpret_cast<void(__stdcall *)()>(0x00AED060);
        static const auto nClient_Connect = reinterpret_cast<void(__stdcall *)()>(0x00AED320);
        static const auto nClient_DataArrival = reinterpret_cast<void(__stdcall *)(int32_t bytesTotal)>(0x00AEDAC0);
        static const auto nServer_Close = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AEE490);
        static const auto nServer_ConnectionRequest = reinterpret_cast<void(__stdcall *)(int16_t& Index, int32_t requestID)>(0x00AEE8B0);
        static const auto nServer_DataArrival = reinterpret_cast<void(__stdcall *)(int16_t& Index, int32_t bytesTotal)>(0x00AEEFE0);
    }}

    namespace Netplay {
        static const auto newData = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr, VB6StrPtr& Action, int16_t& Index)>(0x00AEFA60);
        static const auto sendData = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr, int16_t& exServer)>(0x00B0C0B0);
        static const auto ModPlayer = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B0C4B0);
        static const auto ModNPC = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B0EB70);
        static const auto ModBlock = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B0F2A0);
        static const auto AddBlock = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B0F4B0);
        static const auto EraseBlock = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A, int16_t& noEffects)>(0x00B0FB00);
        static const auto EraseNPC = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A, int16_t& noEffects)>(0x00B0FEF0);
        static const auto EraseBackground = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A, int16_t& noEffects)>(0x00B10250);
        static const auto AddBackground = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B10580);
        static const auto AddNPC = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B10890);
        static const auto AddWater = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B113E0);
        static const auto AddWarp = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B11740);
        static const auto ModSection = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B121B0);
        static const auto ModLayer = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B12420);
        static const auto ModEvent = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B12690);
        static const auto SyncNum = reinterpret_cast<VB6StrPtr(__stdcall *)()>(0x00B137F0);
        static const auto PutPlayerControls = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00B13980);
        static const auto PutPlayerLoc = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00B145C0);
        static const auto GetPlayerControls = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B14980);
        static const auto GetPlayerLoc = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B15680);
        static const auto InitSync = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B15B20);
    }

    namespace Netplay { namespace Private {
        static const auto tmrCheckPorts_Timer = reinterpret_cast<void(__stdcall *)()>(0x00B17480);
        static const auto tmrConnect_Timer = reinterpret_cast<void(__stdcall *)()>(0x00B17890);
        static const auto tmrPing_Timer = reinterpret_cast<void(__stdcall *)()>(0x00B17970);
        static const auto tmrPingC_Timer = reinterpret_cast<void(__stdcall *)()>(0x00B17AA0);
        static const auto tmrPort_Timer = reinterpret_cast<void(__stdcall *)()>(0x00B18270);
        static const auto tmrTimeout_Timer = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B18500);
        static const auto UpdateEvents = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B18980);
    }}

    namespace Netplay {
        static const auto DropClient = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B1A060);
        static const auto DropServer = reinterpret_cast<void(__stdcall *)()>(0x00B1AD90);
    }

    namespace Netplay { namespace Private {
        static const auto tmrTimeoutC_Timer = reinterpret_cast<void(__stdcall *)()>(0x00B1B430);
    }}

    namespace frmNetplay { namespace Private {
        static const auto cmdClearLevel_Click = reinterpret_cast<void(__stdcall *)()>(0x00B1B6F0);
        static const auto cmdGo_Click = reinterpret_cast<void(__stdcall *)()>(0x00B1B960);
        static const auto cPassword_Change = reinterpret_cast<void(__stdcall *)()>(0x00B1D740);
        static const auto Cursor_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B1D820);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00B1D9A0);
        static const auto optMode_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B1DA30);
        static const auto optPlayer_Click = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B1DC20);
        static const auto sPassword_Change = reinterpret_cast<void(__stdcall *)()>(0x00B1DD90);
        static const auto txtNick_Change = reinterpret_cast<void(__stdcall *)()>(0x00B1DE70);
    }}

    namespace frmChat { namespace Private {
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00B1E510);
        static const auto txtSay_KeyPress = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x00B1E5A0);
    }}

    namespace frmLoading { namespace Private {
        static const auto cmdGo_Click = reinterpret_cast<void(__stdcall *)()>(0x00B1F790);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00B1FAE0);
        static const auto tmrLoad_Timer = reinterpret_cast<void(__stdcall *)()>(0x00B1FDF0);
    }}

    namespace frmLoader { namespace Private {
        static const auto cmdEditor_Click = reinterpret_cast<void(__stdcall *)()>(0x00B20000);
        static const auto cmdExit_Click = reinterpret_cast<void(__stdcall *)()>(0x00B20080);
        static const auto cmdGame_Click = reinterpret_cast<void(__stdcall *)()>(0x00B200F0);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00B20160);
        static const auto Form_Unload = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00B20230);
    }}

    namespace modCustom {
        static const auto SaveNPCDefaults = reinterpret_cast<void(__stdcall *)()>(0x00B202B0);
        static const auto LoadNPCDefaults = reinterpret_cast<void(__stdcall *)()>(0x00B20870);
        static const auto FindCustomNPCs = reinterpret_cast<void(__stdcall *)(VB6StrPtr& cFilePath)>(0x00B20E50);
    }

    namespace modCustom { namespace Private {
        static const auto LoadCustomNPC = reinterpret_cast<void(__stdcall *)(int16_t& A, VB6StrPtr& cFileName)>(0x00B21200);
    }}

    namespace frmLevelAdv {
        static const auto RefreshFields = reinterpret_cast<void(__stdcall *)()>(0x00B22900);
    }

    namespace frmLevelAdv { namespace Private {
        static const auto cmdClone_Click = reinterpret_cast<void(__stdcall *)()>(0x00B229C0);
        static const auto Form_Load = reinterpret_cast<void(__stdcall *)()>(0x00B22B20);
        static const auto txtLevelName_Change = reinterpret_cast<void(__stdcall *)()>(0x00B22DB0);
    }}

    namespace frmLevelAdv {
        static const auto CloneSection = reinterpret_cast<void(__stdcall *)(int16_t& SectionS, int16_t& SectionT)>(0x00B22E90);
    }

    namespace frmLevelAdv { namespace Private {
        static const auto CloneLoc = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00B23F40);
    }}

}

#endif // !defined(SMBXINTERNAL_FUNCTIONS_H)

