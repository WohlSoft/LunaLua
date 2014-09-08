#include "AutocodeManager.h"
#include "PlayerMOB.h"
#include "MiscFuncs.h"
#include "Globals.h"
#include "NPCs.h"
#include "Sound.h"
#include "Blocks.h"
#include <cmath>
#include "Input.h"
#include "SMBXEvents.h"
#include "Level.h"
#include <windows.h>
#include <time.h>

using namespace std;

// CTORS
Autocode::Autocode() {
	m_Type = AT_Invalid;	
}

Autocode::Autocode(AutocodeType _Type, double _Target, double _p1, double _p2, double _p3, 
	std::wstring _p4, double _Length, int _Section, std::wstring _VarRef) {
	m_Type = _Type;
	Target = _Target;
	Param1 = _p1;
	Param2 = _p2;
	Param3 = _p3;
	Length = _Length;
	MyString = _p4;	
	MyRef = _VarRef;
	m_OriginalTime = _Length;
	ftype = FT_INVALID;
	Activated = true;
	Expired = false;
	//comp = NULL;

	// Adjust section
	ActiveSection = (_Section < 1000 ? --_Section : _Section);
	Activated = (_Section < 1000 ? true : false);

}

Autocode* Autocode::MakeCopy() {
	Autocode* newcode = new Autocode();

	newcode->Activated = Activated;
	newcode->ActiveSection = ActiveSection;
	newcode->Expired = Expired;
	newcode->ftype = ftype;
	newcode->Length = Length;
	newcode->MyString = MyString;
	newcode->MyRef = MyRef;
	newcode->m_OriginalTime = m_OriginalTime;
	newcode->m_Type = m_Type;
	newcode->Param1 = Param1;	
	newcode->Param2 = Param2;
	newcode->Param3 = Param3;
	newcode->Target = Target;
	//newcode->comp = NULL;

	return newcode;
}

Autocode::~Autocode() {
}

// <!> EnumerizeCommand() func moved to its own file

// DO - Perform autocodes for this section. Only does init codes if "init" is set
void Autocode::Do(bool init) {
	char* dbg = "AC_Do";

	// Is it expired?
	if(Expired || !Activated)
		return;

	// Make sure game is in OK state to run
	PlayerMOB* demo = Player::Get(1);
	if(demo == 0)
		return;

	// Only allow initrun on codes set to section -1
	if(init == true)
		init = ((BYTE)ActiveSection == (BYTE)0xFE ? true : false);

	// Run this code if "always" section, or if current section is a match, or forced by init
	if((BYTE)ActiveSection == (BYTE)0xFF || demo->CurrentSection == ActiveSection || init) {

		this->SelfTick();

		// Run specified autocode
		switch(m_Type) {

		// INVALID
		case Invalid:			
		default:
			break;

		// FILTERS
		case AT_FilterToSmall: {
			PlayerMOB* demo = Player::Get(1);
			Player::FilterToSmall(demo);
			break;		
							}	
		case AT_FilterToBig: {
			PlayerMOB* demo = Player::Get(1);
			Player::FilterToBig(demo);
			break;		
						  }	
		case AT_FilterToFire: {
			PlayerMOB* demo = Player::Get(1);
			Player::FilterToFire(demo);
			break;		
						   }
		case AT_FilterMount: {
			PlayerMOB* demo = Player::Get(1);
			Player::FilterMount(demo);
			break;		
						  }	
		case AT_FilterReservePowerup: {
			PlayerMOB* demo = Player::Get(1);
			Player::FilterReservePowerup(demo);
			break;		
								   }

		case AT_FilterPlayer: {
			PlayerMOB* demo = Player::Get(1);
			if(demo != 0) {
				if(demo->Identity == Param1)
					if(Param2 > 0 && Param2 < 6)
						demo->Identity = (short)Param2;
			}
			break;		
								   }
		
		// INFINITE FLYING
		case AT_InfiniteFlying:
			InfiniteFlying(1);
			break;

		// SET HEARTS
		case AT_SetHearts:
			demo->Hearts = (short)Param1;
			if(Param1 > 1 && demo->CurrentPowerup < 2)
				demo->CurrentPowerup = 2;
			break;

		// HEART SYSTEM
		case AT_HeartSystem:
			HeartSystem();
			break;

		// SCREEN EDGE BUFFER
		case AT_ScreenEdgeBuffer: {
			char* dbg = "SCREEN EDGE DBG";

			// Get all target NPCs in section into a list
			list<NPCMOB*> npcs;
			NPC::FindAll((int)Target, demo->CurrentSection, &npcs);

			if(npcs.size() > 0) {

				// Loop over list of NPCs
				for(list<NPCMOB*>::iterator iter = npcs.begin(), end = npcs.end(); iter != end; ++iter) {
					NPCMOB* npc = *iter;
					switch((int)Param1) {
					default:
					case 0: { // UP
						double* pCamera = (double*)GM_CAMERA_Y;
						double top = -pCamera[1];
						if(npc->Ypos < top + Param2)
							npc->Ypos = (top + Param2) + 1;
						break;
							}

					case 1: { // DOWN
						double* pCamera = (double*)GM_CAMERA_Y;
						double bot = -pCamera[1] + 600;
						if(npc->Ypos > bot - Param2)
							npc->Ypos = (bot - Param2) - 1;
						break;
							}

					case 2: { // LEFT
						double* pCamera = (double*)GM_CAMERA_X;
						double left = -pCamera[1];
						if(npc->Xpos < left + Param2)
							npc->Xpos = (left + Param2) + 1;
						break;
							}

					case 3: { // RIGHT
						double* pCamera = (double*)GM_CAMERA_X;
						double rt = -pCamera[1] + 800;
						if(npc->Xpos > rt - Param2)
							npc->Xpos = (rt - Param2) - 1;
						break;
							}
					}
				}
			}
			break;
								 }



		// SHOW TEXT
		case AT_ShowText:
			gLunaRender.SafePrint(MyString, (int)Param3, (float)Param1, (float)Param2);
			break;

		// SHOW NPC LIFE LEFT
		case AT_ShowNPCLifeLeft: {
			int base_health = _wtoi(MyString.data());
			NPCMOB* npc = NPC::GetFirstMatch((int)Target, (int)Param3 - 1);
			if(npc != NULL) {
				float hits = *(((float*)((&(*(byte*)npc)) + 0x148)));				
				gLunaRender.SafePrint(std::to_wstring((long long)(base_health - hits)), 3, (float)Param1, (float)Param2);
			} else {
				gLunaRender.SafePrint(L"?", 3, (float)Param1, (float)Param2);
			}
			break;
							}

		// AUDIO
		case AT_SFX: {			
			if(this->Length <= 1) { // Play once when delay runs out
				// Play built in sound
				if(Param1 > 0) {
					SMBXSound::PlaySFX((int)Param1);				
				}
				else {
					// Sound from level folder
					if(MyString.length() > 0) {
						char* dbg = "CUSTOM SOUND PLAY DBG";
						wstring world_dir = wstring((wchar_t*)GM_FULLDIR);
						wstring full_path = world_dir.append(Level::GetName());	
						full_path = removeExtension(full_path);
						full_path = full_path.append(L"\\"); // < path into level folder
						full_path = full_path + MyString;
						PlaySound(full_path.c_str(), 0, SND_FILENAME | SND_ASYNC);
					}

				}
				Expired = true;
			}
			break;
					 }

		case AT_SetMusic: {
			SMBXSound::SetMusic((int)Param1, (int)Target-1);

			if(MyString.length() >= 5) {
				SMBXSound::SetMusicPath((int)Target-1, MyString);
			}
			break;
						   }

		case AT_PlayMusic: {
			if(Length <= 1) { // Play once when delay runs out
				SMBXSound::PlayMusic((int)Param1-1);
				Expired = true;
			}
			break;
						   }

		// EVENTS & BRANCHES
		case AT_Trigger: {
			Expired = true;
			gAutoMan.ActivateCustomEvents((int)Target, (int)Param1);
			break;
						 }

		case AT_Timer:
			if(Param2) { // Display timer?
				gLunaRender.SafePrint(L"TIMER", 3, 600, 27);
				gLunaRender.SafePrint(std::to_wstring((long long)Length / 60), 3, 618, 48);
			}
			if(Length == 1 || Length == 0) {
				if(Length == 1)
					Expired = true;
				DoPredicate((int)Target, (int)Param1);

				// Reset time?
				if(Param3) {
					Activated = true;
					Expired = false;
					Length = m_OriginalTime;
				}
			}			
			break;

		case AT_IfNPC: {
			if(GM_NPCS_COUNT < 1)
				break;
			if(NPCConditional((int)Target, (int)Param1)) {
				RunSelfOption();				
				gAutoMan.ActivateCustomEvents((int)Param2, (int)Param3);				
			}
			break;
					   }

		case AT_BlockTrigger: {
			if(Target == 0){ //if target is player						
				if(Blocks::IsPlayerTouchingType((int)Param1, (int)Param2, demo)) {
					RunSelfOption();
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				}
			}
			break;
					   }

		case AT_TriggerRandom: {
			int choice = rand() % 4;
			switch(choice) {
			case 0:
				gAutoMan.ActivateCustomEvents(0, (int)Target);
				break;
			case 1:
				gAutoMan.ActivateCustomEvents(0, (int)Param1);
				break;
			case 2:
				gAutoMan.ActivateCustomEvents(0, (int)Param2);
				break;
			case 3:
				gAutoMan.ActivateCustomEvents(0, (int)Param3);
				break;
			}
			break;
							   }

		case AT_TriggerRandomRange: {
			if(Target > Param1 || Param1 < Target || Target == Param1) // rule out bad values
				break;
			int diff = (int)Param1 - (int)Target;
			int choice = rand() % diff;
			gAutoMan.ActivateCustomEvents(0, (int)Target + choice);
			break;
									}

		case AT_TriggerZone: {
			Length++; // undo length decrement
			if(demo->CurXPos > Param3 && demo->CurYPos > Param1 && demo->CurYPos < Param2 && demo->CurXPos < Length) {
				gAutoMan.ActivateCustomEvents(0, (int)Target);
				RunSelfOption();
			}
			break;
							 }

		case AT_ScreenBorderTrigger: {
			RECT player_screen_rect = Player::GetScreenPosition(demo);
			int depth = 0;
			if(MyString.length() > 0)
				depth = _wtoi(MyString.c_str());

			double L_edge = 0 + depth;
			double U_edge = 0 + depth;
			double D_edge = 600 - depth;
			double R_edge = 800 - depth;
			
			if(demo->WarpTimer < 1) {
				if(player_screen_rect.left <= L_edge && demo->CurXSpeed < 0) {
					gAutoMan.ActivateCustomEvents(0, (int)Target);
					demo->WarpTimer = 2;
				}
				else if(player_screen_rect.top <= U_edge  && demo->CurYSpeed < 0) {
					gAutoMan.ActivateCustomEvents(0, (int)Param1);
					demo->WarpTimer = 2;
				}
				else if(player_screen_rect.right >= R_edge  && demo->CurXSpeed > 0) {
					gAutoMan.ActivateCustomEvents(0, (int)Param2);
					demo->WarpTimer = 2;
				}
				else if(player_screen_rect.bottom >= D_edge && demo->CurYSpeed > 0) {
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
					demo->WarpTimer = 2;
				}
			}
			break;
									 }
		
		case AT_OnInput: {			
			switch((int)Param1) {
			case 1: // Up
				if(Param2 == 1 && Input::UpThisFrame()) // First frame press only
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				else if(Param2 != 1 && Input::PressingUp()) // Any press
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				break;

			case 2: // Down
				if(Param2 == 1 && Input::DownThisFrame()) // First frame press only
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				else if(Param2 != 1 && Input::PressingDown()) // Any press
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				break;

			case 3: // Left
				if(Param2 == 1 && Input::LeftThisFrame()) // First frame press only
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				else if(Param2 != 1 && Input::PressingLeft()) // Any press
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				break;

			case 4: // Right
				if(Param2 == 1 && Input::RightThisFrame()) // First frame press only
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				else if(Param2 != 1 && Input::PressingRight()) // Any press
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				break;

			case 5: //Run
				if(Param2 == 1 && Input::RunThisFrame()) // First frame press only
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				else if(Param2 != 1 && Input::PressingRun()) // Any press
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				break;

			case 6: //Jump
				if(Param2 == 1 && Input::JumpThisFrame()) // First frame press only
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				else if(Param2 != 1 && Input::PressingJump()) // Any press
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				break;
			}
			break;
						 }
						 
		case AT_OnPlayerMem: {
			if(ftype == FT_INVALID) {
				ftype = FT_BYTE;
				ftype = StrToFieldtype(MyString);
			}
			byte* ptr = (byte*)demo;
			ptr += (int)Target; // offset
			bool triggered = CheckMem((int)ptr, Param1, (COMPARETYPE)(int)Param2, ftype);
			if(triggered) {
				gAutoMan.ActivateCustomEvents(0, (int)Param3);
			}
			break;
							 }

		case AT_OnGlobalMem: {
			if(ftype == FT_INVALID) {
				ftype = FT_BYTE;
				ftype = StrToFieldtype(MyString);
			}
			bool triggered = CheckMem((int)Target, Param1, (COMPARETYPE)(int)Param2, ftype);
			if(triggered) {
				gAutoMan.ActivateCustomEvents(0, (int)Param3);
			}
			break;
							 }

		// USER VARS
		case AT_SetVar: {
			if(ReferenceOK()) {				
				gAutoMan.VarOperation(MyRef, Param2, (OPTYPE)(int)Param1);
			}
			else { //if(gAutoMan.m_UserVars.find(MyString) != gAutoMan.m_UserVars.end())				
				gAutoMan.VarOperation(MyString, Param2, (OPTYPE)(int)Param1);
			}			
			break;
						}

		case AT_LoadPlayerVar: {
			char* dbg = "LOAD PLAYER VAR DEBUG";
			if(!this->ReferenceOK() || Param1 > (0x184 * 99))
				break;
			if(ftype == FT_INVALID) {
				ftype = FT_BYTE;
				ftype = StrToFieldtype(MyString);
			}

			// Get the memory
			byte* ptr = (byte*)demo;
			ptr += (int)Param1; // offset
			double gotval = GetMem((int)ptr, ftype);

			// Perform the load/add/sub/etc operation on the banked variable using the ref as the name
			gAutoMan.VarOperation(MyRef, gotval, (OPTYPE)(int)Param2);

			break;			
						}

		case AT_LoadNPCVar: {
			if(!this->ReferenceOK() || Param1 > (0x158))
				break;
			if(ftype == FT_INVALID) {
				ftype = FT_BYTE;
				ftype = StrToFieldtype(MyString);
			}

			NPCMOB* pFound_npc = NPC::GetFirstMatch((int)Target, (int)Param3);
			if(pFound_npc != NULL) {
				byte* ptr = (byte*)pFound_npc;
				ptr += (int)Param1;
				double gotval = GetMem((int)ptr, ftype);

				gAutoMan.VarOperation(MyRef, gotval, (OPTYPE)(int)Param2);
			}

			break;
							}

		case AT_LoadGlobalVar: {
			if(Target >= GM_BASE && Param1 <=  GM_END && ReferenceOK()) {				
				if(ftype == FT_INVALID) {
					ftype = FT_BYTE;
					ftype = StrToFieldtype(MyString);
				}

				byte* ptr = (byte*)(int)Target;				
				double gotval = GetMem((int)ptr, ftype);

				gAutoMan.VarOperation(MyRef, gotval, (OPTYPE)(int)Param1);
			}
			break;
							   }

		case AT_IfVar: {
			if(!ReferenceOK()) {	
				if(gAutoMan.m_UserVars.find(MyString) == gAutoMan.m_UserVars.end())
					break; // Var doesn't exist
			}
			double varval;
			if(ReferenceOK()) {
				varval = gAutoMan.m_UserVars[MyRef];
			}
			else {
				varval = gAutoMan.m_UserVars[MyString];
			}

			// Check if the value meets the criteria and activate event if so
			switch((COMPARETYPE)(int)Param1) {
			case CMPT_EQUALS:
				if(varval == Param2)
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				break;
			case CMPT_GREATER:
				if(varval > Param2)
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				break;
			case CMPT_LESS:
				if(varval < Param2)
					gAutoMan.ActivateCustomEvents(0, (int)Param3);
				break;
			default:
				break;
			}
			break;
					   }

		case AT_CompareVar: {
			if(ReferenceOK()) {
				if(gAutoMan.m_UserVars.find(MyRef) !=  gAutoMan.m_UserVars.end()
					&& gAutoMan.m_UserVars.find(MyString) !=  gAutoMan.m_UserVars.end()) {
					double var1 = gAutoMan.m_UserVars[MyRef];
					double var2 = gAutoMan.m_UserVars[MyString];

					switch((COMPARETYPE)(int)Param1) {
					case CMPT_EQUALS:
						if(var1 == var2)
							gAutoMan.ActivateCustomEvents(0, (int)Param3);
						break;
					case CMPT_GREATER:
						if(var1 > var2)
							gAutoMan.ActivateCustomEvents(0, (int)Param3);
						break;
					case CMPT_LESS:
						if(var1 < var2)
							gAutoMan.ActivateCustomEvents(0, (int)Param3);
						break;
					default:
						break;
					}			
				}
			}
			break;
							 }

		case AT_ShowVar: {
			if(ReferenceOK()) {				
				std::wstring str = std::to_wstring((long double)gAutoMan.GetVar(MyRef));
				if(MyString.length() > 0)
					str = MyString + str;
				gLunaRender.SafePrint(str, (int)Param3, (float)Param1, (float)Param2);
			}
			break;
						 }

		// LUNA CONTROL
		case AT_LunaControl: {
			LunaControl((LunaControlAct)(int)Target, (int)Param1);
			break;
							 }


		// DELETE COMMAND
		case AT_DeleteCommand: {			
				gAutoMan.DeleteEvent(MyString);
			break;
							   }

		// MOD PARAM 
		case AT_ModParam: {
			if(Target < 6 && Target > 0) {
				Autocode* coderef = 0;
				coderef = gAutoMan.GetEventByRef(MyString);
				if(coderef !=0) {
					switch((int)Target) {
					case 1:
						MemAssign((int)&(coderef->Target), Param1, (OPTYPE)(int)Param2, FT_DWORD);
						break;
					case 2:
						MemAssign((int)&(coderef->Param1), Param1, (OPTYPE)(int)Param2, FT_DWORD);
						break;
					case 3:
						MemAssign((int)&(coderef->Param2), Param1, (OPTYPE)(int)Param2, FT_DWORD);
						break;
					case 4:
						MemAssign((int)&(coderef->Param3), Param1, (OPTYPE)(int)Param2, FT_DWORD);
						break;
					case 5:
						MemAssign((int)&(coderef->Length), Param1, (OPTYPE)(int)Param2, FT_DWORD);
						break;
					}
				}
			}
			break;
						  }

		// ChangeTime
		case AT_ChangeTime: {
			Autocode* coderef = 0;
			coderef = gAutoMan.GetEventByRef(MyString);
			if(coderef !=0) {
				MemAssign((int)&coderef->Length, Param1, (OPTYPE)(int)Param2, FT_DFLOAT);
			}
			break;
							}

		// INPUT BUFFER STUFF
		case AT_OnCustomCheat: {			
			std::wstring curbuf = Input::GetInputStringCopy();			
			int org_len = curbuf.length();
			int sought_len = MyString.length();
			if(org_len >= sought_len && 
				curbuf.substr(org_len - sought_len, sought_len) == MyString) {
				gAutoMan.ActivateCustomEvents(0, (int)Param3);
				Input::ClearInputStringBuffer();
				if(Param2 != 0)
					this->Expired = true;
			}
			break;
							   }		

		case AT_ClearInputString: {
			wchar_t* dbg = L"ClearInputString debug";
			Input::ClearInputStringBuffer();
			break;
								  }
								  
		case AT_DeleteEventsFrom: {
			gAutoMan.ForceExpire((int)Target);
			break;
								  }
		
		// LAYER CONTROL
		case AT_LayerXSpeed: {
			LayerControl* layer = Layer::Get((int)Target);
			if(layer) {
				Layer::SetXSpeed(layer, (float)_wtof(MyString.c_str()));
				if(Length == 1 && Param1)
					Layer::SetXSpeed(layer, 0.0001f);
			}
			break;
					   }

		case AT_LayerYSpeed: {
			LayerControl* layer = Layer::Get((int)Target);
			if(layer) {
				Layer::SetYSpeed(layer, (float)_wtof(MyString.c_str()));
				if(Length == 1 && Param1)
					Layer::SetYSpeed(layer, 0.0001f);
			}
			break;
					   }
				
		case AT_AccelerateLayerX: {
			LayerControl* layer = Layer::Get((int)Target);
			if(layer) {
				float accel = (float)_wtof(MyString.c_str());
				if(std::abs(layer->xSpeed) + std::abs(accel) >= std::abs((float)Param1)) {
					Layer::SetXSpeed(layer, (float)Param1);
				} else {
					Layer::SetXSpeed(layer, layer->xSpeed + (float)accel);
				}
			}
			break;
								  }

		case AT_AccelerateLayerY: {
			LayerControl* layer = Layer::Get((int)Target);
			if(layer) {
				float accel = (float)_wtof(MyString.c_str());
				if(std::abs(layer->ySpeed) + std::abs(accel) >= std::abs((float)Param1)) {
					Layer::SetYSpeed(layer, (float)Param1);
				} else {
					Layer::SetYSpeed(layer, layer->ySpeed + (float)accel);
				}
			}
			break;
								  }

		case AT_DeccelerateLayerX: {
			LayerControl* layer = Layer::Get((int)Target);
			if(layer) {
				float deccel = (float)_wtof(MyString.c_str());				
				deccel = std::abs(deccel);				
				if(layer->xSpeed > 0) {
					layer->xSpeed -= deccel;
					if(layer->xSpeed < 0)
						Layer::Stop(layer);
				} else if (layer->xSpeed < 0) {
					layer->xSpeed += deccel;
					if(layer->xSpeed > 0)
						Layer::Stop(layer);
				}
			}
			break;
								   }
			

		case AT_DeccelerateLayerY: {
			LayerControl* layer = Layer::Get((int)Target);
			if(layer) {
				float deccel = (float)_wtof(MyString.c_str());				
				deccel = std::abs(deccel);				
				if(layer->ySpeed > 0) {
					layer->ySpeed -= deccel;
					if(layer->ySpeed < 0)
						Layer::Stop(layer);
				} else if (layer->ySpeed < 0) {
					layer->ySpeed += deccel;
					if(layer->ySpeed > 0)
						Layer::Stop(layer);
				}
			}
			break;
								   }


		case AT_PushScreenBoundary: {
			if(Target > 0 && Target < 22 && Param1 >= 0 && Param1 < 5) {
				Level::PushSectionBoundary((int)Target - 1, (int)Param1, _wtof(MyString.c_str()));
			}
			break;
								   }

		case AT_SnapSectionBounds: {
			if(Target > 0 && Target < 22) { // Make sure valid section
				//RECT current_bounds;
				//int sec = ((int)Target) - 1;				
				//Level::GetBoundary(&current_bounds, sec);
				//double x_dist = Param1 - current_bounds.left;
				//double y_dist = Param2 - current_bounds.top;
				//double x_stepsize = x_dist / Length;
				//double y_stepsize = y_dist / Length;
				//double x_stepped =  current_bounds.left + x_stepsize;
				//double y_stepped =  current_bounds.top + y_stepsize;
				//Level::SetSectionBounds(sec, x_stepped,  y_stepped, x_stepped + 800, y_stepped + 600);

				//if(Length <= 1) { // When travel time is up, force screen into the right place
					Level::SetSectionBounds((int)Target - 1, Param1, Param2, Param1 + 800, Param2 + 600);					
				//}
			}
			break;
								   }

		// PLAYER CYCLE
		case AT_CyclePlayerRight: {
			Player::CycleRight(demo);
			break;
							   }

		case AT_CyclePlayerLeft: {
			Player::CycleLeft(demo);
			break;
							   }	


		// SET HITS
		case AT_SetHits: {			
			NPC::AllSetHits((int)Target, (int)Param1 - 1, (float)Param2);
			break;
						 }

		// FORCE FACING
		case AT_ForceFacing: {
			PlayerMOB* demo = Player::Get(1);
			if(demo != 0) {
				NPC::AllFace((int)Target, (int)Param1 - 1, demo->CurXPos);
			}
			break;
						 }

		case AT_TriggerSMBXEvent: {
			SMBXEvents::TriggerEvent(MyString, (int)Param1);
			break;
								  }


		// PREDICATES
		case AT_Hurt: {
			int tempint = 1;
			if(Target == 0)
				Player::Harm(&tempint);
				RunSelfOption();
			break;
					}

		case AT_Kill: {
			int tempint = 1;
			if(Target == 0)
				Player::Kill(&tempint);
				RunSelfOption();
			break;
					}
		
		// NPC MEMORY SET
		case AT_NPCMemSet: {			
			if(ftype == FT_INVALID) {
				ftype = FT_BYTE;
				ftype = StrToFieldtype(MyString);
			}

			// Assign the mem
			if(ReferenceOK()) { // Use referenced var as value
				double gotval = gAutoMan.GetVar(MyRef);
				NPC::MemSet((int)Target, (int)Param1, gotval, (OPTYPE)(int)Param3, ftype);
			}
			else { // Use given value as value
				NPC::MemSet((int)Target, (int)Param1, Param2, (OPTYPE)(int)Param3, ftype); // NPC ID, offset in obj, value, op, field type
			}

			break;
							}

		// PLAYER MEMORY SET
		case AT_PlayerMemSet: {
			if(ftype == FT_INVALID) {
				ftype = FT_BYTE;
				ftype = StrToFieldtype(MyString);
			}
			if(ReferenceOK()) {
				double gotval = gAutoMan.GetVar(MyRef);
				Player::MemSet((int)Param1, gotval, (OPTYPE)(int)Param3, ftype);
			}
			else {
				Player::MemSet((int)Param1, Param2, (OPTYPE)(int)Param3, ftype);
			}
			break;
							  }
							  

		// MEM ASSIGN
		case AT_MemAssign: {
			if(Target >= GM_BASE && Param1 <=  GM_END) {
				if(ftype == FT_INVALID) {
					ftype = FT_BYTE;
					ftype = StrToFieldtype(MyString);
				}
				if(ReferenceOK()) {
					double gotval = gAutoMan.GetVar(MyRef);
					MemAssign((int)Target, gotval, (OPTYPE)(int)Param2, ftype);
				}
				else {
					MemAssign((int)Target, Param1, (OPTYPE)(int)Param2, ftype);
				}
			}
			break;
						   }

		// DEBUG
		case AT_DebugPrint: {
			gLunaRender.SafePrint(L"LUNADLL VERSION-" + std::to_wstring((long long)LUNA_VERSION), 3, 50, 250);
			//gLunaRender.SafePrint(, 3, 340, 250);

			gLunaRender.SafePrint(L"GLOBL-" + std::to_wstring((long long)gAutoMan.m_GlobalCodes.size()), 3, 50, 300);

			gLunaRender.SafePrint(L"INIT -" + std::to_wstring((long long)gAutoMan.m_InitAutocodes.size()), 3, 50, 330);

			gLunaRender.SafePrint(L"CODES-" + std::to_wstring((long long)gAutoMan.m_Autocodes.size()), 3, 50, 360);

			gLunaRender.SafePrint(L"QUEUE-" + std::to_wstring((long long)gAutoMan.m_CustomCodes.size()), 3, 50, 390);

			gLunaRender.SafePrint(L"SPRITE-" + std::to_wstring((long long)gSpriteMan.CountSprites()), 3, 50, 420);

			gLunaRender.SafePrint(L"BLPRNT-" + std::to_wstring((long long)gSpriteMan.CountBlueprints()), 3, 50, 450);

			gLunaRender.SafePrint(L"COMP-" + std::to_wstring((long long)gSpriteMan.m_ComponentList.size()), 3, 50, 480);

			int buckets = 0, cells = 0, objs = 0;
			gCellMan.CountAll(&buckets, &cells, &objs);
			gLunaRender.SafePrint(L"BCO-" + std::to_wstring((long long)buckets) + L" "
									+ std::to_wstring((long long)cells) + L" "
									+ std::to_wstring((long long)objs), 3, 50, 510);

			std::list<CellObj> cellobjs;
			gCellMan.GetObjectsOfInterest(&cellobjs, demo->CurXPos, demo->CurYPos, (int)demo->Width, (int)demo->Height);
			gLunaRender.SafePrint(L"NEAR-" + std::to_wstring((long long)cellobjs.size()), 3, 50, 540);

			break;
							 }
							 			
		case AT_DebugWindow: {
			MessageBox(0, MyString.c_str(), L"LunaDLL debug message", 0);
			break;
							 }

		case AT_CollisionScan: {
			gCellMan.ScanLevel(true);
			break;
							   }

		// SPRITE FUNCTIONS
		case AT_LoadImage: {
			if(init) { // Only allow loading image during init phase
				gLunaRender.LoadBitmapResource(MyString, (int)Target, (int)Param1);				
			}
			Expired = true;
			break;
						   }

		case AT_SpriteBlueprint: {
			if(ReferenceOK()) {
				CSprite* blueprint = new CSprite();
				gSpriteMan.AddBlueprint(MyRef.c_str(), blueprint);
			}
			Expired = true;
			break;
								 }

		case AT_Attach: {
			char* dbg = "!!! ATTACH DEBUG !!!";
			if(ReferenceOK() && MyString.length() > 0) {
				if(gSpriteMan.m_SpriteBlueprints.find(MyRef) != gSpriteMan.m_SpriteBlueprints.end()) { // BLueprint exists
					CSprite* pSpr = gSpriteMan.m_SpriteBlueprints[MyRef];					// Get blueprint
					Autocode* pComponent = gAutoMan.GetEventByRef(MyString);				// Get autocode containing component
					if(pComponent != NULL) {
						switch((BlueprintAttachType)(int)Target) {
						case BPAT_Behavior:
							pSpr->AddBehaviorComponent(GenerateComponent(pComponent));
							break;
						case BPAT_Draw:
							pSpr->AddDrawComponent(GetDrawFunc(pComponent));
							break;
						case BPAT_Birth:
							pSpr->AddBirthComponent(GenerateComponent(pComponent));
							break;
						case BPAT_Death:
							pSpr->AddDeathComponent(GenerateComponent(pComponent));
							break;
						default:
							break;
						}
					}
				}
			}
			Expired = true;
			break;
						}

		case AT_PlaceSprite: {
			//1: Type	2: ImgResource	3: Xpos			4: Ypos		5:				6: Extra
			CSpriteRequest req;
			req.type = (int)Target;
			req.img_resource_code = (int)Param1;
			req.x = (int)Param2;
			req.y = (int)Param3;
			req.time = (int)Length;
			req.str = MyString;
			gSpriteMan.InstantiateSprite(&req);

			Expired = true;
			break;
							 }
		}//switch

	}//section

}

// SELF TICK
void Autocode::SelfTick() {
	if(Length == 1) {
		Expired = true;
	}
	else if(Length == 0) {
		return;
	}
	else {
		Length--;
	}
}

// DO PREDICATE
void Autocode::DoPredicate(int target, int predicate) {

	// Activate custom event?
	if(predicate >= 1000 && predicate < 100000) {
		gAutoMan.ActivateCustomEvents(target, predicate);
		return;
	}

	// Else, do predicate
	AutocodePredicate pred = (AutocodePredicate)predicate;
	int tempint = 1;

	switch (pred) {

	// DEATH PREDICATE
	case AP_Hurt:
			Player::Harm(&tempint);
			break;
	case AP_Death:
			Player::Kill(&tempint);
			break;

	default:
		break;
	}
}

// NPC CONDITIONAL
bool Autocode::NPCConditional(int target, int cond) {
	char* dbg = "NPC COND DBG";
	bool ret = false;

	switch((AC_Conditional)cond) {

	case AC_Invalid:
	default:
		return ret;
		break;

	case AC_Exists: {
		return (NPC::GetFirstMatch(target, -1) == NULL ? false : true);
		break;
					}

	case AC_DoesNotExist: {
		return (NPC::GetFirstMatch(target, -1) == NULL ? true : false);
		break;
					}
	}
}

// RUN SELF OPTION
void Autocode::RunSelfOption() {
	if(this->MyString.find(L"once") != std::string::npos) {
		this->Expired = true;
	}
}

// REFERENCE OK
bool Autocode::ReferenceOK() {
	if(this->MyRef.length() < 1){
		return false;
	}
	return true;
}