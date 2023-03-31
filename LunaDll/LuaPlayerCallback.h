#include "Defines.h"
// functions relating to player logic which has been ported to lua code.
#ifndef LuaPlayerCallback____Hhh
#define LuaPlayerCallback____Hhh

#define _PLAYERCALLBACK_DEFINE(INDEX, TYPE, SET, GET) TYPE GET (); void SET (TYPE cb);

// calls the playercallback
// eg. PLAYERCALLBACK_CALL( LuaPlayerCallback::PlayerUnDuck(), playerIdx );
#define PLAYERCALLBACK_CALL(____pcb, ...) \
	if (____pcb != 0) { ____pcb( __VA_ARGS__ ); }

namespace LuaPlayerCallback {
	
	// resets all callbacks to null, should be called when the lua state is closed
	void Reset();

	typedef void (*Callback_Void)();
	typedef void (*Callback_Player)(short playerIdx);
	typedef void (*Callback_PlayerNPC)(short playerIdx, short npcIdx);

	// Don't forgot to update LuaPlayerCallback.cpp and LuaProxyFFI too
	_PLAYERCALLBACK_DEFINE( 0 , Callback_Player, Set_PlayerSizeCheck    , PlayerSizeCheck     );
	_PLAYERCALLBACK_DEFINE( 1 , Callback_Player, Set_PlayerUnDuck       , PlayerUnDuck        );
	_PLAYERCALLBACK_DEFINE( 2 , Callback_Player, Set_PlayerYoshiSpit    , PlayerYoshiSpit     );
	_PLAYERCALLBACK_DEFINE( 3 , Callback_Player, Set_PlayerHarm         , PlayerHarm          );
	_PLAYERCALLBACK_DEFINE( 4 , Callback_Player, Set_PlayerDiedKillLate , PlayerDiedKillLate  );
	_PLAYERCALLBACK_DEFINE( 5 , Callback_Player, Set_PlayerKilled       , PlayerKilled        );
	_PLAYERCALLBACK_DEFINE( 6 , Callback_Player, Set_PlayerDucked       , PlayerDucked        );
	_PLAYERCALLBACK_DEFINE( 7 , Callback_Player, Set_PlayerDuckForced   , PlayerDuckForced    );
	_PLAYERCALLBACK_DEFINE( 8 , Callback_Player, Set_PlayerPowerUpCode  , PlayerPowerUpCode   );
	_PLAYERCALLBACK_DEFINE( 9 , Callback_Player, Set_PlayerSetHeldNPCPos, PlayerSetHeldNPCPos );
	_PLAYERCALLBACK_DEFINE( 10, Callback_Player, Set_PlayerDismount     , PlayerDismount      );
	_PLAYERCALLBACK_DEFINE( 11, Callback_PlayerNPC, Set_PlayerMounted   , PlayerMounted       );
	_PLAYERCALLBACK_DEFINE( 12, Callback_Void  , Set_CoopStealBonus     , CoopStealBonus      );
	_PLAYERCALLBACK_DEFINE( 13, Callback_Player, Set_PlayerForcedStateCode, PlayerForcedStateCode);
	_PLAYERCALLBACK_DEFINE( 14, Callback_Player, Set_PlayerRender       , PlayerRender        );
}

#undef _PLAYERCALLBACK_DEFINE

#endif