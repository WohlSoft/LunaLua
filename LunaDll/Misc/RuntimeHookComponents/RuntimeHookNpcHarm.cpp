#include "../../Defines.h"
#include "../../Globals.h"
#include "../RuntimeHook.h"
#include <stack>
#include "../SMBXInternal/NPCs.h"

static bool npcHarmCancelled = false;
static bool npcHarmResultSet = false;

// Stub to execute the original npc collision function
_declspec(naked) static void __stdcall runtimeHookCollideNpc_OrigFunc(short* pNpcIdx, CollidersType* pObjType, short* pObjIdx)
{
    __asm {
        push ebp
        mov ebp, esp
        sub esp, 8
        push 0xa281b6
        ret
    }
}

// Logging function for diagnostics
void __stdcall runtimeHookLogCollideNpc(DWORD addr, short* pNpcIdx, CollidersType* pObjType, short* pObjIdx)
{
    printf("addr=%06x npcIdx=%d type=%d culprit=%d\n", addr, (int)*pNpcIdx, (int)*pObjType, (int)*pObjIdx);
    runtimeHookCollideNpc(pNpcIdx, pObjType, pObjIdx);
}



// npc array idx, initial npc id
std::vector<std::pair<short,short>> processingNPCInfoForIDChangeDetection;
// prevent onNPCTransform event from being executed twice in situations where we're already listening for ID changes
// called by npc::transform
void markNPCTransformationAsHandledByLua(short npcIdx, short oldID, short newID) {
    for (size_t i = 0; i < processingNPCInfoForIDChangeDetection.size(); i++) {
        // if the listening info matches the NPC that was transformed,
        if (processingNPCInfoForIDChangeDetection[i].first == npcIdx && processingNPCInfoForIDChangeDetection[i].second == oldID) {
            // update the info to reflect the new ID
            processingNPCInfoForIDChangeDetection[i].second = newID;
        }
    }
}
// defined in RuntimeHookNPCTransform.cpp
void executeOnNPCTransformIdx(int npcIdx, int oldID, NPCTransformationCause cause);

// Hook for the start of the original npc collision function
void __stdcall runtimeHookCollideNpc(short* pNpcIdx, CollidersType* pObjType, short* pObjIdx)
{
    npcHarmResultSet = false;

    NPCMOB* npc = NULL;
    bool isValidProcessingNPC = false;
    // execute post-hit call ONLY if this is a valid npc index
    if (*pNpcIdx > 0) {
        // prepare npc id on stack for post-hit call
        npc = NPC::Get(*pNpcIdx - 1);
        // store info to detect if the ID was changed
        isValidProcessingNPC = true;
        processingNPCInfoForIDChangeDetection.push_back({*pNpcIdx, npc->id});
    }
    runtimeHookCollideNpc_OrigFunc(pNpcIdx, pObjType, pObjIdx);
    // call post-hit function to handle onNPCTransform call
    if (isValidProcessingNPC) {
        // compare the current ID of the npc to the previously stored one...
        auto info = processingNPCInfoForIDChangeDetection.back();
        processingNPCInfoForIDChangeDetection.pop_back();
        // if the NPC's ID changed..
        if (info.second != npc->id) {
            // execute onNPCTransform event
            executeOnNPCTransformIdx(*pNpcIdx, info.second, NPC_TFCAUSE_HIT);
        }
    }
}

// Hook to catch when NPC harm is about to occur
static unsigned int __stdcall runtimeHookNpcHarm(short* pNpcIdx, CollidersType* pObjType, short* pObjIdx)
{
    if (!npcHarmResultSet && gLunaLua.isValid()) {
        std::shared_ptr<Event> npcKillEvent = std::make_shared<Event>("onNPCHarm", true);
        npcKillEvent->setDirectEventName("onNPCHarm");
        npcKillEvent->setLoopable(false);
        gLunaLua.callEvent(npcKillEvent, (int)*pNpcIdx, (int)*pObjType, (int)*pObjIdx);

        npcHarmCancelled = npcKillEvent->native_cancelled();
        npcHarmResultSet = true;
    }

    return npcHarmCancelled ? -1 : 0;
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a291d8(void)
{
    __asm {
        cmp word ptr [edx + ecx*4 + 0x12], 0
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa291da
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2922d
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a29272(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa29278
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a293ee(void)
{
    __asm {
        neg edx
        test edx, edi
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa293f0
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa293f7
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a29442(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa29448
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2946b(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa29471
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a294aa(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa294b0
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2954f
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a29f8a(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa29f90
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a24c(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a252
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a304(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a30a
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a36e(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a374
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a3e3(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a3e9
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a47b
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a454(void)
{
    __asm {
        cmp ax, 0xa
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a456
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a47b
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a48c(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a492
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a542(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a548
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a55e(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a564
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a57f(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a585
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a623
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a5f4(void)
{
    __asm {
        test ah, 1
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a5f6
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a608
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a627(void)
{
    __asm {
        cmp ax, 8
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a629
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a691
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a662(void)
{
    __asm {
        test ah, 1
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a664
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a676
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a695(void)
{
    __asm {
        cmp ax, 0xa
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a697
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a6a3
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a6d9(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a6df
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a704(void)
{
    __asm {
        cmp ax, 3
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a706
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a77e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a782(void)
{
    __asm {
        cmp ax, 6
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a784
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a79f
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a7a3(void)
{
    __asm {
        cmp ax, 0xa
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a7a5
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a7ca
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a7db(void)
{
    __asm {
        test ah, 1
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a7dd
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a801
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a90e(void)
{
    __asm {
        je testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a9ab
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2a914
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2a92c(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a932
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2aa5b(void)
{
    __asm {
        cmp ax, 6
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2aa5d
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2aa7b
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2aa7f(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2aa85
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2aaa3(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2aaa9
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2ab38
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2ab3c(void)
{
    __asm {
        cmp ax, 0xa
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2ab3e
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2ab7b
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2ab7f(void)
{
    __asm {
        cmp ax, 6
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2ab81
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2ab88
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2ab99(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2ab9f
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2ac38(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2ac3e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2acaf(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2acb5
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2ad2a(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2ad30
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2adeb
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2adca(void)
{
    __asm {
        je testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbb5
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2add0
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2adef(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2adf5
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2ae9e(void)
{
    __asm {
        je testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2aea4
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2af22(void)
{
    __asm {
        cmp ax, 9
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2af24
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2af30
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2af72(void)
{
    __asm {
        cmp ax, 9
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2af74
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2af80
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2b01d(void)
{
    __asm {
        cmp ax, 9
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2b01f
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2b02b
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2b116(void)
{
    __asm {
        je testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2b11c
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2b139(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a548
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2b13f
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2b676(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2b67c
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2b8b6(void)
{
    __asm {
        cmp word ptr [ecx + eax*8 + 0xe2], 0x6c
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2b8b8
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2b8cd
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2b8c4(void)
{
    __asm {
        cmp ax, 8
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2b8c6
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2b8cd
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2b9c0(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2b9c6
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2ba11(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2ba17
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2ba77(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2ba7d
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2badd(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2bae3
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2bcdc(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2bce2
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2bdeb(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2bdf1
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2be29(void)
{
    __asm {
        cmp word ptr [edx + ecx*8 + 0xe2], 0xd
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2be2b
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2be38
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2bf45(void)
{
    __asm {
        cmp word ptr [eax], 9
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2bf47
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2bf78
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2bfc9(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2bfcf
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c17a
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c130(void)
{
    __asm {
        cmp ax, 6
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c132
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c13b
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c13f(void)
{
    __asm {
        cmp ax, 0xa
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c141
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c17a
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c18b(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c191
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c300(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c306
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c404(void)
{
    __asm {
        cmp ax, 8
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c406
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c412
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c437(void)
{
    __asm {
        setne dl
        test ecx, edx
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c439
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c45d
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c4b1(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c4b7
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c50b(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c511
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c524(void)
{
    __asm {
        je testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2a548
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c52a
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c537(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c53d
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c5ad(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c5b3
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c63c
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c640(void)
{
    __asm {
        cmp ax, 3
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c642
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c69b
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c69f(void)
{
    __asm {
        cmp ax, 6
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c6a1
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c6bb
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c6bf(void)
{
    __asm {
        cmp ax, 0xa
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c6c1
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c701
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c712(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c718
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c7ab(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c7b1
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c87a
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c80c(void)
{
    __asm {
        cmp ax, 4
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c80e
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c81a
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c81e(void)
{
    __asm {
        cmp ax, 0xa
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c820
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c87a
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c88b(void)
{
    __asm {
        test ah, 1
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c88d
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2c89a
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c8a1(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c8a7
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2c966(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2c96c
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2cd3c(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2cd42
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2cfc8
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2cffe(void)
{
    __asm {
        je testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2d004
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2d1b9(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2d1bf
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2d2f5(void)
{
    __asm {
        cmp ax, 0x9e
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2d2f7
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2d303
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2d311(void)
{
    __asm {
        cmp cx, 6
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2d313
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2d31f
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2d79f(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2d857
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2d917
        ret
    safe:
        push 0xa2d7a5
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2d7ae(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2d857
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2d917
        ret
    safe:
        push 0xa2d7b4
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2d867(void)
{
    __asm {
        cmp ax, 0x1c
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2d869
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2d88f
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2d8ec(void)
{
    __asm {
        setne dl
        or ecx, edx
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2d8ee
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2d917
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2d96c(void)
{
    __asm {
        je testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2d972
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2d977(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2d97d
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2d9bf(void)
{
    __asm {
        cmp word ptr [esi + 0xe2], 0xf1
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2d9c1
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2d9d2
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2da36(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2da3c
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2dac8(void)
{
    __asm {
        cmp ax, 0x20
        je testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2dad6
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2daca
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2dace(void)
{
    __asm {
        cmp ax, 0xee
        je testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2dad6
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2dad0
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2dad4(void)
{
    __asm {
        cmp ax, 0xef
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2dad6
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2db1f
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2df6d(void)
{
    __asm {
        cmp ax, 0xa
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2df7d
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2df6f
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2df77(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2df7d
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e01d(void)
{
    __asm {
        setne bl
        test ecx, ebx
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e093
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2e01f
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e058(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e05e
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e0c2(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e0c8
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e155(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e15b
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2e274
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e278(void)
{
    __asm {
        cmp cx, 0xa
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e27a
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2e2e4
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e2f5(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e2fb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e556(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e7f0
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2e55c
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e641(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e6da
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2e647
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e64b(void)
{
    __asm {
        cmp ax, 0x18
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e6c8
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2e64d
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e6c2(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e6c8
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2e7f7
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e75c(void)
{
    __asm {
        cmp word ptr [edx + ecx*8 + 0xe2], 0x75
        jl testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e797
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2e75e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e795(void)
{
    __asm {
        cmp word ptr [eax + edx*8 + 0xe2], 0x78
        jle safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e797
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2e7f7
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2e800(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2e806
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2f26c(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2f2f5
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2f272
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2f27a(void)
{
    __asm {
        cmp word ptr [esi + 0xe2], 0x17
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2f2ef
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2f27c
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2f2ed(void)
{
    __asm {
        cmp word ptr [ecx + eax*8 + 0xe2], 0x6c
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2f2ef
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2f2fc
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2f376(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2f37c
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2f792(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2f798
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2f79d(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2f7a3
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2f822(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2f828
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2f8fa(void)
{
    __asm {
        cmp word ptr [ebx], 0xb3
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2f8fc
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2f909
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2f9bc(void)
{
    __asm {
        cmp ax, 6
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2f9be
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2f9ca
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2f9d9(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2f9df
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2fa5d(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fa63
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2fae5(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2faeb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2fb09(void)
{
    __asm {
        cmp ax, 3
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fb19
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2fb0b
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2fb13(void)
{
    __asm {
        jg safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fb19
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2fb29(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa3037e
        ret
    safe:
        push 0xa2fb2f
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2fb34(void)
{
    __asm {
        jne testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbbb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa3037e
        ret
    safe:
        push 0xa2fb3a
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2fbaf(void)
{
    __asm {
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbb5
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2fde3(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fde9
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2fef5
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2fef9(void)
{
    __asm {
        cmp ax, 0xa
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fefb
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2ff2f
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2ff33(void)
{
    __asm {
        cmp ax, 6
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2ff35
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2ff3c
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2ff4d(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2ff53
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3037e
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a2ff9f(void)
{
    __asm {
        setne cl
        test edx, ecx
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2ffa1
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa2ffec
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a30011(void)
{
    __asm {
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa30017
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa30128
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a3012c(void)
{
    __asm {
        cmp ax, 0xa
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa3012e
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa30162
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a30166(void)
{
    __asm {
        cmp ax, 6
        jne safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa30168
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa3016f
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a30180(void)
{
    __asm {
        je testharm
        jmp safe
    testharm:
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa2fbb5
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa30186
        ret
    }
}

__declspec(naked) void __stdcall runtimeHookNpcHarmRaw_a30465(void)
{
    __asm {
        cmp word ptr [esi + 0xe2], ax
        je safe
        pushfd
        push eax
        push ecx
        push edx
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xc]
        push dword ptr [ebp + 0x8]
        call runtimeHookNpcHarm
        cmp eax, 0
        jne restoresafe
        pop edx
        pop ecx
        pop eax
        popfd
        push 0xa30467
        ret
    restoresafe:
        pop edx
        pop ecx
        pop eax
        popfd
    safe:
        push 0xa304ae
        ret
    }
}
