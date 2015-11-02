#ifndef NpcToCoins_hhhh
#define NpcToCoins_hhhh

#include "../../NPCs.h"

namespace Reconstructed{
    namespace Util{

        extern bool* isNotCoinTransformable;

        extern void npcToCoins_setup();
        extern void __stdcall npcToCoin(NPCMOB* mob);
        extern void __stdcall npcToCoins();
    }
}

#endif