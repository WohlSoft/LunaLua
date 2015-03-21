#include "LuaEvents.h"
#include "LuaHelper.h"
#include "../Rendering/Rendering.h"
#include "../Globals.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../SMBXInternal/NPCs.h"
#include "../Misc/MiscFuncs.h"
#include "../SMBXInternal/SMBXEvents.h"
#include "../SMBXInternal/Sound.h"
#include "../SMBXInternal/Level.h"
#include "../CustomSprites/CSprite.h"
#include "../CustomSprites/CSpriteManager.h"
#include "../SMBXInternal/Blocks.h"

std::vector<LuaEvents::SMBXEventQueueItem> LuaEvents::SMBXEventQueue;

char LuaEvents::pressTest(short oldp, short newp)
{
    if(oldp == 0 && newp == -1)
        return 1;

    if(oldp == -1 && oldp == 0)
        return -1;

    return 0;
}

void LuaEvents::processKeyboardEvent(short oldp, short newp, int index, int playerIndex, lua_State *L, std::string eventTable)
{
    luabind::object evTable = LuaHelper::getEventCallbase(L, eventTable);
    char pTest = pressTest(oldp, newp);
    if(pTest == 1){
		luabind::call_function<void>(evTable["onKeyDown"], index, playerIndex);
    }else if(pTest == -1){
		luabind::call_function<void>(evTable["onKeyUp"], index, playerIndex);
    }
}

void LuaEvents::processKeyboardEvents(lua_State* L, std::string eventTable)
{
    for(int i = 1; i <= 2; ++i){
        PlayerMOB* player = ::Player::Get(i);
        LuaEventData* evData = getEvData(i);
        if(player && evData){
            processKeyboardEvent(evData->playerUPressing, player->UKeyState, 0, i, L, eventTable);
            processKeyboardEvent(evData->playerDPressing, player->DKeyState, 1, i, L, eventTable);
            processKeyboardEvent(evData->playerLPressing, player->LKeyState, 2, i, L, eventTable);
            processKeyboardEvent(evData->playerRPressing, player->RKeyState, 3, i, L, eventTable);
            processKeyboardEvent(evData->playerJPressing, player->JKeyState, 4, i, L, eventTable);
            processKeyboardEvent(evData->playerSJPressing, player->SJKeyState, 5, i, L, eventTable);
            processKeyboardEvent(evData->playerXPressing, player->XKeyState, 6, i, L, eventTable);
            processKeyboardEvent(evData->playerRNPressing, player->RNKeyState, 7, i, L, eventTable);
            processKeyboardEvent(evData->playerSELPressing, player->SELKeyState, 8, i, L, eventTable);
            processKeyboardEvent(evData->playerSTRPressing, player->STRKeyState, 9, i, L, eventTable);
        }
    }
}



void LuaEvents::processJumpEvent(lua_State *L, std::string eventTable)
{
    luabind::object evTable = LuaHelper::getEventCallbase(L, eventTable);
    for(int i = 1; i <= 2; ++i){
        PlayerMOB* player = ::Player::Get(i);
        LuaEventData* evData = getEvData(i);
        if(player && evData){
            if(evData->playerJumping == 0 && player->HasJumped == -1){
				luabind::call_function<void>(evTable["onJump"], i);
            }else if(evData->playerJumping == -1 && player->HasJumped == 0){
				luabind::call_function<void>(evTable["onJumpEnd"], i);
            }
        }
    }

}






LuaEvents::LuaEventData *LuaEvents::getEvData(int playerIndex)
{
    if(playerIndex==1)
        return &evPlayer1;
    if(playerIndex==2)
        return &evPlayer2;

    return 0;
}

void LuaEvents::proccesEvents(lua_State *L, std::string eventTable)
{
    processKeyboardEvents(L, eventTable);
    processJumpEvent(L, eventTable);
    processSectionEvents(L, eventTable);
	processSMBXEvents(L, eventTable);
}


void LuaEvents::processSectionEvents(lua_State *L, std::string eventTable)
{
    luabind::object evTable = LuaHelper::getEventCallbase(L, eventTable);
    for(int i = 1; i <= 2; ++i){
        PlayerMOB* player = ::Player::Get(i);
        LuaEventData* evData = getEvData(i);
        if(player && evData){
            if(evData->section != player->CurrentSection){
                std::string curSecLoop = "onLoadSection";
				luabind::call_function<void>(evTable[curSecLoop.c_str()], i); //onLoadSection
                curSecLoop = curSecLoop.append(std::to_string((long long)player->CurrentSection));
				luabind::call_function<void>(evTable[curSecLoop.c_str()], i); //onLoadSection#
            }
            std::string curSecLoop = "onLoopSection";
            curSecLoop = curSecLoop.append(std::to_string((long long)player->CurrentSection));
			luabind::call_function<void>(evTable[curSecLoop.c_str()], i);

        }
    }
}


void LuaEvents::processSMBXEvents(lua_State *L, std::string eventTable)
{
	luabind::object evTable = LuaHelper::getEventCallbase(L, eventTable);
	for(int i = 0; i < (int)SMBXEventQueue.size(); ++i){
		SMBXEventQueueItem& item = SMBXEventQueue[i];
		luabind::call_function<void>(evTable["onSMBXEvent"], item.event, item.callType, item.unkVal);
	}
}

//CLEANUP
void LuaEvents::finishEventHandling()
{
	for(int i = 1; i <= 2; ++i){
		PlayerMOB* player = ::Player::Get(i);
		LuaEventData* evData = getEvData(i);
		if(player && evData){
			evData->playerUPressing = player->UKeyState;
			evData->playerDPressing = player->DKeyState;
			evData->playerLPressing = player->LKeyState;
			evData->playerRPressing = player->RKeyState;
			evData->playerJPressing = player->JKeyState;
			evData->playerSJPressing = player->SJKeyState;
			evData->playerXPressing = player->XKeyState;
			evData->playerRNPressing = player->RNKeyState;
			evData->playerSELPressing = player->SELKeyState;
			evData->playerSTRPressing = player->STRKeyState;
			evData->playerJumping = player->HasJumped;
			evData->section = player->CurrentSection;
		}
	}
	SMBXEventQueue.clear();
}

void LuaEvents::resetToDefaults()
{
	LuaEvents::evPlayer1.playerUPressing = 0;
	LuaEvents::evPlayer1.playerDPressing = 0;
	LuaEvents::evPlayer1.playerLPressing = 0;
	LuaEvents::evPlayer1.playerRPressing = 0;
	LuaEvents::evPlayer1.playerJPressing = 0;
	LuaEvents::evPlayer1.playerSJPressing = 0;
	LuaEvents::evPlayer1.playerXPressing = 0;
	LuaEvents::evPlayer1.playerRNPressing = 0;
	LuaEvents::evPlayer1.playerSELPressing = 0;
	LuaEvents::evPlayer1.playerSTRPressing = 0;
	LuaEvents::evPlayer1.playerJumping = 0;
	LuaEvents::evPlayer1.section = -1;

	LuaEvents::evPlayer2.playerUPressing = 0;
	LuaEvents::evPlayer2.playerDPressing = 0;
	LuaEvents::evPlayer2.playerLPressing = 0;
	LuaEvents::evPlayer2.playerRPressing = 0;
	LuaEvents::evPlayer2.playerJPressing = 0;
	LuaEvents::evPlayer2.playerSJPressing = 0;
	LuaEvents::evPlayer2.playerXPressing = 0;
	LuaEvents::evPlayer2.playerRNPressing = 0;
	LuaEvents::evPlayer2.playerSELPressing = 0;
	LuaEvents::evPlayer2.playerSTRPressing = 0;
	LuaEvents::evPlayer2.playerJumping = 0;
	LuaEvents::evPlayer2.section = -1;
}

