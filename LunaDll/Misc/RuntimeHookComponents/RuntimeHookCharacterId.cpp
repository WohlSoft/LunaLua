#include <unordered_map>
#include <memory>
#include <string>
#include "../AsmPatch.h"
#include "../../SMBXInternal/PlayerMOB.h"
#include "../../SMBXInternal/Blocks.h"
#include "../../SMBXInternal/Animation.h"
#include "../../Defines.h"
#include "../RuntimeHook.h"
#include "../../Rendering/ImageLoader.h"
#include "../../Rendering/LunaImage.h"
#include "../../Rendering/RenderOps/RenderBitmapOp.h"
#include "../../Globals.h"
#include "../../GlobalFuncs.h"


// Prototype
int __stdcall runtimeHookCharacterIdTranslateHook(short* idPtr);
static void __stdcall runtimeHookCharacterIdCopyTemplateToPlayer(int characterId, int playerIdx);
static void __stdcall runtimeHookCharacterIdAnimateBlocks(void);
//static int __stdcall runtimeHookCharacterIdBlockPlayerCheck(PlayerMOB* player, int blockIdx);
static int __stdcall runtimeHookCharacterIdSwitchBlockCheck(int blockId);
static void __stdcall runtimeHookCharacterIdSwitchBlockTransform(int playerIdx, Block* block);

// TODO: Consider moving the following hook outside of this file
static BOOL __stdcall PlayerBitBltHook(
    PlayerMOB* player,
    HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop
    );

static BOOL __stdcall OwPlayerBitBltHook(
    PlayerMOB* player,
    HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop
    );

static BOOL __stdcall OwSpriteBitBltHook(
    HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop
    );

static void __stdcall PlayerDeathAnimationHook(
    PlayerMOB* player,
    short* effectID, Momentum* coor, float* effectFrame, short* npcID, short* showOnlyMask
    );

static int __stdcall EffectUpdateHook(short* effectId);

// Data structures
struct CharacterDataStruct {
public:
    CharacterDataStruct(short id, const std::string& name, short base, short filterBlock, short switchBlock, short deathEffect)
    {
        mId = id;
        mName = name;
        mBaseCharacter = base;
        mFilterBlock = filterBlock;
        mSwitchBlock = switchBlock;
        mDeathEffect = deathEffect;
        memset(&mStoredTemplate, 0, sizeof(PlayerMOB));
        mStoredTemplate.Identity = (Characters)id;
        mStoredTemplate.CurrentPowerup = 1;
        mStoredTemplate.PowerupBoxContents = 0;
        mStoredTemplate.MountType = 0;
        mStoredTemplate.MountColor = 0;
        mStoredTemplate.Hearts = 1;
        memset(mHitbox, 0, 10*sizeof(CharacterHitBoxData));
        if (mFilterBlock != 0)
        {
            Blocks::SetBlockPlayerFilter(mFilterBlock, mId);
        }
    }
    ~CharacterDataStruct()
    {
        if (mFilterBlock != 0)
        {
            Blocks::SetBlockPlayerFilter(mFilterBlock, 0);
        }
        for (int powerupId = 1; powerupId <= 10; powerupId++)
        {
            ImageLoader::UnregisterExtraGfx(mName + "-" + std::to_string(powerupId));
        }
        ImageLoader::UnregisterExtraGfx("player-" + std::to_string(mId));
    }
public:
    short mId;
    std::string mName;
    short mBaseCharacter;
    short mFilterBlock;
    short mSwitchBlock;
    short mDeathEffect;
    PlayerMOB mStoredTemplate;
    CharacterHitBoxData mHitbox[10];
};

// Static Data
static std::unordered_map<short, std::unique_ptr<CharacterDataStruct>> runtimeHookCharacterIdMap;
static bool runtimeHookCharacterIdApplied = false;

// Declare assembly snippets
#define ASM_ARG(ARGUMENT) __asm { __asm pushfd __asm push eax __asm push ecx __asm push edx __asm lea eax, dword ptr ds : [ ARGUMENT ] __asm push eax __asm call runtimeHookCharacterIdTranslateHook }
//lea eax, dword ptr ds : [eax + F0]
//lea eax, dword ptr ds : [ebx + F0]
//lea eax, dword ptr ds : [ecx + F0]
//lea eax, dword ptr ds : [edi + F0]
//lea eax, dword ptr ds : [edx + F0]
//lea eax, dword ptr ds : [esi + F0]
//lea eax, dword ptr ds : [eax + edx + F0]
//lea eax, dword ptr ds : [ecx + eax + F0]
//lea eax, dword ptr ds : [eax + ecx * 4 + F0]
//lea eax, dword ptr ds : [eax + edx * 4 + F0]
//lea eax, dword ptr ds : [ecx + eax * 4 + F0]
//lea eax, dword ptr ds : [ecx + edx * 4 + F0]
//lea eax, dword ptr ds : [edx + eax * 4 + F0]
//lea eax, dword ptr ds : [edx + ecx * 4 + F0]
//lea eax, dword ptr ds : [eax + ecx * 4 + 0xF0]

#define ASM_TAIL_CMP(ARG) __asm { __asm pop edx __asm pop ecx __asm cmp ax, ARG __asm pop eax __asm lea esp, dword ptr ds : [esp + 4] __asm ret }
#define ASM_TAIL_CMP_0() ASM_TAIL_CMP(0)
#define ASM_TAIL_CMP_1() ASM_TAIL_CMP(1)
#define ASM_TAIL_CMP_2() ASM_TAIL_CMP(2)
#define ASM_TAIL_CMP_3() ASM_TAIL_CMP(3)
#define ASM_TAIL_CMP_4() ASM_TAIL_CMP(4)
#define ASM_TAIL_CMP_5() ASM_TAIL_CMP(5)
#define ASM_TAIL_CMP_6() ASM_TAIL_CMP(6)
#define ASM_TAIL_CMP_ax() __asm { __asm pop edx __asm pop ecx __asm cmp eax, dword ptr ss:[esp] __asm pop eax __asm lea esp, dword ptr ds : [esp + 4] __asm ret }
#define ASM_TAIL_CMP_cx() ASM_TAIL_CMP(cx)
#define ASM_TAIL_CMP_di() ASM_TAIL_CMP(di)
#define ASM_TAIL_CMP_dx() ASM_TAIL_CMP(dx)
#define ASM_TAIL_CMP_si() ASM_TAIL_CMP(si)
#define ASM_TAIL_CMP_dx_ax() __asm { __asm pop edx __asm pop ecx __asm cmp dx, ax __asm pop eax __asm lea esp, dword ptr ds : [esp + 4] __asm ret }
#define ASM_TAIL_MOV_ax() __asm { __asm pop edx __asm pop ecx __asm add esp, 4 __asm popfd __asm ret }
#define ASM_TAIL_MOV_cx() __asm { __asm pop edx __asm pop ecx __asm mov cx, ax __asm pop eax __asm popfd __asm ret }
#define ASM_TAIL_MOV_dx() __asm { __asm pop edx __asm pop ecx __asm mov dx, ax __asm pop eax __asm popfd __asm ret }
#define ASM_TAIL_MOV_eax() __asm { __asm pop edx __asm pop ecx __asm add esp, 4 __asm popfd __asm ret }
#define ASM_TAIL_MOV_ebx() __asm { __asm pop edx __asm pop ecx __asm mov ebx, eax __asm pop eax __asm popfd __asm ret }
#define ASM_TAIL_MOV_ecx() __asm { __asm pop edx __asm pop ecx __asm mov ecx, eax __asm pop eax __asm popfd __asm ret }
#define ASM_TAIL_MOV_edi() __asm { __asm pop edx __asm pop ecx __asm mov edi, eax __asm pop eax __asm popfd __asm ret }
#define ASM_TAIL_MOV_edx() __asm { __asm pop edx __asm pop ecx __asm mov edx, eax __asm pop eax __asm popfd __asm ret }
#define ASM_TAIL_MOV_esi() __asm { __asm pop edx __asm pop ecx __asm mov esi, eax __asm pop eax __asm popfd __asm ret }

#define DECL_HOOK(FUNCNAME, ARG, TAIL) __declspec(naked) static void  __stdcall FUNCNAME() { ASM_ARG(ARG); ASM_TAIL_##TAIL(); }

// Declare hook functions
// Some hooks are no longer needed due to the new way to load episodes on boot
//DECL_HOOK(HOOK_0x8C0329, esi + 0xF0, MOV_ebx);
//DECL_HOOK(HOOK_0x8C0362, esi + 0xF0, MOV_ebx);
//DECL_HOOK(HOOK_0x8C0B35, edi + 0xF0, MOV_ebx);
//DECL_HOOK(HOOK_0x8C0B6E, edi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x8D22B3, ecx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x8D376C, edx + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x8D3812, ecx + 0xF0, MOV_edx);
DECL_HOOK(HOOK_0x8D3DCB, eax + 0xF0, MOV_ecx);
DECL_HOOK(HOOK_0x8D3ED1, edx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x8D416E, edx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x8D5F06, edx + 0xF0, CMP_5);
//DECL_HOOK(HOOK_0x8E4857, ecx + eax * 4 + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x8E540F, ecx + eax * 4 + 0xF0, MOV_edi);
//DECL_HOOK(HOOK_0x8E5557, ecx + eax * 4 + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x8E5CB1, edx + ecx * 4 + 0xF0, CMP_1);
DECL_HOOK(HOOK_0x8E5CD8, ecx + eax * 4 + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x8E5F55, eax + ecx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x8E5F8E, eax + edx * 4 + 0xF0, CMP_0);
DECL_HOOK(HOOK_0x8E5FD3, ecx + eax * 4 + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x8E602B, eax + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x8E6033, eax + ecx * 4 + 0xF0, CMP_dx_ax);
DECL_HOOK(HOOK_0x8E606D, eax + ecx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x8E60A6, eax + edx * 4 + 0xF0, CMP_6);
DECL_HOOK(HOOK_0x8E60EB, ecx + eax * 4 + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x8E6143, eax + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x8E614B, eax + ecx * 4 + 0xF0, CMP_dx_ax);
DECL_HOOK(HOOK_0x8E6177, ecx + eax * 4 + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x8EBE05, edx + ecx * 4 + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x8EC124, edx + ecx * 4 + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x8EC236, edx + ecx * 4 + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x8EC555, edx + ecx * 4 + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x8EC667, edx + ecx * 4 + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x8EC986, edx + ecx * 4 + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x8ECA95, edx + ecx * 4 + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x8ECDB0, edx + ecx * 4 + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x8ECEC1, edx + ecx * 4 + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x8ED1E0, edx + ecx * 4 + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x8ED2F2, edx + ecx * 4 + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x8ED611, edx + ecx * 4 + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x8EDD83, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x8EE1FE, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x8EE679, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x8EEAF3, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x8EEF76, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x8EFF86, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x8F0205, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x8F0741, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x8F0BA1, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x8F0E20, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9032F4, esi + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x916B7E, ecx + 0xF0, CMP_1);
DECL_HOOK(HOOK_0x917FF7, ecx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x919470, ecx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x91A8E9, ecx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x91BE48, eax + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9238B0, edx + 0xF0, CMP_1);
DECL_HOOK(HOOK_0x923AFD, eax + 0xF0, MOV_ecx);
DECL_HOOK(HOOK_0x923F4F, edx + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9246CC, ecx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x924919, edx + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x924D67, ecx + 0xF0, MOV_edx);
DECL_HOOK(HOOK_0x9254E7, eax + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x925734, ecx + 0xF0, MOV_edx);
DECL_HOOK(HOOK_0x925B85, eax + 0xF0, MOV_ecx);
DECL_HOOK(HOOK_0x926305, edx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x926552, eax + 0xF0, MOV_ecx);
DECL_HOOK(HOOK_0x9269A4, edx + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x927121, ecx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x927345, ecx + 0xF0, MOV_edx);
DECL_HOOK(HOOK_0x927796, eax + 0xF0, MOV_ecx);
DECL_HOOK(HOOK_0x93DC2C, ecx + eax + 0xF0, MOV_edx);
DECL_HOOK(HOOK_0x93DDC7, eax + edx + 0xF0, CMP_1);
DECL_HOOK(HOOK_0x93E2C5, eax + edx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x93E7C3, eax + edx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x93ECC1, eax + edx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x93F1BF, eax + edx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x944970, eax + edx + 0xF0, MOV_ecx);
DECL_HOOK(HOOK_0x944AE6, eax + edx + 0xF0, CMP_1);
DECL_HOOK(HOOK_0x944F06, eax + edx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x945326, eax + edx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x945746, eax + edx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x945B66, eax + edx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x96C063, eax + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x96E1A7, eax + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x974CFC, eax + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x974EE5, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x9751FA, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x977227, edx + ecx * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x97D9AB, eax + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x97DBA0, eax + ecx * 4 + 0xF0, MOV_cx);
DECL_HOOK(HOOK_0x97DE9B, eax + ecx * 4 + 0xF0, MOV_cx);
DECL_HOOK(HOOK_0x97FCC5, eax + edx * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x980FFC, eax + edx * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x98735A, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x9873FE, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x9874A2, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x987546, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x9875EA, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x9877E8, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x98788C, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x987930, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x9879D4, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x987A78, ecx + edx * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x98A331, edx + 0xF0, CMP_1);
DECL_HOOK(HOOK_0x98B92C, ecx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x98CF29, eax + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x98DF6B, ecx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x98F6B1, edx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x98FBE9, edx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x98FBF6, eax + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x994764, esi + 0xF0, CMP_dx);
DECL_HOOK(HOOK_0x994A24, esi + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x994A78, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x994AD7, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x994AE8, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x994B2D, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x994D83, esi + 0xF0, CMP_ax);
DECL_HOOK(HOOK_0x994F4C, esi + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9953AC, ecx + eax * 4 + 0xF0, MOV_dx);
DECL_HOOK(HOOK_0x99792D, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x99799D, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x997CA1, ebx + 0xF0, MOV_cx);
DECL_HOOK(HOOK_0x998040, ebx + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x998085, ebx + 0xF0, MOV_esi);
DECL_HOOK(HOOK_0x9980E6, ebx + 0xF0, MOV_esi);
DECL_HOOK(HOOK_0x998614, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9987FF, ebx + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x998844, ebx + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x998889, ebx + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x999449, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x999470, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9994E1, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x99966B, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x999833, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x999866, ebx + 0xF0, MOV_esi);
DECL_HOOK(HOOK_0x9999F4, ebx + 0xF0, CMP_si);
DECL_HOOK(HOOK_0x999AB5, ebx + 0xF0, CMP_si);
DECL_HOOK(HOOK_0x999B1A, ebx + 0xF0, CMP_cx);
DECL_HOOK(HOOK_0x999B44, ebx + 0xF0, CMP_si);
DECL_HOOK(HOOK_0x999B69, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x999BA0, ebx + 0xF0, CMP_cx);
DECL_HOOK(HOOK_0x999BCA, ebx + 0xF0, CMP_si);
DECL_HOOK(HOOK_0x999BEF, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x999C15, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x999C94, ebx + 0xF0, CMP_cx);
DECL_HOOK(HOOK_0x999CB9, ebx + 0xF0, CMP_si);
DECL_HOOK(HOOK_0x999CDE, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x999E1B, ebx + 0xF0, CMP_si);
DECL_HOOK(HOOK_0x999E7E, ebx + 0xF0, CMP_cx);
DECL_HOOK(HOOK_0x999EA8, ebx + 0xF0, CMP_si);
DECL_HOOK(HOOK_0x999ECD, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x999F04, ebx + 0xF0, CMP_cx);
DECL_HOOK(HOOK_0x999F2E, ebx + 0xF0, CMP_si);
DECL_HOOK(HOOK_0x999F53, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x999F79, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x999FF8, ebx + 0xF0, CMP_cx);
DECL_HOOK(HOOK_0x99A01D, ebx + 0xF0, CMP_si);
DECL_HOOK(HOOK_0x99A042, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x99A16B, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99A196, ebx + 0xF0, CMP_si);
DECL_HOOK(HOOK_0x99A1BB, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x99A2B5, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x99A607, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x99A6A0, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x99A6E6, ebx + 0xF0, CMP_1);
DECL_HOOK(HOOK_0x99A71B, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99A750, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x99A785, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x99A7B7, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x99A850, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x99ABA6, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x99B7C8, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99BB7B, ebx + 0xF0, MOV_esi);
DECL_HOOK(HOOK_0x99C017, ebx + 0xF0, MOV_esi);
DECL_HOOK(HOOK_0x99C096, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99C510, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99CB3B, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99CEED, ebx + 0xF0, MOV_esi);
DECL_HOOK(HOOK_0x99D00C, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99D40A, ebx + 0xF0, MOV_esi);
DECL_HOOK(HOOK_0x99D4AD, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x99D70E, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x99D736, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99D83D, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99D882, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x99E265, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x99E33A, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99E6AC, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99EB43, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99EC52, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x99ED2F, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x99EE6F, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x99EEB3, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x99F03E, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x99F0A1, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x99F0DD, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x99F12F, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x99F57A, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x99F62E, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9A008E, ebx + 0xF0, MOV_ecx);
DECL_HOOK(HOOK_0x9A04B6, ebx + 0xF0, MOV_esi);
DECL_HOOK(HOOK_0x9A13BE, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9A32CB, ebx + 0xF0, CMP_2);
// Character ! blocks
// DECL_HOOK(HOOK_0x9A3CD9, ebx + 0xF0, CMP_1);
// DECL_HOOK(HOOK_0x9A3D13, ebx + 0xF0, CMP_2);
// DECL_HOOK(HOOK_0x9A3D4D, ebx + 0xF0, CMP_3);
// DECL_HOOK(HOOK_0x9A3D87, ebx + 0xF0, CMP_4);
// DECL_HOOK(HOOK_0x9A3DC1, ebx + 0xF0, CMP_5);
// runs after collision loop, duplicated by runtimeHookPlayerBlockCollisionEnd
//DECL_HOOK(HOOK_0x9A5015, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9A504D, ebx + 0xF0, MOV_esi);
DECL_HOOK(HOOK_0x9A5367, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9A5377, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9A538B, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x9A5C52, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9A5DA6, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9A6856, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9A69A9, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9A72FE, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9A75A5, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9A8348, eax + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9AA469, edx + ecx * 4 + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9AA6B9, eax + edx * 4 + 0xF0, MOV_ecx);
DECL_HOOK(HOOK_0x9AAA0E, eax + 0xF0, CMP_5);
// If Link, become fairy for climbing. Conflicts with defines.player_link_fairyVineEnabled so commented out, and all current extended characters use defines.player_link_fairyVineEnabled to disable this anyway.
//DECL_HOOK(HOOK_0x9AAF8C, ecx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9ABE80, eax + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9ABFB5, edi + 0xF0, MOV_esi);
DECL_HOOK(HOOK_0x9AC153, edx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9AC705, ecx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9AD678, edx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9AD6FB, edi + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9ADAC0, ecx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9AEA0D, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9AEB73, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9B081A, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x9B0863, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9B08F3, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9B11DB, ebx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9B1A55, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9B55CB, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9B57F0, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9B5D21, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9B5D71, esi + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9B61B4, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9B61DD, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9B6222, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9B68C9, ecx + eax * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9B6C34, edi + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9B6F12, edx + ecx * 4 + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9B6FD4, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9B71D3, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9B722D, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9B760F, edx + ecx * 4 + 0xF0, CMP_0);
DECL_HOOK(HOOK_0x9B7D86, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9B86E4, esi + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9B8DFB, esi + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9B9B24, esi + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9B9B88, esi + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9B9BD7, esi + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9B9CD4, esi + 0xF0, CMP_dx);
DECL_HOOK(HOOK_0x9B9D0E, esi + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9BA1D7, esi + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9BA28E, esi + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9BA847, esi + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9BA8F3, esi + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9BA956, esi + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9BAAAC, esi + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9BB469, eax + 0xF0, MOV_cx);
DECL_HOOK(HOOK_0x9BB886, ecx + eax * 4 + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x9BE92D, edx + ecx * 4 + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9C0D14, ecx + 0xF0, MOV_edx);
DECL_HOOK(HOOK_0x9C0EA8, ecx + 0xF0, MOV_edx);
DECL_HOOK(HOOK_0x9C0F5E, edx + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9C1008, edx + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9C10BE, edx + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9C119E, ecx + 0xF0, MOV_edx);
DECL_HOOK(HOOK_0x9C1267, eax + 0xF0, MOV_ecx);
DECL_HOOK(HOOK_0x9C131E, edx + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9C140B, edx + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9C1BAD, eax + 0xF0, MOV_ecx);
DECL_HOOK(HOOK_0x9C1C9D, eax + 0xF0, MOV_ecx);
DECL_HOOK(HOOK_0x9C47DE, edi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9C4817, edi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9C66D1, ebx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x9C6845, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9C6B0C, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9C6E51, ebx + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9C6F03, edx + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9C7378, esi + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9C76B4, edx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9C788F, edx + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x9C79D8, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9C7E49, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9C7EF5, edx + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9C80EC, ecx + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9C8176, ecx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9C8388, edi + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9C8D32, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9C8E62, ebx + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9C94A9, ebx + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9C966B, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9CA23B, edi + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9CB66C, eax + edx * 4 + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9CBA23, edx + ecx * 4 + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9CC44A, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9CC45D, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9CC474, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9CE004, ebx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9CE05E, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9CF1E7, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9D0402, ebx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0x9D05BB, edx + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D079B, edx + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9D214B, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D215E, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D21D5, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D21E8, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D2251, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D2296, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D22F6, esi + 0xF0, CMP_cx);
DECL_HOOK(HOOK_0x9D2360, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D23C3, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D23D6, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D244D, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D2460, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D24C9, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D250E, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D2553, esi + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9D2664, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D26A9, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D26D5, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D2877, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D288A, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D2901, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D2914, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D297D, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D29C2, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D2A32, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D2A45, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D2ABC, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D2ACF, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D2B38, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D2B7D, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D2BEE, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D2C01, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D2C78, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D2C8B, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D2CF4, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D2D39, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D2DAF, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D2DF4, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D2E20, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D2FE5, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D302A, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D3056, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D33DC, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D3A82, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D3D84, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D3E6B, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D4178, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D425F, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D461E, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D4868, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D4B47, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D4C2F, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D4E6E, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D4F55, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D575B, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D5A0A, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D5C1E, esi + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9D5C45, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D5D2C, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D5E92, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D5F79, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D614D, esi + 0xF0, CMP_3);
DECL_HOOK(HOOK_0x9D6174, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D625B, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D6396, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D647D, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D6B14, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D6CD5, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9D6CE8, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9D7311, eax + edx * 4 + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D7437, eax + edx * 4 + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D7571, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D81E0, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9D82CE, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9D82F8, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D830B, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D8382, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D8395, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D83FE, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D8443, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D84A1, esi + 0xF0, CMP_cx);
DECL_HOOK(HOOK_0x9D850C, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D85E7, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9D86D5, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9D86FF, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D8712, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D8789, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D879C, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D8805, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D884A, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D88A8, esi + 0xF0, CMP_cx);
DECL_HOOK(HOOK_0x9D8913, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D89FD, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D8A10, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D8A87, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D8A9A, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D8B0C, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D8B51, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D8BAF, esi + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x9D8C23, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D8E29, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D8E3C, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D8EB3, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D8EC6, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D8F38, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D8F7D, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D8FDB, esi + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x9D904F, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D923D, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D9250, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D92C7, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D92DA, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D934C, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D9391, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9D93EF, esi + 0xF0, CMP_di);
DECL_HOOK(HOOK_0x9D9463, esi + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0x9D9AF3, esi + 0xF0, MOV_eax);
DECL_HOOK(HOOK_0x9D9C6A, esi + 0xF0, MOV_eax);
// Get template:
// DECL_HOOK(HOOK_0x9DA7CA, edx + ecx * 4 + 0xF0, MOV_esi);
// Character change block can activate
// DECL_HOOK(HOOK_0x9DA827, ecx + eax * 4 + 0xF0, CMP_1);
// DECL_HOOK(HOOK_0x9DA863, ecx + eax * 4 + 0xF0, CMP_2);
// DECL_HOOK(HOOK_0x9DA893, ecx + eax * 4 + 0xF0, CMP_3);
// DECL_HOOK(HOOK_0x9DA8C3, ecx + eax * 4 + 0xF0, CMP_di);
// DECL_HOOK(HOOK_0x9DA8F2, ecx + eax * 4 + 0xF0, CMP_5);
// Set template:
// DECL_HOOK(HOOK_0x9DAAA2, eax + edx * 4 + 0xF0, MOV_ebx);
// DECL_HOOK(HOOK_0x9DAB17, ecx + eax * 4 + 0xF0, MOV_ebx);
// DECL_HOOK(HOOK_0x9DAB8C, edx + ecx * 4 + 0xF0, MOV_ebx);
// DECL_HOOK(HOOK_0x9DABFE, eax + edx * 4 + 0xF0, MOV_ebx);
// DECL_HOOK(HOOK_0x9DAC73, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9DAD04, esi + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9DB9CC, eax + edx * 4 + 0xF0, CMP_4);
DECL_HOOK(HOOK_0x9DBA82, ecx + eax * 4 + 0xF0, CMP_2);
DECL_HOOK(HOOK_0x9DBAAB, eax + edx * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9DBD7E, edx + ecx * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9DC126, ecx + eax * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9DCC3F, eax + edx * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9DCD40, edx + ecx * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9DCE6D, ecx + eax * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9DCF6E, eax + edx * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0x9DDE82, edx + ecx * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9DDF1E, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9DE629, eax + edx * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9DE6C4, edx + ecx * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9DEF81, edx + ecx * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9DF01D, ecx + eax * 4 + 0xF0, MOV_ebx);
DECL_HOOK(HOOK_0x9E086C, eax + edx * 4 + 0xF0, CMP_4);
// Character block animation
// DECL_HOOK(HOOK_0x9E19BE, eax + edx * 4 + 0xF0, CMP_1);
// DECL_HOOK(HOOK_0x9E19F3, eax + edx * 4 + 0xF0, CMP_2);
// DECL_HOOK(HOOK_0x9E1A28, eax + edx * 4 + 0xF0, CMP_3);
// DECL_HOOK(HOOK_0x9E1A63, ecx + edx * 4 + 0xF0, CMP_ax);
// DECL_HOOK(HOOK_0x9E1A96, ecx + eax * 4 + 0xF0, CMP_5);
// DECL_HOOK(HOOK_0x9E1BD6, edx + ecx * 4 + 0xF0, CMP_5);
// DECL_HOOK(HOOK_0x9E1BF9, ecx + eax * 4 + 0xF0, MOV_esi);
DECL_HOOK(HOOK_0xA02866, eax + ecx * 4 + 0xF0, MOV_cx);
DECL_HOOK(HOOK_0xA02899, edx + ecx * 4 + 0xF0, CMP_0);
//DECL_HOOK(HOOK_0xA028F1, eax + edx * 4 + 0xF0, MOV_edi);
DECL_HOOK(HOOK_0xA02A72, edx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0xA02AA4, eax + ecx * 4 + 0xF0, CMP_di);
DECL_HOOK(HOOK_0xA03801, edx + ecx * 4 + 0xF0, CMP_3);
DECL_HOOK(HOOK_0xA03828, ecx + eax * 4 + 0xF0, CMP_4);
DECL_HOOK(HOOK_0xA03AD5, edx + eax * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0xA243F0, edx + ecx * 4 + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0xA2509E, ecx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0xA256A7, ecx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0xA25DED, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0xA25E16, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0xA25F58, esi + 0xF0, CMP_ax);
DECL_HOOK(HOOK_0xA25F64, esi + 0xF0, CMP_ax);
DECL_HOOK(HOOK_0xA2771F, ecx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0xA27800, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0xA27835, ecx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0xA27912, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0xA2793B, esi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0xA27A06, ecx + 0xF0, MOV_ax);
DECL_HOOK(HOOK_0xA27AA7, edi + 0xF0, CMP_5);
DECL_HOOK(HOOK_0xA291A5, edx + ecx * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0xA4456C, ecx + eax * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0xA52D55, eax + edx * 4 + 0xF0, CMP_5);
DECL_HOOK(HOOK_0xA60B49, ecx + eax * 4 + 0xF0, CMP_1);
DECL_HOOK(HOOK_0xA60B6B, eax + edx * 4 + 0xF0, CMP_2);
DECL_HOOK(HOOK_0xA60B96, edx + ecx * 4 + 0xF0, CMP_3);
DECL_HOOK(HOOK_0xA60BB9, ecx + eax * 4 + 0xF0, CMP_4);
DECL_HOOK(HOOK_0xA60BE3, eax + edx * 4 + 0xF0, CMP_5);

// Patches
//static auto patch_0x8C0329 = PATCH(0x8C0329).CALL(HOOK_0x8C0329).NOP_PAD_TO_SIZE<7>();
//static auto patch_0x8C0362 = PATCH(0x8C0362).CALL(HOOK_0x8C0362).NOP_PAD_TO_SIZE<7>();
//static auto patch_0x8C0B35 = PATCH(0x8C0B35).CALL(HOOK_0x8C0B35).NOP_PAD_TO_SIZE<7>();
//static auto patch_0x8C0B6E = PATCH(0x8C0B6E).CALL(HOOK_0x8C0B6E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x8D22B3 = PATCH(0x8D22B3).CALL(HOOK_0x8D22B3).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8D376C = PATCH(0x8D376C).CALL(HOOK_0x8D376C).NOP_PAD_TO_SIZE<7>();
static auto patch_0x8D3812 = PATCH(0x8D3812).CALL(HOOK_0x8D3812).NOP_PAD_TO_SIZE<7>();
static auto patch_0x8D3DCB = PATCH(0x8D3DCB).CALL(HOOK_0x8D3DCB).NOP_PAD_TO_SIZE<7>();
static auto patch_0x8D3ED1 = PATCH(0x8D3ED1).CALL(HOOK_0x8D3ED1).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8D416E = PATCH(0x8D416E).CALL(HOOK_0x8D416E).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8D5F06 = PATCH(0x8D5F06).CALL(HOOK_0x8D5F06).NOP_PAD_TO_SIZE<8>();
//static auto patch_0x8E4857 = PATCH(0x8E4857).CALL(HOOK_0x8E4857).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8E540F = PATCH(0x8E540F).CALL(HOOK_0x8E540F).NOP_PAD_TO_SIZE<8>();
//static auto patch_0x8E5557 = PATCH(0x8E5557).CALL(HOOK_0x8E5557).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8E5CB1 = PATCH(0x8E5CB1).CALL(HOOK_0x8E5CB1).NOP_PAD_TO_SIZE<9>();
static auto patch_0x8E5CD8 = PATCH(0x8E5CD8).CALL(HOOK_0x8E5CD8).NOP_PAD_TO_SIZE<9>();
static auto patch_0x8E5F55 = PATCH(0x8E5F55).CALL(HOOK_0x8E5F55).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8E5F8E = PATCH(0x8E5F8E).CALL(HOOK_0x8E5F8E).NOP_PAD_TO_SIZE<9>();
static auto patch_0x8E5FD3 = PATCH(0x8E5FD3).CALL(HOOK_0x8E5FD3).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8E602B = PATCH(0x8E602B).CALL(HOOK_0x8E602B).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8E6033 = PATCH(0x8E6033).CALL(HOOK_0x8E6033).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8E606D = PATCH(0x8E606D).CALL(HOOK_0x8E606D).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8E60A6 = PATCH(0x8E60A6).CALL(HOOK_0x8E60A6).NOP_PAD_TO_SIZE<9>();
static auto patch_0x8E60EB = PATCH(0x8E60EB).CALL(HOOK_0x8E60EB).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8E6143 = PATCH(0x8E6143).CALL(HOOK_0x8E6143).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8E614B = PATCH(0x8E614B).CALL(HOOK_0x8E614B).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8E6177 = PATCH(0x8E6177).CALL(HOOK_0x8E6177).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EBE05 = PATCH(0x8EBE05).CALL(HOOK_0x8EBE05).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EC124 = PATCH(0x8EC124).CALL(HOOK_0x8EC124).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EC236 = PATCH(0x8EC236).CALL(HOOK_0x8EC236).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EC555 = PATCH(0x8EC555).CALL(HOOK_0x8EC555).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EC667 = PATCH(0x8EC667).CALL(HOOK_0x8EC667).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EC986 = PATCH(0x8EC986).CALL(HOOK_0x8EC986).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8ECA95 = PATCH(0x8ECA95).CALL(HOOK_0x8ECA95).NOP_PAD_TO_SIZE<9>();
static auto patch_0x8ECDB0 = PATCH(0x8ECDB0).CALL(HOOK_0x8ECDB0).NOP_PAD_TO_SIZE<9>();
static auto patch_0x8ECEC1 = PATCH(0x8ECEC1).CALL(HOOK_0x8ECEC1).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8ED1E0 = PATCH(0x8ED1E0).CALL(HOOK_0x8ED1E0).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8ED2F2 = PATCH(0x8ED2F2).CALL(HOOK_0x8ED2F2).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8ED611 = PATCH(0x8ED611).CALL(HOOK_0x8ED611).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EDD83 = PATCH(0x8EDD83).CALL(HOOK_0x8EDD83).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EE1FE = PATCH(0x8EE1FE).CALL(HOOK_0x8EE1FE).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EE679 = PATCH(0x8EE679).CALL(HOOK_0x8EE679).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EEAF3 = PATCH(0x8EEAF3).CALL(HOOK_0x8EEAF3).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EEF76 = PATCH(0x8EEF76).CALL(HOOK_0x8EEF76).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8EFF86 = PATCH(0x8EFF86).CALL(HOOK_0x8EFF86).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8F0205 = PATCH(0x8F0205).CALL(HOOK_0x8F0205).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8F0741 = PATCH(0x8F0741).CALL(HOOK_0x8F0741).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8F0BA1 = PATCH(0x8F0BA1).CALL(HOOK_0x8F0BA1).NOP_PAD_TO_SIZE<8>();
static auto patch_0x8F0E20 = PATCH(0x8F0E20).CALL(HOOK_0x8F0E20).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9032F4 = PATCH(0x9032F4).CALL(HOOK_0x9032F4).NOP_PAD_TO_SIZE<7>();
static auto patch_0x916B7E = PATCH(0x916B7E).CALL(HOOK_0x916B7E).NOP_PAD_TO_SIZE<8>();
static auto patch_0x917FF7 = PATCH(0x917FF7).CALL(HOOK_0x917FF7).NOP_PAD_TO_SIZE<8>();
static auto patch_0x919470 = PATCH(0x919470).CALL(HOOK_0x919470).NOP_PAD_TO_SIZE<8>();
static auto patch_0x91A8E9 = PATCH(0x91A8E9).CALL(HOOK_0x91A8E9).NOP_PAD_TO_SIZE<8>();
static auto patch_0x91BE48 = PATCH(0x91BE48).CALL(HOOK_0x91BE48).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9238B0 = PATCH(0x9238B0).CALL(HOOK_0x9238B0).NOP_PAD_TO_SIZE<8>();
static auto patch_0x923AFD = PATCH(0x923AFD).CALL(HOOK_0x923AFD).NOP_PAD_TO_SIZE<7>();
static auto patch_0x923F4F = PATCH(0x923F4F).CALL(HOOK_0x923F4F).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9246CC = PATCH(0x9246CC).CALL(HOOK_0x9246CC).NOP_PAD_TO_SIZE<8>();
static auto patch_0x924919 = PATCH(0x924919).CALL(HOOK_0x924919).NOP_PAD_TO_SIZE<7>();
static auto patch_0x924D67 = PATCH(0x924D67).CALL(HOOK_0x924D67).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9254E7 = PATCH(0x9254E7).CALL(HOOK_0x9254E7).NOP_PAD_TO_SIZE<8>();
static auto patch_0x925734 = PATCH(0x925734).CALL(HOOK_0x925734).NOP_PAD_TO_SIZE<7>();
static auto patch_0x925B85 = PATCH(0x925B85).CALL(HOOK_0x925B85).NOP_PAD_TO_SIZE<7>();
static auto patch_0x926305 = PATCH(0x926305).CALL(HOOK_0x926305).NOP_PAD_TO_SIZE<8>();
static auto patch_0x926552 = PATCH(0x926552).CALL(HOOK_0x926552).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9269A4 = PATCH(0x9269A4).CALL(HOOK_0x9269A4).NOP_PAD_TO_SIZE<7>();
static auto patch_0x927121 = PATCH(0x927121).CALL(HOOK_0x927121).NOP_PAD_TO_SIZE<8>();
static auto patch_0x927345 = PATCH(0x927345).CALL(HOOK_0x927345).NOP_PAD_TO_SIZE<7>();
static auto patch_0x927796 = PATCH(0x927796).CALL(HOOK_0x927796).NOP_PAD_TO_SIZE<7>();
static auto patch_0x93DC2C = PATCH(0x93DC2C).CALL(HOOK_0x93DC2C).NOP_PAD_TO_SIZE<8>();
static auto patch_0x93DDC7 = PATCH(0x93DDC7).CALL(HOOK_0x93DDC7).NOP_PAD_TO_SIZE<9>();
static auto patch_0x93E2C5 = PATCH(0x93E2C5).CALL(HOOK_0x93E2C5).NOP_PAD_TO_SIZE<9>();
static auto patch_0x93E7C3 = PATCH(0x93E7C3).CALL(HOOK_0x93E7C3).NOP_PAD_TO_SIZE<9>();
static auto patch_0x93ECC1 = PATCH(0x93ECC1).CALL(HOOK_0x93ECC1).NOP_PAD_TO_SIZE<9>();
static auto patch_0x93F1BF = PATCH(0x93F1BF).CALL(HOOK_0x93F1BF).NOP_PAD_TO_SIZE<9>();
static auto patch_0x944970 = PATCH(0x944970).CALL(HOOK_0x944970).NOP_PAD_TO_SIZE<8>();
static auto patch_0x944AE6 = PATCH(0x944AE6).CALL(HOOK_0x944AE6).NOP_PAD_TO_SIZE<9>();
static auto patch_0x944F06 = PATCH(0x944F06).CALL(HOOK_0x944F06).NOP_PAD_TO_SIZE<9>();
static auto patch_0x945326 = PATCH(0x945326).CALL(HOOK_0x945326).NOP_PAD_TO_SIZE<9>();
static auto patch_0x945746 = PATCH(0x945746).CALL(HOOK_0x945746).NOP_PAD_TO_SIZE<9>();
static auto patch_0x945B66 = PATCH(0x945B66).CALL(HOOK_0x945B66).NOP_PAD_TO_SIZE<9>();
static auto patch_0x96C063 = PATCH(0x96C063).CALL(HOOK_0x96C063).NOP_PAD_TO_SIZE<7>();
static auto patch_0x96E1A7 = PATCH(0x96E1A7).CALL(HOOK_0x96E1A7).NOP_PAD_TO_SIZE<7>();
static auto patch_0x974CFC = PATCH(0x974CFC).CALL(HOOK_0x974CFC).NOP_PAD_TO_SIZE<7>();
static auto patch_0x974EE5 = PATCH(0x974EE5).CALL(HOOK_0x974EE5).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9751FA = PATCH(0x9751FA).CALL(HOOK_0x9751FA).NOP_PAD_TO_SIZE<8>();
static auto patch_0x977227 = PATCH(0x977227).CALL(HOOK_0x977227).NOP_PAD_TO_SIZE<9>();
static auto patch_0x97D9AB = PATCH(0x97D9AB).CALL(HOOK_0x97D9AB).NOP_PAD_TO_SIZE<7>();
static auto patch_0x97DBA0 = PATCH(0x97DBA0).CALL(HOOK_0x97DBA0).NOP_PAD_TO_SIZE<8>();
static auto patch_0x97DE9B = PATCH(0x97DE9B).CALL(HOOK_0x97DE9B).NOP_PAD_TO_SIZE<8>();
static auto patch_0x97FCC5 = PATCH(0x97FCC5).CALL(HOOK_0x97FCC5).NOP_PAD_TO_SIZE<9>();
static auto patch_0x980FFC = PATCH(0x980FFC).CALL(HOOK_0x980FFC).NOP_PAD_TO_SIZE<9>();
static auto patch_0x98735A = PATCH(0x98735A).CALL(HOOK_0x98735A).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9873FE = PATCH(0x9873FE).CALL(HOOK_0x9873FE).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9874A2 = PATCH(0x9874A2).CALL(HOOK_0x9874A2).NOP_PAD_TO_SIZE<8>();
static auto patch_0x987546 = PATCH(0x987546).CALL(HOOK_0x987546).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9875EA = PATCH(0x9875EA).CALL(HOOK_0x9875EA).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9877E8 = PATCH(0x9877E8).CALL(HOOK_0x9877E8).NOP_PAD_TO_SIZE<8>();
static auto patch_0x98788C = PATCH(0x98788C).CALL(HOOK_0x98788C).NOP_PAD_TO_SIZE<8>();
static auto patch_0x987930 = PATCH(0x987930).CALL(HOOK_0x987930).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9879D4 = PATCH(0x9879D4).CALL(HOOK_0x9879D4).NOP_PAD_TO_SIZE<8>();
static auto patch_0x987A78 = PATCH(0x987A78).CALL(HOOK_0x987A78).NOP_PAD_TO_SIZE<8>();
static auto patch_0x98A331 = PATCH(0x98A331).CALL(HOOK_0x98A331).NOP_PAD_TO_SIZE<8>();
static auto patch_0x98B92C = PATCH(0x98B92C).CALL(HOOK_0x98B92C).NOP_PAD_TO_SIZE<8>();
static auto patch_0x98CF29 = PATCH(0x98CF29).CALL(HOOK_0x98CF29).NOP_PAD_TO_SIZE<8>();
static auto patch_0x98DF6B = PATCH(0x98DF6B).CALL(HOOK_0x98DF6B).NOP_PAD_TO_SIZE<8>();
static auto patch_0x98F6B1 = PATCH(0x98F6B1).CALL(HOOK_0x98F6B1).NOP_PAD_TO_SIZE<8>();
static auto patch_0x98FBE9 = PATCH(0x98FBE9).CALL(HOOK_0x98FBE9).NOP_PAD_TO_SIZE<8>();
static auto patch_0x98FBF6 = PATCH(0x98FBF6).CALL(HOOK_0x98FBF6).NOP_PAD_TO_SIZE<8>();
static auto patch_0x994764 = PATCH(0x994764).CALL(HOOK_0x994764).NOP_PAD_TO_SIZE<7>();
static auto patch_0x994A24 = PATCH(0x994A24).CALL(HOOK_0x994A24).NOP_PAD_TO_SIZE<7>();
static auto patch_0x994A78 = PATCH(0x994A78).CALL(HOOK_0x994A78).NOP_PAD_TO_SIZE<8>();
static auto patch_0x994AD7 = PATCH(0x994AD7).CALL(HOOK_0x994AD7).NOP_PAD_TO_SIZE<8>();
static auto patch_0x994AE8 = PATCH(0x994AE8).CALL(HOOK_0x994AE8).NOP_PAD_TO_SIZE<7>();
static auto patch_0x994B2D = PATCH(0x994B2D).CALL(HOOK_0x994B2D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x994D83 = PATCH(0x994D83).CALL(HOOK_0x994D83).NOP_PAD_TO_SIZE<7>();
static auto patch_0x994F4C = PATCH(0x994F4C).CALL(HOOK_0x994F4C).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9953AC = PATCH(0x9953AC).CALL(HOOK_0x9953AC).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99792D = PATCH(0x99792D).CALL(HOOK_0x99792D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99799D = PATCH(0x99799D).CALL(HOOK_0x99799D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x997CA1 = PATCH(0x997CA1).CALL(HOOK_0x997CA1).NOP_PAD_TO_SIZE<7>();
static auto patch_0x998040 = PATCH(0x998040).CALL(HOOK_0x998040).NOP_PAD_TO_SIZE<7>();
static auto patch_0x998085 = PATCH(0x998085).CALL(HOOK_0x998085).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9980E6 = PATCH(0x9980E6).CALL(HOOK_0x9980E6).NOP_PAD_TO_SIZE<7>();
static auto patch_0x998614 = PATCH(0x998614).CALL(HOOK_0x998614).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9987FF = PATCH(0x9987FF).CALL(HOOK_0x9987FF).NOP_PAD_TO_SIZE<7>();
static auto patch_0x998844 = PATCH(0x998844).CALL(HOOK_0x998844).NOP_PAD_TO_SIZE<7>();
static auto patch_0x998889 = PATCH(0x998889).CALL(HOOK_0x998889).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999449 = PATCH(0x999449).CALL(HOOK_0x999449).NOP_PAD_TO_SIZE<8>();
static auto patch_0x999470 = PATCH(0x999470).CALL(HOOK_0x999470).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9994E1 = PATCH(0x9994E1).CALL(HOOK_0x9994E1).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99966B = PATCH(0x99966B).CALL(HOOK_0x99966B).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999833 = PATCH(0x999833).CALL(HOOK_0x999833).NOP_PAD_TO_SIZE<8>();
static auto patch_0x999866 = PATCH(0x999866).CALL(HOOK_0x999866).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9999F4 = PATCH(0x9999F4).CALL(HOOK_0x9999F4).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999AB5 = PATCH(0x999AB5).CALL(HOOK_0x999AB5).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999B1A = PATCH(0x999B1A).CALL(HOOK_0x999B1A).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999B44 = PATCH(0x999B44).CALL(HOOK_0x999B44).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999B69 = PATCH(0x999B69).CALL(HOOK_0x999B69).NOP_PAD_TO_SIZE<8>();
static auto patch_0x999BA0 = PATCH(0x999BA0).CALL(HOOK_0x999BA0).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999BCA = PATCH(0x999BCA).CALL(HOOK_0x999BCA).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999BEF = PATCH(0x999BEF).CALL(HOOK_0x999BEF).NOP_PAD_TO_SIZE<8>();
static auto patch_0x999C15 = PATCH(0x999C15).CALL(HOOK_0x999C15).NOP_PAD_TO_SIZE<8>();
static auto patch_0x999C94 = PATCH(0x999C94).CALL(HOOK_0x999C94).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999CB9 = PATCH(0x999CB9).CALL(HOOK_0x999CB9).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999CDE = PATCH(0x999CDE).CALL(HOOK_0x999CDE).NOP_PAD_TO_SIZE<8>();
static auto patch_0x999E1B = PATCH(0x999E1B).CALL(HOOK_0x999E1B).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999E7E = PATCH(0x999E7E).CALL(HOOK_0x999E7E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999EA8 = PATCH(0x999EA8).CALL(HOOK_0x999EA8).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999ECD = PATCH(0x999ECD).CALL(HOOK_0x999ECD).NOP_PAD_TO_SIZE<8>();
static auto patch_0x999F04 = PATCH(0x999F04).CALL(HOOK_0x999F04).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999F2E = PATCH(0x999F2E).CALL(HOOK_0x999F2E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x999F53 = PATCH(0x999F53).CALL(HOOK_0x999F53).NOP_PAD_TO_SIZE<8>();
static auto patch_0x999F79 = PATCH(0x999F79).CALL(HOOK_0x999F79).NOP_PAD_TO_SIZE<8>();
static auto patch_0x999FF8 = PATCH(0x999FF8).CALL(HOOK_0x999FF8).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99A01D = PATCH(0x99A01D).CALL(HOOK_0x99A01D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99A042 = PATCH(0x99A042).CALL(HOOK_0x99A042).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99A16B = PATCH(0x99A16B).CALL(HOOK_0x99A16B).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99A196 = PATCH(0x99A196).CALL(HOOK_0x99A196).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99A1BB = PATCH(0x99A1BB).CALL(HOOK_0x99A1BB).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99A2B5 = PATCH(0x99A2B5).CALL(HOOK_0x99A2B5).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99A607 = PATCH(0x99A607).CALL(HOOK_0x99A607).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99A6A0 = PATCH(0x99A6A0).CALL(HOOK_0x99A6A0).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99A6E6 = PATCH(0x99A6E6).CALL(HOOK_0x99A6E6).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99A71B = PATCH(0x99A71B).CALL(HOOK_0x99A71B).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99A750 = PATCH(0x99A750).CALL(HOOK_0x99A750).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99A785 = PATCH(0x99A785).CALL(HOOK_0x99A785).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99A7B7 = PATCH(0x99A7B7).CALL(HOOK_0x99A7B7).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99A850 = PATCH(0x99A850).CALL(HOOK_0x99A850).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99ABA6 = PATCH(0x99ABA6).CALL(HOOK_0x99ABA6).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99B7C8 = PATCH(0x99B7C8).CALL(HOOK_0x99B7C8).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99BB7B = PATCH(0x99BB7B).CALL(HOOK_0x99BB7B).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99C017 = PATCH(0x99C017).CALL(HOOK_0x99C017).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99C096 = PATCH(0x99C096).CALL(HOOK_0x99C096).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99C510 = PATCH(0x99C510).CALL(HOOK_0x99C510).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99CB3B = PATCH(0x99CB3B).CALL(HOOK_0x99CB3B).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99CEED = PATCH(0x99CEED).CALL(HOOK_0x99CEED).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99D00C = PATCH(0x99D00C).CALL(HOOK_0x99D00C).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99D40A = PATCH(0x99D40A).CALL(HOOK_0x99D40A).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99D4AD = PATCH(0x99D4AD).CALL(HOOK_0x99D4AD).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99D70E = PATCH(0x99D70E).CALL(HOOK_0x99D70E).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99D736 = PATCH(0x99D736).CALL(HOOK_0x99D736).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99D83D = PATCH(0x99D83D).CALL(HOOK_0x99D83D).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99D882 = PATCH(0x99D882).CALL(HOOK_0x99D882).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99E265 = PATCH(0x99E265).CALL(HOOK_0x99E265).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99E33A = PATCH(0x99E33A).CALL(HOOK_0x99E33A).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99E6AC = PATCH(0x99E6AC).CALL(HOOK_0x99E6AC).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99EB43 = PATCH(0x99EB43).CALL(HOOK_0x99EB43).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99EC52 = PATCH(0x99EC52).CALL(HOOK_0x99EC52).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99ED2F = PATCH(0x99ED2F).CALL(HOOK_0x99ED2F).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99EE6F = PATCH(0x99EE6F).CALL(HOOK_0x99EE6F).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99EEB3 = PATCH(0x99EEB3).CALL(HOOK_0x99EEB3).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99F03E = PATCH(0x99F03E).CALL(HOOK_0x99F03E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99F0A1 = PATCH(0x99F0A1).CALL(HOOK_0x99F0A1).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99F0DD = PATCH(0x99F0DD).CALL(HOOK_0x99F0DD).NOP_PAD_TO_SIZE<8>();
static auto patch_0x99F12F = PATCH(0x99F12F).CALL(HOOK_0x99F12F).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99F57A = PATCH(0x99F57A).CALL(HOOK_0x99F57A).NOP_PAD_TO_SIZE<7>();
static auto patch_0x99F62E = PATCH(0x99F62E).CALL(HOOK_0x99F62E).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A008E = PATCH(0x9A008E).CALL(HOOK_0x9A008E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9A04B6 = PATCH(0x9A04B6).CALL(HOOK_0x9A04B6).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9A13BE = PATCH(0x9A13BE).CALL(HOOK_0x9A13BE).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A32CB = PATCH(0x9A32CB).CALL(HOOK_0x9A32CB).NOP_PAD_TO_SIZE<8>();
// Character ! blocks
// static auto patch_0x9A3CD9 = PATCH(0x9A3CD9).CALL(HOOK_0x9A3CD9).NOP_PAD_TO_SIZE<8>();
// static auto patch_0x9A3D13 = PATCH(0x9A3D13).CALL(HOOK_0x9A3D13).NOP_PAD_TO_SIZE<8>();
// static auto patch_0x9A3D4D = PATCH(0x9A3D4D).CALL(HOOK_0x9A3D4D).NOP_PAD_TO_SIZE<8>();
// static auto patch_0x9A3D87 = PATCH(0x9A3D87).CALL(HOOK_0x9A3D87).NOP_PAD_TO_SIZE<8>();
// static auto patch_0x9A3DC1 = PATCH(0x9A3DC1).CALL(HOOK_0x9A3DC1).NOP_PAD_TO_SIZE<8>();
// runs after collision loop, duplicated by runtimeHookPlayerBlockCollisionEnd
//static auto patch_0x9A5015 = PATCH(0x9A5015).CALL(HOOK_0x9A5015).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A504D = PATCH(0x9A504D).CALL(HOOK_0x9A504D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9A5367 = PATCH(0x9A5367).CALL(HOOK_0x9A5367).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A5377 = PATCH(0x9A5377).CALL(HOOK_0x9A5377).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A538B = PATCH(0x9A538B).CALL(HOOK_0x9A538B).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A5C52 = PATCH(0x9A5C52).CALL(HOOK_0x9A5C52).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A5DA6 = PATCH(0x9A5DA6).CALL(HOOK_0x9A5DA6).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A6856 = PATCH(0x9A6856).CALL(HOOK_0x9A6856).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A69A9 = PATCH(0x9A69A9).CALL(HOOK_0x9A69A9).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A72FE = PATCH(0x9A72FE).CALL(HOOK_0x9A72FE).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A75A5 = PATCH(0x9A75A5).CALL(HOOK_0x9A75A5).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9A8348 = PATCH(0x9A8348).CALL(HOOK_0x9A8348).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9AA469 = PATCH(0x9AA469).CALL(HOOK_0x9AA469).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9AA6B9 = PATCH(0x9AA6B9).CALL(HOOK_0x9AA6B9).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9AAA0E = PATCH(0x9AAA0E).CALL(HOOK_0x9AAA0E).NOP_PAD_TO_SIZE<8>();
// If Link, become fairy for climbing. Conflicts with defines.player_link_fairyVineEnabled so commented out, and all current extended characters use defines.player_link_fairyVineEnabled to disable this anyway.
//static auto patch_0x9AAF8C = PATCH(0x9AAF8C).CALL(HOOK_0x9AAF8C).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9ABE80 = PATCH(0x9ABE80).CALL(HOOK_0x9ABE80).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9ABFB5 = PATCH(0x9ABFB5).CALL(HOOK_0x9ABFB5).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9AC153 = PATCH(0x9AC153).CALL(HOOK_0x9AC153).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9AC705 = PATCH(0x9AC705).CALL(HOOK_0x9AC705).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9AD678 = PATCH(0x9AD678).CALL(HOOK_0x9AD678).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9AD6FB = PATCH(0x9AD6FB).CALL(HOOK_0x9AD6FB).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9ADAC0 = PATCH(0x9ADAC0).CALL(HOOK_0x9ADAC0).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9AEA0D = PATCH(0x9AEA0D).CALL(HOOK_0x9AEA0D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9AEB73 = PATCH(0x9AEB73).CALL(HOOK_0x9AEB73).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9B081A = PATCH(0x9B081A).CALL(HOOK_0x9B081A).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9B0863 = PATCH(0x9B0863).CALL(HOOK_0x9B0863).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9B08F3 = PATCH(0x9B08F3).CALL(HOOK_0x9B08F3).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9B11DB = PATCH(0x9B11DB).CALL(HOOK_0x9B11DB).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9B1A55 = PATCH(0x9B1A55).CALL(HOOK_0x9B1A55).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9B55CB = PATCH(0x9B55CB).CALL(HOOK_0x9B55CB).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9B57F0 = PATCH(0x9B57F0).CALL(HOOK_0x9B57F0).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B5D21 = PATCH(0x9B5D21).CALL(HOOK_0x9B5D21).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B5D71 = PATCH(0x9B5D71).CALL(HOOK_0x9B5D71).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B61B4 = PATCH(0x9B61B4).CALL(HOOK_0x9B61B4).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B61DD = PATCH(0x9B61DD).CALL(HOOK_0x9B61DD).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B6222 = PATCH(0x9B6222).CALL(HOOK_0x9B6222).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B68C9 = PATCH(0x9B68C9).CALL(HOOK_0x9B68C9).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9B6C34 = PATCH(0x9B6C34).CALL(HOOK_0x9B6C34).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B6F12 = PATCH(0x9B6F12).CALL(HOOK_0x9B6F12).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9B6FD4 = PATCH(0x9B6FD4).CALL(HOOK_0x9B6FD4).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B71D3 = PATCH(0x9B71D3).CALL(HOOK_0x9B71D3).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B722D = PATCH(0x9B722D).CALL(HOOK_0x9B722D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B760F = PATCH(0x9B760F).CALL(HOOK_0x9B760F).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9B7D86 = PATCH(0x9B7D86).CALL(HOOK_0x9B7D86).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B86E4 = PATCH(0x9B86E4).CALL(HOOK_0x9B86E4).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B8DFB = PATCH(0x9B8DFB).CALL(HOOK_0x9B8DFB).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B9B24 = PATCH(0x9B9B24).CALL(HOOK_0x9B9B24).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9B9B88 = PATCH(0x9B9B88).CALL(HOOK_0x9B9B88).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9B9BD7 = PATCH(0x9B9BD7).CALL(HOOK_0x9B9BD7).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9B9CD4 = PATCH(0x9B9CD4).CALL(HOOK_0x9B9CD4).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9B9D0E = PATCH(0x9B9D0E).CALL(HOOK_0x9B9D0E).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9BA1D7 = PATCH(0x9BA1D7).CALL(HOOK_0x9BA1D7).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9BA28E = PATCH(0x9BA28E).CALL(HOOK_0x9BA28E).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9BA847 = PATCH(0x9BA847).CALL(HOOK_0x9BA847).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9BA8F3 = PATCH(0x9BA8F3).CALL(HOOK_0x9BA8F3).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9BA956 = PATCH(0x9BA956).CALL(HOOK_0x9BA956).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9BAAAC = PATCH(0x9BAAAC).CALL(HOOK_0x9BAAAC).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9BB469 = PATCH(0x9BB469).CALL(HOOK_0x9BB469).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9BB886 = PATCH(0x9BB886).CALL(HOOK_0x9BB886).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9BE92D = PATCH(0x9BE92D).CALL(HOOK_0x9BE92D).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9C0D14 = PATCH(0x9C0D14).CALL(HOOK_0x9C0D14).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C0EA8 = PATCH(0x9C0EA8).CALL(HOOK_0x9C0EA8).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C0F5E = PATCH(0x9C0F5E).CALL(HOOK_0x9C0F5E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C1008 = PATCH(0x9C1008).CALL(HOOK_0x9C1008).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C10BE = PATCH(0x9C10BE).CALL(HOOK_0x9C10BE).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C119E = PATCH(0x9C119E).CALL(HOOK_0x9C119E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C1267 = PATCH(0x9C1267).CALL(HOOK_0x9C1267).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C131E = PATCH(0x9C131E).CALL(HOOK_0x9C131E).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9C140B = PATCH(0x9C140B).CALL(HOOK_0x9C140B).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C1BAD = PATCH(0x9C1BAD).CALL(HOOK_0x9C1BAD).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C1C9D = PATCH(0x9C1C9D).CALL(HOOK_0x9C1C9D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C47DE = PATCH(0x9C47DE).CALL(HOOK_0x9C47DE).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C4817 = PATCH(0x9C4817).CALL(HOOK_0x9C4817).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C66D1 = PATCH(0x9C66D1).CALL(HOOK_0x9C66D1).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9C6845 = PATCH(0x9C6845).CALL(HOOK_0x9C6845).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9C6B0C = PATCH(0x9C6B0C).CALL(HOOK_0x9C6B0C).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C6E51 = PATCH(0x9C6E51).CALL(HOOK_0x9C6E51).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C6F03 = PATCH(0x9C6F03).CALL(HOOK_0x9C6F03).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C7378 = PATCH(0x9C7378).CALL(HOOK_0x9C7378).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C76B4 = PATCH(0x9C76B4).CALL(HOOK_0x9C76B4).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C788F = PATCH(0x9C788F).CALL(HOOK_0x9C788F).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9C79D8 = PATCH(0x9C79D8).CALL(HOOK_0x9C79D8).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9C7E49 = PATCH(0x9C7E49).CALL(HOOK_0x9C7E49).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C7EF5 = PATCH(0x9C7EF5).CALL(HOOK_0x9C7EF5).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C80EC = PATCH(0x9C80EC).CALL(HOOK_0x9C80EC).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C8176 = PATCH(0x9C8176).CALL(HOOK_0x9C8176).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C8388 = PATCH(0x9C8388).CALL(HOOK_0x9C8388).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C8D32 = PATCH(0x9C8D32).CALL(HOOK_0x9C8D32).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9C8E62 = PATCH(0x9C8E62).CALL(HOOK_0x9C8E62).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9C94A9 = PATCH(0x9C94A9).CALL(HOOK_0x9C94A9).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9C966B = PATCH(0x9C966B).CALL(HOOK_0x9C966B).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9CA23B = PATCH(0x9CA23B).CALL(HOOK_0x9CA23B).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9CB66C = PATCH(0x9CB66C).CALL(HOOK_0x9CB66C).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9CBA23 = PATCH(0x9CBA23).CALL(HOOK_0x9CBA23).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9CC44A = PATCH(0x9CC44A).CALL(HOOK_0x9CC44A).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9CC45D = PATCH(0x9CC45D).CALL(HOOK_0x9CC45D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9CC474 = PATCH(0x9CC474).CALL(HOOK_0x9CC474).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9CE004 = PATCH(0x9CE004).CALL(HOOK_0x9CE004).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9CE05E = PATCH(0x9CE05E).CALL(HOOK_0x9CE05E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9CF1E7 = PATCH(0x9CF1E7).CALL(HOOK_0x9CF1E7).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D0402 = PATCH(0x9D0402).CALL(HOOK_0x9D0402).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D05BB = PATCH(0x9D05BB).CALL(HOOK_0x9D05BB).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D079B = PATCH(0x9D079B).CALL(HOOK_0x9D079B).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D214B = PATCH(0x9D214B).CALL(HOOK_0x9D214B).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D215E = PATCH(0x9D215E).CALL(HOOK_0x9D215E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D21D5 = PATCH(0x9D21D5).CALL(HOOK_0x9D21D5).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D21E8 = PATCH(0x9D21E8).CALL(HOOK_0x9D21E8).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2251 = PATCH(0x9D2251).CALL(HOOK_0x9D2251).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2296 = PATCH(0x9D2296).CALL(HOOK_0x9D2296).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D22F6 = PATCH(0x9D22F6).CALL(HOOK_0x9D22F6).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2360 = PATCH(0x9D2360).CALL(HOOK_0x9D2360).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D23C3 = PATCH(0x9D23C3).CALL(HOOK_0x9D23C3).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D23D6 = PATCH(0x9D23D6).CALL(HOOK_0x9D23D6).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D244D = PATCH(0x9D244D).CALL(HOOK_0x9D244D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2460 = PATCH(0x9D2460).CALL(HOOK_0x9D2460).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D24C9 = PATCH(0x9D24C9).CALL(HOOK_0x9D24C9).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D250E = PATCH(0x9D250E).CALL(HOOK_0x9D250E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2553 = PATCH(0x9D2553).CALL(HOOK_0x9D2553).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D2664 = PATCH(0x9D2664).CALL(HOOK_0x9D2664).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D26A9 = PATCH(0x9D26A9).CALL(HOOK_0x9D26A9).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D26D5 = PATCH(0x9D26D5).CALL(HOOK_0x9D26D5).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2877 = PATCH(0x9D2877).CALL(HOOK_0x9D2877).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D288A = PATCH(0x9D288A).CALL(HOOK_0x9D288A).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2901 = PATCH(0x9D2901).CALL(HOOK_0x9D2901).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2914 = PATCH(0x9D2914).CALL(HOOK_0x9D2914).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D297D = PATCH(0x9D297D).CALL(HOOK_0x9D297D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D29C2 = PATCH(0x9D29C2).CALL(HOOK_0x9D29C2).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2A32 = PATCH(0x9D2A32).CALL(HOOK_0x9D2A32).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2A45 = PATCH(0x9D2A45).CALL(HOOK_0x9D2A45).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2ABC = PATCH(0x9D2ABC).CALL(HOOK_0x9D2ABC).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2ACF = PATCH(0x9D2ACF).CALL(HOOK_0x9D2ACF).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2B38 = PATCH(0x9D2B38).CALL(HOOK_0x9D2B38).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2B7D = PATCH(0x9D2B7D).CALL(HOOK_0x9D2B7D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2BEE = PATCH(0x9D2BEE).CALL(HOOK_0x9D2BEE).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2C01 = PATCH(0x9D2C01).CALL(HOOK_0x9D2C01).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2C78 = PATCH(0x9D2C78).CALL(HOOK_0x9D2C78).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2C8B = PATCH(0x9D2C8B).CALL(HOOK_0x9D2C8B).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2CF4 = PATCH(0x9D2CF4).CALL(HOOK_0x9D2CF4).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2D39 = PATCH(0x9D2D39).CALL(HOOK_0x9D2D39).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2DAF = PATCH(0x9D2DAF).CALL(HOOK_0x9D2DAF).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2DF4 = PATCH(0x9D2DF4).CALL(HOOK_0x9D2DF4).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2E20 = PATCH(0x9D2E20).CALL(HOOK_0x9D2E20).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D2FE5 = PATCH(0x9D2FE5).CALL(HOOK_0x9D2FE5).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D302A = PATCH(0x9D302A).CALL(HOOK_0x9D302A).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D3056 = PATCH(0x9D3056).CALL(HOOK_0x9D3056).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D33DC = PATCH(0x9D33DC).CALL(HOOK_0x9D33DC).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D3A82 = PATCH(0x9D3A82).CALL(HOOK_0x9D3A82).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D3D84 = PATCH(0x9D3D84).CALL(HOOK_0x9D3D84).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D3E6B = PATCH(0x9D3E6B).CALL(HOOK_0x9D3E6B).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D4178 = PATCH(0x9D4178).CALL(HOOK_0x9D4178).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D425F = PATCH(0x9D425F).CALL(HOOK_0x9D425F).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D461E = PATCH(0x9D461E).CALL(HOOK_0x9D461E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D4868 = PATCH(0x9D4868).CALL(HOOK_0x9D4868).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D4B47 = PATCH(0x9D4B47).CALL(HOOK_0x9D4B47).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D4C2F = PATCH(0x9D4C2F).CALL(HOOK_0x9D4C2F).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D4E6E = PATCH(0x9D4E6E).CALL(HOOK_0x9D4E6E).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D4F55 = PATCH(0x9D4F55).CALL(HOOK_0x9D4F55).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D575B = PATCH(0x9D575B).CALL(HOOK_0x9D575B).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D5A0A = PATCH(0x9D5A0A).CALL(HOOK_0x9D5A0A).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D5C1E = PATCH(0x9D5C1E).CALL(HOOK_0x9D5C1E).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D5C45 = PATCH(0x9D5C45).CALL(HOOK_0x9D5C45).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D5D2C = PATCH(0x9D5D2C).CALL(HOOK_0x9D5D2C).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D5E92 = PATCH(0x9D5E92).CALL(HOOK_0x9D5E92).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D5F79 = PATCH(0x9D5F79).CALL(HOOK_0x9D5F79).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D614D = PATCH(0x9D614D).CALL(HOOK_0x9D614D).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D6174 = PATCH(0x9D6174).CALL(HOOK_0x9D6174).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D625B = PATCH(0x9D625B).CALL(HOOK_0x9D625B).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D6396 = PATCH(0x9D6396).CALL(HOOK_0x9D6396).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D647D = PATCH(0x9D647D).CALL(HOOK_0x9D647D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D6B14 = PATCH(0x9D6B14).CALL(HOOK_0x9D6B14).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D6CD5 = PATCH(0x9D6CD5).CALL(HOOK_0x9D6CD5).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D6CE8 = PATCH(0x9D6CE8).CALL(HOOK_0x9D6CE8).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D7311 = PATCH(0x9D7311).CALL(HOOK_0x9D7311).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D7437 = PATCH(0x9D7437).CALL(HOOK_0x9D7437).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D7571 = PATCH(0x9D7571).CALL(HOOK_0x9D7571).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D81E0 = PATCH(0x9D81E0).CALL(HOOK_0x9D81E0).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D82CE = PATCH(0x9D82CE).CALL(HOOK_0x9D82CE).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D82F8 = PATCH(0x9D82F8).CALL(HOOK_0x9D82F8).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D830B = PATCH(0x9D830B).CALL(HOOK_0x9D830B).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8382 = PATCH(0x9D8382).CALL(HOOK_0x9D8382).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8395 = PATCH(0x9D8395).CALL(HOOK_0x9D8395).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D83FE = PATCH(0x9D83FE).CALL(HOOK_0x9D83FE).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8443 = PATCH(0x9D8443).CALL(HOOK_0x9D8443).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D84A1 = PATCH(0x9D84A1).CALL(HOOK_0x9D84A1).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D850C = PATCH(0x9D850C).CALL(HOOK_0x9D850C).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D85E7 = PATCH(0x9D85E7).CALL(HOOK_0x9D85E7).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D86D5 = PATCH(0x9D86D5).CALL(HOOK_0x9D86D5).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9D86FF = PATCH(0x9D86FF).CALL(HOOK_0x9D86FF).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8712 = PATCH(0x9D8712).CALL(HOOK_0x9D8712).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8789 = PATCH(0x9D8789).CALL(HOOK_0x9D8789).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D879C = PATCH(0x9D879C).CALL(HOOK_0x9D879C).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8805 = PATCH(0x9D8805).CALL(HOOK_0x9D8805).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D884A = PATCH(0x9D884A).CALL(HOOK_0x9D884A).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D88A8 = PATCH(0x9D88A8).CALL(HOOK_0x9D88A8).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8913 = PATCH(0x9D8913).CALL(HOOK_0x9D8913).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D89FD = PATCH(0x9D89FD).CALL(HOOK_0x9D89FD).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8A10 = PATCH(0x9D8A10).CALL(HOOK_0x9D8A10).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8A87 = PATCH(0x9D8A87).CALL(HOOK_0x9D8A87).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8A9A = PATCH(0x9D8A9A).CALL(HOOK_0x9D8A9A).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8B0C = PATCH(0x9D8B0C).CALL(HOOK_0x9D8B0C).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8B51 = PATCH(0x9D8B51).CALL(HOOK_0x9D8B51).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8BAF = PATCH(0x9D8BAF).CALL(HOOK_0x9D8BAF).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8C23 = PATCH(0x9D8C23).CALL(HOOK_0x9D8C23).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8E29 = PATCH(0x9D8E29).CALL(HOOK_0x9D8E29).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8E3C = PATCH(0x9D8E3C).CALL(HOOK_0x9D8E3C).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8EB3 = PATCH(0x9D8EB3).CALL(HOOK_0x9D8EB3).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8EC6 = PATCH(0x9D8EC6).CALL(HOOK_0x9D8EC6).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8F38 = PATCH(0x9D8F38).CALL(HOOK_0x9D8F38).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8F7D = PATCH(0x9D8F7D).CALL(HOOK_0x9D8F7D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D8FDB = PATCH(0x9D8FDB).CALL(HOOK_0x9D8FDB).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D904F = PATCH(0x9D904F).CALL(HOOK_0x9D904F).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D923D = PATCH(0x9D923D).CALL(HOOK_0x9D923D).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D9250 = PATCH(0x9D9250).CALL(HOOK_0x9D9250).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D92C7 = PATCH(0x9D92C7).CALL(HOOK_0x9D92C7).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D92DA = PATCH(0x9D92DA).CALL(HOOK_0x9D92DA).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D934C = PATCH(0x9D934C).CALL(HOOK_0x9D934C).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D9391 = PATCH(0x9D9391).CALL(HOOK_0x9D9391).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D93EF = PATCH(0x9D93EF).CALL(HOOK_0x9D93EF).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D9463 = PATCH(0x9D9463).CALL(HOOK_0x9D9463).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D9AF3 = PATCH(0x9D9AF3).CALL(HOOK_0x9D9AF3).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9D9C6A = PATCH(0x9D9C6A).CALL(HOOK_0x9D9C6A).NOP_PAD_TO_SIZE<7>();
// Get template:
// static auto patch_0x9DA7CA = PATCH(0x9DA7CA).CALL(HOOK_0x9DA7CA).NOP_PAD_TO_SIZE<8>();
// Character change block can activate
// static auto patch_0x9DA827 = PATCH(0x9DA827).CALL(HOOK_0x9DA827).NOP_PAD_TO_SIZE<9>();
// static auto patch_0x9DA863 = PATCH(0x9DA863).CALL(HOOK_0x9DA863).NOP_PAD_TO_SIZE<9>();
// static auto patch_0x9DA893 = PATCH(0x9DA893).CALL(HOOK_0x9DA893).NOP_PAD_TO_SIZE<9>();
// static auto patch_0x9DA8C3 = PATCH(0x9DA8C3).CALL(HOOK_0x9DA8C3).NOP_PAD_TO_SIZE<9>();
// static auto patch_0x9DA8F2 = PATCH(0x9DA8F2).CALL(HOOK_0x9DA8F2).NOP_PAD_TO_SIZE<9>();
// Set template:
// static auto patch_0x9DAAA2 = PATCH(0x9DAAA2).CALL(HOOK_0x9DAAA2).NOP_PAD_TO_SIZE<8>();
// static auto patch_0x9DAB17 = PATCH(0x9DAB17).CALL(HOOK_0x9DAB17).NOP_PAD_TO_SIZE<8>();
// static auto patch_0x9DAB8C = PATCH(0x9DAB8C).CALL(HOOK_0x9DAB8C).NOP_PAD_TO_SIZE<8>();
// static auto patch_0x9DABFE = PATCH(0x9DABFE).CALL(HOOK_0x9DABFE).NOP_PAD_TO_SIZE<8>();
// static auto patch_0x9DAC73 = PATCH(0x9DAC73).CALL(HOOK_0x9DAC73).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9DAD04 = PATCH(0x9DAD04).CALL(HOOK_0x9DAD04).NOP_PAD_TO_SIZE<7>();
static auto patch_0x9DB9CC = PATCH(0x9DB9CC).CALL(HOOK_0x9DB9CC).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9DBA82 = PATCH(0x9DBA82).CALL(HOOK_0x9DBA82).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9DBAAB = PATCH(0x9DBAAB).CALL(HOOK_0x9DBAAB).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9DBD7E = PATCH(0x9DBD7E).CALL(HOOK_0x9DBD7E).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9DC126 = PATCH(0x9DC126).CALL(HOOK_0x9DC126).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9DCC3F = PATCH(0x9DCC3F).CALL(HOOK_0x9DCC3F).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9DCD40 = PATCH(0x9DCD40).CALL(HOOK_0x9DCD40).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9DCE6D = PATCH(0x9DCE6D).CALL(HOOK_0x9DCE6D).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9DCF6E = PATCH(0x9DCF6E).CALL(HOOK_0x9DCF6E).NOP_PAD_TO_SIZE<9>();
static auto patch_0x9DDE82 = PATCH(0x9DDE82).CALL(HOOK_0x9DDE82).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9DDF1E = PATCH(0x9DDF1E).CALL(HOOK_0x9DDF1E).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9DE629 = PATCH(0x9DE629).CALL(HOOK_0x9DE629).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9DE6C4 = PATCH(0x9DE6C4).CALL(HOOK_0x9DE6C4).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9DEF81 = PATCH(0x9DEF81).CALL(HOOK_0x9DEF81).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9DF01D = PATCH(0x9DF01D).CALL(HOOK_0x9DF01D).NOP_PAD_TO_SIZE<8>();
static auto patch_0x9E086C = PATCH(0x9E086C).CALL(HOOK_0x9E086C).NOP_PAD_TO_SIZE<9>();
// Character block animation
// static auto patch_0x9E19BE = PATCH(0x9E19BE).CALL(HOOK_0x9E19BE).NOP_PAD_TO_SIZE<9>();
// static auto patch_0x9E19F3 = PATCH(0x9E19F3).CALL(HOOK_0x9E19F3).NOP_PAD_TO_SIZE<9>();
// static auto patch_0x9E1A28 = PATCH(0x9E1A28).CALL(HOOK_0x9E1A28).NOP_PAD_TO_SIZE<9>();
// static auto patch_0x9E1A63 = PATCH(0x9E1A63).CALL(HOOK_0x9E1A63).NOP_PAD_TO_SIZE<8>();
// static auto patch_0x9E1A96 = PATCH(0x9E1A96).CALL(HOOK_0x9E1A96).NOP_PAD_TO_SIZE<9>();
// static auto patch_0x9E1BD6 = PATCH(0x9E1BD6).CALL(HOOK_0x9E1BD6).NOP_PAD_TO_SIZE<9>();
// static auto patch_0x9E1BF9 = PATCH(0x9E1BF9).CALL(HOOK_0x9E1BF9).NOP_PAD_TO_SIZE<8>();
static auto patch_0xA02866 = PATCH(0xA02866).CALL(HOOK_0xA02866).NOP_PAD_TO_SIZE<8>();
static auto patch_0xA02899 = PATCH(0xA02899).CALL(HOOK_0xA02899).NOP_PAD_TO_SIZE<9>();
//static auto patch_0xA028F1 = PATCH(0xA028F1).CALL(HOOK_0xA028F1).NOP_PAD_TO_SIZE<8>();
static auto patch_0xA02A72 = PATCH(0xA02A72).CALL(HOOK_0xA02A72).NOP_PAD_TO_SIZE<7>();
static auto patch_0xA02AA4 = PATCH(0xA02AA4).CALL(HOOK_0xA02AA4).NOP_PAD_TO_SIZE<8>();
static auto patch_0xA03801 = PATCH(0xA03801).CALL(HOOK_0xA03801).NOP_PAD_TO_SIZE<9>();
static auto patch_0xA03828 = PATCH(0xA03828).CALL(HOOK_0xA03828).NOP_PAD_TO_SIZE<9>();
static auto patch_0xA03AD5 = PATCH(0xA03AD5).CALL(HOOK_0xA03AD5).NOP_PAD_TO_SIZE<9>();
static auto patch_0xA243F0 = PATCH(0xA243F0).CALL(HOOK_0xA243F0).NOP_PAD_TO_SIZE<8>();
static auto patch_0xA2509E = PATCH(0xA2509E).CALL(HOOK_0xA2509E).NOP_PAD_TO_SIZE<7>();
static auto patch_0xA256A7 = PATCH(0xA256A7).CALL(HOOK_0xA256A7).NOP_PAD_TO_SIZE<7>();
static auto patch_0xA25DED = PATCH(0xA25DED).CALL(HOOK_0xA25DED).NOP_PAD_TO_SIZE<8>();
static auto patch_0xA25E16 = PATCH(0xA25E16).CALL(HOOK_0xA25E16).NOP_PAD_TO_SIZE<8>();
static auto patch_0xA25F58 = PATCH(0xA25F58).CALL(HOOK_0xA25F58).NOP_PAD_TO_SIZE<7>();
static auto patch_0xA25F64 = PATCH(0xA25F64).CALL(HOOK_0xA25F64).NOP_PAD_TO_SIZE<7>();
static auto patch_0xA2771F = PATCH(0xA2771F).CALL(HOOK_0xA2771F).NOP_PAD_TO_SIZE<7>();
static auto patch_0xA27800 = PATCH(0xA27800).CALL(HOOK_0xA27800).NOP_PAD_TO_SIZE<8>();
static auto patch_0xA27835 = PATCH(0xA27835).CALL(HOOK_0xA27835).NOP_PAD_TO_SIZE<7>();
static auto patch_0xA27912 = PATCH(0xA27912).CALL(HOOK_0xA27912).NOP_PAD_TO_SIZE<8>();
static auto patch_0xA2793B = PATCH(0xA2793B).CALL(HOOK_0xA2793B).NOP_PAD_TO_SIZE<8>();
static auto patch_0xA27A06 = PATCH(0xA27A06).CALL(HOOK_0xA27A06).NOP_PAD_TO_SIZE<7>();
static auto patch_0xA27AA7 = PATCH(0xA27AA7).CALL(HOOK_0xA27AA7).NOP_PAD_TO_SIZE<8>();
static auto patch_0xA291A5 = PATCH(0xA291A5).CALL(HOOK_0xA291A5).NOP_PAD_TO_SIZE<9>();
static auto patch_0xA4456C = PATCH(0xA4456C).CALL(HOOK_0xA4456C).NOP_PAD_TO_SIZE<9>();
static auto patch_0xA52D55 = PATCH(0xA52D55).CALL(HOOK_0xA52D55).NOP_PAD_TO_SIZE<9>();
static auto patch_0xA60B49 = PATCH(0xA60B49).CALL(HOOK_0xA60B49).NOP_PAD_TO_SIZE<9>();
static auto patch_0xA60B6B = PATCH(0xA60B6B).CALL(HOOK_0xA60B6B).NOP_PAD_TO_SIZE<9>();
static auto patch_0xA60B96 = PATCH(0xA60B96).CALL(HOOK_0xA60B96).NOP_PAD_TO_SIZE<9>();
static auto patch_0xA60BB9 = PATCH(0xA60BB9).CALL(HOOK_0xA60BB9).NOP_PAD_TO_SIZE<9>();
static auto patch_0xA60BE3 = PATCH(0xA60BE3).CALL(HOOK_0xA60BE3).NOP_PAD_TO_SIZE<9>();

// Player template patches
static auto patch_play2temp_0x8E485F = PATCH(0x8E485F)
    .PUSHFD().PUSH_EAX().PUSH_ECX().PUSH_EDX() // Safety
    .PUSH_ESI().PUSH_EDI() // Arguments
    .CALL(runtimeHookCharacterIdCopyPlayerToTemplate)
    .POP_EDX().POP_ECX().POP_EAX().POPFD() // Safety
    .JMP(0x8E4897)
    .NOP_PAD_TO_SIZE<56>();
static auto patch_play2temp_0x8E555F = PATCH(0x8E555F)
    .PUSHFD().PUSH_EAX().PUSH_ECX().PUSH_EDX() // Safety
    .PUSH_ESI().PUSH_EDI() // Arguments
    .CALL(runtimeHookCharacterIdCopyPlayerToTemplate)
    .POP_EDX().POP_ECX().POP_EAX().POPFD() // Safety
    .JMP(0x8E5597)
    .NOP_PAD_TO_SIZE<56>();
static auto patch_play2temp_0x9DA7D2 = PATCH(0x9DA7D2)
    .PUSHFD().PUSH_EAX().PUSH_ECX().PUSH_EDX() // Safety
    .PUSH_EDI().PUSH_ESI() // Arguments
    .CALL(runtimeHookCharacterIdCopyPlayerToTemplate)
    .POP_EDX().POP_ECX().POP_EAX().POPFD() // Safety
    .JMP(0x9DA808)
    .NOP_PAD_TO_SIZE<54>();
static auto patch_temp2play_0x9DAAAA = PATCH(0x9DAAAA)
    .PUSHFD().PUSH_EAX().PUSH_ECX().PUSH_EDX() // Safety
    .PUSH_EDI().PUSH_EBX() // Arguments
    .CALL(runtimeHookCharacterIdCopyTemplateToPlayer)
    .POP_EDX().POP_ECX().POP_EAX().POPFD() // Safety
    .JMP(0x9DACC0)
    .NOP_PAD_TO_SIZE<534>();
static auto patch_play2temp_0xA028F9 = PATCH(0xA028F9)
    .PUSHFD().PUSH_EAX().PUSH_ECX().PUSH_EDX() // Safety
    .PUSH_ESI().PUSH_EDI() // Arguments
    .CALL(runtimeHookCharacterIdCopyPlayerToTemplate)
    .POP_EDX().POP_ECX().POP_EAX().POPFD() // Safety
    .JMP(0xA02933)
    .NOP_PAD_TO_SIZE<58>();

// Animation hook, a bit messy, but ehh
__declspec(naked) static void  __stdcall HOOK_0x9E1CA9() {
    __asm {
        pushfd
        push ecx
        push edx
        call runtimeHookCharacterIdAnimateBlocks
        mov eax, dword ptr ds : [0xB2BEA0]
        pop edx
        pop ecx
        popfd
        ret
    }
}
static auto patch_animate_hook_0x9E1CA9 = PATCH(0x9E1CA9).CALL(HOOK_0x9E1CA9);

// Check to allow player to pass through their own filter block type
/*__declspec(naked) static void  __stdcall HOOK_0x9A3CC5() {
    __asm {
        push ecx
        push edx
        push esi // Args #2
        push ebx // Args #1
        call runtimeHookCharacterIdBlockPlayerCheck
        and dword ptr ds:[ebp-0x54], eax
        pop edx
        pop ecx
        ret
    }
}
static auto patch_block_passthrough_0x9A3CC5 = PATCH(0x9A3CC5).CALL(HOOK_0x9A3CC5).JMP(0x9A3DD2).NOP_PAD_TO_SIZE<269>();*/
static auto patch_block_passthrough_0x9A3CC5 = PATCH(0x9A3CC5).NOP_PAD_TO_SIZE<269>();

// Check if this is a hittable switch block... 
__declspec(naked) static void  __stdcall HOOK_0x9DA747() {
    __asm {
        push eax
        push ecx
        push edx
        movsx eax, cx
        push eax // Args #1
        call runtimeHookCharacterIdSwitchBlockCheck
        cmp eax, 0
        jne alternate_exit
        pop edx
        pop ecx
        pop eax
        cmp cx, 0x277
        push 0x9DA74C
        ret
alternate_exit:
        pop edx
        pop ecx
        pop eax
        push 0x9DA752
        ret
    }
}
static auto patch_switch_block_check_0x9DA747 = PATCH(0x9DA747).JMP(HOOK_0x9DA747);

// Perform character switch block transformation for custom block types
__declspec(naked) static void  __stdcall HOOK_0x9DAA31() {
    __asm {
        push eax
        push ecx
        push edx
        push ebx // Args #2
        movsx ecx, word ptr ds:[edi]
        push ecx // Args #1
        call runtimeHookCharacterIdSwitchBlockTransform
        pop edx
        pop ecx
        pop eax
        cmp word ptr ds : [ebx + 0x1E], 0x277
        ret
    }
}
static auto patch_switch_block_transform_0x9DAA31 = PATCH(0x9DAA31).CALL(HOOK_0x9DAA31).NOP_PAD_TO_SIZE<6>();

// Patch player rendering so that we can do custom character sprites
__declspec(naked) static BOOL  __stdcall PlayerBitBltRawHook_74(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop) {
    __asm {
        pop eax
        push dword ptr ds : [ebp - 0x74] // Attach player argument
        push eax
        jmp PlayerBitBltHook
    }
}
static auto patch_player_bitblt_0x98A5CA = PATCH(0x98A5CA).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98A85D = PATCH(0x98A85D).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98ABD3 = PATCH(0x98ABD3).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98AF2E = PATCH(0x98AF2E).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98B65A = PATCH(0x98B65A).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98B912 = PATCH(0x98B912).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98BBC8 = PATCH(0x98BBC8).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98BE5E = PATCH(0x98BE5E).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98C1D8 = PATCH(0x98C1D8).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98C52E = PATCH(0x98C52E).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98CC5A = PATCH(0x98CC5A).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98CF0F = PATCH(0x98CF0F).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98D1BF = PATCH(0x98D1BF).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98D44B = PATCH(0x98D44B).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98D7A6 = PATCH(0x98D7A6).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98DAE4 = PATCH(0x98DAE4).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98E202 = PATCH(0x98E202).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98E492 = PATCH(0x98E492).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98E82B = PATCH(0x98E82B).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98EB8B = PATCH(0x98EB8B).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98EED4 = PATCH(0x98EED4).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98F22A = PATCH(0x98F22A).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98F943 = PATCH(0x98F943).CALL(PlayerBitBltRawHook_74);
static auto patch_player_bitblt_0x98FBD4 = PATCH(0x98FBD4).CALL(PlayerBitBltRawHook_74);

__declspec(naked) static BOOL  __stdcall PlayerBitBltRawHook_25C(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop) {
    __asm {
        pop eax
        push dword ptr ds : [ebp - 0x25C] // Attach player argument
        push eax
        jmp PlayerBitBltHook
    }
}
static auto patch_player_bitblt_0x91712C = PATCH(0x91712C).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x917345 = PATCH(0x917345).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x917DAD = PATCH(0x917DAD).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x917FDA = PATCH(0x917FDA).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x9185A5 = PATCH(0x9185A5).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x9187BE = PATCH(0x9187BE).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x919226 = PATCH(0x919226).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x919453 = PATCH(0x919453).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x919A1E = PATCH(0x919A1E).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x919C37 = PATCH(0x919C37).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x91A69F = PATCH(0x91A69F).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x91A8CC = PATCH(0x91A8CC).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x91AF7A = PATCH(0x91AF7A).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x91B191 = PATCH(0x91B191).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x91BBFB = PATCH(0x91BBFB).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x91BE2B = PATCH(0x91BE2B).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x91C2B5 = PATCH(0x91C2B5).CALL(PlayerBitBltRawHook_25C);
static auto patch_player_bitblt_0x91C4E2 = PATCH(0x91C4E2).CALL(PlayerBitBltRawHook_25C);

__declspec(naked) static BOOL  __stdcall PlayerBitBltRawHook_340(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop) {
    __asm {
        pop eax
        push dword ptr ds : [ebp - 0x340] // Attach player argument
        push eax
        jmp PlayerBitBltHook
    }
}
static auto patch_player_bitblt_0x923D28 = PATCH(0x923D28).CALL(PlayerBitBltRawHook_340);
static auto patch_player_bitblt_0x924176 = PATCH(0x924176).CALL(PlayerBitBltRawHook_340);
static auto patch_player_bitblt_0x924B40 = PATCH(0x924B40).CALL(PlayerBitBltRawHook_340);
static auto patch_player_bitblt_0x924F91 = PATCH(0x924F91).CALL(PlayerBitBltRawHook_340);
static auto patch_player_bitblt_0x92595E = PATCH(0x92595E).CALL(PlayerBitBltRawHook_340);
static auto patch_player_bitblt_0x925DB0 = PATCH(0x925DB0).CALL(PlayerBitBltRawHook_340);
static auto patch_player_bitblt_0x92677D = PATCH(0x92677D).CALL(PlayerBitBltRawHook_340);
static auto patch_player_bitblt_0x926BCB = PATCH(0x926BCB).CALL(PlayerBitBltRawHook_340);
static auto patch_player_bitblt_0x92756F = PATCH(0x92756F).CALL(PlayerBitBltRawHook_340);
static auto patch_player_bitblt_0x9279C1 = PATCH(0x9279C1).CALL(PlayerBitBltRawHook_340);

__declspec(naked) static BOOL  __stdcall PlayerBitBltRawHook_Editor(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop) {
    __asm {
        pop eax
        mov edx, dword ptr ds : [ebp - 0x1CC]
        imul edx, edx, 0x184
        add edx, dword ptr ds : [0xB250C8]
        push edx // Attach player argument
        push eax
        jmp PlayerBitBltHook
    }
}
static auto patch_player_bitblt_0x93E01D = PATCH(0x93E01D).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x93E26F = PATCH(0x93E26F).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x93E51B = PATCH(0x93E51B).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x93E76D = PATCH(0x93E76D).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x93EA19 = PATCH(0x93EA19).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x93EC6B = PATCH(0x93EC6B).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x93EF17 = PATCH(0x93EF17).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x93F169 = PATCH(0x93F169).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x93F415 = PATCH(0x93F415).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x93F667 = PATCH(0x93F667).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x944CCD = PATCH(0x944CCD).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x9450ED = PATCH(0x9450ED).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x9452D0 = PATCH(0x9452D0).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x94550D = PATCH(0x94550D).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x9456F0 = PATCH(0x9456F0).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x94592D = PATCH(0x94592D).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x945B10 = PATCH(0x945B10).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x945D4D = PATCH(0x945D4D).CALL(PlayerBitBltRawHook_Editor);
static auto patch_player_bitblt_0x945F30 = PATCH(0x945F30).CALL(PlayerBitBltRawHook_Editor);

__declspec(naked) static BOOL  __stdcall OwPlayerBitBltRawHook(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop) {
    __asm {
        pop eax
        push esi // Attach player argument
        push eax
        jmp OwPlayerBitBltHook
    }
}
static auto patch_player_bitblt_0x9034E7 = PATCH(0x9034E7).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x9036BC = PATCH(0x9036BC).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x903D66 = PATCH(0x903D66).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x90411B = PATCH(0x90411B).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x9042F0 = PATCH(0x9042F0).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x90499A = PATCH(0x90499A).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x904D4F = PATCH(0x904D4F).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x904F24 = PATCH(0x904F24).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x9055CE = PATCH(0x9055CE).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x905990 = PATCH(0x905990).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x905D29 = PATCH(0x905D29).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x905F00 = PATCH(0x905F00).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x9065DE = PATCH(0x9065DE).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x906973 = PATCH(0x906973).CALL(OwPlayerBitBltRawHook);
static auto patch_player_bitblt_0x906B31 = PATCH(0x906B31).CALL(OwPlayerBitBltRawHook);

// Patch overworld sprite rendering
static auto patch_owsprite_bitblt_0x902BA4 = PATCH(0x902BA4).CALL(OwSpriteBitBltHook);
static auto patch_owsprite_bitblt_0x902C86 = PATCH(0x902C86).CALL(OwSpriteBitBltHook);

// Patch player death animations
__declspec(naked) static void __stdcall PlayerDeathAnimationRawHook(short* effectID, Momentum* coor, float* effectFrame, short* npcID, short* showOnlyMask) {
    __asm {
        pop eax
        push edi // Attach player argument
        push eax
        jmp PlayerDeathAnimationHook
    }
}
static auto patch_player_death_effect_0x9B6C9B = PATCH(0x9B6C9B).CALL(PlayerDeathAnimationRawHook);
static auto patch_player_death_effect_0x9B6CE6 = PATCH(0x9B6CE6).CALL(PlayerDeathAnimationRawHook);
static auto patch_player_death_effect_0x9B6D31 = PATCH(0x9B6D31).CALL(PlayerDeathAnimationRawHook);
static auto patch_player_death_effect_0x9B6D7C = PATCH(0x9B6D7C).CALL(PlayerDeathAnimationRawHook);
static auto patch_player_death_effect_0x9B6DDA = PATCH(0x9B6DDA).CALL(PlayerDeathAnimationRawHook);

__declspec(naked) static void __stdcall EffectUpdateRawHook() {
    // 009E4C2B | 66 8B 0E | mov cx, word ptr ds : [esi] |
    // 009E4C2E | DF E0 | fnstsw ax |
    __asm {
        pushfd
        push eax
        push edx
        push esi // Attach effect argument
        call EffectUpdateHook
        mov cx, ax
        pop edx
        pop eax
        popfd
        fnstsw ax
        ret
    }
}
static auto patch_variant_effect_0x9E4C2B = PATCH(0x9E4C2B).CALL(EffectUpdateRawHook);

__declspec(naked) static void __stdcall RunEffectRawHook() {
    // 009E73C9 | 33 C0 | xor eax, eax |
    // 009E73CB | 66 8B 3B | mov di, word ptr ds : [ebx] |
    __asm {
        pushfd
        push ecx
        push edx
        push ebx // Attach effect argument
        call EffectUpdateHook
        mov di, ax
        pop edx
        pop ecx
        popfd
        xor eax, eax
        ret
    }
}
static auto patch_variant_effect_0x9E73C9 = PATCH(0x9E73C9).CALL(RunEffectRawHook);

// Patch list
static Patchable* runtimeHookCharacterIdPatchList[] = {
    //&patch_0x8C0329,
    //&patch_0x8C0362,
    //&patch_0x8C0B35,
    //&patch_0x8C0B6E,
    &patch_0x8D22B3,
    &patch_0x8D376C,
    &patch_0x8D3812,
    &patch_0x8D3DCB,
    &patch_0x8D3ED1,
    &patch_0x8D416E,
    &patch_0x8D5F06,
    //&patch_0x8E4857,
    &patch_play2temp_0x8E485F,
    &patch_0x8E540F,
    //&patch_0x8E5557,
    &patch_play2temp_0x8E555F,
    &patch_0x8E5CB1,
    &patch_0x8E5CD8,
    &patch_0x8E5F55,
    &patch_0x8E5F8E,
    &patch_0x8E5FD3,
    &patch_0x8E602B,
    &patch_0x8E6033,
    &patch_0x8E606D,
    &patch_0x8E60A6,
    &patch_0x8E60EB,
    &patch_0x8E6143,
    &patch_0x8E614B,
    &patch_0x8E6177,
    &patch_0x8EBE05,
    &patch_0x8EC124,
    &patch_0x8EC236,
    &patch_0x8EC555,
    &patch_0x8EC667,
    &patch_0x8EC986,
    &patch_0x8ECA95,
    &patch_0x8ECDB0,
    &patch_0x8ECEC1,
    &patch_0x8ED1E0,
    &patch_0x8ED2F2,
    &patch_0x8ED611,
    &patch_0x8EDD83,
    &patch_0x8EE1FE,
    &patch_0x8EE679,
    &patch_0x8EEAF3,
    &patch_0x8EEF76,
    &patch_0x8EFF86,
    &patch_0x8F0205,
    &patch_0x8F0741,
    &patch_0x8F0BA1,
    &patch_0x8F0E20,
    &patch_owsprite_bitblt_0x902BA4,
    &patch_owsprite_bitblt_0x902C86,
    &patch_0x9032F4,
    &patch_player_bitblt_0x9034E7,
    &patch_player_bitblt_0x9036BC,
    &patch_player_bitblt_0x903D66,
    &patch_player_bitblt_0x90411B,
    &patch_player_bitblt_0x9042F0,
    &patch_player_bitblt_0x90499A,
    &patch_player_bitblt_0x904D4F,
    &patch_player_bitblt_0x904F24,
    &patch_player_bitblt_0x9055CE,
    &patch_player_bitblt_0x905990,
    &patch_player_bitblt_0x905D29,
    &patch_player_bitblt_0x905F00,
    &patch_player_bitblt_0x9065DE,
    &patch_player_bitblt_0x906973,
    &patch_player_bitblt_0x906B31,
    &patch_0x916B7E,
    &patch_0x917FF7,
    &patch_player_bitblt_0x91712C,
    &patch_player_bitblt_0x917345,
    &patch_player_bitblt_0x917DAD,
    &patch_player_bitblt_0x917FDA,
    &patch_player_bitblt_0x9185A5,
    &patch_player_bitblt_0x9187BE,
    &patch_player_bitblt_0x919226,
    &patch_player_bitblt_0x919453,
    &patch_0x919470,
    &patch_player_bitblt_0x919A1E,
    &patch_player_bitblt_0x919C37,
    &patch_player_bitblt_0x91A69F,
    &patch_player_bitblt_0x91A8CC,
    &patch_0x91A8E9,
    &patch_player_bitblt_0x91AF7A,
    &patch_player_bitblt_0x91B191,
    &patch_player_bitblt_0x91BBFB,
    &patch_player_bitblt_0x91BE2B,
    &patch_0x91BE48,
    &patch_player_bitblt_0x91C2B5,
    &patch_player_bitblt_0x91C4E2,
    &patch_0x9238B0,
    &patch_0x923AFD,
    &patch_player_bitblt_0x923D28,
    &patch_0x923F4F,
    &patch_player_bitblt_0x924176,
    &patch_0x9246CC,
    &patch_0x924919,
    &patch_player_bitblt_0x924B40,
    &patch_0x924D67,
    &patch_player_bitblt_0x924F91,
    &patch_0x9254E7,
    &patch_0x925734,
    &patch_player_bitblt_0x92595E,
    &patch_0x925B85,
    &patch_player_bitblt_0x925DB0,
    &patch_0x926305,
    &patch_0x926552,
    &patch_player_bitblt_0x92677D,
    &patch_0x9269A4,
    &patch_player_bitblt_0x926BCB,
    &patch_0x927121,
    &patch_0x927345,
    &patch_player_bitblt_0x92756F,
    &patch_0x927796,
    &patch_player_bitblt_0x9279C1,
    &patch_0x93DC2C,
    &patch_0x93DDC7,
    &patch_player_bitblt_0x93E01D,
    &patch_player_bitblt_0x93E26F,
    &patch_0x93E2C5,
    &patch_player_bitblt_0x93E51B,
    &patch_player_bitblt_0x93E76D,
    &patch_0x93E7C3,
    &patch_player_bitblt_0x93EA19,
    &patch_player_bitblt_0x93EC6B,
    &patch_0x93ECC1,
    &patch_player_bitblt_0x93EF17,
    &patch_player_bitblt_0x93F169,
    &patch_0x93F1BF,
    &patch_player_bitblt_0x93F415,
    &patch_player_bitblt_0x93F667,
    &patch_0x944970,
    &patch_0x944AE6,
    &patch_player_bitblt_0x944CCD,
    &patch_0x944F06,
    &patch_player_bitblt_0x9450ED,
    &patch_player_bitblt_0x9452D0,
    &patch_0x945326,
    &patch_player_bitblt_0x94550D,
    &patch_player_bitblt_0x9456F0,
    &patch_0x945746,
    &patch_player_bitblt_0x94592D,
    &patch_player_bitblt_0x945B10,
    &patch_0x945B66,
    &patch_player_bitblt_0x945D4D,
    &patch_player_bitblt_0x945F30,
    &patch_0x96C063,
    &patch_0x96E1A7,
    &patch_0x974CFC,
    &patch_0x974EE5,
    &patch_0x9751FA,
    &patch_0x977227,
    &patch_0x97D9AB,
    &patch_0x97DBA0,
    &patch_0x97DE9B,
    &patch_0x97FCC5,
    &patch_0x980FFC,
    &patch_0x98735A,
    &patch_0x9873FE,
    &patch_0x9874A2,
    &patch_0x987546,
    &patch_0x9875EA,
    &patch_0x9877E8,
    &patch_0x98788C,
    &patch_0x987930,
    &patch_0x9879D4,
    &patch_0x987A78,
    &patch_0x98A331,
    &patch_player_bitblt_0x98A5CA,
    &patch_player_bitblt_0x98A85D,
    &patch_player_bitblt_0x98ABD3,
    &patch_player_bitblt_0x98AF2E,
    &patch_player_bitblt_0x98B65A,
    &patch_player_bitblt_0x98B912,
    &patch_0x98B92C,
    &patch_player_bitblt_0x98BBC8,
    &patch_player_bitblt_0x98BE5E,
    &patch_player_bitblt_0x98C1D8,
    &patch_player_bitblt_0x98C52E,
    &patch_player_bitblt_0x98CC5A,
    &patch_player_bitblt_0x98CF0F,
    &patch_0x98CF29,
    &patch_player_bitblt_0x98D1BF,
    &patch_player_bitblt_0x98D44B,
    &patch_player_bitblt_0x98D7A6,
    &patch_player_bitblt_0x98DAE4,
    &patch_0x98DF6B,
    &patch_player_bitblt_0x98E202,
    &patch_player_bitblt_0x98E492,
    &patch_player_bitblt_0x98E82B,
    &patch_player_bitblt_0x98EB8B,
    &patch_player_bitblt_0x98EED4,
    &patch_player_bitblt_0x98F22A,
    &patch_0x98F6B1,
    &patch_player_bitblt_0x98F943,
    &patch_player_bitblt_0x98FBD4,
    &patch_0x98FBE9,
    &patch_0x98FBF6,
    &patch_0x994764,
    &patch_0x994A24,
    &patch_0x994A78,
    &patch_0x994AD7,
    &patch_0x994AE8,
    &patch_0x994B2D,
    &patch_0x994D83,
    &patch_0x994F4C,
    &patch_0x9953AC,
    &patch_0x99792D,
    &patch_0x99799D,
    &patch_0x997CA1,
    &patch_0x998040,
    &patch_0x998085,
    &patch_0x9980E6,
    &patch_0x998614,
    &patch_0x9987FF,
    &patch_0x998844,
    &patch_0x998889,
    &patch_0x999449,
    &patch_0x999470,
    &patch_0x9994E1,
    &patch_0x99966B,
    &patch_0x999833,
    &patch_0x999866,
    &patch_0x9999F4,
    &patch_0x999AB5,
    &patch_0x999B1A,
    &patch_0x999B44,
    &patch_0x999B69,
    &patch_0x999BA0,
    &patch_0x999BCA,
    &patch_0x999BEF,
    &patch_0x999C15,
    &patch_0x999C94,
    &patch_0x999CB9,
    &patch_0x999CDE,
    &patch_0x999E1B,
    &patch_0x999E7E,
    &patch_0x999EA8,
    &patch_0x999ECD,
    &patch_0x999F04,
    &patch_0x999F2E,
    &patch_0x999F53,
    &patch_0x999F79,
    &patch_0x999FF8,
    &patch_0x99A01D,
    &patch_0x99A042,
    &patch_0x99A16B,
    &patch_0x99A196,
    &patch_0x99A1BB,
    &patch_0x99A2B5,
    &patch_0x99A607,
    &patch_0x99A6A0,
    &patch_0x99A6E6,
    &patch_0x99A71B,
    &patch_0x99A750,
    &patch_0x99A785,
    &patch_0x99A7B7,
    &patch_0x99A850,
    &patch_0x99ABA6,
    &patch_0x99B7C8,
    &patch_0x99BB7B,
    &patch_0x99C017,
    &patch_0x99C096,
    &patch_0x99C510,
    &patch_0x99CB3B,
    &patch_0x99CEED,
    &patch_0x99D00C,
    &patch_0x99D40A,
    &patch_0x99D4AD,
    &patch_0x99D70E,
    &patch_0x99D736,
    &patch_0x99D83D,
    &patch_0x99D882,
    &patch_0x99E265,
    &patch_0x99E33A,
    &patch_0x99E6AC,
    &patch_0x99EB43,
    &patch_0x99EC52,
    &patch_0x99ED2F,
    &patch_0x99EE6F,
    &patch_0x99EEB3,
    &patch_0x99F03E,
    &patch_0x99F0A1,
    &patch_0x99F0DD,
    &patch_0x99F12F,
    &patch_0x99F57A,
    &patch_0x99F62E,
    &patch_0x9A008E,
    &patch_0x9A04B6,
    &patch_0x9A13BE,
    &patch_0x9A32CB,
    &patch_block_passthrough_0x9A3CC5,
// Character ! blocks
//    &patch_0x9A3CD9,
//    &patch_0x9A3D13,
//    &patch_0x9A3D4D,
//    &patch_0x9A3D87,
//    &patch_0x9A3DC1,
// runs after collision loop, duplicated by runtimeHookPlayerBlockCollisionEnd
//    &patch_0x9A5015,
    &patch_0x9A504D,
    &patch_0x9A5367,
    &patch_0x9A5377,
    &patch_0x9A538B,
    &patch_0x9A5C52,
    &patch_0x9A5DA6,
    &patch_0x9A6856,
    &patch_0x9A69A9,
    &patch_0x9A72FE,
    &patch_0x9A75A5,
    &patch_0x9A8348,
    &patch_0x9AA469,
    &patch_0x9AA6B9,
    &patch_0x9AAA0E,
    // If Link, become fairy for climbing. Conflicts with defines.player_link_fairyVineEnabled so commented out, and all current extended characters use defines.player_link_fairyVineEnabled to disable this anyway.
    // &patch_0x9AAF8C,
    &patch_0x9ABE80,
    &patch_0x9ABFB5,
    &patch_0x9AC153,
    &patch_0x9AC705,
    &patch_0x9AD678,
    &patch_0x9AD6FB,
    &patch_0x9ADAC0,
    &patch_0x9AEA0D,
    &patch_0x9AEB73, 
    &patch_0x9B081A,
    &patch_0x9B0863,
    &patch_0x9B08F3,
    &patch_0x9B11DB,
    &patch_0x9B1A55,
    &patch_0x9B55CB,
    &patch_0x9B57F0,
    &patch_0x9B5D21,
    &patch_0x9B5D71,
    &patch_0x9B61B4,
    &patch_0x9B61DD,
    &patch_0x9B6222,
    &patch_0x9B68C9,
    &patch_0x9B6C34,
    &patch_player_death_effect_0x9B6C9B,
    &patch_player_death_effect_0x9B6CE6,
    &patch_player_death_effect_0x9B6D31,
    &patch_player_death_effect_0x9B6D7C,
    &patch_player_death_effect_0x9B6DDA,
    &patch_0x9B6F12,
    &patch_0x9B6FD4,
    &patch_0x9B71D3,
    &patch_0x9B722D,
    &patch_0x9B760F,
    &patch_0x9B7D86,
    &patch_0x9B86E4,
    &patch_0x9B8DFB,
    &patch_0x9B9B24,
    &patch_0x9B9B88,
    &patch_0x9B9BD7,
    &patch_0x9B9CD4,
    &patch_0x9B9D0E,
    &patch_0x9BA1D7,
    &patch_0x9BA28E,
    &patch_0x9BA847,
    &patch_0x9BA8F3,
    &patch_0x9BA956,
    &patch_0x9BAAAC,
    &patch_0x9BB469,
    &patch_0x9BB886,
    &patch_0x9BE92D,
    &patch_0x9C0D14,
    &patch_0x9C0EA8,
    &patch_0x9C0F5E,
    &patch_0x9C1008,
    &patch_0x9C10BE,
    &patch_0x9C119E,
    &patch_0x9C1267,
    &patch_0x9C131E,
    &patch_0x9C140B,
    &patch_0x9C1BAD,
    &patch_0x9C1C9D,
    &patch_0x9C47DE,
    &patch_0x9C4817,
    &patch_0x9C66D1,
    &patch_0x9C6845,
    &patch_0x9C6B0C,
    &patch_0x9C6E51,
    &patch_0x9C6F03,
    &patch_0x9C7378,
    &patch_0x9C76B4,
    &patch_0x9C788F,
    &patch_0x9C79D8,
    &patch_0x9C7E49,
    &patch_0x9C7EF5,
    &patch_0x9C80EC,
    &patch_0x9C8176,
    &patch_0x9C8388,
    &patch_0x9C8D32,
    &patch_0x9C8E62,
    &patch_0x9C94A9,
    &patch_0x9C966B,
    &patch_0x9CA23B,
    &patch_0x9CB66C,
    &patch_0x9CBA23,
    &patch_0x9CC44A,
    &patch_0x9CC45D,
    &patch_0x9CC474,
    &patch_0x9CE004,
    &patch_0x9CE05E,
    &patch_0x9CF1E7,
    &patch_0x9D0402,
    &patch_0x9D05BB,
    &patch_0x9D079B,
    &patch_0x9D214B,
    &patch_0x9D215E,
    &patch_0x9D21D5,
    &patch_0x9D21E8,
    &patch_0x9D2251,
    &patch_0x9D2296,
    &patch_0x9D22F6,
    &patch_0x9D2360,
    &patch_0x9D23C3,
    &patch_0x9D23D6,
    &patch_0x9D244D,
    &patch_0x9D2460,
    &patch_0x9D24C9,
    &patch_0x9D250E,
    &patch_0x9D2553,
    &patch_0x9D2664,
    &patch_0x9D26A9,
    &patch_0x9D26D5,
    &patch_0x9D2877,
    &patch_0x9D288A,
    &patch_0x9D2901,
    &patch_0x9D2914,
    &patch_0x9D297D,
    &patch_0x9D29C2,
    &patch_0x9D2A32,
    &patch_0x9D2A45,
    &patch_0x9D2ABC,
    &patch_0x9D2ACF,
    &patch_0x9D2B38,
    &patch_0x9D2B7D,
    &patch_0x9D2BEE,
    &patch_0x9D2C01,
    &patch_0x9D2C78,
    &patch_0x9D2C8B,
    &patch_0x9D2CF4,
    &patch_0x9D2D39,
    &patch_0x9D2DAF,
    &patch_0x9D2DF4,
    &patch_0x9D2E20,
    &patch_0x9D2FE5,
    &patch_0x9D302A,
    &patch_0x9D3056,
    &patch_0x9D33DC,
    &patch_0x9D3A82,
    &patch_0x9D3D84,
    &patch_0x9D3E6B,
    &patch_0x9D4178,
    &patch_0x9D425F,
    &patch_0x9D461E,
    &patch_0x9D4868,
    &patch_0x9D4B47,
    &patch_0x9D4C2F,
    &patch_0x9D4E6E,
    &patch_0x9D4F55,
    &patch_0x9D575B,
    &patch_0x9D5A0A,
    &patch_0x9D5C1E,
    &patch_0x9D5C45,
    &patch_0x9D5D2C,
    &patch_0x9D5E92,
    &patch_0x9D5F79,
    &patch_0x9D614D,
    &patch_0x9D6174,
    &patch_0x9D625B,
    &patch_0x9D6396,
    &patch_0x9D647D,
    &patch_0x9D6B14,
    &patch_0x9D6CD5,
    &patch_0x9D6CE8,
    &patch_0x9D7311,
    &patch_0x9D7437,
    &patch_0x9D7571,
    &patch_0x9D81E0,
    &patch_0x9D82CE,
    &patch_0x9D82F8,
    &patch_0x9D830B,
    &patch_0x9D8382,
    &patch_0x9D8395,
    &patch_0x9D83FE,
    &patch_0x9D8443,
    &patch_0x9D84A1,
    &patch_0x9D850C,
    &patch_0x9D85E7,
    &patch_0x9D86D5,
    &patch_0x9D86FF,
    &patch_0x9D8712,
    &patch_0x9D8789,
    &patch_0x9D879C,
    &patch_0x9D8805,
    &patch_0x9D884A,
    &patch_0x9D88A8,
    &patch_0x9D8913,
    &patch_0x9D89FD,
    &patch_0x9D8A10,
    &patch_0x9D8A87,
    &patch_0x9D8A9A,
    &patch_0x9D8B0C,
    &patch_0x9D8B51,
    &patch_0x9D8BAF,
    &patch_0x9D8C23,
    &patch_0x9D8E29,
    &patch_0x9D8E3C,
    &patch_0x9D8EB3,
    &patch_0x9D8EC6,
    &patch_0x9D8F38,
    &patch_0x9D8F7D,
    &patch_0x9D8FDB,
    &patch_0x9D904F,
    &patch_0x9D923D,
    &patch_0x9D9250,
    &patch_0x9D92C7,
    &patch_0x9D92DA,
    &patch_0x9D934C,
    &patch_0x9D9391,
    &patch_0x9D93EF,
    &patch_0x9D9463,
    &patch_0x9D9AF3,
    &patch_0x9D9C6A,
    &patch_switch_block_check_0x9DA747,
    //&patch_0x9DA7CA,
    &patch_play2temp_0x9DA7D2,
// Character change block can activate
//    &patch_0x9DA827,
//    &patch_0x9DA863,
//    &patch_0x9DA893,
//    &patch_0x9DA8C3,
//    &patch_0x9DA8F2,
    //&patch_0x9DAAA2,
    &patch_switch_block_transform_0x9DAA31,
    &patch_temp2play_0x9DAAAA,// 0x9DACC0
    //&patch_0x9DAB17,
    //&patch_0x9DAB8C,
    //&patch_0x9DABFE,
    //&patch_0x9DAC73,
    &patch_0x9DAD04,
    &patch_0x9DB9CC,
    &patch_0x9DBA82,
    &patch_0x9DBAAB,
    &patch_0x9DBD7E,
    &patch_0x9DC126,
    &patch_0x9DCC3F,
    &patch_0x9DCD40,
    &patch_0x9DCE6D,
    &patch_0x9DCF6E,
    &patch_0x9DDE82,
    &patch_0x9DDF1E,
    &patch_0x9DE629,
    &patch_0x9DE6C4,
    &patch_0x9DEF81,
    &patch_0x9DF01D,
    &patch_0x9E086C,
// Character block animation
//    &patch_0x9E19BE,
//    &patch_0x9E19F3,
//    &patch_0x9E1A28,
//    &patch_0x9E1A63,
//    &patch_0x9E1A96,
//    &patch_0x9E1BD6,
//    &patch_0x9E1BF9,
    &patch_animate_hook_0x9E1CA9,
    &patch_variant_effect_0x9E4C2B,
    &patch_variant_effect_0x9E73C9,
    &patch_0xA02866,
    &patch_0xA02899,
//    &patch_0xA028F1,
    &patch_play2temp_0xA028F9,
    &patch_0xA02A72,
    &patch_0xA02AA4,
    &patch_0xA03801,
    &patch_0xA03828,
    &patch_0xA03AD5,
    &patch_0xA243F0,
    &patch_0xA2509E,
    &patch_0xA256A7,
    &patch_0xA25DED,
    &patch_0xA25E16,
    &patch_0xA25F58,
    &patch_0xA25F64,
    &patch_0xA2771F,
    &patch_0xA27800,
    &patch_0xA27835,
    &patch_0xA27912,
    &patch_0xA2793B,
    &patch_0xA27A06,
    &patch_0xA27AA7,
    &patch_0xA291A5,
    &patch_0xA4456C,
    &patch_0xA52D55,
    &patch_0xA60B49,
    &patch_0xA60B6B,
    &patch_0xA60B96,
    &patch_0xA60BB9,
    &patch_0xA60BE3,
    nullptr
};

///////////////////////
// Hook support code //
///////////////////////

short* getValidCharacterIDArray()
{
    static short* ret = nullptr;
    static unsigned int lastListSize = 0;
    int newListSize = 5 + runtimeHookCharacterIdMap.size() + 1;

    if (lastListSize != newListSize)
    {
        // Free old memory if there was any
        if (ret != nullptr)
        {
            delete[] ret;
            ret = nullptr;
        }

        // Allocate memory
        ret = new short[newListSize]();
    }

    int idx = 0;

    // Populate with vanilla IDs
    for (int id = 1; id <= 5; id++)
    {
        ret[idx++] = id;
    }

    // Populate with extended IDs
    for (auto it = runtimeHookCharacterIdMap.cbegin(); it != runtimeHookCharacterIdMap.cend(); it++) {
        ret[idx++] = it->first;
    }

    // Terminate with 0
    ret[idx++] = 0;

    return ret;
}

PlayerMOB* getTemplateForCharacter(int id)
{
    // Return vanilla character template
    if (id >= 1 && id <= 5) {
        PlayerMOB* playerTemplate = &((PlayerMOB*)GM_PLAYERS_TEMPLATE)[id];
        return playerTemplate;
    }

    // Return mapped character template
    auto it = runtimeHookCharacterIdMap.find(id);
    if (it != runtimeHookCharacterIdMap.end())
    {
        return &it->second->mStoredTemplate;
    }

    return nullptr;
}

PlayerMOB* getTemplateForCharacterWithDummyFallback(int id)
{
    static PlayerMOB dummyPlayerStruct = {0};

    PlayerMOB* ret = getTemplateForCharacter(id);

    if (ret)
    {
        return ret;
    }
    else
    {
        // None found, return dummy character template, because hey, let's not crash
        memset(&dummyPlayerStruct, 0, sizeof(PlayerMOB));
        dummyPlayerStruct.Identity = (Characters)id;
        dummyPlayerStruct.Hearts = 1;
        return &dummyPlayerStruct;
    }
}

///////////////////////
// Hook support code //
///////////////////////

int16_t CharacterIdTranslate(int16_t characterId)
{
    // Vanilla ones are always unchanged
    if (characterId <= 5)
    {
        return characterId;
    }

    // Return mapped character id
    auto it = runtimeHookCharacterIdMap.find(characterId);
    if (it != runtimeHookCharacterIdMap.end())
    {
        return it->second->mBaseCharacter;
    }

    return 0;
}

int __stdcall runtimeHookCharacterIdTranslateHook(short* idPtr)
{
    return CharacterIdTranslate(*idPtr);
}

///////////
// Hooks //
///////////

void __stdcall runtimeHookCharacterIdCopyPlayerToTemplate(int characterId, int playerIdx)
{
    PlayerMOB* temp = getTemplateForCharacterWithDummyFallback(characterId);
    PlayerMOB* player = &((PlayerMOB*)GM_PLAYERS_PTR)[playerIdx];
    memcpy(temp, player, sizeof(PlayerMOB));
}

static void __stdcall runtimeHookCharacterIdCopyTemplateToPlayer(int characterId, int playerIdx)
{
    PlayerMOB* temp = getTemplateForCharacterWithDummyFallback(characterId);
    PlayerMOB* player = &((PlayerMOB*)GM_PLAYERS_PTR)[playerIdx];
    player->CurrentPowerup     = temp->CurrentPowerup;
    player->PowerupBoxContents = temp->PowerupBoxContents;
    player->MountType          = temp->MountType;
    player->MountColor         = temp->MountColor;
    player->Hearts             = temp->Hearts;
}

static void __stdcall runtimeHookCharacterIdAnimateBlocks(void)
{
    // Get current filter block frame
    short filterBlockTimer = GM_BLOCK_ANIM_TIMER[626 - 1] + 1;
    short filterBlockFrame;
    if (filterBlockTimer == 0 || filterBlockTimer >= 0x10)
    {
        filterBlockFrame = 1;
    }
    else if (filterBlockTimer >= 8)
    {
        filterBlockFrame = 2;
    }
    else
    {
        filterBlockFrame = 3;
    }

    // Get current switch block frame
    short switchBlockFrame = GM_BLOCK_ANIM_FRAME[4 - 1];

    // Loop over registered character IDs to check their block types
    for (auto it = runtimeHookCharacterIdMap.cbegin(); it != runtimeHookCharacterIdMap.cend(); it++) {
        if (it->second->mFilterBlock == 0 && it->second->mSwitchBlock == 0) continue;
        short id = it->second->mId;
        bool anyIsNot = (GM_PLAYERS_COUNT == 0);
        bool anyIs = false;

        // Check if any characters
        for (short playerIdx = 1; playerIdx <= GM_PLAYERS_COUNT; playerIdx++) {
            PlayerMOB* player = Player::Get(playerIdx);
            if (player->Identity == id)
            {
                anyIs = true;
            }
            else
            {
                anyIsNot = true;
            }
        }

        // Filter block handling
        short filterBlockId = it->second->mFilterBlock;
        if (filterBlockId != 0)
        {
            if (anyIs) // Any player is the character
            {
                GM_BLOCK_ANIM_FRAME[filterBlockId - 1] = filterBlockFrame;
            }
            else
            {
                GM_BLOCK_ANIM_FRAME[filterBlockId - 1] = 0;
            }
        }

        // Switch block handling
        short switchBlockId = it->second->mSwitchBlock;
        if (switchBlockId != 0)
        {
            if (anyIsNot) // Any player is not the character
            {
                GM_BLOCK_ANIM_FRAME[switchBlockId - 1] = switchBlockFrame;
            }
            else
            {
                GM_BLOCK_ANIM_FRAME[switchBlockId - 1] = 4;
            }
        }
    }
}

/*static int __stdcall runtimeHookCharacterIdBlockPlayerCheck(PlayerMOB* player, int blockIdx)
{
    short blockId = Block::GetRaw(blockIdx)->BlockType;
    short characterFilter = Blocks::GetBlockPlayerFilter(blockId);

    // -1 means allow all characters
    if (characterFilter == -1)
    {
        return 0;
    }

    short characterId = (short)player->Identity;
    if (characterFilter == characterId)
    {
        return 0;
    }

    return -1;
}*/

static int __stdcall runtimeHookCharacterIdSwitchBlockCheck(int blockId)
{
    if (blockId == 0) return 0;

    for (auto it = runtimeHookCharacterIdMap.cbegin(); it != runtimeHookCharacterIdMap.cend(); it++) {
        if (it->second->mSwitchBlock == blockId)
        {
            return -1;
        }
    }

    return 0;
}

static void __stdcall runtimeHookCharacterIdSwitchBlockTransform(int playerIdx, Block* block)
{
    short blockId = block->BlockType;
    if (blockId == 0) return;

    for (auto it = runtimeHookCharacterIdMap.cbegin(); it != runtimeHookCharacterIdMap.cend(); it++) {
        if (it->second->mSwitchBlock == blockId)
        {
            PlayerMOB* player = Player::Get(playerIdx);
            player->Identity = (Characters)it->second->mId;
            return;
        }
    }
}

static BOOL __stdcall PlayerBitBltHook(
    PlayerMOB* player,
    HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop
    )
{
    short characterId = player->Identity;
    if (!(characterId >= 1 && characterId <= 5))
    {
        auto it = runtimeHookCharacterIdMap.find(characterId);
        if (it != runtimeHookCharacterIdMap.end())
        {
            // If we have a custom character with this character ID...
            std::shared_ptr<LunaImage> sprite = ImageLoader::GetByName(it->second->mName + "-" + std::to_string(player->CurrentPowerup));
            if (sprite)
            {
                // If we have a valid sprite for this powerup...
                if (GM_PLAYER_SHADOWSTAR || (dwRop != SRCAND)) {
                    sprite->draw(nXDest, nYDest, nWidth, nHeight, nXSrc, nYSrc);
                }
                return TRUE;
            }
        }
    }

    return BitBltHook(hdcDest, nXDest, nYDest, nWidth, nHeight,
        hdcSrc, nXSrc, nYSrc, dwRop);
}


static BOOL __stdcall OwPlayerBitBltHook(
    PlayerMOB* player,
    HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop
)
{
    if (gSMBXHUDSettings.overworldHudState == WHUD_NONE || gSMBXHUDSettings.overworldHudState == WHUD_ONLY_OVERLAY)
        return -1;

    return PlayerBitBltHook(player, hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}

static BOOL __stdcall OwSpriteBitBltHook(
    HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
    HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop
    )
{
    // Always first player in this case
    PlayerMOB* player = Player::Get(1);
    short characterId = player->Identity;

    if (!(characterId >= 1 && characterId <= 5))
    {
        auto it = runtimeHookCharacterIdMap.find(characterId);
        if (it != runtimeHookCharacterIdMap.end())
        {
            // If we have a custom character with this character ID...
            std::shared_ptr<LunaImage> owSprite = ImageLoader::GetByName("player-" + std::to_string(it->second->mId));
            if (owSprite)
            {
                // If we have a valid overworld sprite...
                if (dwRop != SRCAND) {
                    owSprite->draw(nXDest, nYDest, nWidth, nHeight, nXSrc, nYSrc);
                }
                return TRUE;
            }
        }
    }

    return BitBltHook(hdcDest, nXDest, nYDest, nWidth, nHeight,
        hdcSrc, nXSrc, nYSrc, dwRop);
}

static void __stdcall PlayerDeathAnimationHook(
    PlayerMOB* player,
    short* effectID, Momentum* coor, float* effectFrame, short* npcID, short* showOnlyMask
    )
{
    short newEffectId;
    short characterId = player->Identity;

    if (!(characterId >= 1 && characterId <= 5))
    {
        auto it = runtimeHookCharacterIdMap.find(characterId);
        if (it != runtimeHookCharacterIdMap.end())
        {
            // If we have a custom character with this character ID...
            newEffectId = it->second->mDeathEffect;
            if (newEffectId > 0)
            {
                native_runEffect(&newEffectId, coor, effectFrame, npcID, showOnlyMask);
                return;
            }
        }
    }

    native_runEffect(effectID, coor, effectFrame, npcID, showOnlyMask);
}

static int __stdcall EffectUpdateHook(short* effectId)
{
    for (auto it = runtimeHookCharacterIdMap.cbegin(); it != runtimeHookCharacterIdMap.cend(); it++) {
        if (it->second->mDeathEffect == 0) continue;
        if (it->second->mDeathEffect == *effectId)
        {
            // Act like effect 3 if it's a player death effect
            return 3;
        }
    }
    
    return *effectId;
}

/////////////////////
// Management code //
/////////////////////

void runtimeHookCharacterIdApplyPatch(void)
{
    if (runtimeHookCharacterIdApplied) return;
    for (unsigned int i = 0; runtimeHookCharacterIdPatchList[i] != nullptr; i++)
    {
        runtimeHookCharacterIdPatchList[i]->Apply();
    }
    runtimeHookCharacterIdApplied = true;
}

void runtimeHookCharacterIdUnpplyPatch(void)
{
    if (!runtimeHookCharacterIdApplied) return;
    for (unsigned int i = 0; runtimeHookCharacterIdPatchList[i] != nullptr; i++)
    {
        runtimeHookCharacterIdPatchList[i]->Unapply();
    }
    runtimeHookCharacterIdApplied = false;
}

void runtimeHookCharacterIdRegister(short id, const std::string& name, short base, short filterBlock, short switchBlock, short deathEffect)
{
    // Load Sprites
    if (name.size() > 0)
    {
        for (int powerupId = 1; powerupId <= 10; powerupId++)
        {
            ImageLoader::RegisterExtraGfx(name, name + "-" + std::to_string(powerupId));
        }
    }

    // Load overworld sprite
    std::shared_ptr<LunaImage> owsprite = nullptr;
    {
        ImageLoader::RegisterExtraGfx("player", "player-" + std::to_string(id));
    }

    // Set death effect size based on death effect image
    if (deathEffect > 0) {
        std::shared_ptr<LunaImage> deathImage = ImageLoader::GetByName("effect-" + std::to_string(deathEffect));
        if (deathImage)
        {
            int w, h;
            w = deathImage->getW();
            h = deathImage->getH();

            // If the base character is 5, there are left/right frames
            if (base == 5)
            {
                h /= 2;
            }

            effectdef_width[deathEffect - 1] = w;
            effectdef_height[deathEffect - 1] = h;
        }
    }

    // Switch blocks are generally bumpable
    if (switchBlock) Blocks::SetBlockBumpable(switchBlock, true);

    runtimeHookCharacterIdMap[id] = std::make_unique<CharacterDataStruct>(id, name, base, filterBlock, switchBlock, deathEffect);
    // Note: No longer applied on-demand. Always have these patches in place for consistency
    // runtimeHookCharacterIdApplyPatch();
}

void runtimeHookCharacterIdUnregister(short id)
{
    runtimeHookCharacterIdMap.erase(id);
    if (runtimeHookCharacterIdMap.size() == 0)
    {
        // Note: No longer undo character ID patches, at this point it's safer to leave them in place
        // runtimeHookCharacterIdUnpplyPatch();
    }
}

void runtimeHookCharacterIdReset()
{
    runtimeHookCharacterIdMap.clear();
    // Note: No longer undo character ID patches, at this point it's safer to leave them in place
    // runtimeHookCharacterIdUnpplyPatch();
}

CharacterHitBoxData* runtimeHookGetExtCharacterHitBoxData(short characterId, short powerupId)
{
    if (characterId >= 1 && characterId <= 5)
    {
        return nullptr;
    }

    if (powerupId < 1 || characterId > 10)
    {
        return nullptr;
    }

    auto it = runtimeHookCharacterIdMap.find(characterId);
    if (it != runtimeHookCharacterIdMap.end())
    {
        return &it->second->mHitbox[powerupId];
    }

    return nullptr;
}
