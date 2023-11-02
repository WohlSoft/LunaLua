#include <comutil.h>
#include "string.h"
#include "../../Globals.h"
#include "../AsmPatch.h"
#include "../../Main.h"
#include "../MiscFuncs.h"

#define DEFINES_FFI_EXPORT(sig) extern "C" __declspec(dllexport) sig __cdecl

std::map<int, FIELDTYPE> definesLegalAddresses = {
    {0x00A3C86E, FT_BYTE},
    {0x00A3C87F, FT_BYTE},
    {0x00A3C891, FT_BYTE},
    {0x00A3C8EE, FT_BYTE},
    {0x00A266AC, FT_DWORD},
    {0x00A26719, FT_DWORD},
    {0x00A2677E, FT_DWORD},
    {0x00A262BD, FT_BYTE},
    {0x00A262C9, FT_BYTE},
    {0x00A262B7, FT_BYTE},
    {0x009DBD9A, FT_WORD},
    {0x009DBDFF, FT_WORD},
    {0x009DBE64, FT_WORD},
    {0x00A32943, FT_WORD},
    {0x00A329D5, FT_WORD},
    {0x00A32A6F, FT_WORD}
};
// set memory, only allowing a pre-defined set of addresses within program memory
DEFINES_FFI_EXPORT(bool) LunaLua_Defines_mem_set(int address, double value) {
    if (definesLegalAddresses.find(address) != definesLegalAddresses.end()) {
        FIELDTYPE ft = definesLegalAddresses[address];
        MemAssign(address, value, OP_Assign, ft);
        return true;
    }
    return false;
}

// patch that disables side grabbing
auto patch_player_grabSideEnabled = PATCH(0x009AD622).bytes(0x90, 0xE9);
DEFINES_FFI_EXPORT(void) LunaLua_Defines__player_grabSideEnabled__set(bool value) {
    if (value) {
        patch_player_grabSideEnabled.Unapply();
    }
    else {
        patch_player_grabSideEnabled.Apply();
    }
}
DEFINES_FFI_EXPORT(bool) LunaLua_Defines__player_grabSideEnabled__get() {
    return !patch_player_grabSideEnabled.IsPatched();
}

// patch that disables top grabbing
auto patch_player_grabTopEnabled = PATCH(0x009CC392).bytes(0x90, 0xE9);
DEFINES_FFI_EXPORT(void) LunaLua_Defines__player_grabTopEnabled__set(bool value) {
    if (value) {
        patch_player_grabTopEnabled.Unapply();
    }
    else {
        patch_player_grabTopEnabled.Apply();
    }
}
DEFINES_FFI_EXPORT(bool) LunaLua_Defines__player_grabTopEnabled__get() {
    return !patch_player_grabTopEnabled.IsPatched();
}


// patches that disable shell grabbing
auto patch_player_grabShellEnabled1 = PATCH(0x009ADA63).NOP_PAD_TO_SIZE<2>();
auto patch_player_grabShellEnabled2 = PATCH(0x009AC6C4).bytes(0x90, 0xE9);
DEFINES_FFI_EXPORT(void) LunaLua_Defines__player_grabShellEnabled__set(bool value) {
    if (value) {
        patch_player_grabShellEnabled1.Unapply();
        patch_player_grabShellEnabled2.Unapply();
    }
    else {
        patch_player_grabShellEnabled1.Apply();
        patch_player_grabShellEnabled2.Apply();
    }
}
DEFINES_FFI_EXPORT(bool) LunaLua_Defines__player_grabShellEnabled__get() {
    return !patch_player_grabShellEnabled1.IsPatched();
}

// path that disables link shield
auto patch_player_link_shieldEnabled = PATCH(0x00A53042).NOP_PAD_TO_SIZE<7>();
DEFINES_FFI_EXPORT(void) LunaLua_Defines__player_link_shieldEnabled__set(bool value) {
    if (value) {
        patch_player_link_shieldEnabled.Unapply();
    }
    else {
        patch_player_link_shieldEnabled.Apply();
    }
}
DEFINES_FFI_EXPORT(bool) LunaLua_Defines__player_link_shieldEnabled__get() {
    return !patch_player_link_shieldEnabled.IsPatched();
}

// path that disables link turning into a fairy when climbing
auto patch_player_link_fairyVineEnabled = PATCH(0x009AAF93).byte(0xFF);
DEFINES_FFI_EXPORT(void) LunaLua_Defines__player_link_fairyVineEnabled__set(bool value) {
    if (value) {
        patch_player_link_fairyVineEnabled.Unapply();
    }
    else {
        patch_player_link_fairyVineEnabled.Apply();
    }
}
DEFINES_FFI_EXPORT(bool) LunaLua_Defines__player_link_fairyVineEnabled__get() {
    return !patch_player_link_fairyVineEnabled.IsPatched();
}

// patches to disable pswitch overridding the music
auto patch_pswitch_music_0x9E330B = PATCH(0x9E330B).bytes(0x0F, 0x1F, 0x44, 0x00, 0x00);
auto patch_pswitch_music_0x9E3317 = PATCH(0x9E3317).bytes(0x90);
auto patch_pswitch_music_0x9E331C = PATCH(0x9E331C).bytes(0x0F, 0x1F, 0x44, 0x00, 0x00);
auto patch_pswitch_music_0x9E336B = PATCH(0x9E336B).bytes(0x0F, 0x1F, 0x44, 0x00, 0x00);
auto patch_pswitch_music_0x9E3396 = PATCH(0x9E3396).bytes(0x90);
auto patch_pswitch_music_0x9E3397 = PATCH(0x9E3397).bytes(0x0F, 0x1F, 0x44, 0x00, 0x00);
auto patch_pswitch_music_0xA747DD = PATCH(0xA747DD).bytes(0x90, 0x90);
auto patch_pswitch_music_0xA61DBE = PATCH(0xA61DBE).bytes(0x90, 0x90, 0x90, 0x90, 0x90, 0x90);
auto patch_pswitch_music_0x8E65F0 = PATCH(0x8E65F0).bytes(0xEB, 0x59);
auto patch_pswitch_music_0x8E5628 = PATCH(0x8E5628).bytes(0xEB, 0x59);
auto patch_pswitch_music_0x8CA4B9 = PATCH(0x8CA4B9).bytes(0xE9, 0x4A, 0x01, 0x00, 0x00, 0x90);
auto patch_pswitch_music_0x8CA52B = PATCH(0x8CA52B).bytes(0xE9, 0xD8, 0x00, 0x00, 0x00, 0x90);
DEFINES_FFI_EXPORT(void) LunaLua_Defines__pswitch_music__set(bool value) {
    if (value) {
        patch_pswitch_music_0x9E330B.Unapply();
        patch_pswitch_music_0x9E3317.Unapply();
        patch_pswitch_music_0x9E331C.Unapply();
        patch_pswitch_music_0x9E336B.Unapply();
        patch_pswitch_music_0x9E3396.Unapply();
        patch_pswitch_music_0x9E3397.Unapply();
        patch_pswitch_music_0xA747DD.Unapply();
        patch_pswitch_music_0xA61DBE.Unapply();
        patch_pswitch_music_0x8E65F0.Unapply();
        patch_pswitch_music_0x8E5628.Unapply();
        patch_pswitch_music_0x8CA4B9.Unapply();
        patch_pswitch_music_0x8CA52B.Unapply();
    }
    else {
        patch_pswitch_music_0x9E330B.Apply();
        patch_pswitch_music_0x9E3317.Apply();
        patch_pswitch_music_0x9E331C.Apply();
        patch_pswitch_music_0x9E336B.Apply();
        patch_pswitch_music_0x9E3396.Apply();
        patch_pswitch_music_0x9E3397.Apply();
        patch_pswitch_music_0xA747DD.Apply();
        patch_pswitch_music_0xA61DBE.Apply();
        patch_pswitch_music_0x8E65F0.Apply();
        patch_pswitch_music_0x8E5628.Apply();
        patch_pswitch_music_0x8CA4B9.Apply();
        patch_pswitch_music_0x8CA52B.Apply();
    }
}
DEFINES_FFI_EXPORT(bool) LunaLua_Defines__pswitch_music__get() {
    return !patch_pswitch_music_0x9E330B.IsPatched();
}

// path that disables the explosion effect for the zoomer, i guess?
auto patch_effect_Zoomer_killEffectEnabled = PATCH(0x00A33160).byte(0xFFFF);
DEFINES_FFI_EXPORT(void) LunaLua_Defines__effect_Zoomer_killEffectEnabled__set(bool value) {
    if (value) {
        patch_effect_Zoomer_killEffectEnabled.Unapply();
    }
    else {
        patch_effect_Zoomer_killEffectEnabled.Apply();
    }
}
DEFINES_FFI_EXPORT(bool) LunaLua_Defines__effect_Zoomer_killEffectEnabled__get() {
    return !patch_effect_Zoomer_killEffectEnabled.IsPatched();
}

#undef DEFINES_FFI_EXPORT
