#if !defined(SMBXINTERNAL_FUNCTIONS_H)
#define SMBXINTERNAL_FUNCTIONS_H

#include <cstdint>
#include "../Misc/VB6StrPtr.h"
#include "../Misc/VB6Bool.h"

namespace SMBX13 {
    namespace Types {
        struct Location_t;
    }

    namespace frmMain_Private {
        static const auto _Form_DblClick_ptr = reinterpret_cast<void(__stdcall *)()>(0x008BD770);
        inline void Form_DblClick() { _Form_DblClick_ptr(); }
        static const auto _Form_KeyDown_ptr = reinterpret_cast<void(__stdcall *)(int16_t& KeyCode, int16_t& Shift)>(0x008BD900);
        inline void Form_KeyDown(int16_t KeyCode, int16_t Shift) { _Form_KeyDown_ptr(KeyCode, Shift); }
        static const auto _Form_KeyPress_ptr = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x008BD9E0);
        inline void Form_KeyPress(int16_t KeyAscii) { _Form_KeyPress_ptr(KeyAscii); }
        static const auto _Form_KeyUp_ptr = reinterpret_cast<void(__stdcall *)(int16_t& KeyCode, int16_t& Shift)>(0x008BDAB0);
        inline void Form_KeyUp(int16_t KeyCode, int16_t Shift) { _Form_KeyUp_ptr(KeyCode, Shift); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x008BDB40);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_MouseDown_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x008BDE80);
        inline void Form_MouseDown(int16_t Button, int16_t Shift, float X, float Y) { _Form_MouseDown_ptr(Button, Shift, X, Y); }
        static const auto _Form_MouseMove_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x008BDF10);
        inline void Form_MouseMove(int16_t Button, int16_t Shift, float X, float Y) { _Form_MouseMove_ptr(Button, Shift, X, Y); }
        static const auto _Form_MouseUp_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x008BE080);
        inline void Form_MouseUp(int16_t Button, int16_t Shift, float X, float Y) { _Form_MouseUp_ptr(Button, Shift, X, Y); }
        static const auto _Form_QueryUnload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel, int16_t& UnloadMode)>(0x008BE100);
        inline void Form_QueryUnload(int16_t Cancel, int16_t UnloadMode) { _Form_QueryUnload_ptr(Cancel, UnloadMode); }
        static const auto _Form_Resize_ptr = reinterpret_cast<void(__stdcall *)()>(0x008BE1A0);
        inline void Form_Resize() { _Form_Resize_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x008BE360);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
    }

    namespace modMain {
        static const auto _SetupPhysics_ptr = reinterpret_cast<void(__stdcall *)()>(0x008BE410);
        inline void SetupPhysics() { _SetupPhysics_ptr(); }
        static const auto _Main_ptr = reinterpret_cast<void(__stdcall *)()>(0x008BE9C0);
        inline void Main() { _Main_ptr(); }
        static const auto _SetupVars_ptr = reinterpret_cast<void(__stdcall *)()>(0x008C2720);
        inline void SetupVars() { _SetupVars_ptr(); }
        static const auto _GameLoop_ptr = reinterpret_cast<void(__stdcall *)()>(0x008CA210);
        inline void GameLoop() { _GameLoop_ptr(); }
        static const auto _MenuLoop_ptr = reinterpret_cast<void(__stdcall *)()>(0x008CA650);
        inline void MenuLoop() { _MenuLoop_ptr(); }
        static const auto _EditorLoop_ptr = reinterpret_cast<void(__stdcall *)()>(0x008D6B80);
        inline void EditorLoop() { _EditorLoop_ptr(); }
        static const auto _KillIt_ptr = reinterpret_cast<void(__stdcall *)()>(0x008D6BB0);
        inline void KillIt() { _KillIt_ptr(); }
        static const auto _SaveLevel_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& FilePath)>(0x008D6CF0);
        inline void SaveLevel(VB6StrPtr FilePath) { _SaveLevel_ptr(FilePath); }
        static const auto _OpenLevel_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& FilePath)>(0x008D8F40);
        inline void OpenLevel(VB6StrPtr FilePath) { _OpenLevel_ptr(FilePath); }
        static const auto _ClearLevel_ptr = reinterpret_cast<void(__stdcall *)()>(0x008DC6E0);
        inline void ClearLevel() { _ClearLevel_ptr(); }
        static const auto _NextLevel_ptr = reinterpret_cast<void(__stdcall *)()>(0x008DDE30);
        inline void NextLevel() { _NextLevel_ptr(); }
        static const auto _UpdateMacro_ptr = reinterpret_cast<void(__stdcall *)()>(0x008DE080);
        inline void UpdateMacro() { _UpdateMacro_ptr(); }
        static const auto _SaveWorld_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& FilePath)>(0x008DEB60);
        inline void SaveWorld(VB6StrPtr FilePath) { _SaveWorld_ptr(FilePath); }
        static const auto _OpenWorld_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& FilePath)>(0x008DF5B0);
        inline void OpenWorld(VB6StrPtr FilePath) { _OpenWorld_ptr(FilePath); }
        static const auto _WorldLoop_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E06B0);
        inline void WorldLoop() { _WorldLoop_ptr(); }
        static const auto _LevelPath_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Lvl, int16_t& Direction, VB6Bool& Skp)>(0x008E1DF0);
        inline void LevelPath(int16_t Lvl, int16_t Direction, VB6Bool Skp=false) { _LevelPath_ptr(Lvl, Direction, Skp); }
        static const auto _PathPath_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Pth, VB6Bool& Skp)>(0x008E2350);
        inline void PathPath(int16_t Pth, VB6Bool Skp=false) { _PathPath_ptr(Pth, Skp); }
        static const auto _PathWait_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E2A40);
        inline void PathWait() { _PathWait_ptr(); }
        static const auto _ClearWorld_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E2E40);
        inline void ClearWorld() { _ClearWorld_ptr(); }
        static const auto _FindWorlds_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E35E0);
        inline void FindWorlds() { _FindWorlds_ptr(); }
        static const auto _FindLevels_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E3D10);
        inline void FindLevels() { _FindLevels_ptr(); }
        static const auto _FindSaves_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E41D0);
        inline void FindSaves() { _FindSaves_ptr(); }
        static const auto _SaveGame_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E47D0);
        inline void SaveGame() { _SaveGame_ptr(); }
        static const auto _LoadGame_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E4E00);
        inline void LoadGame() { _LoadGame_ptr(); }
        static const auto _PauseGame_ptr = reinterpret_cast<void(__stdcall *)(int16_t& plr)>(0x008E54C0);
        inline void PauseGame(int16_t plr) { _PauseGame_ptr(plr); }
        static const auto _InitControls_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E6700);
        inline void InitControls() { _InitControls_ptr(); }
        static const auto _OpenConfig_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E6910);
        inline void OpenConfig() { _OpenConfig_ptr(); }
        static const auto _SaveConfig_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E6CC0);
        inline void SaveConfig() { _SaveConfig_ptr(); }
    }

    namespace modMain_Private {
        static const auto _CheckActive_ptr = reinterpret_cast<void(__stdcall *)()>(0x008E6F70);
        inline void CheckActive() { _CheckActive_ptr(); }
    }

    namespace modMain {
        static const auto _CheatCode_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& NewKey)>(0x008E7490);
        inline void CheatCode(VB6StrPtr NewKey) { _CheatCode_ptr(NewKey); }
        static const auto _OutroLoop_ptr = reinterpret_cast<void(__stdcall *)()>(0x008F6D20);
        inline void OutroLoop() { _OutroLoop_ptr(); }
        static const auto _SetupCredits_ptr = reinterpret_cast<void(__stdcall *)()>(0x008F7200);
        inline void SetupCredits() { _SetupCredits_ptr(); }
        static const auto _FindStars_ptr = reinterpret_cast<void(__stdcall *)()>(0x008F7D70);
        inline void FindStars() { _FindStars_ptr(); }
        static const auto _AddCredit_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newCredit)>(0x008F8000);
        inline void AddCredit(VB6StrPtr newCredit) { _AddCredit_ptr(newCredit); }
        static const auto _newLoc_ptr = reinterpret_cast<SMBX13::Types::Location_t(__stdcall *)(double& X, double& Y, double& Width, double& Height)>(0x008F8050);
        inline SMBX13::Types::Location_t newLoc(double X, double Y, double Width=0, double Height=0) { return _newLoc_ptr(X, Y, Width, Height); }
        static const auto _MoreScore_ptr = reinterpret_cast<void(__stdcall *)(int16_t& addScore, SMBX13::Types::Location_t& Loc, int16_t& Multiplier)>(0x008F80C0);
        inline void MoreScore(int16_t addScore, SMBX13::Types::Location_t& Loc, int16_t Multiplier=0) { _MoreScore_ptr(addScore, Loc, Multiplier); }
        static const auto _SetupPlayerFrames_ptr = reinterpret_cast<void(__stdcall *)()>(0x008F8260);
        inline void SetupPlayerFrames() { _SetupPlayerFrames_ptr(); }
    }

    namespace modMain_Private {
        static const auto _SizableBlocks_ptr = reinterpret_cast<void(__stdcall *)()>(0x008FE2A0);
        inline void SizableBlocks() { _SizableBlocks_ptr(); }
    }

    namespace modMain {
        static const auto _StartBattleMode_ptr = reinterpret_cast<void(__stdcall *)()>(0x008FE440);
        inline void StartBattleMode() { _StartBattleMode_ptr(); }
        static const auto _FixComma_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(VB6StrPtr& newStr)>(0x008FE930);
        inline VB6StrPtr FixComma(VB6StrPtr newStr) { return _FixComma_ptr(newStr); }
    }

    namespace modGraphics {
        static const auto _UpdateGraphics2_ptr = reinterpret_cast<void(__stdcall *)()>(0x008FEB10);
        inline void UpdateGraphics2() { _UpdateGraphics2_ptr(); }
        static const auto _UpdateGraphics_ptr = reinterpret_cast<void(__stdcall *)()>(0x00909290);
        inline void UpdateGraphics() { _UpdateGraphics_ptr(); }
        static const auto _GetvScreen_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x0094DF20);
        inline void GetvScreen(int16_t A) { _GetvScreen_ptr(A); }
        static const auto _GetvScreenAverage_ptr = reinterpret_cast<void(__stdcall *)()>(0x0094EEE0);
        inline void GetvScreenAverage() { _GetvScreenAverage_ptr(); }
        static const auto _GetvScreenAverage2_ptr = reinterpret_cast<void(__stdcall *)()>(0x0094F440);
        inline void GetvScreenAverage2() { _GetvScreenAverage2_ptr(); }
        static const auto _SetupGraphics_ptr = reinterpret_cast<void(__stdcall *)()>(0x0094F680);
        inline void SetupGraphics() { _SetupGraphics_ptr(); }
        static const auto _SetupEditorGraphics_ptr = reinterpret_cast<void(__stdcall *)()>(0x0094F8D0);
        inline void SetupEditorGraphics() { _SetupEditorGraphics_ptr(); }
        static const auto _SetupScreens_ptr = reinterpret_cast<void(__stdcall *)()>(0x009502E0);
        inline void SetupScreens() { _SetupScreens_ptr(); }
        static const auto _DynamicScreen_ptr = reinterpret_cast<void(__stdcall *)()>(0x00950770);
        inline void DynamicScreen() { _DynamicScreen_ptr(); }
        static const auto _SuperPrint_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& SuperWords, int16_t& Font, float& X, float& Y)>(0x00951F50);
        inline void SuperPrint(VB6StrPtr SuperWords, int16_t Font, float X, float Y) { _SuperPrint_ptr(SuperWords, Font, X, Y); }
        static const auto _SetRes_ptr = reinterpret_cast<void(__stdcall *)()>(0x00954240);
        inline void SetRes() { _SetRes_ptr(); }
        static const auto _CheckKey_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(VB6StrPtr& newStrizzle)>(0x009542C0);
        inline VB6StrPtr CheckKey(VB6StrPtr newStrizzle) { return _CheckKey_ptr(newStrizzle); }
    }

    namespace modGraphics_Private {
        static const auto _SpecialFrames_ptr = reinterpret_cast<void(__stdcall *)()>(0x00954AC0);
        inline void SpecialFrames() { _SpecialFrames_ptr(); }
    }

    namespace modGraphics {
        static const auto _DrawBackground_ptr = reinterpret_cast<void(__stdcall *)(int16_t& S, int16_t& Z)>(0x00954F50);
        inline void DrawBackground(int16_t S, int16_t Z) { _DrawBackground_ptr(S, Z); }
        static const auto _PlayerWarpGFX_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, SMBX13::Types::Location_t& tempLocation, float& X2, float& Y2)>(0x00969C00);
        inline void PlayerWarpGFX(int16_t A, SMBX13::Types::Location_t& tempLocation, float X2, float Y2) { _PlayerWarpGFX_ptr(A, tempLocation, X2, Y2); }
        static const auto _NPCWarpGFX_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, SMBX13::Types::Location_t& tempLocation, float& X2, float& Y2)>(0x0096A2F0);
        inline void NPCWarpGFX(int16_t A, SMBX13::Types::Location_t& tempLocation, float X2, float Y2) { _NPCWarpGFX_ptr(A, tempLocation, X2, Y2); }
        static const auto _ChangeScreen_ptr = reinterpret_cast<void(__stdcall *)()>(0x0096AD80);
        inline void ChangeScreen() { _ChangeScreen_ptr(); }
        static const auto _GetvScreenCredits_ptr = reinterpret_cast<void(__stdcall *)()>(0x0096B640);
        inline void GetvScreenCredits() { _GetvScreenCredits_ptr(); }
        static const auto _DoCredits_ptr = reinterpret_cast<void(__stdcall *)()>(0x0096BAF0);
        inline void DoCredits() { _DoCredits_ptr(); }
        static const auto _DrawInterface_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Z, void* numScreens)>(0x0096BF20);
        inline void DrawInterface(int16_t Z, void* numScreens) { _DrawInterface_ptr(Z, numScreens); }
        static const auto _pfrX_ptr = reinterpret_cast<int16_t(__stdcall *)(int16_t& plrFrame)>(0x00987CE0);
        inline int16_t pfrX(int16_t plrFrame) { return _pfrX_ptr(plrFrame); }
        static const auto _pfrY_ptr = reinterpret_cast<int16_t(__stdcall *)(int16_t& plrFrame)>(0x00987D90);
        inline int16_t pfrY(int16_t plrFrame) { return _pfrY_ptr(plrFrame); }
        static const auto _GameThing_ptr = reinterpret_cast<void(__stdcall *)()>(0x00987DE0);
        inline void GameThing() { _GameThing_ptr(); }
        static const auto _DrawPlayer_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& Z)>(0x00988E10);
        inline void DrawPlayer(int16_t A, int16_t Z) { _DrawPlayer_ptr(A, Z); }
        static const auto _ScreenShot_ptr = reinterpret_cast<void(__stdcall *)()>(0x009918C0);
        inline void ScreenShot() { _ScreenShot_ptr(); }
        static const auto _DrawFrozenNPC_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Z, int16_t& A)>(0x009920D0);
        inline void DrawFrozenNPC(int16_t Z, int16_t A) { _DrawFrozenNPC_ptr(Z, A); }
    }

    namespace modCollision {
        static const auto _CheckCollision_ptr = reinterpret_cast<VB6Bool(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00992F50);
        inline VB6Bool CheckCollision(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2) { return _CheckCollision_ptr(Loc1, Loc2); }
        static const auto _n00bCollision_ptr = reinterpret_cast<VB6Bool(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00992FE0);
        inline VB6Bool n00bCollision(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2) { return _n00bCollision_ptr(Loc1, Loc2); }
        static const auto _WarpCollision_ptr = reinterpret_cast<VB6Bool(__stdcall *)(SMBX13::Types::Location_t& Loc1, int16_t& A)>(0x009931D0);
        inline VB6Bool WarpCollision(SMBX13::Types::Location_t& Loc1, int16_t A) { return _WarpCollision_ptr(Loc1, A); }
        static const auto _FindCollision_ptr = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00993330);
        inline int16_t FindCollision(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2) { return _FindCollision_ptr(Loc1, Loc2); }
        static const auto _FindCollisionBelt_ptr = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, float& BeltSpeed)>(0x00993470);
        inline int16_t FindCollisionBelt(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, float BeltSpeed) { return _FindCollisionBelt_ptr(Loc1, Loc2, BeltSpeed); }
        static const auto _NPCFindCollision_ptr = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x009935C0);
        inline int16_t NPCFindCollision(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2) { return _NPCFindCollision_ptr(Loc1, Loc2); }
        static const auto _EasyModeCollision_ptr = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, VB6Bool& StandOn)>(0x00993700);
        inline int16_t EasyModeCollision(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, VB6Bool StandOn=false) { return _EasyModeCollision_ptr(Loc1, Loc2, StandOn); }
        static const auto _BootCollision_ptr = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, VB6Bool& StandOn)>(0x00993970);
        inline int16_t BootCollision(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, VB6Bool StandOn=false) { return _BootCollision_ptr(Loc1, Loc2, StandOn); }
        static const auto _CursorCollision_ptr = reinterpret_cast<VB6Bool(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00993BE0);
        inline VB6Bool CursorCollision(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2) { return _CursorCollision_ptr(Loc1, Loc2); }
        static const auto _ShakeCollision_ptr = reinterpret_cast<VB6Bool(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, int16_t& ShakeY3)>(0x00993CD0);
        inline VB6Bool ShakeCollision(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, int16_t ShakeY3) { return _ShakeCollision_ptr(Loc1, Loc2, ShakeY3); }
        static const auto _vScreenCollision_ptr = reinterpret_cast<VB6Bool(__stdcall *)(int16_t& A, SMBX13::Types::Location_t& Loc2)>(0x00993DE0);
        inline VB6Bool vScreenCollision(int16_t A, SMBX13::Types::Location_t& Loc2) { return _vScreenCollision_ptr(A, Loc2); }
        static const auto _vScreenCollision2_ptr = reinterpret_cast<VB6Bool(__stdcall *)(int16_t& A, SMBX13::Types::Location_t& Loc2)>(0x00993F90);
        inline VB6Bool vScreenCollision2(int16_t A, SMBX13::Types::Location_t& Loc2) { return _vScreenCollision2_ptr(A, Loc2); }
        static const auto _WalkingCollision_ptr = reinterpret_cast<VB6Bool(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00994130);
        inline VB6Bool WalkingCollision(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2) { return _WalkingCollision_ptr(Loc1, Loc2); }
        static const auto _WalkingCollision3_ptr = reinterpret_cast<VB6Bool(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, float& BeltSpeed)>(0x009941B0);
        inline VB6Bool WalkingCollision3(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2, float BeltSpeed) { return _WalkingCollision3_ptr(Loc1, Loc2, BeltSpeed); }
        static const auto _FindRunningCollision_ptr = reinterpret_cast<int16_t(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00994250);
        inline int16_t FindRunningCollision(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2) { return _FindRunningCollision_ptr(Loc1, Loc2); }
        static const auto _CanComeOut_ptr = reinterpret_cast<VB6Bool(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00994390);
        inline VB6Bool CanComeOut(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2) { return _CanComeOut_ptr(Loc1, Loc2); }
        static const auto _CheckHitSpot1_ptr = reinterpret_cast<VB6Bool(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00994480);
        inline VB6Bool CheckHitSpot1(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2) { return _CheckHitSpot1_ptr(Loc1, Loc2); }
    }

    namespace modPlayer {
        static const auto _SetupPlayers_ptr = reinterpret_cast<void(__stdcall *)()>(0x009944F0);
        inline void SetupPlayers() { _SetupPlayers_ptr(); }
        static const auto _UpdatePlayer_ptr = reinterpret_cast<void(__stdcall *)()>(0x00995A20);
        inline void UpdatePlayer() { _UpdatePlayer_ptr(); }
        static const auto _PlayerHurt_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B51E0);
        inline void PlayerHurt(int16_t A) { _PlayerHurt_ptr(A); }
        static const auto _PlayerDead_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B66D0);
        inline void PlayerDead(int16_t A) { _PlayerDead_ptr(A); }
        static const auto _KillPlayer_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B6EC0);
        inline void KillPlayer(int16_t A) { _KillPlayer_ptr(A); }
        static const auto _CheckDead_ptr = reinterpret_cast<int16_t(__stdcall *)()>(0x009B7590);
        inline int16_t CheckDead() { return _CheckDead_ptr(); }
        static const auto _CheckLiving_ptr = reinterpret_cast<int16_t(__stdcall *)()>(0x009B7680);
        inline int16_t CheckLiving() { return _CheckLiving_ptr(); }
        static const auto _LivingPlayers_ptr = reinterpret_cast<VB6Bool(__stdcall *)()>(0x009B7710);
        inline VB6Bool LivingPlayers() { return _LivingPlayers_ptr(); }
        static const auto _EveryonesDead_ptr = reinterpret_cast<void(__stdcall *)()>(0x009B7780);
        inline void EveryonesDead() { _EveryonesDead_ptr(); }
        static const auto _UnDuck_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B7BF0);
        inline void UnDuck(int16_t A) { _UnDuck_ptr(A); }
        static const auto _CheckSection_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B7E10);
        inline void CheckSection(int16_t A) { _CheckSection_ptr(A); }
        static const auto _PlayerFrame_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009B8620);
        inline void PlayerFrame(int16_t A) { _PlayerFrame_ptr(A); }
        static const auto _UpdatePlayerBonus_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& B)>(0x009BB2B0);
        inline void UpdatePlayerBonus(int16_t A, int16_t B) { _UpdatePlayerBonus_ptr(A, B); }
        static const auto _TailSwipe_ptr = reinterpret_cast<void(__stdcall *)(int16_t& plr, VB6Bool& bool_, VB6Bool& Stab, int16_t& StabDir)>(0x009BB490);
        inline void TailSwipe(int16_t plr, VB6Bool bool_=false, VB6Bool Stab=false, int16_t StabDir=0) { _TailSwipe_ptr(plr, bool_, Stab, StabDir); }
        static const auto _YoshiHeight_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009BD560);
        inline void YoshiHeight(int16_t A) { _YoshiHeight_ptr(A); }
        static const auto _YoshiEat_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009BD610);
        inline void YoshiEat(int16_t A) { _YoshiEat_ptr(A); }
        static const auto _YoshiSpit_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009BE6D0);
        inline void YoshiSpit(int16_t A) { _YoshiSpit_ptr(A); }
        static const auto _YoshiPound_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& C, VB6Bool& BreakBlocks)>(0x009C0040);
        inline void YoshiPound(int16_t A, int16_t C, VB6Bool BreakBlocks=false) { _YoshiPound_ptr(A, C, BreakBlocks); }
        static const auto _SwapCoop_ptr = reinterpret_cast<void(__stdcall *)()>(0x009C06A0);
        inline void SwapCoop() { _SwapCoop_ptr(); }
        static const auto _PlayerPush_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& HitSpot)>(0x009C0800);
        inline void PlayerPush(int16_t A, int16_t HitSpot) { _PlayerPush_ptr(A, HitSpot); }
        static const auto _SizeCheck_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C0C50);
        inline void SizeCheck(int16_t A) { _SizeCheck_ptr(A); }
        static const auto _YoshiEatCode_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C1D90);
        inline void YoshiEatCode(int16_t A) { _YoshiEatCode_ptr(A); }
        static const auto _StealBonus_ptr = reinterpret_cast<void(__stdcall *)()>(0x009C46C0);
        inline void StealBonus() { _StealBonus_ptr(); }
        static const auto _ClownCar_ptr = reinterpret_cast<void(__stdcall *)()>(0x009C4A70);
        inline void ClownCar() { _ClownCar_ptr(); }
    }

    namespace modPlayer_Private {
        static const auto _WaterCheck_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C5AC0);
        inline void WaterCheck(int16_t A) { _WaterCheck_ptr(A); }
        static const auto _Tanooki_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C6430);
        inline void Tanooki(int16_t A) { _Tanooki_ptr(A); }
        static const auto _PowerUps_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C6600);
        inline void PowerUps(int16_t A) { _PowerUps_ptr(A); }
        static const auto _SuperWarp_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009C9740);
        inline void SuperWarp(int16_t A) { _SuperWarp_ptr(A); }
        static const auto _PlayerCollide_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009CAEC0);
        inline void PlayerCollide(int16_t A) { _PlayerCollide_ptr(A); }
    }

    namespace modPlayer {
        static const auto _PlayerGrabCode_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, VB6Bool& DontResetGrabTime)>(0x009CC2B0);
        inline void PlayerGrabCode(int16_t A, VB6Bool DontResetGrabTime=false) { _PlayerGrabCode_ptr(A, DontResetGrabTime); }
        static const auto _LinkFrame_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009D1480);
        inline void LinkFrame(int16_t A) { _LinkFrame_ptr(A); }
    }

    namespace modPlayer_Private {
        static const auto _PlayerEffects_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009D1E80);
        inline void PlayerEffects(int16_t A) { _PlayerEffects_ptr(A); }
    }

    namespace modBlocks {
        static const auto _BlockHit_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, VB6Bool& HitDown, int16_t& whatPlayer)>(0x009DA620);
        inline void BlockHit(int16_t A, VB6Bool HitDown=false, int16_t whatPlayer=0) { _BlockHit_ptr(A, HitDown, whatPlayer); }
        static const auto _BlockShakeUp_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009E0940);
        inline void BlockShakeUp(int16_t A) { _BlockShakeUp_ptr(A); }
        static const auto _BlockShakeUpPow_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009E0A00);
        inline void BlockShakeUpPow(int16_t A) { _BlockShakeUpPow_ptr(A); }
        static const auto _BlockShakeDown_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009E0AC0);
        inline void BlockShakeDown(int16_t A) { _BlockShakeDown_ptr(A); }
        static const auto _BlockHitHard_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009E0B80);
        inline void BlockHitHard(int16_t A) { _BlockHitHard_ptr(A); }
        static const auto _KillBlock_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, VB6Bool& Splode)>(0x009E0D50);
        inline void KillBlock(int16_t A, VB6Bool Splode=true) { _KillBlock_ptr(A, Splode); }
        static const auto _BlockFrames_ptr = reinterpret_cast<void(__stdcall *)()>(0x009E14B0);
        inline void BlockFrames() { _BlockFrames_ptr(); }
        static const auto _UpdateBlocks_ptr = reinterpret_cast<void(__stdcall *)()>(0x009E2090);
        inline void UpdateBlocks() { _UpdateBlocks_ptr(); }
        static const auto _PSwitch_ptr = reinterpret_cast<void(__stdcall *)(VB6Bool& bool_)>(0x009E33B0);
        inline void PSwitch(VB6Bool bool_) { _PSwitch_ptr(bool_); }
        static const auto _PowBlock_ptr = reinterpret_cast<void(__stdcall *)()>(0x009E4600);
        inline void PowBlock() { _PowBlock_ptr(); }
    }

    namespace modEffect {
        static const auto _UpdateEffects_ptr = reinterpret_cast<void(__stdcall *)()>(0x009E4900);
        inline void UpdateEffects() { _UpdateEffects_ptr(); }
        static const auto _NewEffect_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, SMBX13::Types::Location_t& Location, float& Direction, int16_t& NewNpc, VB6Bool& Shadow)>(0x009E7380);
        inline void NewEffect(int16_t A, SMBX13::Types::Location_t& Location, float Direction=1, int16_t NewNpc=0, VB6Bool Shadow=false) { _NewEffect_ptr(A, Location, Direction, NewNpc, Shadow); }
        static const auto _KillEffect_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x009EC150);
        inline void KillEffect(int16_t A) { _KillEffect_ptr(A); }
    }

    namespace modEditor {
        static const auto _UpdateEditor_ptr = reinterpret_cast<void(__stdcall *)()>(0x009EC200);
        inline void UpdateEditor() { _UpdateEditor_ptr(); }
        static const auto _GetEditorControls_ptr = reinterpret_cast<void(__stdcall *)()>(0x009FCD10);
        inline void GetEditorControls() { _GetEditorControls_ptr(); }
        static const auto _SetCursor_ptr = reinterpret_cast<void(__stdcall *)()>(0x009FCDA0);
        inline void SetCursor() { _SetCursor_ptr(); }
        static const auto _PositionCursor_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A00C50);
        inline void PositionCursor() { _PositionCursor_ptr(); }
        static const auto _HideCursor_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A02090);
        inline void HideCursor() { _HideCursor_ptr(); }
        static const auto _KillWarp_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A02120);
        inline void KillWarp(int16_t A) { _KillWarp_ptr(A); }
        static const auto _zTestLevel_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A02220);
        inline void zTestLevel() { _zTestLevel_ptr(); }
        static const auto _EditorNPCFrame_ptr = reinterpret_cast<int16_t(__stdcall *)(int16_t& A, float& C, int16_t& N)>(0x00A03630);
        inline int16_t EditorNPCFrame(int16_t A, float C, int16_t N) { return _EditorNPCFrame_ptr(A, C, N); }
        static const auto _MouseMove_ptr = reinterpret_cast<void(__stdcall *)(float& X, float& Y, VB6Bool& nCur)>(0x00A041D0);
        inline void MouseMove(float X, float Y, VB6Bool nCur=false) { _MouseMove_ptr(X, Y, nCur); }
        static const auto _ResetNPC_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A05C40);
        inline void ResetNPC(int16_t A) { _ResetNPC_ptr(A); }
        static const auto _BlockFill_ptr = reinterpret_cast<void(__stdcall *)(SMBX13::Types::Location_t& Loc)>(0x00A05D20);
        inline void BlockFill(SMBX13::Types::Location_t& Loc) { _BlockFill_ptr(Loc); }
    }

    namespace modNPC {
        static const auto _UpdateNPCs_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A06090);
        inline void UpdateNPCs() { _UpdateNPCs_ptr(); }
        static const auto _DropBonus_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A24310);
        inline void DropBonus(int16_t A) { _DropBonus_ptr(A); }
        static const auto _TouchBonus_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& B)>(0x00A24CD0);
        inline void TouchBonus(int16_t A, int16_t B) { _TouchBonus_ptr(A, B); }
        static const auto _NPCHit_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& B, int16_t& C)>(0x00A281B0);
        inline void NPCHit(int16_t A, int16_t B, int16_t C=0) { _NPCHit_ptr(A, B, C); }
        static const auto _KillNPC_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, int16_t& B)>(0x00A315A0);
        inline void KillNPC(int16_t A, int16_t B) { _KillNPC_ptr(A, B); }
        static const auto _CheckSectionNPC_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A3B680);
        inline void CheckSectionNPC(int16_t A) { _CheckSectionNPC_ptr(A); }
        static const auto _Deactivate_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A3B880);
        inline void Deactivate(int16_t A) { _Deactivate_ptr(A); }
        static const auto _Bomb_ptr = reinterpret_cast<void(__stdcall *)(SMBX13::Types::Location_t& Location, int16_t& Game, int16_t& ImmunePlayer)>(0x00A3BA90);
        inline void Bomb(SMBX13::Types::Location_t& Location, int16_t Game, int16_t ImmunePlayer=0) { _Bomb_ptr(Location, Game, ImmunePlayer); }
        static const auto _TurnNPCsIntoCoins_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A3C580);
        inline void TurnNPCsIntoCoins() { _TurnNPCsIntoCoins_ptr(); }
        static const auto _NPCFrames_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A3C990);
        inline void NPCFrames(int16_t A) { _NPCFrames_ptr(A); }
        static const auto _SkullRide_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A446A0);
        inline void SkullRide(int16_t A) { _SkullRide_ptr(A); }
        static const auto _NPCSpecial_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A448D0);
        inline void NPCSpecial(int16_t A) { _NPCSpecial_ptr(A); }
        static const auto _SpecialNPC_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A52BB0);
        inline void SpecialNPC(int16_t A) { _SpecialNPC_ptr(A); }
        static const auto _CharStuff_ptr = reinterpret_cast<void(__stdcall *)(int16_t& WhatNPC, VB6Bool& CheckEggs)>(0x00A60AF0);
        inline void CharStuff(int16_t WhatNPC=0, VB6Bool CheckEggs=false) { _CharStuff_ptr(WhatNPC, CheckEggs); }
        static const auto _RandomBonus_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A61A00);
        inline void RandomBonus() { _RandomBonus_ptr(); }
    }

    namespace modSound {
        static const auto _StartMusic_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A61B40);
        inline void StartMusic(int16_t A) { _StartMusic_ptr(A); }
        static const auto _StopMusic_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A621A0);
        inline void StopMusic() { _StopMusic_ptr(); }
        static const auto _InitSound_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A624D0);
        inline void InitSound() { _InitSound_ptr(); }
        static const auto _PlaySound_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00A73FD0);
        inline void PlaySound(int16_t A) { _PlaySound_ptr(A); }
        static const auto _BlockSound_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A74420);
        inline void BlockSound() { _BlockSound_ptr(); }
        static const auto _UpdateSound_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A74460);
        inline void UpdateSound() { _UpdateSound_ptr(); }
    }

    namespace modJoystick {
        static const auto _UpdateControls_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A74910);
        inline void UpdateControls() { _UpdateControls_ptr(); }
        static const auto _StartJoystick_ptr = reinterpret_cast<VB6Bool(__stdcall *)(int16_t JoystickNumber)>(0x00A75680);
        inline VB6Bool StartJoystick(int16_t JoystickNumber=0) { return _StartJoystick_ptr(JoystickNumber); }
        static const auto _PollJoystick_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A75820);
        inline void PollJoystick() { _PollJoystick_ptr(); }
    }

    namespace frmOpen_Private {
        static const auto _cmdCancel_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A75970);
        inline void cmdCancel_Click() { _cmdCancel_Click_ptr(); }
        static const auto _cmdOpen_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A75A40);
        inline void cmdOpen_Click() { _cmdOpen_Click_ptr(); }
        static const auto _Dir1_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A75AB0);
        inline void Dir1_Change() { _Dir1_Change_ptr(); }
        static const auto _Drive1_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A75BE0);
        inline void Drive1_Change() { _Drive1_Change_ptr(); }
        static const auto _File1_DblClick_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A75D10);
        inline void File1_DblClick() { _File1_DblClick_ptr(); }
        static const auto _File1_KeyPress_ptr = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x00A75D80);
        inline void File1_KeyPress(int16_t KeyAscii) { _File1_KeyPress_ptr(KeyAscii); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A75E00);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A76100);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _OpenIt_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A761B0);
        inline void OpenIt() { _OpenIt_ptr(); }
    }

    namespace frmSave_Private {
        static const auto _cmdCancel_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A76A80);
        inline void cmdCancel_Click() { _cmdCancel_Click_ptr(); }
        static const auto _cmdSave_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A76B50);
        inline void cmdSave_Click() { _cmdSave_Click_ptr(); }
        static const auto _Dir1_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A76BC0);
        inline void Dir1_Change() { _Dir1_Change_ptr(); }
        static const auto _Drive1_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A76CF0);
        inline void Drive1_Change() { _Drive1_Change_ptr(); }
        static const auto _File1_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A76E90);
        inline void File1_Click() { _File1_Click_ptr(); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A77060);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A773D0);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _txtFileName_KeyDown_ptr = reinterpret_cast<void(__stdcall *)(int16_t& KeyCode, int16_t& Shift)>(0x00A77480);
        inline void txtFileName_KeyDown(int16_t KeyCode, int16_t Shift) { _txtFileName_KeyDown_ptr(KeyCode, Shift); }
        static const auto _SaveIt_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A77500);
        inline void SaveIt() { _SaveIt_ptr(); }
    }

    namespace frmLevelEditor_Private {
        static const auto _chkAlign_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A77F30);
        inline void chkAlign_Click() { _chkAlign_Click_ptr(); }
        static const auto _chkAlign2_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A78050);
        inline void chkAlign2_Click() { _chkAlign2_Click_ptr(); }
        static const auto _MDIForm_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A78170);
        inline void MDIForm_Load() { _MDIForm_Load_ptr(); }
        static const auto _MDIForm_QueryUnload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel, int16_t& UnloadMode)>(0x00A7C7B0);
        inline void MDIForm_QueryUnload(int16_t Cancel, int16_t UnloadMode) { _MDIForm_QueryUnload_ptr(Cancel, UnloadMode); }
        static const auto _MDIForm_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A7C820);
        inline void MDIForm_Unload(int16_t Cancel) { _MDIForm_Unload_ptr(Cancel); }
        static const auto _menHelp_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7C890);
        inline void menHelp_Click() { _menHelp_Click_ptr(); }
        static const auto _menuGameplay_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7CB40);
        inline void menuGameplay_Click() { _menuGameplay_Click_ptr(); }
        static const auto _menuLevelDebugger_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7CDF0);
        inline void menuLevelDebugger_Click() { _menuLevelDebugger_Click_ptr(); }
        static const auto _menuEvents_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7CEE0);
        inline void menuEvents_Click() { _menuEvents_Click_ptr(); }
        static const auto _menuFileExit_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7D0D0);
        inline void menuFileExit_Click() { _menuFileExit_Click_ptr(); }
        static const auto _menuFileNew_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7D140);
        inline void menuFileNew_Click() { _menuFileNew_Click_ptr(); }
        static const auto _menuFileOpen_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7D570);
        inline void menuFileOpen_Click() { _menuFileOpen_Click_ptr(); }
        static const auto _menuFileSave_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7D680);
        inline void menuFileSave_Click() { _menuFileSave_Click_ptr(); }
        static const auto _menuLayers_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7D790);
        inline void menuLayers_Click() { _menuLayers_Click_ptr(); }
        static const auto _menuNetplay_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7D980);
        inline void menuNetplay_Click() { _menuNetplay_Click_ptr(); }
        static const auto _menureset_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7DBE0);
        inline void menureset_Click() { _menureset_Click_ptr(); }
        static const auto _menuStatus_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7DC70);
        inline void menuStatus_Click() { _menuStatus_Click_ptr(); }
        static const auto _menuTestLevel_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7DCE0);
        inline void menuTestLevel_Click() { _menuTestLevel_Click_ptr(); }
        static const auto _menuTestLevel128_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7DE00);
        inline void menuTestLevel128_Click() { _menuTestLevel128_Click_ptr(); }
        static const auto _menuTestLevel16_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7DE80);
        inline void menuTestLevel16_Click() { _menuTestLevel16_Click_ptr(); }
        static const auto _menuTestLevel2_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7DF00);
        inline void menuTestLevel2_Click() { _menuTestLevel2_Click_ptr(); }
        static const auto _menuTestLevel32_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7E020);
        inline void menuTestLevel32_Click() { _menuTestLevel32_Click_ptr(); }
        static const auto _menuTestLevel4_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7E0A0);
        inline void menuTestLevel4_Click() { _menuTestLevel4_Click_ptr(); }
        static const auto _menuTestLevel64_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7E120);
        inline void menuTestLevel64_Click() { _menuTestLevel64_Click_ptr(); }
        static const auto _menuTestLevel8_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7E1A0);
        inline void menuTestLevel8_Click() { _menuTestLevel8_Click_ptr(); }
        static const auto _MenuTestLevelBattle_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7E220);
        inline void MenuTestLevelBattle_Click() { _MenuTestLevelBattle_Click_ptr(); }
        static const auto _menuTestLevelTwo_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7E2B0);
        inline void menuTestLevelTwo_Click() { _menuTestLevelTwo_Click_ptr(); }
        static const auto _mnuChat_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7E330);
        inline void mnuChat_Click() { _mnuChat_Click_ptr(); }
        static const auto _mnuLevelEditor_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7E420);
        inline void mnuLevelEditor_Click() { _mnuLevelEditor_Click_ptr(); }
        static const auto _mnuTestSetting_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7E8C0);
        inline void mnuTestSetting_Click() { _mnuTestSetting_Click_ptr(); }
        static const auto _mnuWorldEditor_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A7E9B0);
        inline void mnuWorldEditor_Click() { _mnuWorldEditor_Click_ptr(); }
        static const auto _optCursor_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A7EF10);
        inline void optCursor_Click(int16_t Index) { _optCursor_Click_ptr(Index); }
    }

    namespace frmLevelWindow_Private {
        static const auto _Form_KeyPress_ptr = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x00A814C0);
        inline void Form_KeyPress(int16_t KeyAscii) { _Form_KeyPress_ptr(KeyAscii); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A81530);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_LostFocus_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A81E20);
        inline void Form_LostFocus() { _Form_LostFocus_ptr(); }
        static const auto _Form_Resize_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A81E90);
        inline void Form_Resize() { _Form_Resize_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A82050);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _vScreen_KeyDown_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index, int16_t& KeyCode, int16_t& Shift)>(0x00A820C0);
        inline void vScreen_KeyDown(int16_t Index, int16_t KeyCode, int16_t Shift) { _vScreen_KeyDown_ptr(Index, KeyCode, Shift); }
        static const auto _vScreen_KeyPress_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index, int16_t& KeyAscii)>(0x00A82140);
        inline void vScreen_KeyPress(int16_t Index, int16_t KeyAscii) { _vScreen_KeyPress_ptr(Index, KeyAscii); }
        static const auto _vScreen_LostFocus_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A82210);
        inline void vScreen_LostFocus(int16_t Index) { _vScreen_LostFocus_ptr(Index); }
        static const auto _vScreen_MouseDown_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index, int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x00A82280);
        inline void vScreen_MouseDown(int16_t Index, int16_t Button, int16_t Shift, float X, float Y) { _vScreen_MouseDown_ptr(Index, Button, Shift, X, Y); }
        static const auto _vScreen_MouseMove_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index, int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x00A824E0);
        inline void vScreen_MouseMove(int16_t Index, int16_t Button, int16_t Shift, float X, float Y) { _vScreen_MouseMove_ptr(Index, Button, Shift, X, Y); }
        static const auto _vScreen_MouseUp_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index, int16_t& Button, int16_t& Shift, float& X, float& Y)>(0x00A829E0);
        inline void vScreen_MouseUp(int16_t Index, int16_t Button, int16_t Shift, float X, float Y) { _vScreen_MouseUp_ptr(Index, Button, Shift, X, Y); }
    }

    namespace frmLevelSettings_Private {
        static const auto _cmdAdvanced_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A82A60);
        inline void cmdAdvanced_Click() { _cmdAdvanced_Click_ptr(); }
        static const auto _cmdExit_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A82EC0);
        inline void cmdExit_Click() { _cmdExit_Click_ptr(); }
        static const auto _cmdNoTurnBack_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A83300);
        inline void cmdNoTurnBack_Click() { _cmdNoTurnBack_Click_ptr(); }
        static const auto _cmdWater_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A83740);
        inline void cmdWater_Click() { _cmdWater_Click_ptr(); }
        static const auto _cmdWrap_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A83B80);
        inline void cmdWrap_Click() { _cmdWrap_Click_ptr(); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A83FC0);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A84280);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _optBackground_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A843A0);
        inline void optBackground_Click(int16_t Index) { _optBackground_Click_ptr(Index); }
        static const auto _optBackgroundColor_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A84840);
        inline void optBackgroundColor_Click(int16_t Index) { _optBackgroundColor_Click_ptr(Index); }
        static const auto _optGame_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A84A00);
        inline void optGame_Click(int16_t Index) { _optGame_Click_ptr(Index); }
        static const auto _optLevel_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A84DC0);
        inline void optLevel_Click(int16_t Index) { _optLevel_Click_ptr(Index); }
        static const auto _optMusic_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A850C0);
        inline void optMusic_Click(int16_t Index) { _optMusic_Click_ptr(Index); }
        static const auto _optSection_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A85590);
        inline void optSection_Click(int16_t Index) { _optSection_Click_ptr(Index); }
        static const auto _txtMusic_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A86D30);
        inline void txtMusic_Change() { _txtMusic_Change_ptr(); }
    }

    namespace frmBlocks_Private {
        static const auto _Block_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A86F50);
        inline void Block_Click(int16_t Index) { _Block_Click_ptr(Index); }
        static const auto _BlockH_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A87790);
        inline void BlockH_Change() { _BlockH_Change_ptr(); }
        static const auto _BlockR_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A87900);
        inline void BlockR_Click() { _BlockR_Click_ptr(); }
        static const auto _BlockW_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A87B40);
        inline void BlockW_Change() { _BlockW_Change_ptr(); }
        static const auto _cmdAdvanced_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A87CB0);
        inline void cmdAdvanced_Click() { _cmdAdvanced_Click_ptr(); }
        static const auto _cmdInvis_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A87EA0);
        inline void cmdInvis_Click() { _cmdInvis_Click_ptr(); }
        static const auto _cmdSlip_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A87FF0);
        inline void cmdSlip_Click() { _cmdSlip_Click_ptr(); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A88140);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A89250);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _optBlockS_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A89370);
        inline void optBlockS_Change() { _optBlockS_Change_ptr(); }
        static const auto _optBlockS_Scroll_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A89520);
        inline void optBlockS_Scroll() { _optBlockS_Scroll_ptr(); }
        static const auto _optBlockSpecial_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A896D0);
        inline void optBlockSpecial_Click(int16_t Index) { _optBlockSpecial_Click_ptr(Index); }
        static const auto _optGame_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A89840);
        inline void optGame_Click(int16_t Index) { _optGame_Click_ptr(Index); }
        static const auto _optMisc_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A89C00);
        inline void optMisc_Click(int16_t Index) { _optMisc_Click_ptr(Index); }
        static const auto _optSMW_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A89FC0);
        inline void optSMW_Click(int16_t Index) { _optSMW_Click_ptr(Index); }
        static const auto _optSMB3_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A8A380);
        inline void optSMB3_Click(int16_t Index) { _optSMB3_Click_ptr(Index); }
        static const auto _optSMB2_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A8A740);
        inline void optSMB2_Click(int16_t Index) { _optSMB2_Click_ptr(Index); }
        static const auto _optSMB1_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A8AB00);
        inline void optSMB1_Click(int16_t Index) { _optSMB1_Click_ptr(Index); }
        static const auto _qBlockS_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A8AEC0);
        inline void qBlockS_Click(int16_t Index) { _qBlockS_Click_ptr(Index); }
        static const auto _qCoin_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A8B0A0);
        inline void qCoin_Click(int16_t Index) { _qCoin_Click_ptr(Index); }
    }

    namespace frmBlocks {
        static const auto _CheckBlock_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8B150);
        inline void CheckBlock() { _CheckBlock_ptr(); }
    }

    namespace frmNPCs_Private {
        static const auto _Bubble_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8D850);
        inline void Bubble_Click() { _Bubble_Click_ptr(); }
        static const auto _Buried_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8DD40);
        inline void Buried_Click() { _Buried_Click_ptr(); }
        static const auto _chkMessage_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8E300);
        inline void chkMessage_Click() { _chkMessage_Click_ptr(); }
        static const auto _cmdAdvanced_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8E5B0);
        inline void cmdAdvanced_Click() { _cmdAdvanced_Click_ptr(); }
        static const auto _cmdEvents_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8E7A0);
        inline void cmdEvents_Click() { _cmdEvents_Click_ptr(); }
        static const auto _cmdGenerator_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8E990);
        inline void cmdGenerator_Click() { _cmdGenerator_Click_ptr(); }
        static const auto _DontMove_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8EB80);
        inline void DontMove_Click() { _DontMove_Click_ptr(); }
        static const auto _Egg_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8EE60);
        inline void Egg_Click() { _Egg_Click_ptr(); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8F420);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A8F950);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _Friendly_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8FA70);
        inline void Friendly_Click() { _Friendly_Click_ptr(); }
        static const auto _Lakitu_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A8FD50);
        inline void Lakitu_Click() { _Lakitu_Click_ptr(); }
        static const auto _NPC_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A90310);
        inline void NPC_Click(int16_t Index) { _NPC_Click_ptr(Index); }
        static const auto _NPCText_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A93770);
        inline void NPCText_Change() { _NPCText_Change_ptr(); }
        static const auto _optEffect_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A93830);
        inline void optEffect_Click(int16_t Index) { _optEffect_Click_ptr(Index); }
        static const auto _optGame_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A93970);
        inline void optGame_Click(int16_t Index) { _optGame_Click_ptr(Index); }
        static const auto _optNPCDirection_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A94050);
        inline void optNPCDirection_Click(int16_t Index) { _optNPCDirection_Click_ptr(Index); }
    }

    namespace frmNPCs {
        static const auto _ShowLak_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A941D0);
        inline void ShowLak() { _ShowLak_ptr(); }
    }

    namespace frmBackgrounds_Private {
        static const auto _Background_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A943F0);
        inline void Background_Click(int16_t Index) { _Background_Click_ptr(Index); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A946F0);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A94AF0);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _optGame_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A94C10);
        inline void optGame_Click(int16_t Index) { _optGame_Click_ptr(Index); }
    }

    namespace frmWarp_Private {
        static const auto _chkEntrance_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A94FD0);
        inline void chkEntrance_Click() { _chkEntrance_Click_ptr(); }
        static const auto _chkMapWarp_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A95140);
        inline void chkMapWarp_Click() { _chkMapWarp_Click_ptr(); }
        static const auto _cmdLocked_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A952B0);
        inline void cmdLocked_Click() { _cmdLocked_Click_ptr(); }
        static const auto _cmdNPC_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A95590);
        inline void cmdNPC_Click() { _cmdNPC_Click_ptr(); }
        static const auto _cmdYoshi_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A95870);
        inline void cmdYoshi_Click() { _cmdYoshi_Click_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A95B50);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _optDirection_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A95C70);
        inline void optDirection_Click(int16_t Index) { _optDirection_Click_ptr(Index); }
        static const auto _optDirection2_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A95DE0);
        inline void optDirection2_Click(int16_t Index) { _optDirection2_Click_ptr(Index); }
        static const auto _optE_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A95F50);
        inline void optE_Click(int16_t Index) { _optE_Click_ptr(Index); }
        static const auto _optEffect_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A960C0);
        inline void optEffect_Click(int16_t Index) { _optEffect_Click_ptr(Index); }
        static const auto _scrWarp_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A966B0);
        inline void scrWarp_Change() { _scrWarp_Change_ptr(); }
    }

    namespace frmTiles_Private {
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A96A40);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _optGame_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A96D60);
        inline void optGame_Click(int16_t Index) { _optGame_Click_ptr(Index); }
        static const auto _Tile_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97140);
        inline void Tile_Click(int16_t Index) { _Tile_Click_ptr(Index); }
    }

    namespace frmScene_Private {
        static const auto _Scene_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97440);
        inline void Scene_Click(int16_t Index) { _Scene_Click_ptr(Index); }
    }

    namespace frmLevels_Private {
        static const auto _cmbExit_Change_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A975B0);
        inline void cmbExit_Change(int16_t Index) { _cmbExit_Change_ptr(Index); }
        static const auto _cmbExit_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97720);
        inline void cmbExit_Click(int16_t Index) { _cmbExit_Click_ptr(Index); }
        static const auto _scrWarp_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A97890);
        inline void scrWarp_Change() { _scrWarp_Change_ptr(); }
        static const auto _WorldLevel_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97C20);
        inline void WorldLevel_Click(int16_t Index) { _WorldLevel_Click_ptr(Index); }
    }

    namespace frmPaths_Private {
        static const auto _WorldPath_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97D90);
        inline void WorldPath_Click(int16_t Index) { _WorldPath_Click_ptr(Index); }
    }

    namespace frmMusic_Private {
        static const auto _optMusic_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A97F00);
        inline void optMusic_Click(int16_t Index) { _optMusic_Click_ptr(Index); }
    }

    namespace modChangeRes {
        static const auto _GetCurrentRes_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A98070);
        inline void GetCurrentRes() { _GetCurrentRes_ptr(); }
        static const auto _SetOrigRes_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A98150);
        inline void SetOrigRes() { _SetOrigRes_ptr(); }
        static const auto _ChangeRes_ptr = reinterpret_cast<void(__stdcall *)(int16_t& ScreenX, int16_t& ScreenY, int16_t& ScreenColor, int16_t& ScreenFreq)>(0x00A98190);
        inline void ChangeRes(int16_t ScreenX, int16_t ScreenY, int16_t ScreenColor, int16_t ScreenFreq) { _ChangeRes_ptr(ScreenX, ScreenY, ScreenColor, ScreenFreq); }
    }

    namespace modChangeRes_Private {
        static const auto _SaveIt_ptr = reinterpret_cast<void(__stdcall *)(int16_t& ScX, int16_t& ScY, int16_t& ScC, int16_t& ScF, VB6StrPtr& ScreenChanged)>(0x00A984A0);
        inline void SaveIt(int16_t ScX, int16_t ScY, int16_t ScC, int16_t ScF, VB6StrPtr ScreenChanged) { _SaveIt_ptr(ScX, ScY, ScC, ScF, ScreenChanged); }
    }

    namespace modSorting {
        static const auto _qSortBlocksY_ptr = reinterpret_cast<void(__stdcall *)(int16_t& min, int16_t& max)>(0x00A985A0);
        inline void qSortBlocksY(int16_t min, int16_t max) { _qSortBlocksY_ptr(min, max); }
        static const auto _qSortBlocksX_ptr = reinterpret_cast<void(__stdcall *)(int16_t& min, int16_t& max)>(0x00A988E0);
        inline void qSortBlocksX(int16_t min, int16_t max) { _qSortBlocksX_ptr(min, max); }
        static const auto _qSortBackgrounds_ptr = reinterpret_cast<void(__stdcall *)(int16_t& min, int16_t& max)>(0x00A98C20);
        inline void qSortBackgrounds(int16_t min, int16_t max) { _qSortBackgrounds_ptr(min, max); }
        static const auto _FindBlocks_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A98FE0);
        inline void FindBlocks() { _FindBlocks_ptr(); }
        static const auto _BackGroundPri_ptr = reinterpret_cast<double(__stdcall *)(int16_t& A)>(0x00A992C0);
        inline double BackGroundPri(int16_t A) { return _BackGroundPri_ptr(A); }
        static const auto _NPCSort_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A99620);
        inline void NPCSort() { _NPCSort_ptr(); }
        static const auto _FindSBlocks_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A99870);
        inline void FindSBlocks() { _FindSBlocks_ptr(); }
        static const auto _qSortSBlocks_ptr = reinterpret_cast<void(__stdcall *)(int16_t& min, int16_t& max)>(0x00A99930);
        inline void qSortSBlocks(int16_t min, int16_t max) { _qSortSBlocks_ptr(min, max); }
        static const auto _qSortNPCsY_ptr = reinterpret_cast<void(__stdcall *)(int16_t& min, int16_t& max)>(0x00A99C10);
        inline void qSortNPCsY(int16_t min, int16_t max) { _qSortNPCsY_ptr(min, max); }
        static const auto _UpdateBackgrounds_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9A000);
        inline void UpdateBackgrounds() { _UpdateBackgrounds_ptr(); }
    }

    namespace frmWorld_Private {
        static const auto _chkChar_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9A1E0);
        inline void chkChar_Click(int16_t Index) { _chkChar_Click_ptr(Index); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A9A430);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _txtStars_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9A4C0);
        inline void txtStars_Change() { _txtStars_Change_ptr(); }
    }

    namespace frmTestSettings_Private {
        static const auto _chkFullscreen_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9A680);
        inline void chkFullscreen_Click() { _chkFullscreen_Click_ptr(); }
        static const auto _chkGodMode_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9A760);
        inline void chkGodMode_Click() { _chkGodMode_Click_ptr(); }
        static const auto _chkGrabAll_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9A840);
        inline void chkGrabAll_Click() { _chkGrabAll_Click_ptr(); }
        static const auto _chkMaxFPS_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9A920);
        inline void chkMaxFPS_Click() { _chkMaxFPS_Click_ptr(); }
        static const auto _chkShowFPS_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9AA00);
        inline void chkShowFPS_Click() { _chkShowFPS_Click_ptr(); }
        static const auto _cmdClose_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9AAE0);
        inline void cmdClose_Click() { _cmdClose_Click_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A9AB70);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _optBlackYoshi_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9AC00);
        inline void optBlackYoshi_Click(int16_t Index) { _optBlackYoshi_Click_ptr(Index); }
        static const auto _optBlueBoot_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9ACC0);
        inline void optBlueBoot_Click(int16_t Index) { _optBlueBoot_Click_ptr(Index); }
        static const auto _optBlueYoshi_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9AD80);
        inline void optBlueYoshi_Click(int16_t Index) { _optBlueYoshi_Click_ptr(Index); }
        static const auto _optBoot_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9AE40);
        inline void optBoot_Click(int16_t Index) { _optBoot_Click_ptr(Index); }
        static const auto _optChrLink_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9AF00);
        inline void optChrLink_Click(int16_t Index) { _optChrLink_Click_ptr(Index); }
        static const auto _optChrLuigi_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9B850);
        inline void optChrLuigi_Click(int16_t Index) { _optChrLuigi_Click_ptr(Index); }
        static const auto _optChrMario_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9BCF0);
        inline void optChrMario_Click(int16_t Index) { _optChrMario_Click_ptr(Index); }
        static const auto _optChrPeach_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9C190);
        inline void optChrPeach_Click(int16_t Index) { _optChrPeach_Click_ptr(Index); }
        static const auto _optChrToad_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9C980);
        inline void optChrToad_Click(int16_t Index) { _optChrToad_Click_ptr(Index); }
        static const auto _optFireMario_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D170);
        inline void optFireMario_Click(int16_t Index) { _optFireMario_Click_ptr(Index); }
        static const auto _optGreenYoshi_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D200);
        inline void optGreenYoshi_Click(int16_t Index) { _optGreenYoshi_Click_ptr(Index); }
        static const auto _optKoopaClownCar_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D2C0);
        inline void optKoopaClownCar_Click(int16_t Index) { _optKoopaClownCar_Click_ptr(Index); }
        static const auto _optHammerMario_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D300);
        inline void optHammerMario_Click(int16_t Index) { _optHammerMario_Click_ptr(Index); }
        static const auto _optIceMario_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D390);
        inline void optIceMario_Click(int16_t Index) { _optIceMario_Click_ptr(Index); }
        static const auto _optIceYoshi_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D420);
        inline void optIceYoshi_Click(int16_t Index) { _optIceYoshi_Click_ptr(Index); }
        static const auto _optMario_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D4E0);
        inline void optMario_Click(int16_t Index) { _optMario_Click_ptr(Index); }
        static const auto _optMarioC_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D570);
        inline void optMarioC_Click(int16_t Index) { _optMarioC_Click_ptr(Index); }
        static const auto _optNoMount_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D590);
        inline void optNoMount_Click(int16_t Index) { _optNoMount_Click_ptr(Index); }
        static const auto _optPinkYoshi_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D620);
        inline void optPinkYoshi_Click(int16_t Index) { _optPinkYoshi_Click_ptr(Index); }
        static const auto _optPurpleYoshi_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D6E0);
        inline void optPurpleYoshi_Click(int16_t Index) { _optPurpleYoshi_Click_ptr(Index); }
        static const auto _optRacoonMario_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D7A0);
        inline void optRacoonMario_Click(int16_t Index) { _optRacoonMario_Click_ptr(Index); }
        static const auto _optRedBoot_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D830);
        inline void optRedBoot_Click(int16_t Index) { _optRedBoot_Click_ptr(Index); }
        static const auto _optRedYoshi_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D8F0);
        inline void optRedYoshi_Click(int16_t Index) { _optRedYoshi_Click_ptr(Index); }
        static const auto _optSuperMario_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9D9B0);
        inline void optSuperMario_Click(int16_t Index) { _optSuperMario_Click_ptr(Index); }
        static const auto _optTanookiMario_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9DA40);
        inline void optTanookiMario_Click(int16_t Index) { _optTanookiMario_Click_ptr(Index); }
        static const auto _optYellowYoshi_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9DAD0);
        inline void optYellowYoshi_Click(int16_t Index) { _optYellowYoshi_Click_ptr(Index); }
        static const auto _ShowAll_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00A9DB90);
        inline void ShowAll(int16_t Index) { _ShowAll_ptr(Index); }
    }

    namespace frmAdvanced_Private {
        static const auto _cmbPara_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9E440);
        inline void cmbPara_Click() { _cmbPara_Click_ptr(); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9E7D0);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A9E910);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
    }

    namespace frmLayers_Private {
        static const auto _cmdAdd_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9E9A0);
        inline void cmdAdd_Click() { _cmdAdd_Click_ptr(); }
        static const auto _cmdDelete_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9F260);
        inline void cmdDelete_Click() { _cmdDelete_Click_ptr(); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9FE00);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00A9FF10);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _lstLayer_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00A9FFA0);
        inline void lstLayer_Click() { _lstLayer_Click_ptr(); }
        static const auto _lstLayer_DblClick_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA0F20);
        inline void lstLayer_DblClick() { _lstLayer_DblClick_ptr(); }
        static const auto _txtLayer_KeyPress_ptr = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x00AA1250);
        inline void txtLayer_KeyPress(int16_t KeyAscii) { _txtLayer_KeyPress_ptr(KeyAscii); }
    }

    namespace frmLayers {
        static const auto _UpdateLayers_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA2160);
        inline void UpdateLayers() { _UpdateLayers_ptr(); }
    }

    namespace modLayers {
        static const auto _ShowLayer_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& LayerName, VB6Bool& NoEffect)>(0x00AA2760);
        inline void ShowLayer(VB6StrPtr LayerName, VB6Bool NoEffect=false) { _ShowLayer_ptr(LayerName, NoEffect); }
        static const auto _HideLayer_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& LayerName, VB6Bool& NoEffect)>(0x00AA3730);
        inline void HideLayer(VB6StrPtr LayerName, VB6Bool NoEffect=false) { _HideLayer_ptr(LayerName, NoEffect); }
        static const auto _ProcEvent_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& EventName, VB6Bool& NoEffect)>(0x00AA42D0);
        inline void ProcEvent(VB6StrPtr EventName, VB6Bool NoEffect=false) { _ProcEvent_ptr(EventName, NoEffect); }
        static const auto _UpdateEvents_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA60E0);
        inline void UpdateEvents() { _UpdateEvents_ptr(); }
        static const auto _UpdateLayers_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA6710);
        inline void UpdateLayers() { _UpdateLayers_ptr(); }
    }

    namespace frmEvents_Private {
        static const auto _AutoSection_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA7F30);
        inline void AutoSection_Change() { _AutoSection_Change_ptr(); }
        static const auto _cAltJump_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA8410);
        inline void cAltJump_Click() { _cAltJump_Click_ptr(); }
        static const auto _cAltRun_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA87A0);
        inline void cAltRun_Click() { _cAltRun_Click_ptr(); }
        static const auto _cDown_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA8B30);
        inline void cDown_Click() { _cDown_Click_ptr(); }
        static const auto _cDrop_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA8EC0);
        inline void cDrop_Click() { _cDrop_Click_ptr(); }
        static const auto _chkSmoke_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA9250);
        inline void chkSmoke_Click() { _chkSmoke_Click_ptr(); }
        static const auto _chkStart_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA96A0);
        inline void chkStart_Click() { _chkStart_Click_ptr(); }
        static const auto _cJump_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA9AF0);
        inline void cJump_Click() { _cJump_Click_ptr(); }
        static const auto _cLeft_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AA9E80);
        inline void cLeft_Click() { _cLeft_Click_ptr(); }
        static const auto _cmbEndGame_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AAA210);
        inline void cmbEndGame_Click() { _cmbEndGame_Click_ptr(); }
        static const auto _cmbLayerMove_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AAA560);
        inline void cmbLayerMove_Click() { _cmbLayerMove_Click_ptr(); }
        static const auto _cmbSound_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AAA970);
        inline void cmbSound_Click() { _cmbSound_Click_ptr(); }
        static const auto _cmdAdd_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AAADE0);
        inline void cmdAdd_Click() { _cmdAdd_Click_ptr(); }
        static const auto _cmdBackground_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AAB770);
        inline void cmdBackground_Click(int16_t Index) { _cmdBackground_Click_ptr(Index); }
        static const auto _cmdDelete_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AABDD0);
        inline void cmdDelete_Click() { _cmdDelete_Click_ptr(); }
        static const auto _cmdDupe_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AAD210);
        inline void cmdDupe_Click() { _cmdDupe_Click_ptr(); }
        static const auto _cmdHideAdd_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AADD30);
        inline void cmdHideAdd_Click() { _cmdHideAdd_Click_ptr(); }
        static const auto _cmdHideRemove_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AAE1F0);
        inline void cmdHideRemove_Click() { _cmdHideRemove_Click_ptr(); }
        static const auto _cmdMusic_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AAE660);
        inline void cmdMusic_Click(int16_t Index) { _cmdMusic_Click_ptr(Index); }
        static const auto _cmdPosition_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AAECC0);
        inline void cmdPosition_Click(int16_t Index) { _cmdPosition_Click_ptr(Index); }
        static const auto _cmdShowAdd_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AAF5C0);
        inline void cmdShowAdd_Click() { _cmdShowAdd_Click_ptr(); }
        static const auto _cmdShowRemove_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AAFA80);
        inline void cmdShowRemove_Click() { _cmdShowRemove_Click_ptr(); }
        static const auto _cmdToggleAdd_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AAFEF0);
        inline void cmdToggleAdd_Click() { _cmdToggleAdd_Click_ptr(); }
        static const auto _cmdToggleRemove_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB03C0);
        inline void cmdToggleRemove_Click() { _cmdToggleRemove_Click_ptr(); }
        static const auto _cRight_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB0840);
        inline void cRight_Click() { _cRight_Click_ptr(); }
        static const auto _cRun_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB0BD0);
        inline void cRun_Click() { _cRun_Click_ptr(); }
        static const auto _cStart_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB0F60);
        inline void cStart_Click() { _cStart_Click_ptr(); }
        static const auto _cUp_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB12F0);
        inline void cUp_Click() { _cUp_Click_ptr(); }
        static const auto _EventText_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB1680);
        inline void EventText_Change() { _EventText_Change_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00AB1D30);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _lstEvent_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB1DC0);
        inline void lstEvent_Click() { _lstEvent_Click_ptr(); }
        static const auto _lstLayers_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB2110);
        inline void lstLayers_Click() { _lstLayers_Click_ptr(); }
        static const auto _scrDelay_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB2250);
        inline void scrDelay_Change() { _scrDelay_Change_ptr(); }
        static const auto _scrX_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB28E0);
        inline void scrX_Change() { _scrX_Change_ptr(); }
        static const auto _scrY_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB2D00);
        inline void scrY_Change() { _scrY_Change_ptr(); }
        static const auto _TriggerEvent_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB3120);
        inline void TriggerEvent_Click() { _TriggerEvent_Click_ptr(); }
        static const auto _txtEvent_KeyPress_ptr = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x00AB3490);
        inline void txtEvent_KeyPress(int16_t KeyAscii) { _txtEvent_KeyPress_ptr(KeyAscii); }
    }

    namespace frmEvents {
        static const auto _RefreshEvents_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB4A70);
        inline void RefreshEvents() { _RefreshEvents_ptr(); }
    }

    namespace frmEvents_Private {
        static const auto _txtX_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB92B0);
        inline void txtX_Change() { _txtX_Change_ptr(); }
        static const auto _txtY_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB96D0);
        inline void txtY_Change() { _txtY_Change_ptr(); }
    }

    namespace frmAdvancedBlock_Private {
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00AB9AF0);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
    }

    namespace frmSplash_Private {
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AB9B80);
        inline void Form_Load() { _Form_Load_ptr(); }
    }

    namespace modLoadGFX {
        static const auto _LoadGFX_ptr = reinterpret_cast<void(__stdcall *)()>(0x00ABA390);
        inline void LoadGFX() { _LoadGFX_ptr(); }
        static const auto _UnloadGFX_ptr = reinterpret_cast<void(__stdcall *)()>(0x00ACB440);
        inline void UnloadGFX() { _UnloadGFX_ptr(); }
        static const auto _LoadCustomGFX_ptr = reinterpret_cast<void(__stdcall *)()>(0x00ACD220);
        inline void LoadCustomGFX() { _LoadCustomGFX_ptr(); }
        static const auto _UnloadCustomGFX_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AD3420);
        inline void UnloadCustomGFX() { _UnloadCustomGFX_ptr(); }
        static const auto _LoadCustomGFX2_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& GFXFilePath)>(0x00AD9DC0);
        inline void LoadCustomGFX2(VB6StrPtr GFXFilePath) { _LoadCustomGFX2_ptr(GFXFilePath); }
        static const auto _LoadWorldCustomGFX_ptr = reinterpret_cast<void(__stdcall *)()>(0x00ADFF90);
        inline void LoadWorldCustomGFX() { _LoadWorldCustomGFX_ptr(); }
        static const auto _UnloadWorldCustomGFX_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AE2630);
        inline void UnloadWorldCustomGFX() { _UnloadWorldCustomGFX_ptr(); }
    }

    namespace modLoadGFX_Private {
        static const auto _cBlockGFX_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE5250);
        inline void cBlockGFX(int16_t A) { _cBlockGFX_ptr(A); }
        static const auto _cNPCGFX_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE57E0);
        inline void cNPCGFX(int16_t A) { _cNPCGFX_ptr(A); }
        static const auto _cBackgroundGFX_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE65E0);
        inline void cBackgroundGFX(int16_t A) { _cBackgroundGFX_ptr(A); }
        static const auto _cTileGFX_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE6B70);
        inline void cTileGFX(int16_t A) { _cTileGFX_ptr(A); }
        static const auto _cSceneGFX_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE7100);
        inline void cSceneGFX(int16_t A) { _cSceneGFX_ptr(A); }
        static const auto _cLevelGFX_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE7680);
        inline void cLevelGFX(int16_t A) { _cLevelGFX_ptr(A); }
        static const auto _cPathGFX_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00AE7D20);
        inline void cPathGFX(int16_t A) { _cPathGFX_ptr(A); }
    }

    namespace modLoadGFX {
        static const auto _UpdateLoad_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AE8110);
        inline void UpdateLoad() { _UpdateLoad_ptr(); }
    }

    namespace frmLevelDebugger_Private {
        static const auto _cmdSortBlocks_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AE8B30);
        inline void cmdSortBlocks_Click() { _cmdSortBlocks_Click_ptr(); }
        static const auto _cmdSwap_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AE8C70);
        inline void cmdSwap_Click() { _cmdSwap_Click_ptr(); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AE90D0);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00AE91D0);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
    }

    namespace frmLevelDebugger {
        static const auto _UpdateDisplay_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AE9240);
        inline void UpdateDisplay() { _UpdateDisplay_ptr(); }
    }

    namespace frmLevelDebugger_Private {
        static const auto _plrSprites_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AEA740);
        inline void plrSprites_Click() { _plrSprites_Click_ptr(); }
    }

    namespace frmGenerator_Private {
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AEAB70);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00AEAC00);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _optEffect_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AEACF0);
        inline void optEffect_Click(int16_t Index) { _optEffect_Click_ptr(Index); }
        static const auto _Spawn_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AEAE60);
        inline void Spawn_Click() { _Spawn_Click_ptr(); }
    }

    namespace frmGenerator {
        static const auto _CheckSpawn_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AEB180);
        inline void CheckSpawn() { _CheckSpawn_ptr(); }
    }

    namespace frmGenerator_Private {
        static const auto _optSpawnDirection_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AEB9E0);
        inline void optSpawnDirection_Click(int16_t Index) { _optSpawnDirection_Click_ptr(Index); }
        static const auto _scrDelay_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AEBB50);
        inline void scrDelay_Change() { _scrDelay_Change_ptr(); }
    }

    namespace frmNPCAdvanced_Private {
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AEC010);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00AEC1C0);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _Legacy_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AEC250);
        inline void Legacy_Click() { _Legacy_Click_ptr(); }
        static const auto _scrFire_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AEC530);
        inline void scrFire_Change() { _scrFire_Change_ptr(); }
    }

    namespace frmWater_Private {
        static const auto _Quicksand_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AEC690);
        inline void Quicksand_Click() { _Quicksand_Click_ptr(); }
        static const auto _WaterH_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AEC970);
        inline void WaterH_Change() { _WaterH_Change_ptr(); }
        static const auto _WaterR_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AECAE0);
        inline void WaterR_Click() { _WaterR_Click_ptr(); }
        static const auto _WaterW_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AECD20);
        inline void WaterW_Change() { _WaterW_Change_ptr(); }
    }

    namespace frmSprites_Private {
        static const auto _Form_Resize_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AECE90);
        inline void Form_Resize() { _Form_Resize_ptr(); }
    }

    namespace Netplay_Private {
        static const auto _nClient_Close_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AED060);
        inline void nClient_Close() { _nClient_Close_ptr(); }
        static const auto _nClient_Connect_ptr = reinterpret_cast<void(__stdcall *)()>(0x00AED320);
        inline void nClient_Connect() { _nClient_Connect_ptr(); }
        static const auto _nClient_DataArrival_ptr = reinterpret_cast<void(__stdcall *)(int32_t bytesTotal)>(0x00AEDAC0);
        inline void nClient_DataArrival(int32_t bytesTotal) { _nClient_DataArrival_ptr(bytesTotal); }
        static const auto _nServer_Close_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00AEE490);
        inline void nServer_Close(int16_t Index) { _nServer_Close_ptr(Index); }
        static const auto _nServer_ConnectionRequest_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index, int32_t requestID)>(0x00AEE8B0);
        inline void nServer_ConnectionRequest(int16_t Index, int32_t requestID) { _nServer_ConnectionRequest_ptr(Index, requestID); }
        static const auto _nServer_DataArrival_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index, int32_t bytesTotal)>(0x00AEEFE0);
        inline void nServer_DataArrival(int16_t Index, int32_t bytesTotal) { _nServer_DataArrival_ptr(Index, bytesTotal); }
    }

    namespace Netplay {
        static const auto _newData_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr, VB6StrPtr& Action, int16_t& Index)>(0x00AEFA60);
        inline void newData(VB6StrPtr newStr, VB6StrPtr Action, int16_t Index) { _newData_ptr(newStr, Action, Index); }
        static const auto _sendData_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr, int16_t& exServer)>(0x00B0C0B0);
        inline void sendData(VB6StrPtr newStr, int16_t exServer=0) { _sendData_ptr(newStr, exServer); }
        static const auto _ModPlayer_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B0C4B0);
        inline void ModPlayer(VB6StrPtr newStr) { _ModPlayer_ptr(newStr); }
        static const auto _ModNPC_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B0EB70);
        inline void ModNPC(VB6StrPtr newStr) { _ModNPC_ptr(newStr); }
        static const auto _ModBlock_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B0F2A0);
        inline void ModBlock(VB6StrPtr newStr) { _ModBlock_ptr(newStr); }
        static const auto _AddBlock_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B0F4B0);
        inline VB6StrPtr AddBlock(int16_t A) { return _AddBlock_ptr(A); }
        static const auto _EraseBlock_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A, int16_t& noEffects)>(0x00B0FB00);
        inline VB6StrPtr EraseBlock(int16_t A, int16_t noEffects=0) { return _EraseBlock_ptr(A, noEffects); }
        static const auto _EraseNPC_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A, int16_t& noEffects)>(0x00B0FEF0);
        inline VB6StrPtr EraseNPC(int16_t A, int16_t noEffects=0) { return _EraseNPC_ptr(A, noEffects); }
        static const auto _EraseBackground_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A, int16_t& noEffects)>(0x00B10250);
        inline VB6StrPtr EraseBackground(int16_t A, int16_t noEffects=0) { return _EraseBackground_ptr(A, noEffects); }
        static const auto _AddBackground_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B10580);
        inline VB6StrPtr AddBackground(int16_t A) { return _AddBackground_ptr(A); }
        static const auto _AddNPC_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B10890);
        inline VB6StrPtr AddNPC(int16_t A) { return _AddNPC_ptr(A); }
        static const auto _AddWater_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B113E0);
        inline VB6StrPtr AddWater(int16_t A) { return _AddWater_ptr(A); }
        static const auto _AddWarp_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B11740);
        inline VB6StrPtr AddWarp(int16_t A) { return _AddWarp_ptr(A); }
        static const auto _ModSection_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B121B0);
        inline VB6StrPtr ModSection(int16_t A) { return _ModSection_ptr(A); }
        static const auto _ModLayer_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B12420);
        inline VB6StrPtr ModLayer(int16_t A) { return _ModLayer_ptr(A); }
        static const auto _ModEvent_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)(int16_t& A)>(0x00B12690);
        inline VB6StrPtr ModEvent(int16_t A) { return _ModEvent_ptr(A); }
        static const auto _SyncNum_ptr = reinterpret_cast<VB6StrPtr(__stdcall *)()>(0x00B137F0);
        inline VB6StrPtr SyncNum() { return _SyncNum_ptr(); }
        static const auto _PutPlayerControls_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00B13980);
        inline void PutPlayerControls(int16_t A) { _PutPlayerControls_ptr(A); }
        static const auto _PutPlayerLoc_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A)>(0x00B145C0);
        inline void PutPlayerLoc(int16_t A) { _PutPlayerLoc_ptr(A); }
        static const auto _GetPlayerControls_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B14980);
        inline void GetPlayerControls(VB6StrPtr newStr) { _GetPlayerControls_ptr(newStr); }
        static const auto _GetPlayerLoc_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B15680);
        inline void GetPlayerLoc(VB6StrPtr newStr) { _GetPlayerLoc_ptr(newStr); }
        static const auto _InitSync_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B15B20);
        inline void InitSync(int16_t Index) { _InitSync_ptr(Index); }
    }

    namespace Netplay_Private {
        static const auto _tmrCheckPorts_Timer_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B17480);
        inline void tmrCheckPorts_Timer() { _tmrCheckPorts_Timer_ptr(); }
        static const auto _tmrConnect_Timer_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B17890);
        inline void tmrConnect_Timer() { _tmrConnect_Timer_ptr(); }
        static const auto _tmrPing_Timer_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B17970);
        inline void tmrPing_Timer() { _tmrPing_Timer_ptr(); }
        static const auto _tmrPingC_Timer_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B17AA0);
        inline void tmrPingC_Timer() { _tmrPingC_Timer_ptr(); }
        static const auto _tmrPort_Timer_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B18270);
        inline void tmrPort_Timer() { _tmrPort_Timer_ptr(); }
        static const auto _tmrTimeout_Timer_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B18500);
        inline void tmrTimeout_Timer(int16_t Index) { _tmrTimeout_Timer_ptr(Index); }
        static const auto _UpdateEvents_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& newStr)>(0x00B18980);
        inline void UpdateEvents(VB6StrPtr newStr) { _UpdateEvents_ptr(newStr); }
    }

    namespace Netplay {
        static const auto _DropClient_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B1A060);
        inline void DropClient(int16_t Index) { _DropClient_ptr(Index); }
        static const auto _DropServer_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B1AD90);
        inline void DropServer() { _DropServer_ptr(); }
    }

    namespace Netplay_Private {
        static const auto _tmrTimeoutC_Timer_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B1B430);
        inline void tmrTimeoutC_Timer() { _tmrTimeoutC_Timer_ptr(); }
    }

    namespace frmNetplay_Private {
        static const auto _cmdClearLevel_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B1B6F0);
        inline void cmdClearLevel_Click() { _cmdClearLevel_Click_ptr(); }
        static const auto _cmdGo_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B1B960);
        inline void cmdGo_Click() { _cmdGo_Click_ptr(); }
        static const auto _cPassword_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B1D740);
        inline void cPassword_Change() { _cPassword_Change_ptr(); }
        static const auto _Cursor_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B1D820);
        inline void Cursor_Click(int16_t Index) { _Cursor_Click_ptr(Index); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00B1D9A0);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _optMode_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B1DA30);
        inline void optMode_Click(int16_t Index) { _optMode_Click_ptr(Index); }
        static const auto _optPlayer_Click_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Index)>(0x00B1DC20);
        inline void optPlayer_Click(int16_t Index) { _optPlayer_Click_ptr(Index); }
        static const auto _sPassword_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B1DD90);
        inline void sPassword_Change() { _sPassword_Change_ptr(); }
        static const auto _txtNick_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B1DE70);
        inline void txtNick_Change() { _txtNick_Change_ptr(); }
    }

    namespace frmChat_Private {
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00B1E510);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _txtSay_KeyPress_ptr = reinterpret_cast<void(__stdcall *)(int16_t& KeyAscii)>(0x00B1E5A0);
        inline void txtSay_KeyPress(int16_t KeyAscii) { _txtSay_KeyPress_ptr(KeyAscii); }
    }

    namespace frmLoading_Private {
        static const auto _cmdGo_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B1F790);
        inline void cmdGo_Click() { _cmdGo_Click_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00B1FAE0);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
        static const auto _tmrLoad_Timer_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B1FDF0);
        inline void tmrLoad_Timer() { _tmrLoad_Timer_ptr(); }
    }

    namespace frmLoader_Private {
        static const auto _cmdEditor_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B20000);
        inline void cmdEditor_Click() { _cmdEditor_Click_ptr(); }
        static const auto _cmdExit_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B20080);
        inline void cmdExit_Click() { _cmdExit_Click_ptr(); }
        static const auto _cmdGame_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B200F0);
        inline void cmdGame_Click() { _cmdGame_Click_ptr(); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B20160);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _Form_Unload_ptr = reinterpret_cast<void(__stdcall *)(int16_t& Cancel)>(0x00B20230);
        inline void Form_Unload(int16_t Cancel) { _Form_Unload_ptr(Cancel); }
    }

    namespace modCustom {
        static const auto _SaveNPCDefaults_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B202B0);
        inline void SaveNPCDefaults() { _SaveNPCDefaults_ptr(); }
        static const auto _LoadNPCDefaults_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B20870);
        inline void LoadNPCDefaults() { _LoadNPCDefaults_ptr(); }
        static const auto _FindCustomNPCs_ptr = reinterpret_cast<void(__stdcall *)(VB6StrPtr& cFilePath)>(0x00B20E50);
        inline void FindCustomNPCs(VB6StrPtr cFilePath=L"") { _FindCustomNPCs_ptr(cFilePath); }
    }

    namespace modCustom_Private {
        static const auto _LoadCustomNPC_ptr = reinterpret_cast<void(__stdcall *)(int16_t& A, VB6StrPtr& cFileName)>(0x00B21200);
        inline void LoadCustomNPC(int16_t A, VB6StrPtr cFileName) { _LoadCustomNPC_ptr(A, cFileName); }
    }

    namespace frmLevelAdv {
        static const auto _RefreshFields_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B22900);
        inline void RefreshFields() { _RefreshFields_ptr(); }
    }

    namespace frmLevelAdv_Private {
        static const auto _cmdClone_Click_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B229C0);
        inline void cmdClone_Click() { _cmdClone_Click_ptr(); }
        static const auto _Form_Load_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B22B20);
        inline void Form_Load() { _Form_Load_ptr(); }
        static const auto _txtLevelName_Change_ptr = reinterpret_cast<void(__stdcall *)()>(0x00B22DB0);
        inline void txtLevelName_Change() { _txtLevelName_Change_ptr(); }
    }

    namespace frmLevelAdv {
        static const auto _CloneSection_ptr = reinterpret_cast<void(__stdcall *)(int16_t& SectionS, int16_t& SectionT)>(0x00B22E90);
        inline void CloneSection(int16_t SectionS, int16_t SectionT) { _CloneSection_ptr(SectionS, SectionT); }
    }

    namespace frmLevelAdv_Private {
        static const auto _CloneLoc_ptr = reinterpret_cast<VB6Bool(__stdcall *)(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2)>(0x00B23F40);
        inline VB6Bool CloneLoc(SMBX13::Types::Location_t& Loc1, SMBX13::Types::Location_t& Loc2) { return _CloneLoc_ptr(Loc1, Loc2); }
    }

    // All public functions
    namespace Functions {
        using namespace modMain;
        using namespace modGraphics;
        using namespace modCollision;
        using namespace modPlayer;
        using namespace modBlocks;
        using namespace modEffect;
        using namespace modEditor;
        using namespace modNPC;
        using namespace modSound;
        using namespace modJoystick;
        using namespace frmBlocks;
        using namespace frmNPCs;
        using namespace modChangeRes;
        using namespace modSorting;
        using namespace frmLayers;
        using namespace modLayers;
        using namespace frmEvents;
        using namespace modLoadGFX;
        using namespace frmLevelDebugger;
        using namespace frmGenerator;
        using namespace Netplay;
        using namespace modCustom;
        using namespace frmLevelAdv;
    }
}

#endif // !defined(SMBXINTERNAL_FUNCTIONS_H)

