#include "PlayerMOB.h"
#include "../Defines.h"
#include "../Misc/MiscFuncs.h"
#include "NPCs.h"

// ACCESSORS
PlayerMOB* Player::Get(int index) {
	if(index > GM_PLAYERS_COUNT || index < 0)
		return 0;
	return (PlayerMOB*)(GM_PLAYERS_PTR + (0x00000184 * index));
}


// MANAGEMENT
bool Player::InternalSwap(int player1, int player2) {
	char temp[500];

	PlayerMOB* p1 = Player::Get(1);
	PlayerMOB* p2 = Player::Get(2);

	if(p1 == 0 || p2 == 0)
		return false;

	memcpy(temp, p1, 0x184);
	memcpy(p1, p2, 0x184);
	memcpy(p2, temp, 0x184);
	return true;
}

bool Player::InternalSwap(PlayerMOB* player1, PlayerMOB* player2) {
	char temp[500];

	if(player1 == 0 || player2 == 0)
		return false;

	memcpy(temp, player1, 0x184);
	memcpy(player1, player2, 0x184);
	memcpy(player2, temp, 0x184);
	return true;
}

void Player::MemSet(int offset, double value, OPTYPE operation, FIELDTYPE ftype) {
	char* dbg =  "PLAYER MEM SET";
	if(ftype == FT_INVALID || offset > (0x184 * 99) || offset < 0)
		return;
	PlayerMOB* pPlayer = Player::Get(1);
	void* ptr = ((&(*(byte*)pPlayer)) + offset);
	MemAssign((int)ptr, value, operation, ftype);
}


// BUTTON FUNCS
bool Player::PressingDown(PlayerMOB* pPlayer) {
    return (unsigned)pPlayer->keymap.DKeyState == 0xFFFFFFFF ? true : false;
}

bool Player::PressingJump(PlayerMOB* pPlayer) {
    return (unsigned)pPlayer->keymap.JKeyState == 0xFFFFFFFF ? true : false;
}

bool Player::PressingUp(PlayerMOB* pPlayer) {
    return (unsigned)pPlayer->keymap.UKeyState == 0xFFFFFFFF ? true : false;
}

bool Player::PressingRight(PlayerMOB* pPlayer) {
    return (unsigned)pPlayer->keymap.RKeyState == 0xFFFFFFFF ? true : false;
}

bool Player::PressingLeft(PlayerMOB* pPlayer) {
    return (unsigned)pPlayer->keymap.LKeyState == 0xFFFFFFFF ? true : false;
}

bool Player::PressingRun(PlayerMOB* pPlayer) {
    return (unsigned)pPlayer->keymap.RNKeyState == 0xFFFFFFFF ? true : false;
}

bool Player::PressingSEL(PlayerMOB* pPlayer) {
    return (unsigned)pPlayer->keymap.SELKeyState == 0xFFFFFFFF ? true : false;
}

void Player::ConsumeAllKeys(PlayerMOB* pPlayer) {
	//pPlayer->
}


// PLAYER CYCLE FUNCS
void Player::CycleRight(PlayerMOB* pPlayer) {

	// Check mount stuff for safe cycle
	//if(pPlayer->MountType != 0) {
	//	if(pPlayer->Identity > 1)
	//		return false; // fail to cycle because next character can't be mounted
	//}

	pPlayer->Identity++;
	if(pPlayer->Identity > 5)
		pPlayer->Identity = 1;


}

void Player::CycleLeft(PlayerMOB* pPlayer) {
	pPlayer->Identity--;
	if(pPlayer->Identity <= 0)
		pPlayer->Identity = 5;
}


// STATE FUNCS
bool Player::UsesHearts(PlayerMOB* pPlayer) {
	return (pPlayer->Identity == 1 || pPlayer->Identity == 2 ? false : true);
}

bool Player::IsSpinjumping(PlayerMOB* pPlayer) {
    return (unsigned)pPlayer->IsSpinjumping == 0xFFFFFFFF ? true : false;
}

bool Player::IsInForcedAnimation(PlayerMOB* player) {
	short state = player->ForcedAnimationState;
	if(state == 1 || state == 2 || state == 3 || state == 4 || state == 7)
		return true;
	return false;
}

bool Player::IsHoldingSpriteType(PlayerMOB* player, int NPC_ID) {
	if(player->HeldNPCIndex != 0) {
		NPCMOB* npc = NPC::Get(player->HeldNPCIndex);
		if(npc->id == NPC_ID)
			return true;
	}
	return false;
}

int	Player::IsStandingOnNPC(PlayerMOB* player) {
	return player->NPCBeingStoodOnIndex;
}

// FILTER FUNCS
void Player::FilterToFire(PlayerMOB* player) {
	if(player->CurrentPowerup > 3)
		player->CurrentPowerup = 3;
}

void Player::FilterToBig(PlayerMOB* player) {
	if(player->CurrentPowerup > 2)
		player->CurrentPowerup = 2;
}

void Player::FilterToSmall(PlayerMOB* player) {
	if(player->CurrentPowerup > 1)
		player->CurrentPowerup = 1;
}

void Player::FilterReservePowerup(PlayerMOB* player) {
	player->PowerupBoxContents = 0;
}

void Player::FilterMount(PlayerMOB* player) {
	player->MountType = 0;
	player->MountState = 0;
}

// HEARTS
void Player::SetHearts(PlayerMOB* player, int new_hearts) {
	if(new_hearts <= 1) {
		player->Hearts = new_hearts;
		player->CurrentPowerup = 1;
	}
	else {
		player->Hearts = new_hearts;
		player->CurrentPowerup = (player->CurrentPowerup >= 1 ? player->CurrentPowerup : 2);
	}
}


// ACTIONS
int Player::Kill(int* pIndex) {
	typedef int __stdcall killfunc(int*);	
	killfunc* f = (killfunc*)GF_KILL_PLAYER;
	return f(pIndex);
}

int Player::Harm(int* pIndex) {
	typedef int __stdcall harmfunc(int*);
	harmfunc* f = (harmfunc*)GF_HARM_PLAYER;
	return f(pIndex);
}


// GET SCREEN POSITION
RECT Player::GetScreenPosition(PlayerMOB* player) {
	double* pCameraY = (double*)GM_CAMERA_Y;
	double* pCameraX = (double*)GM_CAMERA_X;
	double cam_y = -pCameraY[1];
	double cam_x = -pCameraX[1];	
	double cam_d = cam_y + 600;
	double cam_r = cam_x + 800;

	RECT ret_rect;
    ret_rect.left = (LONG)(player->momentum.x - cam_x);
    ret_rect.top = (LONG)(player->momentum.y - cam_y);
    ret_rect.right = ret_rect.left + (LONG)player->momentum.width;
    ret_rect.bottom = ret_rect.top + (LONG)player->momentum.height;
	return ret_rect;
}
