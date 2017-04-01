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



//Setup Code Start (when patched)
extern bool* Reconstructed::Util::isNotCoinTransformable = 0;



void Reconstructed::Util::npcToCoins_setup()
{
    //dbgbox(L"Test");
    isNotCoinTransformable = (bool*)calloc(NPC::MAX_ID + 1, sizeof(bool));
    isNotCoinTransformable[NPCID_PLAYERFIREBALL] = true;
    isNotCoinTransformable[NPCID_YOSHIFIRE] = true;
    isNotCoinTransformable[NPCID_SPRING] = true;
    
    isNotCoinTransformable[NPCID_BURIEDPLANT] = true;
    isNotCoinTransformable[NPCID_PLAYERHAMMER] = true;

    isNotCoinTransformable[NPCID_CLOWNCAR] = true;
    isNotCoinTransformable[NPCID_YELBLOCKS] = true;
    isNotCoinTransformable[NPCID_BLUBLOCKS] = true;
    isNotCoinTransformable[NPCID_GRNBLOCKS] = true;
    isNotCoinTransformable[NPCID_REDBLOCKS] = true;
    isNotCoinTransformable[NPCID_PLATFORM_SMB3] = true;
    isNotCoinTransformable[NPCID_ENEMYHAMMER] = true;
    isNotCoinTransformable[NPCID_BULLET_SMB3] = true;
    isNotCoinTransformable[NPCID_PEACHBOMB] = true;
    isNotCoinTransformable[NPCID_BOOMERANG] = true;
    isNotCoinTransformable[NPCID_SWORDBEAM] = true;
    isNotCoinTransformable[NPCID_CONVEYER] = true;
    isNotCoinTransformable[NPCID_METALBARREL] = true;
    isNotCoinTransformable[NPCID_TANKTREADS] = true;
    isNotCoinTransformable[NPCID_SHORTWOOD] = true;
    isNotCoinTransformable[NPCID_LONGWOOD] = true;
    isNotCoinTransformable[NPCID_SLANTWOOD_L] = true;
    isNotCoinTransformable[NPCID_SLANTWOOD_R] = true;
    isNotCoinTransformable[NPCID_SLANTWOOD_M] = true;

    isNotCoinTransformable[NPCID_BURIEDPLANT] = true;
    isNotCoinTransformable[NPCID_FIREBAR] = true;
    isNotCoinTransformable[NPCID_ROTODISK] = true;

    //isInteractableNPC_ptr
    isNotCoinTransformable[9] = true; //Mushroom
    isNotCoinTransformable[10] = true; //Coin
    isNotCoinTransformable[11] = true; //Star
    isNotCoinTransformable[14] = true; //Flower
    isNotCoinTransformable[16] = true; //Question Item
    isNotCoinTransformable[33] = true; //Coin
    isNotCoinTransformable[34] = true; //Leaf
    isNotCoinTransformable[41] = true; //Crystal Ball
    isNotCoinTransformable[75] = true; //Toad
    isNotCoinTransformable[88] = true; //Coin
    isNotCoinTransformable[90] = true; //Life
    isNotCoinTransformable[94] = true; //Toad
    isNotCoinTransformable[97] = true; //Star
    isNotCoinTransformable[101] = true; //Luigi
    isNotCoinTransformable[102] = true; //Link
    isNotCoinTransformable[103] = true; //Red Coin
    isNotCoinTransformable[107] = true; //Bob-omb
    isNotCoinTransformable[138] = true; //Coin
    isNotCoinTransformable[152] = true; //Ring
    isNotCoinTransformable[153] = true; //Killer Mushroom
    isNotCoinTransformable[169] = true; //Tanookie
    isNotCoinTransformable[170] = true; //Tanookie
    isNotCoinTransformable[178] = true; //Axe
    isNotCoinTransformable[182] = true; //Flower
    isNotCoinTransformable[183] = true; //Flower
    isNotCoinTransformable[184] = true; //Red mushroom
    isNotCoinTransformable[185] = true; //Red mushroom
    isNotCoinTransformable[186] = true; //Green mushroom
    isNotCoinTransformable[187] = true; //Green mushroom
    isNotCoinTransformable[188] = true; //Moon
    isNotCoinTransformable[192] = true; //Checkpoint
    isNotCoinTransformable[196] = true; //Star
    isNotCoinTransformable[197] = true; //Goalline
    isNotCoinTransformable[198] = true; //Peach
    isNotCoinTransformable[240] = true; //Stopwatch
    isNotCoinTransformable[248] = true; //Stopwatch
    isNotCoinTransformable[249] = true; //Mushroom
    isNotCoinTransformable[250] = true; //Heart
    isNotCoinTransformable[251] = true; //Green Ruby
    isNotCoinTransformable[252] = true; //Blue Ruby
    isNotCoinTransformable[253] = true; //Red Ruby
    isNotCoinTransformable[254] = true; //???
    isNotCoinTransformable[258] = true; //Blue Coin
    isNotCoinTransformable[264] = true; //Ice Flower
    isNotCoinTransformable[273] = true; //Random
    isNotCoinTransformable[274] = true; //Yoshi Coin
    isNotCoinTransformable[277] = true; //Ice Flower

    //isYoshi
    isNotCoinTransformable[95] = true;
    isNotCoinTransformable[98] = true;
    isNotCoinTransformable[99] = true;
    isNotCoinTransformable[100] = true;
    isNotCoinTransformable[148] = true;
    isNotCoinTransformable[149] = true;
    isNotCoinTransformable[150] = true;
    isNotCoinTransformable[228] = true;

    //isShoe
    isNotCoinTransformable[35] = true;
    isNotCoinTransformable[191] = true;
    isNotCoinTransformable[193] = true;

    //isVegetable
    isNotCoinTransformable[92] = true;
    isNotCoinTransformable[139] = true;
    isNotCoinTransformable[140] = true;
    isNotCoinTransformable[141] = true;
    isNotCoinTransformable[142] = true;
    isNotCoinTransformable[143] = true;
    isNotCoinTransformable[144] = true;
    isNotCoinTransformable[145] = true;
    isNotCoinTransformable[146] = true;
    isNotCoinTransformable[147] = true;

    //IsVine
    isNotCoinTransformable[213] = true;
    isNotCoinTransformable[214] = true;
    isNotCoinTransformable[215] = true;
    isNotCoinTransformable[216] = true;
    isNotCoinTransformable[217] = true;
    isNotCoinTransformable[218] = true;
    isNotCoinTransformable[219] = true;
    isNotCoinTransformable[220] = true;
    isNotCoinTransformable[221] = true;
    isNotCoinTransformable[222] = true;
    isNotCoinTransformable[223] = true;
    isNotCoinTransformable[224] = true;
}

void __stdcall Reconstructed::Util::npcToCoin(NPCMOB * mob)
{
    if (mob->unknown_124 && !mob->isGenerator) {
        //Get the id of the npc, we want to check if it is on the list of "non-transformable" npcs
        int id = mob->id;
        if (!mob->isHidden // (probably isHidden-Field)
            && !mob->killFlag // is currently not killed
            && !mob->friendly // is not friendly
            && !isCollectableGoalNPC_ptr[id]) { //check the built-in isCollectableGoal, if it is a collectable coin then skip
            if (isNotCoinTransformable[id]) { // check our array, if the npc is NOT Transformable
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
                EffectID effectID = EFFECTID_KICKED_COIN;
                //run the "kicked coin" effect with the new positioned (y + 32) npc. 
                native_runEffect((short*)&effectID, &mob->momentum, &animationFrame, &npcID, &mask);
                //play the "got coin" sound
                short soundID_GotCoin = 14;
                native_playSFX(&soundID_GotCoin);
                //if more than 100 coins then add a life
                if (++(GM_COINS) >= 100) {
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
                        (GM_COINS) -= 100; //and really, really remove those 100 coins.
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