#ifndef NpcToCoins_hhhh
#define NpcToCoins_hhhh

#include "../../NPCs.h"
#include "../../Animation.h"

namespace Reconstructed{
    namespace Util{
        extern EffectID npcToCoinEffect;
        extern short npcToCoinSound;
        extern short npcToCoinValue;
        extern short npcToCoinValueReset;
        extern void __stdcall npcToCoin(NPCMOB* mob);
        extern void __stdcall npcToCoins();
    }
}

#endif