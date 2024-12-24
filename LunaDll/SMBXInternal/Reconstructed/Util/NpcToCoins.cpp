/*

Name: NpcToCoins
Function Signature: void __cdecl npcToCoins()

Description:
This Function is used to convert all onscreen npcs into coins.
It is mostly used by the ending function (if the Player hits the exit)

*/


#include "NpcToCoins.h"
#include "../../Defines.h"
#include "../../Animation.h"

EffectID Reconstructed::Util::npcToCoinEffect = EFFECTID_KICKED_COIN;
short Reconstructed::Util::npcToCoinSound = 14;
short Reconstructed::Util::npcToCoinValue = 1;
short Reconstructed::Util::npcToCoinValueReset = 100;

void __stdcall Reconstructed::Util::npcToCoin(NPCMOB * mob)
{
    if (mob->unknown_124 && !mob->isGenerator) {
        //Get the id of the npc, we want to check if it is on the list of "non-transformable" npcs
        int id = mob->id;
        if (!mob->isHidden // (probably isHidden-Field)
            && !mob->killFlag // is currently not killed
            && !mob->friendly // is not friendly
            && !isCollectableGoalNPC_ptr[id]) { //check the built-in isCollectableGoal, if it is a collectable coin then skip
            if (NPC::GetNotCoinTransformable(id)) { // check our array, if the npc is NOT Transformable
                if (id == NPCID_GOALTAPE //If it is NOT transformable then check for these special npcs
                    || id == NPCID_FIREBAR
                    || id == NPCID_ROTODISK)
                    mob->unknown_124 = 0; //and set this unknown field to 0
            }
            else {
                //If it is a transformable npc:

                mob->momentum.y = mob->momentum.y + 32.0; //now put the npc y pos +32 to offset the coin effect
                short mask = 0;
                short npcID = 0;
                float animationFrame = 1.0;
                EffectID effectID = npcToCoinEffect;
                //run the "kicked coin" effect with the new positioned (y + 32) npc. 
                native_runEffect((short*)&effectID, &mob->momentum, &animationFrame, &npcID, &mask);
                //play the "got coin" sound
                short soundID_GotCoin = npcToCoinSound;
                native_playSFX(&soundID_GotCoin);
                //if more than 100 coins then add a life
                GM_COINS += npcToCoinValue;
                if (GM_COINS >= 100) {
                    //if live more than 99 then reset to 99 coins anyway
                    if (GM_PLAYER_LIVES >= 99) {
                        GM_COINS = 99;
                    }
                    else {
                        //Otherwise just add a life to the life counter
                        short soundID_GotLife = 15;
                        //And play the sound for it
                        native_playSFX(&soundID_GotLife);
                        (GM_PLAYER_LIVES)++;
                        (GM_COINS) -= npcToCoinValueReset; //and really, really remove those 100 coins.
                    }
                }
                //Now put up the kill flags for the transformed npcs
                mob->killFlag = 9; // 9 = vanish
                mob->momentum.height = 0.0;
                mob->unknown_124 = 0;
            }
        }
    }
    else {
        //If it is a generator, then kill the generator
        if (mob->isGenerator == -1) {
            mob->killFlag = 9; // 9 = vanish
            mob->isHidden = -1;
        }
    }
}

//Setup Code End (when patched)

void __stdcall Reconstructed::Util::npcToCoins()
{
    //Go through every npc
    for (int i = 0; i < GM_NPCS_COUNT; i++){
        //Get next npc
        NPCMOB* nextNPC = NPC::Get(i);
        //If is not a generator
        npcToCoin(nextNPC);
    }
}