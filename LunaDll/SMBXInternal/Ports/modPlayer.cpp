#include "../Types.h"
#include "../Variables.h"
#include "../Functions.h"
#include "../Ports.h"
#include "../../Misc/VB6RNG.h"
#include "../../Misc/VB6Logic.h"
#include "../../Misc/RuntimeHookComponents/CharacterIdExtension.h"

// Forward declare a hook we use, don't want the whole header
void __stdcall runtimeHookWarpPipeDoorInternal(short* playerIdx);

// Fix enablement
bool SMBX13::Ports::_enablePowerupPowerdownPositionFixes = true;

// Function to update player location based on a change in state
static void UpdatePlayerPositionForStateChange(SMBX13::Types::Player_t& _)
{
    using namespace SMBX13::Vars;
    int16_t BaseCharacter = CharacterIdTranslate(_.Character);
    int16_t newState = _.State;
    double oldWidth = _.Location.Width;
    double newWidth = Physics.PlayerWidth[newState][BaseCharacter];
    _.Location.X = ((_.Location.X - (newWidth * 0.5)) + (_.Location.Width * 0.5));
    _.Location.Width = newWidth;
    if (newWidth > oldWidth)
    {
        // TODO: Consider if we want block collision here to avoid some more glitches?
    }
    if (_.Duck) {
        _.Location.Y = ((_.Location.Y - Physics.PlayerDuckHeight[newState][BaseCharacter]) + _.Location.Height);
        _.Location.Height = Physics.PlayerDuckHeight[newState][BaseCharacter];
    } else {
        _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[newState][BaseCharacter]) + _.Location.Height);
        _.Location.Height = Physics.PlayerHeight[newState][BaseCharacter];
    }
}

// This is an automatically translated copy of PlayerEffects(A) from modPlayer.bas
// It contains few manual alterations or edits. It currently has been edited to:
//  1) Account for character ID extension
//  2) Include the required hooks for "onWarp" events
void __stdcall SMBX13::Ports::PlayerEffects(int16_t& A) {
    using namespace SMBX13::Types;
    using namespace SMBX13::Vars;
    using namespace SMBX13::Functions;
    #pragma warning( push )
    #pragma warning( disable: 4244 ) // Disable loss of precision warning
    int16_t B = 0;
    float C = 0.0f;
    float D = 0.0f;
    VB6Bool tempBool = false;
    Location_t tempLocation = {};
    {
        auto& _ = Player[A];
        if ((_.Effect != 8) && (_.Fairy == true)) {
            _.Fairy = false;
            SizeCheck(A);
        }
        _.TailCount = 0;
        _.Pinched1 = 0;
        _.Pinched2 = 0;
        _.Pinched3 = 0;
        _.Pinched4 = 0;
        _.NPCPinched = 0;
        _.SwordPoke = 0;
        if ((_.YoshiBlue == false) && (_.Effect != 500)) {
            _.CanFly = false;
            _.CanFly2 = false;
            _.RunCount = 0;
        }
        _.Immune2 = false;
        int16_t BaseCharacter = CharacterIdTranslate(_.Character);
        // Player growing effect
        if (_.Effect == 1) {
            _.Frame = 1;
            _.Effect2 = (_.Effect2 + 1);
            if ((_.Effect2 / 5) == ::floor((_.Effect2 / 5))) {
                if (_.State == 1) {
                    _.State = 2;
                    if (_.Mount == 0) {
                        if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                            UpdatePlayerPositionForStateChange(_);
                        }
                        else {
                            _.Location.X = ((_.Location.X - (Physics.PlayerWidth[2][BaseCharacter] * 0.5)) + (Physics.PlayerWidth[1][BaseCharacter] * 0.5));
                            _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][BaseCharacter]) + Physics.PlayerHeight[1][BaseCharacter]);
                            _.Location.Width = Physics.PlayerWidth[_.State][BaseCharacter];
                            _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                        }
                    }
                    else if (_.Mount == 3) {
                        YoshiHeight(A);
                    }
                    else if ((BaseCharacter == 2) && (_.Mount != 2)) {
                        if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                            UpdatePlayerPositionForStateChange(_);
                        }
                        else {
                            _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][2]) + Physics.PlayerHeight[2][1]);
                            _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                        }
                    }
                }
                else {
                    _.State = 1;
                    if (_.Mount == 0) {
                        if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                            UpdatePlayerPositionForStateChange(_);
                        }
                        else {
                            _.Location.X = ((_.Location.X - (Physics.PlayerWidth[1][BaseCharacter] * 0.5)) + (Physics.PlayerWidth[2][BaseCharacter] * 0.5));
                            _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[1][BaseCharacter]) + Physics.PlayerHeight[2][BaseCharacter]);
                            _.Location.Width = Physics.PlayerWidth[_.State][BaseCharacter];
                            _.Location.Height = Physics.PlayerHeight[1][BaseCharacter];
                        }
                    }
                    else if (_.Mount == 3) {
                        YoshiHeight(A);
                    }
                    else if ((BaseCharacter == 2) && (_.Mount != 2)) {
                        if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                            UpdatePlayerPositionForStateChange(_);
                        }
                        else {
                            _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][1]) + Physics.PlayerHeight[2][2]);
                            _.Location.Height = Physics.PlayerHeight[2][1];
                        }
                    }
                }
            }
            if ((_.Effect2 >= 50) && (_.State == 2)) {
                _.Immune = (_.Immune + 50);
                _.Immune2 = true;
                _.Effect = 0;
                _.Effect2 = 0;
                _.StandUp = true;
            }
        }
        // Player shrinking effect
        else if (_.Effect == 2) {
            if (_.Duck == true) {
                // Fixes a block collision bug
                _.StandUp = true;
                _.Duck = false;
                _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[_.State][BaseCharacter]) + Physics.PlayerDuckHeight[_.State][BaseCharacter]);
            }
            _.Frame = 1;
            _.Effect2 = (_.Effect2 + 1);
            if ((_.Effect2 / 5) == ::floor((_.Effect2 / 5))) {
                if (_.State == 1) {
                    _.State = 2;
                    if (_.Mount == 3) {
                        YoshiHeight(A);
                    }
                    else if (!(_.Mount == 2)) {
                        if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                            UpdatePlayerPositionForStateChange(_);
                        }
                        else {
                            _.Location.X = ((_.Location.X - (Physics.PlayerWidth[2][BaseCharacter] * 0.5)) + (Physics.PlayerWidth[1][BaseCharacter] * 0.5));
                            _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][BaseCharacter]) + Physics.PlayerHeight[1][BaseCharacter]);
                            _.Location.Width = Physics.PlayerWidth[_.State][BaseCharacter];
                            _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                        }
                    }
                }
                else {
                    _.State = 1;
                    if (_.Mount == 3) {
                        YoshiHeight(A);
                    }
                    else if (!(_.Mount == 2)) {
                        if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                            UpdatePlayerPositionForStateChange(_);
                        }
                        else {
                            _.Location.X = ((_.Location.X - (Physics.PlayerWidth[1][BaseCharacter] * 0.5)) + (Physics.PlayerWidth[2][BaseCharacter] * 0.5));
                            _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[1][BaseCharacter]) + Physics.PlayerHeight[2][BaseCharacter]);
                            _.Location.Width = Physics.PlayerWidth[_.State][BaseCharacter];
                            _.Location.Height = Physics.PlayerHeight[1][BaseCharacter];
                        }
                    }
                }
            }
            if (_.Effect2 >= 50) {
                if (_.State == 2) {
                    _.State = 1;
                    if (!(_.Mount == 2)) {
                        if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                            UpdatePlayerPositionForStateChange(_);
                        }
                        else {
                            _.Location.X = ((_.Location.X - (Physics.PlayerWidth[1][BaseCharacter] * 0.5)) + (Physics.PlayerWidth[2][BaseCharacter] * 0.5));
                            _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[1][BaseCharacter]) + Physics.PlayerHeight[2][BaseCharacter]);
                            _.Location.Width = Physics.PlayerWidth[_.State][BaseCharacter];
                            _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                        }
                    }
                }
                _.Immune = 150;
                _.Immune2 = true;
                _.Effect = 0;
                _.Effect2 = 0;
            }
        }
        // Player losing firepower
        else if (_.Effect == 227) {
            if (_.Duck == true) {
                // Fixes a block collision bug
                _.StandUp = true;
                _.Duck = false;
                _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[_.State][BaseCharacter]) + Physics.PlayerDuckHeight[_.State][BaseCharacter]);
            }
            _.Frame = 1;
            _.Effect2 = (_.Effect2 + 1);
            if ((_.Effect2 / 5) == ::floor((_.Effect2 / 5))) {
                if (_.State == 2) {
                    _.State = 3;
                }
                else {
                    _.State = 2;
                }
            }
            if (_.Effect2 >= 50) {
                if (_.State == 3) {
                    _.State = 2;
                }
                _.Immune = 150;
                _.Immune2 = true;
                _.Effect = 0;
                _.Effect2 = 0;
            }
        }
        // Player losing icepower
        else if (_.Effect == 228) {
            if (_.Duck == true) {
                // Fixes a block collision bug
                _.StandUp = true;
                _.Duck = false;
                _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[_.State][BaseCharacter]) + Physics.PlayerDuckHeight[_.State][BaseCharacter]);
            }
            _.Frame = 1;
            _.Effect2 = (_.Effect2 + 1);
            if ((_.Effect2 / 5) == ::floor((_.Effect2 / 5))) {
                if (_.State == 2) {
                    _.State = 7;
                }
                else {
                    _.State = 2;
                }
            }
            if (_.Effect2 >= 50) {
                if (_.State == 7) {
                    _.State = 2;
                }
                _.Immune = 150;
                _.Immune2 = true;
                _.Effect = 0;
                _.Effect2 = 0;
            }
        }
        // Warp effect
        else if (_.Effect == 3) {
            _.SpinJump = false;
            _.TailCount = 0;
            _.Location.SpeedY = 0;
            if (_.Effect2 == 0) {
                if (Warp[_.Warp].Direction == 3) {
                    _.Location.Y = (_.Location.Y + 1);
                    _.Location.X = ((Warp[_.Warp].Entrance.X + (Warp[_.Warp].Entrance.Width / 2)) - (_.Location.Width / 2));
                    if (_.Location.Y > ((Warp[_.Warp].Entrance.Y + Warp[_.Warp].Entrance.Height) + 8)) {
                        _.Effect2 = 1;
                    }
                    if (_.Mount == 0) { _.Frame = 15; }
                    if (_.HoldingNPC > 0) {
                        NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                        NPC[_.HoldingNPC].Location.X = ((_.Location.X + (_.Location.Width / 2)) - (NPC[_.HoldingNPC].Location.Width / 2));
                    }
                }
                else if (Warp[_.Warp].Direction == 1) {
                    _.Location.Y = (_.Location.Y - 1);
                    _.Location.X = ((Warp[_.Warp].Entrance.X + (Warp[_.Warp].Entrance.Width / 2)) - (_.Location.Width / 2));
                    if (((_.Location.Y + _.Location.Height) + 8) < Warp[_.Warp].Entrance.Y) {
                        _.Effect2 = 1;
                    }
                    if (_.HoldingNPC > 0) {
                        NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                        NPC[_.HoldingNPC].Location.X = ((_.Location.X + (_.Location.Width / 2)) - (NPC[_.HoldingNPC].Location.Width / 2));
                    }
                    if (_.Mount == 0) { _.Frame = 15; }
                }
                else if (Warp[_.Warp].Direction == 2) {
                    if (_.Mount == 3) {
                        _.Duck = true;
                        _.Location.Height = 30;
                    }
                    _.Direction = -1;
                    _.Location.Y = (((Warp[_.Warp].Entrance.Y + Warp[_.Warp].Entrance.Height) - _.Location.Height) - 2);
                    _.Location.X = (_.Location.X - 0.5);
                    if (((_.Location.X + _.Location.Width) + 8) < Warp[_.Warp].Entrance.X) {
                        _.Effect2 = 1;
                    }
                    if (_.HoldingNPC > 0) {
                        NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                        if (_.Direction > 0) {
                            NPC[_.HoldingNPC].Location.X = (_.Location.X + Physics.PlayerGrabSpotX[_.State][BaseCharacter]);
                        }
                        else {
                            NPC[_.HoldingNPC].Location.X = (((_.Location.X + _.Location.Width) - Physics.PlayerGrabSpotX[_.State][BaseCharacter]) - NPC[_.HoldingNPC].Location.Width);
                        }
                    }
                    _.Location.SpeedX = -0.5;
                    PlayerFrame(A);
                    _.Location.SpeedX = 0;
                }
                else if (Warp[_.Warp].Direction == 4) {
                    if (_.Mount == 3) {
                        _.Duck = true;
                        _.Location.Height = 30;
                    }
                    _.Direction = 1;
                    _.Location.Y = (((Warp[_.Warp].Entrance.Y + Warp[_.Warp].Entrance.Height) - _.Location.Height) - 2);
                    _.Location.X = (_.Location.X + 0.5);
                    if (_.Location.X > ((Warp[_.Warp].Entrance.X + Warp[_.Warp].Entrance.Width) + 8)) {
                        _.Effect2 = 1;
                    }
                    if (_.HoldingNPC > 0) {
                        NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                        if (_.Direction > 0) {
                            NPC[_.HoldingNPC].Location.X = (_.Location.X + Physics.PlayerGrabSpotX[_.State][BaseCharacter]);
                        }
                        else {
                            NPC[_.HoldingNPC].Location.X = (((_.Location.X + _.Location.Width) - Physics.PlayerGrabSpotX[_.State][BaseCharacter]) - NPC[_.HoldingNPC].Location.Width);
                        }
                    }
                    _.Location.SpeedX = 0.5;
                    PlayerFrame(A);
                    _.Location.SpeedX = 0;
                }
            }
            else if (_.Effect2 == 1) {
                if (Warp[_.Warp].NoYoshi == true) {
                    if (((OwedMount[A] == 0) && (_.Mount > 0)) && (_.Mount != 2)) {
                        OwedMount[A] = _.Mount;
                        OwedMountType[A] = _.MountType;
                    }
                    UnDuck(A);
                    _.Mount = 0;
                    _.MountType = 0;
                    _.MountOffsetY = 0;
                    SizeCheck(A);
                }
                if (Warp[_.Warp].Direction2 == 1) {
                    _.Location.X = ((Warp[_.Warp].Exit.X + (Warp[_.Warp].Exit.Width / 2)) - (_.Location.Width / 2));
                    _.Location.Y = ((Warp[_.Warp].Exit.Y - _.Location.Height) - 8);
                    if (_.Mount == 0) { _.Frame = 15; }
                    if (_.HoldingNPC > 0) {
                        NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                        NPC[_.HoldingNPC].Location.X = ((_.Location.X + (_.Location.Width / 2)) - (NPC[_.HoldingNPC].Location.Width / 2));
                    }
                }
                else if (Warp[_.Warp].Direction2 == 3) {
                    _.Location.X = ((Warp[_.Warp].Exit.X + (Warp[_.Warp].Exit.Width / 2)) - (_.Location.Width / 2));
                    _.Location.Y = ((Warp[_.Warp].Exit.Y + Warp[_.Warp].Exit.Height) + 8);
                    if (_.Mount == 0) { _.Frame = 15; }
                    if (_.HoldingNPC > 0) {
                        NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                        NPC[_.HoldingNPC].Location.X = ((_.Location.X + (_.Location.Width / 2)) - (NPC[_.HoldingNPC].Location.Width / 2));
                    }
                }
                else if (Warp[_.Warp].Direction2 == 2) {
                    if (_.Mount == 3) {
                        _.Duck = true;
                        _.Location.Height = 30;
                    }
                    _.Location.X = ((Warp[_.Warp].Exit.X - _.Location.Width) - 8);
                    _.Location.Y = (((Warp[_.Warp].Exit.Y + Warp[_.Warp].Exit.Height) - _.Location.Height) - 2);
                    if (_.Mount == 0) { _.Frame = 1; }
                    _.Direction = 1;
                    if (_.HoldingNPC > 0) {
                        if (_.State == 1) {
                            _.Frame = 5;
                        }
                        else {
                            _.Frame = 8;
                        }
                        NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                        _.Direction = -1;
                        if (_.Direction > 0) {
                            NPC[_.HoldingNPC].Location.X = (_.Location.X + Physics.PlayerGrabSpotX[_.State][BaseCharacter]);
                        }
                        else {
                            NPC[_.HoldingNPC].Location.X = (((_.Location.X + _.Location.Width) - Physics.PlayerGrabSpotX[_.State][BaseCharacter]) - NPC[_.HoldingNPC].Location.Width);
                        }
                    }
                }
                else if (Warp[_.Warp].Direction2 == 4) {
                    if (_.Mount == 3) {
                        _.Duck = true;
                        _.Location.Height = 30;
                    }
                    _.Location.X = ((Warp[_.Warp].Exit.X + Warp[_.Warp].Exit.Width) + 8);
                    _.Location.Y = (((Warp[_.Warp].Exit.Y + Warp[_.Warp].Exit.Height) - _.Location.Height) - 2);
                    if (_.Mount == 0) { _.Frame = 1; }
                    _.Direction = -1;
                    if (_.HoldingNPC > 0) {
                        if (_.State == 1) {
                            _.Frame = 5;
                        }
                        else {
                            _.Frame = 8;
                        }
                        _.Direction = 1;
                        NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                        if (_.Direction > 0) {
                            NPC[_.HoldingNPC].Location.X = (_.Location.X + Physics.PlayerGrabSpotX[_.State][BaseCharacter]);
                        }
                        else {
                            NPC[_.HoldingNPC].Location.X = (((_.Location.X + _.Location.Width) - Physics.PlayerGrabSpotX[_.State][BaseCharacter]) - NPC[_.HoldingNPC].Location.Width);
                        }
                    }
                }
                _.Effect2 = 100;
                if (_.Duck == true) {
                    if ((Warp[_.Warp].Direction2 == 1) || (Warp[_.Warp].Direction2 == 3)) {
                        UnDuck(A);
                    }
                }
                CheckSection(A);
                if (_.HoldingNPC > 0) {
                    CheckSectionNPC(_.HoldingNPC);
                }
                if ((numPlayers > 2) && (nPlay.Online == false)) {
                    for (B = 1; B <= numPlayers; B++) {
                        if (B != A) {
                            if (Warp[_.Warp].Direction2 != 3) {
                                Player[B].Location.Y = ((Player[A].Location.Y + Player[A].Location.Height) - Player[B].Location.Height);
                            }
                            else {
                                Player[B].Location.Y = Player[A].Location.Y;
                            }
                            Player[B].Location.X = ((Player[A].Location.X + (Player[A].Location.Width / 2)) - (Player[B].Location.Width / 2));
                            Player[B].Location.SpeedY = ((VB6RNG::generateNumber() * 24) - 12);
                            Player[B].Effect = 8;
                            Player[B].Effect2 = 0;
                            CheckSection(B);
                            if (Player[B].HoldingNPC > 0) {
                                CheckSectionNPC(Player[B].HoldingNPC);
                            }
                        }
                    }
                }
                if (Warp[_.Warp].level != L"") {
                    GoToLevel = Warp[_.Warp].level;
                    _.Effect = 8;
                    _.Effect2 = 2970;
                    ReturnWarp = _.Warp;
                    StartWarp = Warp[_.Warp].LevelWarp;
                }
                else if (Warp[_.Warp].MapWarp == true) {
                    _.Effect = 8;
                    _.Effect2 = 2970;
                }
                runtimeHookWarpPipeDoorInternal(&A); // Hook for onWarp event (was at 0x9D55CD, 0x9D55F1 and 0x9D5614 )
            }
            else if (_.Effect2 >= 100) {
                _.Effect2 = (_.Effect2 + 1);
                if (_.Effect2 >= 110) {
                    _.Effect2 = 2;
                    PlaySound(17);
                }
            }
            else if (_.Effect2 == 2) {
                if (Warp[_.Warp].Direction2 == 1) {
                    _.Location.Y = (_.Location.Y + 1);
                    if (_.Location.Y >= Warp[_.Warp].Exit.Y) {
                        _.Effect2 = 3;
                    }
                    if (_.HoldingNPC > 0) {
                        NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                        NPC[_.HoldingNPC].Location.X = ((_.Location.X + (_.Location.Width / 2)) - (NPC[_.HoldingNPC].Location.Width / 2));
                    }
                    if (_.Mount == 0) { _.Frame = 15; }
                }
                else if (Warp[_.Warp].Direction2 == 3) {
                    _.Location.Y = (_.Location.Y - 1);
                    if ((_.Location.Y + _.Location.Height) <= (Warp[_.Warp].Exit.Y + Warp[_.Warp].Exit.Height)) {
                        _.Effect2 = 3;
                    }
                    if (_.HoldingNPC > 0) {
                        NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                        NPC[_.HoldingNPC].Location.X = ((_.Location.X + (_.Location.Width / 2)) - (NPC[_.HoldingNPC].Location.Width / 2));
                    }
                    if (_.Mount == 0) { _.Frame = 15; }
                }
                else if (Warp[_.Warp].Direction2 == 4) {
                    _.Location.X = (_.Location.X - 0.5);
                    _.Direction = -1;
                    if ((_.Location.X + _.Location.Width) <= (Warp[_.Warp].Exit.X + Warp[_.Warp].Exit.Width)) {
                        _.Effect2 = 3;
                    }
                    if (_.HoldingNPC > 0) {
                        // peach/toad leaving a pipe
                        if (BaseCharacter >= 3) {
                            _.Location.SpeedX = 1;
                            PlayerFrame(A);
                            NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                            if (_.Direction < 0) {
                                NPC[_.HoldingNPC].Location.X = (_.Location.X + Physics.PlayerGrabSpotX[_.State][BaseCharacter]);
                            }
                            else {
                                NPC[_.HoldingNPC].Location.X = (((_.Location.X + _.Location.Width) - Physics.PlayerGrabSpotX[_.State][BaseCharacter]) - NPC[_.HoldingNPC].Location.Width);
                            }
                        }
                        else {
                            _.Direction = 1;
                            if (_.State == 1) {
                                _.Frame = 5;
                            }
                            else {
                                _.Frame = 8;
                            }
                            NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                            if (_.Direction > 0) {
                                NPC[_.HoldingNPC].Location.X = (_.Location.X + Physics.PlayerGrabSpotX[_.State][BaseCharacter]);
                            }
                            else {
                                NPC[_.HoldingNPC].Location.X = (((_.Location.X + _.Location.Width) - Physics.PlayerGrabSpotX[_.State][BaseCharacter]) - NPC[_.HoldingNPC].Location.Width);
                            }
                        }
                    }
                    else {
                        _.Location.SpeedX = -0.5;
                        PlayerFrame(A);
                        _.Location.SpeedX = 0;
                    }
                }
                else if (Warp[_.Warp].Direction2 == 2) {
                    _.Location.X = (_.Location.X + 0.5);
                    _.Direction = 1;
                    if (_.Location.X >= Warp[_.Warp].Exit.X) {
                        _.Effect2 = 3;
                    }
                    if (_.HoldingNPC > 0) {
                        // peach/toad leaving a pipe
                        if (BaseCharacter >= 3) {
                            _.Location.SpeedX = 1;
                            PlayerFrame(A);
                            NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                            if (_.Direction < 0) {
                                NPC[_.HoldingNPC].Location.X = (_.Location.X + Physics.PlayerGrabSpotX[_.State][BaseCharacter]);
                            }
                            else {
                                NPC[_.HoldingNPC].Location.X = (((_.Location.X + _.Location.Width) - Physics.PlayerGrabSpotX[_.State][BaseCharacter]) - NPC[_.HoldingNPC].Location.Width);
                            }
                        }
                        else {
                            _.Direction = -1;
                            if (_.State == 1) {
                                _.Frame = 5;
                            }
                            else {
                                _.Frame = 8;
                            }
                            NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                            if (_.Direction > 0) {
                                NPC[_.HoldingNPC].Location.X = (_.Location.X + Physics.PlayerGrabSpotX[_.State][BaseCharacter]);
                            }
                            else {
                                NPC[_.HoldingNPC].Location.X = (((_.Location.X + _.Location.Width) - Physics.PlayerGrabSpotX[_.State][BaseCharacter]) - NPC[_.HoldingNPC].Location.Width);
                            }
                        }
                    }
                    else {
                        _.Location.SpeedX = -0.5;
                        PlayerFrame(A);
                        _.Location.SpeedX = 0;
                    }
                }
            }
            else if (_.Effect2 == 3) {
                if (_.HoldingNPC > 0) {
                    if ((Warp[_.Warp].Direction2 == 2) || (Warp[_.Warp].Direction2 == 4)) {
                        if (Warp[_.Warp].Direction2 == 2) {
                            _.Direction = 1;
                        }
                        else if (Warp[_.Warp].Direction2 == 4) {
                            _.Direction = -1;
                        }
                        if (_.State == 1) {
                            _.Frame = 5;
                        }
                        else {
                            _.Frame = 8;
                        }
                        if (_.Controls.Run == false) { _.Controls.Run = true; }
                        PlayerGrabCode(A);
                    }
                }
                _.Effect = 0;
                _.Effect2 = 0;
                _.WarpCD = 20;
                _.Location.SpeedY = 0;
                _.CanJump = false;
                _.CanAltJump = false;
                _.Location.SpeedX = 0;
                _.Bumped2 = 0;
                if (_.HoldingNPC > 0) {
                    NPC[_.HoldingNPC].Effect = 0;
                }
                if ((numPlayers > 2) && (nPlay.Online == false)) {
                    for (B = 1; B <= numPlayers; B++) {
                        if (B != A) {
                            if (Warp[_.Warp].Direction2 != 1) {
                                Player[B].Location.Y = ((Player[A].Location.Y + Player[A].Location.Height) - Player[B].Location.Height);
                            }
                            else {
                                Player[B].Location.Y = Player[A].Location.Y;
                            }
                            Player[B].Location.X = ((Player[A].Location.X + (Player[A].Location.Width / 2)) - (Player[B].Location.Width / 2));
                            Player[B].Location.SpeedY = ((VB6RNG::generateNumber() * 24) - 12);
                            Player[B].Effect = 0;
                            Player[B].Effect2 = 0;
                            CheckSection(B);
                        }
                    }
                }
            }
        }
        // Door effect
        else if (_.Effect == 7) {
            if (_.HoldingNPC > 0) {
                NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                NPC[_.HoldingNPC].Location.X = ((_.Location.X + (_.Location.Width / 2)) - (NPC[_.HoldingNPC].Location.Width / 2));
            }
            _.Effect2 = (_.Effect2 + 1);
            if ((_.Mount == 0) && (BaseCharacter != 5)) { _.Frame = 13; }
            if (BaseCharacter == 5) { _.Frame = 1; }
            if (_.Effect2 >= 30) {
                if (Warp[_.Warp].NoYoshi == true) {
                    if (((OwedMount[A] == 0) && (_.Mount > 0)) && (_.Mount != 2)) {
                        OwedMount[A] = _.Mount;
                        OwedMountType[A] = _.MountType;
                    }
                    _.Mount = 0;
                    _.MountType = 0;
                    SizeCheck(A);
                    _.MountOffsetY = 0;
                    _.Frame = 1;
                }
                _.Location.X = ((Warp[_.Warp].Exit.X + (Warp[_.Warp].Exit.Width / 2)) - (_.Location.Width / 2));
                _.Location.Y = ((Warp[_.Warp].Exit.Y + Warp[_.Warp].Exit.Height) - _.Location.Height);
                CheckSection(A);
                if (_.HoldingNPC > 0) {
                    if (_.Controls.Run == false) { _.Controls.Run = true; }
                    PlayerGrabCode(A);
                }
                _.Effect = 0;
                _.Effect2 = 0;
                _.WarpCD = 40;
                if (Warp[_.Warp].level != L"") {
                    GoToLevel = Warp[_.Warp].level;
                    _.Effect = 8;
                    _.Effect2 = 3000;
                    ReturnWarp = _.Warp;
                    StartWarp = Warp[_.Warp].LevelWarp;
                }
                else if (Warp[_.Warp].MapWarp == true) {
                    _.Effect = 8;
                    _.Effect2 = 2970;
                }
                runtimeHookWarpPipeDoorInternal(&A); // Hook for onWarp event (was at 0x9D7037)
                if ((numPlayers > 2) && (nPlay.Online == false)) {
                    for (B = 1; B <= numPlayers; B++) {
                        if (B != A) {
                            Player[B].Location.Y = ((Player[A].Location.Y + Player[A].Location.Height) - Player[B].Location.Height);
                            Player[B].Location.X = ((Player[A].Location.X + (Player[A].Location.Width / 2)) - (Player[B].Location.Width / 2));
                            Player[B].Location.SpeedY = ((VB6RNG::generateNumber() * 24) - 12);
                            CheckSection(B);
                            if (Player[B].HoldingNPC > 0) {
                                int16_t PlayerB_BaseCharacter = CharacterIdTranslate(Player[B].Character);
                                if (Player[B].Direction > 0) {
                                    NPC[Player[B].HoldingNPC].Location.X = (Player[B].Location.X + Physics.PlayerGrabSpotX[Player[B].State][PlayerB_BaseCharacter]);
                                }
                                else {
                                    NPC[Player[B].HoldingNPC].Location.X = (((Player[B].Location.X + Player[B].Location.Width) - Physics.PlayerGrabSpotX[Player[B].State][PlayerB_BaseCharacter]) - NPC[_.HoldingNPC].Location.Width);
                                }
                                BaseCharacter = CharacterIdTranslate(_.Character); // Update just in case
                                NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
                                NPC[Player[B].HoldingNPC].Section = Player[B].Section;
                            }
                        }
                    }
                }
            }
        }
        // Holding Pattern
        else if (_.Effect == 8) {
            if (_.Effect2 < 0) {
                _.Location.X = Player[-_.Effect2].Location.X;
                _.Location.Y = Player[-_.Effect2].Location.Y;
                if (Player[-_.Effect2].Dead == true) { _.Dead = true; }
            }
            else if (_.Effect2 <= 30) {
                _.Effect2 = (_.Effect2 - 1);
                if (_.Effect2 == 0) {
                    _.Effect = 0;
                    _.Effect2 = 0;
                }
            }
            else if (_.Effect2 == 131) {
                tempBool = false;
                for (B = 1; B <= numPlayers; B++) {
                    if (VBAnd((B != A), CheckCollision(_.Location, Player[B].Location))) {
                        tempBool = true;
                    }
                }
                if (tempBool == false) {
                    _.Effect2 = 130;
                    for (C = 1; C <= numBackground; C++) {
                        if (CheckCollision(Warp[_.Warp].Exit, Background[C].Location)) {
                            if (Background[C].Type == 88) {
                                NewEffect(54, Background[C].Location);
                            }
                            else if (Background[C].Type == 87) {
                                NewEffect(55, Background[C].Location);
                            }
                            else if (Background[C].Type == 107) {
                                NewEffect(59, Background[C].Location);
                            }
                        }
                    }
                    SoundPause[46] = 0;
                    PlaySound(46);
                }
            }
            else if (_.Effect2 <= 130) {
                _.Effect2 = (_.Effect2 - 1);
                if (_.Effect2 == 100) {
                    _.Effect = 0;
                    _.Effect2 = 0;
                }
            }
            else if (_.Effect2 <= 300) {
                _.Effect2 = (_.Effect2 - 1);
                if (_.Effect2 == 200) {
                    _.Effect2 = 100;
                    _.Effect = 3;
                }
            }
            // Start Wait
            else if (_.Effect2 <= 1000) {
                _.Effect2 = (_.Effect2 - 1);
                if (_.Effect2 == 900) {
                    _.Effect = 3;
                    _.Effect2 = 100;
                    if (A == 2) {
                        _.Effect = 8;
                        _.Effect2 = 300;
                    }
                }
            }
            // Start Wait
            else if (_.Effect2 <= 2000) {
                _.Effect2 = (_.Effect2 - 1);
                if (_.Effect2 == 1900) {
                    for (C = 1; C <= numBackground; C++) {
                        if (CheckCollision(Warp[_.Warp].Exit, Background[C].Location)) {
                            if (Background[C].Type == 88) {
                                NewEffect(54, Background[C].Location);
                            }
                            else if (Background[C].Type == 87) {
                                NewEffect(55, Background[C].Location);
                            }
                            else if (Background[C].Type == 107) {
                                NewEffect(59, Background[C].Location);
                            }
                        }
                    }
                    SoundPause[46] = 0;
                    PlaySound(46);
                    _.Effect = 8;
                    _.Effect2 = 30;
                    if (A == 2) {
                        _.Effect = 8;
                        _.Effect2 = 131;
                    }
                }
            }
            // warp wait
            else if (_.Effect2 <= 3000) {
                _.Effect2 = (_.Effect2 - 1);
                if (_.Effect2 == 2920) {
                    if (Warp[_.Warp].MapWarp == true) {
                        LevelBeatCode = 6;
                        if (!((Warp[_.Warp].MapX == -1) && (Warp[_.Warp].MapY == -1))) {
                            WorldPlayer[1].Location.X = Warp[_.Warp].MapX;
                            WorldPlayer[1].Location.Y = Warp[_.Warp].MapY;
                            for (B = 1; B <= numWorldLevels; B++) {
                                if (CheckCollision(WorldPlayer[1].Location, WorldLevel[B].Location) == true) {
                                    WorldLevel[B].Active = true;
                                    curWorldLevel = B;
                                }
                            }
                        }
                    }
                    EndLevel = true;
                    return;
                }
            }
        }
        // Player got fire power
        else if (_.Effect == 4) {
            if ((_.Duck == true) && (BaseCharacter != 5)) {
                UnDuck(A);
                _.Frame = 1;
            }
            _.Effect2 = (_.Effect2 + 1);
            if ((_.Effect2 / 5) == ::floor((_.Effect2 / 5))) {
                if ((_.State == 1) && (BaseCharacter != 5)) {
                    _.State = 2;
                    if (_.Mount == 0) {
                        if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                            UpdatePlayerPositionForStateChange(_);
                        }
                        else {
                            _.Location.X = ((_.Location.X - (Physics.PlayerWidth[2][BaseCharacter] * 0.5)) + (Physics.PlayerWidth[1][BaseCharacter] * 0.5));
                            _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][BaseCharacter]) + Physics.PlayerHeight[1][BaseCharacter]);
                            _.Location.Width = Physics.PlayerWidth[_.State][BaseCharacter];
                            _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                        }
                    }
                    else if (_.Mount == 3) {
                        YoshiHeight(A);
                    }
                    else if ((BaseCharacter == 2) && (_.Mount != 2)) {
                        if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                            UpdatePlayerPositionForStateChange(_);
                        }
                        else {
                            _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][2]) + Physics.PlayerHeight[2][1]);
                            _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                        }
                    }
                }
                else if (!(_.State == 3)) {
                    _.State = 3;
                }
                else {
                    _.State = 2;
                }
            }
            if (_.Effect2 >= 50) {
                if (_.State == 2) {
                    _.State = 3;
                }
                _.Immune = (_.Immune + 50);
                _.Immune2 = true;
                _.Effect = 0;
                _.Effect2 = 0;
                _.StandUp = true;
            }
        }
        // Player got ice power
        else if (_.Effect == 41) {
            if ((_.Duck == true) && (BaseCharacter != 5)) {
                UnDuck(A);
                _.Frame = 1;
            }
            _.Effect2 = (_.Effect2 + 1);
            if ((_.Effect2 / 5) == ::floor((_.Effect2 / 5))) {
                if ((_.State == 1) && (BaseCharacter != 5)) {
                    _.State = 2;
                    if (_.Mount == 0) {
                        if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                            UpdatePlayerPositionForStateChange(_);
                        }
                        else {
                            _.Location.X = ((_.Location.X - (Physics.PlayerWidth[2][BaseCharacter] * 0.5)) + (Physics.PlayerWidth[1][BaseCharacter] * 0.5));
                            _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][BaseCharacter]) + Physics.PlayerHeight[1][BaseCharacter]);
                            _.Location.Width = Physics.PlayerWidth[_.State][BaseCharacter];
                            _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                        }
                    }
                    else if (_.Mount == 3) {
                        YoshiHeight(A);
                    }
                    else if ((BaseCharacter == 2) && (_.Mount != 2)) {
                        if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                            UpdatePlayerPositionForStateChange(_);
                        }
                        else {
                            _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][2]) + Physics.PlayerHeight[2][1]);
                            _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                        }
                    }
                }
                else if (!(_.State == 7)) {
                    _.State = 7;
                }
                else {
                    _.State = 2;
                }
            }
            if (_.Effect2 >= 50) {
                if (_.State == 2) {
                    _.State = 7;
                }
                _.Immune = (_.Immune + 50);
                _.Immune2 = true;
                _.Effect = 0;
                _.Effect2 = 0;
                _.StandUp = true;
            }
        }
        // Player got a leaf
        else if (_.Effect == 5) {
            _.Frame = 1;
            if (_.Effect2 == 0) {
                if ((_.State == 1) && (_.Mount == 0)) {
                    if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                        _.State = 4;
                        UpdatePlayerPositionForStateChange(_);
                    }
                    else {
                        _.Location.X = ((_.Location.X - (Physics.PlayerWidth[2][BaseCharacter] * 0.5)) + (Physics.PlayerWidth[1][BaseCharacter] * 0.5));
                        _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][BaseCharacter]) + Physics.PlayerHeight[1][BaseCharacter]);
                        _.State = 4;
                        _.Location.Width = Physics.PlayerWidth[_.State][BaseCharacter];
                        _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                    }
                }
                else if (_.Mount == 3) {
                    YoshiHeight(A);
                }
                else if (((BaseCharacter == 2) && (_.State == 1)) && (_.Mount == 1)) {
                    if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                        _.State = 4;
                        UpdatePlayerPositionForStateChange(_);
                    }
                    else {
                        _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][2]) + Physics.PlayerHeight[2][1]);
                        _.Location.Height = Physics.PlayerHeight[4][BaseCharacter];
                    }
                }
                _.State = 4;
                tempLocation.Width = 32;
                tempLocation.Height = 32;
                tempLocation.X = ((_.Location.X + (_.Location.Width / 2)) - (tempLocation.Width / 2));
                tempLocation.Y = ((_.Location.Y + (_.Location.Height / 2)) - (tempLocation.Height / 2));
                NewEffect(131, tempLocation, 1, 0, ShadowMode);
            }
            _.Effect2 = (_.Effect2 + 1);
            if (_.Effect2 == 14) {
                _.Immune = (_.Immune + 50);
                _.Immune2 = true;
                _.Effect = 0;
                _.Effect2 = 0;
                _.StandUp = true;
            }
        }
        // Player got a tanooki suit
        else if (_.Effect == 11) {
            _.Frame = 1;
            _.Immune2 = true;
            if (_.Effect2 == 0) {
                if ((_.State == 1) && (_.Mount == 0)) {
                    if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                        _.State = 5;
                        UpdatePlayerPositionForStateChange(_);
                    }
                    else {
                        _.Location.X = ((_.Location.X - (Physics.PlayerWidth[2][BaseCharacter] * 0.5)) + (Physics.PlayerWidth[1][BaseCharacter] * 0.5));
                        _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][BaseCharacter]) + Physics.PlayerHeight[1][BaseCharacter]);
                        _.State = 5;
                        _.Location.Width = Physics.PlayerWidth[_.State][BaseCharacter];
                        _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                    }
                }
                else if (_.Mount == 3) {
                    YoshiHeight(A);
                }
                else if (((BaseCharacter == 2) && (_.State == 1)) && (_.Mount == 1)) {
                    if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                        _.State = 5;
                        UpdatePlayerPositionForStateChange(_);
                    }
                    else {
                        _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][2]) + Physics.PlayerHeight[2][1]);
                        _.Location.Height = Physics.PlayerHeight[4][BaseCharacter];
                    }
                }
                _.State = 5;
                tempLocation.Width = 32;
                tempLocation.Height = 32;
                tempLocation.X = ((_.Location.X + (_.Location.Width / 2)) - (tempLocation.Width / 2));
                tempLocation.Y = ((_.Location.Y + (_.Location.Height / 2)) - (tempLocation.Height / 2));
                NewEffect(131, tempLocation, 1, 0, ShadowMode);
            }
            _.Effect2 = (_.Effect2 + 1);
            if (_.Effect2 == 14) {
                _.Immune = (_.Immune + 50);
                _.Immune2 = true;
                _.Effect = 0;
                _.Effect2 = 0;
                _.StandUp = true;
            }
        }
        // Player got a hammer suit
        else if (_.Effect == 12) {
            _.Frame = 1;
            _.Immune2 = true;
            if (_.Effect2 == 0) {
                if ((_.State == 1) && (_.Mount == 0)) {
                    if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                        _.State = 6;
                        UpdatePlayerPositionForStateChange(_);
                    }
                    else {
                        _.Location.X = ((_.Location.X - (Physics.PlayerWidth[2][BaseCharacter] * 0.5)) + (Physics.PlayerWidth[1][BaseCharacter] * 0.5));
                        _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][BaseCharacter]) + Physics.PlayerHeight[1][BaseCharacter]);
                        _.State = 5;
                        _.Location.Width = Physics.PlayerWidth[_.State][BaseCharacter];
                        _.Location.Height = Physics.PlayerHeight[_.State][BaseCharacter];
                    }
                }
                else if (_.Mount == 3) {
                    YoshiHeight(A);
                }
                else if (((BaseCharacter == 2) && (_.State == 1)) && (_.Mount == 1)) {
                    if (SMBX13::Ports::_enablePowerupPowerdownPositionFixes) {
                        _.State = 6;
                        UpdatePlayerPositionForStateChange(_);
                    }
                    else {
                        _.Location.Y = ((_.Location.Y - Physics.PlayerHeight[2][2]) + Physics.PlayerHeight[2][1]);
                        _.Location.Height = Physics.PlayerHeight[6][BaseCharacter];
                    }
                }
                _.State = 6;
                tempLocation.Width = 32;
                tempLocation.Height = 32;
                tempLocation.X = ((_.Location.X + (_.Location.Width / 2)) - (tempLocation.Width / 2));
                tempLocation.Y = ((_.Location.Y + (_.Location.Height / 2)) - (tempLocation.Height / 2));
                NewEffect(131, tempLocation, 1, 0, ShadowMode);
            }
            _.Effect2 = (_.Effect2 + 1);
            if (_.Effect2 == 14) {
                _.Immune = (_.Immune + 50);
                _.Immune2 = true;
                _.Effect = 0;
                _.Effect2 = 0;
                _.StandUp = true;
            }
        }
        // Change to / from tanooki
        else if (_.Effect == 500) {
            for (B = 1; B <= 2; B++) {
                NewEffect(80, newLoc(((_.Location.X + (VB6RNG::generateNumber() * (_.Location.Width + 8))) - 8), ((_.Location.Y + (VB6RNG::generateNumber() * (_.Location.Height + 8))) - 4)), 1, 0, ShadowMode);
                Effect[numEffects].Location.SpeedX = ((VB6RNG::generateNumber() * 2) - 1);
                Effect[numEffects].Location.SpeedY = ((VB6RNG::generateNumber() * 2) - 1);
            }
            if (_.Effect2 == 0) {
                UnDuck(A);
                PlaySound(34);
                tempLocation.Width = 32;
                tempLocation.Height = 32;
                tempLocation.X = ((_.Location.X + (_.Location.Width / 2)) - (tempLocation.Width / 2));
                tempLocation.Y = ((_.Location.Y + (_.Location.Height / 2)) - (tempLocation.Height / 2));
                NewEffect(10, tempLocation, 1, 0, ShadowMode);
                if (_.Stoned == false) {
                    _.Frame = 0;
                    _.Stoned = true;
                    _.StonedCD = 15;
                }
                else {
                    _.StonedCD = 60;
                    _.Frame = 1;
                    _.Stoned = false;
                }
            }
            _.Effect2 = (_.Effect2 + 1);
            _.Immune = 10;
            _.Immune2 = true;
            _.StonedTime = 0;
            if (_.Effect2 >= 5) {
                _.Effect2 = 0;
                _.Effect = 0;
                _.Immune = 0;
                _.Immune2 = 0;
            }
        }
        // MultiMario
        else if (_.Effect == 9) {
            // Can't hold an NPC that is dead
            if (_.HoldingNPC > numNPCs) { _.HoldingNPC = 0; }
            if (_.HoldingNPC > 0) {
                NPC[_.HoldingNPC].Effect = 0;
                NPC[_.HoldingNPC].CantHurt = Physics.NPCCanHurtWait;
                NPC[_.HoldingNPC].CantHurtPlayer = A;
                if (_.Direction > 0) {
                    NPC[_.HoldingNPC].Location.X = (_.Location.X + Physics.PlayerGrabSpotX[_.State][BaseCharacter]);
                }
                else {
                    NPC[_.HoldingNPC].Location.X = (((_.Location.X + _.Location.Width) - Physics.PlayerGrabSpotX[_.State][BaseCharacter]) - NPC[_.HoldingNPC].Location.Width);
                }
                NPC[_.HoldingNPC].Location.Y = (((_.Location.Y + Physics.PlayerGrabSpotY[_.State][BaseCharacter]) + 32) - NPC[_.HoldingNPC].Location.Height);
            }
            _.MountSpecial = 0;
            _.YoshiTongueLength = 0;
            _.Immune = (_.Immune + 1);
            if (_.Immune >= 5) {
                _.Immune = 0;
                if (_.Immune2 == true) {
                    _.Immune2 = false;
                }
                else {
                    _.Immune2 = true;
                }
            }
            tempBool = true;
            for (B = 1; B <= numPlayers; B++) {
                if (VBAnd(((((B != A) && ((Player[B].Effect == 0) || (B == _.Effect2))) && (Player[B].Dead == false)) && (Player[B].TimeToLive == 0)), (CheckCollision(_.Location, Player[B].Location) == true))) {
                    tempBool = false;
                }
            }
            if (tempBool == true) {
                _.Effect = 0;
                _.Effect2 = 0;
                _.Immune = 0;
                _.Immune2 = 0;
                _.Location.SpeedY = 0.01;
            }
            else if (_.Effect2 > 0) {
                D = _.Effect2;
                if (Player[D].Effect == 0) {
                    _.Effect2 = 0;
                }
                _.Immune2 = true;
                _.Location.X = ((Player[D].Location.X + (Player[D].Location.Width / 2)) - (_.Location.Width / 2));
                _.Location.Y = ((Player[D].Location.Y + Player[D].Location.Height) - _.Location.Height);
            }
        }
        // Yoshi eat
        else if (_.Effect == 9) {
            _.HoldingNPC = 0;
            _.StandingOnNPC = 0;
            if (Player[_.Effect2].YoshiPlayer != A) {
                _.Effect = 0;
                _.Effect2 = 0;
            }
        }
        // Yoshi swallow
        else if (_.Effect == 10) {
            _.HoldingNPC = 0;
            _.StandingOnNPC = 0;
            _.Section = Player[_.Effect2].Section;
            _.Location.X = ((Player[_.Effect2].Location.X + (Player[_.Effect2].Location.Width / 2)) - (_.Location.Width / 2));
            _.Location.Y = ((Player[_.Effect2].Location.Y + (Player[_.Effect2].Location.Height / 2)) - (_.Location.Height / 2));
            if (Player[_.Effect2].YoshiPlayer != A) {
                _.Effect = 0;
                _.Effect2 = 0;
            }
        }
        // player stole a heldbonus
        else if (_.Effect == 6) {
            _.Immune = (_.Immune + 1);
            if (_.Immune >= 5) {
                _.Immune = 0;
                if (_.Immune2 == true) {
                    _.Immune2 = false;
                }
                else {
                    _.Immune2 = true;
                }
            }
            _.Location.Y = (_.Location.Y + 2.2);
            if (_.Location.Y >= _.Effect2) {
                _.Location.Y = _.Effect2;
                tempBool = true;
                for (B = 1; B <= numPlayers; B++) {
                    if (VBAnd(((B != A) && (Player[B].Effect != 6)), (CheckCollision(_.Location, Player[B].Location) == true))) { tempBool = false; }
                }
                if (tempBool == true) {
                    _.Effect = 0;
                    _.Effect2 = 0;
                    _.Immune = 50;
                    _.Immune2 = 0;
                    _.Location.SpeedY = 0.01;
                }
            }
            for (B = 1; B <= numPlayers; B++) {
                if (VBAnd((B != A), (CheckCollision(_.Location, Player[B].Location) == true))) {
                    if (Player[B].Mount == 2) {
                        _.Effect = 0;
                        _.Immune = 50;
                        _.Immune2 = false;
                        _.Location.Y = (Player[B].Location.Y - _.Location.Height);
                        _.Location.SpeedY = 0.01;
                    }
                }
            }
        }
        if ((_.Mount == 3) && (_.Effect != 9)) { PlayerFrame(A); }
        if (_.Effect == 0) {
            if ((nPlay.Online == true) && (A == (nPlay.MySlot + 1))) {
                // Netplay.sendData Netplay.PutPlayerControls(nPlay.MySlot) & "1c" & A & "|" & Player(A).Effect & "|" & Player(A).Effect2 & LB & "1h" & A & "|" & Player(A).State & LB
            }
        }
    }
    #pragma warning( pop )
}
