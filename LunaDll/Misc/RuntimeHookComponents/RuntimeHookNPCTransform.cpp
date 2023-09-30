#include <comutil.h>
#include "string.h"
#include "../../Globals.h"
#include "../RuntimeHook.h"
#include "../../LuaMain/LunaLuaMain.h"
#include "../../LuaMain/LuaProxy.h"
#include "../../Input/Input.h"
#include "../../GlobalFuncs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../SdlMusic/MusicManager.h"
#include "../../HardcodedGraphics/HardcodedGraphicsManager.h"
#include "../ErrorReporter.h"

#include "../AsmPatch.h"

#include "../../Rendering/GL/GLEngine.h"
#include "../../Main.h"
#include <IniProcessor/ini_processing.h>

#include "../RunningStat.h"
#include "../../Rendering/BitBltEmulation.h"
#include "../../Rendering/RenderUtils.h"
#include "../../Rendering/RenderOps/RenderStringOp.h"
#include "../../Rendering/WindowSizeHandler.h"

#include "../../SMBXInternal/NPCs.h"
#include "../../SMBXInternal/Blocks.h"
#include "../../SMBXInternal/Level.h"
#include "../../SMBXInternal/Sound.h"

#include "../PerfTracker.h"

#include "../../Misc/TestMode.h"
#include "../../Misc/WaitForTickEnd.h"
#include "../../Rendering/ImageLoader.h"
#include "../../Misc/LoadScreen.h"

#include "../../SMBXInternal/HardcodedGraphicsAccess.h"
#include "../../Rendering/LunaImage.h"

#include "../../libs/PGE_File_Formats/file_formats.h"

#include "../../Misc/VB6RNG.h"


// invoke the onNPCTransform event
void executeOnNPCTransformIdx(int npcIdx, int oldID, NPCTransformationCause cause)
{
    if (gLunaLua.isValid()) {
        // dispatch transform event
        std::shared_ptr<Event> npcTransformEvent = std::make_shared<Event>("onNPCTransform", false);
        npcTransformEvent->setDirectEventName("onNPCTransform");
        npcTransformEvent->setLoopable(false);
        gLunaLua.callEvent(npcTransformEvent, npcIdx, oldID, (int)cause);
    }
}
// invoke the event using the NPCMOB* pointer
// kind of ugly but it works
void executeOnNPCTransformPtr(NPCMOB* npc, int oldID, NPCTransformationCause cause)
{
    executeOnNPCTransformIdx(((int)(npc - (NPCMOB*)GM_NPCS_PTR) - 128), oldID, cause);
}


void __stdcall runtimeHookNPCTransformRandomVeggie_internal(NPCMOB* npc)
{
    // update size and position of the transformed NPC, to match basegame code overwritten by this patch
    npc->momentum.x = npc->momentum.x + npc->momentum.width / 2.0;
    npc->momentum.y = npc->momentum.y + npc->momentum.height / 2.0;
    npc->momentum.width = npc_width[npc->id];
    npc->momentum.height = npc_height[npc->id];
    npc->momentum.x = npc->momentum.x - npc->momentum.width / 2.0;
    npc->momentum.y = npc->momentum.y - npc->momentum.height / 2.0;
    executeOnNPCTransformPtr(npc, 147, NPC_TFCAUSE_AI);
}
_declspec(naked) void __stdcall runtimeHookNPCTransformRandomVeggie()
{
    __asm {
        // call function to handle transformation
        pushfd
        push eax
        push ebx
        push ecx
        push edx
        push esi
        push esi // pointer to the npc struct
        call runtimeHookNPCTransformRandomVeggie_internal
        pop esi
        pop edx
        pop ecx
        pop ebx
        pop eax
        popfd
        // jump to end position
        push 0xA0AE01
        ret
    }
}

void __stdcall runtimeHookNPCTransformSprout_internal(short* pNpcIdx)
{
    // replicate the basegame code that this hook overwrites
    native_setNPCFrame(pNpcIdx);
    // invoke transformation event
    executeOnNPCTransformIdx((int)*pNpcIdx, 91, NPC_TFCAUSE_CONTAINER);
}
const static int _transformSprouteJmpDestination = 0x9CCB46;
_declspec(naked) void __stdcall runtimeHookNPCTransformSprout()
{
    __asm {
        pushfd
        push eax
        push ebx
        push ecx
        push edx
        push esi
        push ecx // pointer to npc index
        call runtimeHookNPCTransformSprout_internal
        pop esi
        pop edx
        pop ecx
        pop ebx
        pop eax
        popfd

        jmp _transformSprouteJmpDestination
    }
}



void __stdcall runtimeHookNPCTransformRandomBonus_internal(NPCMOB* npc, int newType)
{
    // replicate the basegame code that this hook overwrites
    npc->id = newType;
    executeOnNPCTransformPtr(npc, 287, NPC_TFCAUSE_AI);
}
const static int _transformRandomBonusJmpDestination = 0xA4555F;
_declspec(naked) void __stdcall runtimeHookNPCTransformRandomBonus()
{
    __asm {
        pushfd
        push ebx
        push ecx
        push edx
        push esi
        push eax // new NPC type
        push esi // address of the NPC in memory
        call runtimeHookNPCTransformRandomBonus_internal
        pop esi
        pop edx
        pop ecx
        pop ebx
        popfd

        lea ecx, dword ptr ss : [ebp - 0x1EC] // instruction overwritten by this code
        jmp _transformRandomBonusJmpDestination
    }
}



void __stdcall runtimeHookNPCTransformMushToHeart_internal(int npcIdx)
{
    npcIdx -= 129;
    NPCMOB* npc = NPC::Get(npcIdx);
    // store previous ID to pass to event
    int oldID = npc->id;
    // replicate the basegame code that this hook overwrites
    npc->animationFrame = 0;
    npc->id = 250;
    npc->momentum.speedX = 0;
    npc->momentum.y += npc->momentum.height - npc_height[250] - 1;
    npc->momentum.x += npc->momentum.width / 2 - npc_width[250] / 2;
    npc->momentum.width = npc_width[250];
    npc->momentum.height = npc_height[250];
    // invoke transformation event
    executeOnNPCTransformIdx(npcIdx + 1, oldID, NPC_TFCAUSE_LINK);
}
const static int _transformMushToHeartJmpDestination = 0xA615F5;
_declspec(naked) void __stdcall runtimeHookNPCTransformMushToHeart()
{
    __asm {
        pushfd
        push ebx
        push ecx
        push esi
        push esi // NPC IDX
        call runtimeHookNPCTransformMushToHeart_internal
        pop esi
        pop ecx
        pop ebx
        popfd

        jmp _transformMushToHeartJmpDestination
    }
}



void __stdcall runtimeHookNPCTransformCoinToRupee_internal(int npcIdx)
{
    npcIdx -= 129;
    NPCMOB* npc = NPC::Get(npcIdx);
    // store previous ID to pass to event
    int oldID = npc->id;
    // replicate the basegame code that this hook overwrites
    if (npc->id == 258) {
        npc->id = 252;
    }
    else {
        npc->id = 251;
    }
    npc->momentum.y += npc->momentum.height - npc_height[npc->id];
    npc->momentum.x += npc->momentum.width / 2 - npc_width[npc->id] / 2;
    npc->momentum.width = npc_width[npc->id];
    npc->momentum.height = npc_height[npc->id];
    npc->animationFrame = 0;
    // invoke transformation event
    executeOnNPCTransformIdx(npcIdx + 1, oldID, NPC_TFCAUSE_LINK);
}
const static int _transformCoinToRupeeJmpDestination = 0xA61335;
_declspec(naked) void __stdcall runtimeHookNPCTransformCoinToRupee()
{
    __asm {
        pushfd
        push ebx
        push ecx
        push esi
        push esi // NPC IDX
        call runtimeHookNPCTransformCoinToRupee_internal
        pop esi
        pop ecx
        pop ebx
        popfd

        jmp _transformCoinToRupeeJmpDestination
    }
}



void __stdcall runtimeHookNPCTransformSnifitBulletToSMB2Coin_internal(NPCMOB* npc)
{
    // replicate the basegame code that this hook overwrites
    npc->momentum.x += npc->momentum.width / 2;
    npc->momentum.y += npc->momentum.height / 2;
    npc->id = 138;
    npc->momentum.width = npc_width[138];
    npc->momentum.height = npc_height[138];
    npc->momentum.x -= npc->momentum.width / 2;
    npc->momentum.y -= npc->momentum.height / 2;
    executeOnNPCTransformPtr(npc, 133, NPC_TFCAUSE_AI);
}
const static int _transformSnifitBulletToSMB2CoinJmpDestination = 0xA0B875;
_declspec(naked) void __stdcall runtimeHookNPCTransformSnifitBulletToSMB2Coin()
{
    __asm {
        pushfd
        push ebx
        push ecx
        push esi
        push esi // NPC ptr
        call runtimeHookNPCTransformSnifitBulletToSMB2Coin_internal
        pop esi
        pop ecx
        pop ebx
        popfd

        jmp _transformSnifitBulletToSMB2CoinJmpDestination
    }
}



void __stdcall runtimeHookNPCTransformHeldYoshiToEgg_internal(NPCMOB* npc)
{
    // store previous ID to pass to event
    int oldID = npc->id;
    // replicate the basegame code that this hook overwrites
    npc->ai1 = npc->id;
    npc->id = 96;
    executeOnNPCTransformPtr(npc, oldID, NPC_TFCAUSE_AI);
}
_declspec(naked) void __stdcall runtimeHookNPCTransformHeldYoshiToEgg()
{
    __asm {
        push ebx
        push ecx
        push esi
        push esi // NPC ptr
        call runtimeHookNPCTransformHeldYoshiToEgg_internal
        pop esi
        pop ecx
        pop ebx
        ret
    }
}


void __stdcall runtimeHookNPCTransformBubblePopped_internal(NPCMOB* npc)
{
    // replicate the basegame code that this hook overwrites
    if (npc->id == 134) {
        // if it's a bomb, set projectile flag
        npc->collidesWithNPC = -1;
    }
    executeOnNPCTransformPtr(npc, 283, NPC_TFCAUSE_CONTAINER);
}
_declspec(naked) void __stdcall runtimeHookNPCTransformBubblePopped()
{
    __asm {
        push ebx
        push ecx
        push esi
        push esi // NPC ptr
        call runtimeHookNPCTransformBubblePopped_internal
        pop esi
        pop ecx
        pop ebx
        ret
    }
}


void __stdcall runtimeHookNPCTransformSMWSpinyEgg_internal(NPCMOB* npc)
{
    // replicate the basegame code that this hook overwrites
    npc->momentum.speedX = GM_NPC_WALKSPEED * npc->directionFaced;
    executeOnNPCTransformPtr(npc, 286, NPC_TFCAUSE_AI);
}
_declspec(naked) void __stdcall runtimeHookNPCTransformSMWSpinyEgg()
{
    __asm {
        push ebx
        push ecx
        push esi
        push esi // NPC ptr
        call runtimeHookNPCTransformSMWSpinyEgg_internal
        pop esi
        pop ecx
        pop ebx
        ret
    }
}



void __stdcall runtimeHookNPCTransformLudwigShell_internal(NPCMOB* npc)
{
    executeOnNPCTransformPtr(npc, 280, NPC_TFCAUSE_AI);
}
const static int _transformLudwigShellJmpDestination = 0xA5211F;
_declspec(naked) void __stdcall runtimeHookNPCTransformLudwigShell()
{
    __asm {
        push ebx
        push ecx
        push esi
        push esi // NPC ptr
        call runtimeHookNPCTransformLudwigShell_internal
        pop esi
        pop ecx
        pop ebx

        jmp _transformLudwigShellJmpDestination
    }
}



void __stdcall runtimeHookNPCTransformKoopalingUnshell_internal(NPCMOB* npc)
{
    executeOnNPCTransformPtr(npc, npc->id + 1, NPC_TFCAUSE_AI);
}
const static int _transformKoopalingUnshellJmpDestination = 0xA52B74;
_declspec(naked) void __stdcall runtimeHookNPCTransformKoopalingUnshell()
{
    __asm {
        push ebx
        push ecx
        push esi
        push esi // NPC ptr
        call runtimeHookNPCTransformKoopalingUnshell_internal
        pop esi
        pop ecx
        pop ebx

        jmp _transformKoopalingUnshellJmpDestination
    }
}


void __stdcall runtimeHookNPCTransformPotionToDoor_internal(NPCMOB* npc)
{
    // replicate the basegame code that this hook overwrites
    npc->effect2 = 16;
    // invoke transformation event
    executeOnNPCTransformPtr(npc, 288, NPC_TFCAUSE_AI);
}
_declspec(naked) void __stdcall runtimeHookNPCTransformPotionToDoor()
{
    __asm {
        push ebx
        push ecx
        push esi
        push esi // NPC ptr
        call runtimeHookNPCTransformPotionToDoor_internal
        pop esi
        pop ecx
        pop ebx
        ret
    }
}


void __stdcall runtimeHookNPCTransformGaloombaUnflip_internal(NPCMOB* npc)
{
    // replicate the basegame code that this hook overwrites
    npc->momentum.y -= 1;
    // invoke transformation event
    executeOnNPCTransformPtr(npc, 166, NPC_TFCAUSE_AI);
}
const static int _transformGaloombaUnflipJmpDestination = 0xA5C14E;
_declspec(naked) void __stdcall runtimeHookNPCTransformGaloombaUnflip()
{
    __asm {
        push ebx
        push ecx
        push esi
        push esi // NPC ptr
        call runtimeHookNPCTransformGaloombaUnflip_internal
        pop esi
        pop ecx
        pop ebx
        jmp _transformGaloombaUnflipJmpDestination
    }
}





// this is kind of janky but the way it works is as follows:
// at the start of the code that may transform the npc, we store the current id for the npc held by yoshi
// at the end of the function, if that value was set, we check and execute the event if needed
int previousNPCID = -1;
int npcCheckIdx = -1;
void __stdcall runtimeHookYoshiEatPossibleNPCTransform_internal(int npcIdx)
{
    npcIdx -= 129;
    NPCMOB* npc = NPC::Get(npcIdx);
    previousNPCID = npc->id;
    npcCheckIdx = npcIdx;
}
_declspec(naked) void __stdcall runtimeHookYoshiEatPossibleNPCTransform()
{
    __asm {
        push eax
        push ebx
        push ecx
        push esi
        push ebx //npc idx
        call runtimeHookYoshiEatPossibleNPCTransform_internal
        pop esi
        pop ecx
        pop ebx
        pop eax
        ret
    }
}
// called when the above hooked function exits
void __stdcall runtimeHookYoshiEatExit_internal()
{
    if (npcCheckIdx != -1 && previousNPCID != -1)
    {
        NPCMOB* npc = NPC::Get(npcCheckIdx);
        if (npc->id != previousNPCID) {
            // NPC ID changed during yoshi mouth code
            // invoke transformation event
            executeOnNPCTransformIdx(npcCheckIdx + 1, previousNPCID, NPC_TFCAUSE_EATEN);
        }
        npcCheckIdx = -1;
        previousNPCID = -1;
    }
}
_declspec(naked) void __stdcall runtimeHookYoshiEatExit()
{
    __asm {

        push eax
        push ebx
        push ecx
        push esi
        call runtimeHookYoshiEatExit_internal
        pop esi
        pop ecx
        pop ebx
        pop eax

        // instruction overwritten by this hook
        mov dword ptr fs : [0] , ecx
        ret
    }
}




void __stdcall runtimeHookNPCTransformDespawned_internal(NPCMOB* npc)
{
    npc->crushPinched = 0;
    npc->crushMovingPinched = 0;
    if (npc->id != npc->spawnID) {
        // if the npc id changed during its life, change it back
        int oldID = npc->id;
        npc->id = npc->spawnID;

        // invoke transformation event
        executeOnNPCTransformPtr(npc, oldID, NPC_TFCAUSE_DESPAWN);
    }
}
_declspec(naked) void __stdcall runtimeHookNPCTransformDespawned()
{
    __asm {
        push ecx
        push esi // npc ptr
        call runtimeHookNPCTransformDespawned_internal
        pop ebx
        ret
    }
}




void __stdcall runtimeHookNPCTransformPSwitchResetRupeeCoins_internal(int npcIdx)
{
    npcIdx -= 129;
    NPCMOB* npc = NPC::Get(npcIdx);
    if (npc->id != npc->spawnID) {
        // reset npc id
        int oldID = npc->id;
        npc->id = npc->spawnID;
        // and invoke lua event
        executeOnNPCTransformIdx(npcIdx + 1, oldID, NPC_TFCAUSE_SWITCH);
    }
}
_declspec(naked) void __stdcall runtimeHookNPCTransformPSwitchResetRupeeCoins()
{
    __asm {
        pushfd
        push ebx
        push ecx
        push esi
        push edi
        push edi // npc idx
        call runtimeHookNPCTransformPSwitchResetRupeeCoins_internal
        pop edi
        pop esi
        pop ecx
        pop ebx
        popfd

        ret
    }
}



void __stdcall runtimeHookNPCTransformHeldSprout_internal(NPCMOB* npc)
{
    executeOnNPCTransformPtr(npc, 91, NPC_TFCAUSE_CONTAINER);
}
_declspec(naked) void __stdcall runtimeHookNPCTransformHeldSproutA()
{
    __asm {
        pushfd
        push ebx
        push ecx
        push esi
        push edi
        push esi // npc ptr
        call runtimeHookNPCTransformHeldSprout_internal
        pop edi
        pop esi
        pop ecx
        pop ebx
        popfd

        push 0xA0B6EC
        ret
    }
}
_declspec(naked) void __stdcall runtimeHookNPCTransformHeldSproutB()
{
    __asm {
        pushfd
        push ebx
        push ecx
        push esi
        push edi
        push esi // npc ptr
        call runtimeHookNPCTransformHeldSprout_internal
        pop edi
        pop esi
        pop ecx
        pop ebx
        popfd

        ret
    }
}



void __stdcall runtimeHookNPCTransformSMWKoopaEnterShell_internal(int npcIdx)
{
    npcIdx -= 129;

    // determine the old id from the current id
    NPCMOB* npc = NPC::Get(npcIdx);
    int oldID = npc->id + 4;
    if (npc->id == 194) {
        oldID = 116;
    }

    // and invoke lua event
    executeOnNPCTransformIdx(npcIdx + 1, oldID, NPC_TFCAUSE_AI);
}
_declspec(naked) void __stdcall runtimeHookNPCTransformSMWKoopaEnterShell()
{
    __asm {
        pushfd
        push ebx
        push ecx
        push esi
        push edi
        push edi // npc idx
        call runtimeHookNPCTransformSMWKoopaEnterShell_internal
        pop edi
        pop esi
        pop ecx
        pop ebx
        popfd

        ret
    }
}


void __stdcall runtimeHookNPCTransformYoshiEatRandomVeggie_internal(int npcIdx)
{
    npcIdx -= 129;
    executeOnNPCTransformIdx(npcIdx + 1, 147, NPC_TFCAUSE_AI);
}
_declspec(naked) void __stdcall runtimeHookNPCTransformYoshiEatRandomVeggie()
{
    __asm {
        push eax
        push ebx
        push ecx
        push esi
        push esi // NPC idx
        call runtimeHookNPCTransformYoshiEatRandomVeggie_internal
        pop esi
        pop ecx
        pop ebx
        pop eax
        ret
    }
}

