#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "AsmPatch.h"
#include "NpcIdExtender.h"
#include "../SMBXInternal/NPCs.h"
#include "../SMBXInternal/BGOs.h"
#include "../SMBXInternal/Blocks.h"

class ResizableGameArray
{
private:
    void**           m_ptr;
    uint32_t         m_size;
    uint32_t         m_elementBytes;
    const uint32_t*  m_patches;
    const uint32_t*  m_ptrsList;

public:
    constexpr ResizableGameArray(void** ptr, uint32_t size, uint32_t elementBytes, const uint32_t* patches, const uint32_t* ptrsList = nullptr) :
        m_ptr(ptr), m_size(size), m_elementBytes(elementBytes), m_patches(patches), m_ptrsList(ptrsList)
    {}

    inline void Patch(uint32_t size) {
        // Allocate new array and substitute it in
        void* newArray = std::malloc(size * m_elementBytes);
        uint32_t minBytes = ((m_size < size) ? m_size : size) * m_elementBytes;
        uint32_t bytes = size * m_elementBytes;
        uint32_t i = 0;
        if (m_ptr != nullptr)
        {
            for (; i < minBytes; i++)
            {
                ((uint8_t*)newArray)[i] = ((uint8_t*)*m_ptr)[i];
            }
        }
        for (; i < bytes; i++)
        {
            ((uint8_t*)newArray)[i] = 0;
        }
        if (m_ptr != nullptr)
        {
            *m_ptr = newArray;
        }
        if (m_ptrsList != nullptr)
        {
            for (uint32_t i = 0; m_ptrsList[i] != 0; i++)
            {
                PATCH(m_ptrsList[i]).dword(reinterpret_cast<uintptr_t>(newArray)).Apply();
            }
        }
        m_size = size;

        // Apply patches
        for (uint32_t i = 0; m_patches[i] != 0; i++)
        {
            PATCH(m_patches[i]).dword(size).Apply();
        }
    }

    inline bool ValidateSize()
    {
        for (uint32_t i = 0; m_patches[i] != 0; i++)
        {
            if (*((uint32_t*)m_patches[i]) != m_size) {
                return false;
            }
        }
        return true;
    }
};


static const uint32_t array_npc_gfxoffsetx_patches[] = { 0x912c05, 0x912e77, 0x91313c, 0x91342e, 0x913719, 0x91398b, 0x913c24, 0x913f14, 0x91421e, 0x914494, 0x914797, 0x914b90, 0x915a64, 0x91cb2f, 0x91e6da, 0x91e930, 0x91ec51, 0x91f067, 0x9206a3, 0x9208fc, 0x920e78, 0x92103a, 0x921308, 0x92171e, 0x9282c9, 0x928522, 0x928840, 0x928c53, 0x92a2ae, 0x92a504, 0x92a825, 0x92ac3b, 0x92ee8f, 0x92f0cf, 0x92f3ed, 0x92f7cd, 0x93a1e7, 0x93a4b1, 0x93ae26, 0x93b487, 0x93bb5e, 0x93bd4a, 0x93c696, 0x93c96e, 0x93d277, 0x93d8f4, 0x9469da, 0x946c17, 0x947194, 0x947354, 0x947624, 0x947a04, 0x98fef8, 0x99010a, 0x9903c3, 0x990753, 0x99252c, 0x99268d, 0x9927e3, 0x992939, 0x992a84, 0x992bda, 0x992d25, 0x992e6d, 0xb202c3, 0xb2088d, 0xb2133d, 0x000000 };
static ResizableGameArray array_npc_gfxoffsetx_obj((void**)0xb25b70, 301, 2, array_npc_gfxoffsetx_patches);
static const uint32_t array_npc_gfxoffsety_patches[] = { 0x912ba0, 0x912e12, 0x9130d7, 0x9133c9, 0x9136b4, 0x913926, 0x913bbf, 0x913eaf, 0x9141b9, 0x91442f, 0x9146fb, 0x914af4, 0x915ae0, 0x91cbab, 0x91e675, 0x91e8cb, 0x91ebb5, 0x91efcb, 0x92063e, 0x920897, 0x92126c, 0x921682, 0x928264, 0x9284bd, 0x9287a4, 0x928bb7, 0x92a249, 0x92a49f, 0x92a789, 0x92ab9f, 0x92ee2a, 0x92f06a, 0x92f351, 0x92f731, 0x93ad04, 0x93b367, 0x93d151, 0x93d7cd, 0x946975, 0x946bb2, 0x947588, 0x947968, 0x98fea0, 0x9900b2, 0x99033d, 0x9906cd, 0x9924fc, 0x99265d, 0x9927b3, 0x992909, 0x992a54, 0x992baa, 0x992cf5, 0x992e3d, 0xb202dc, 0xb208b3, 0xb21416, 0x000000 };
static ResizableGameArray array_npc_gfxoffsety_obj((void**)0xb25b8c, 301, 2, array_npc_gfxoffsety_patches);
static const uint32_t array_GM_CONF_WIDTH_patches[] = { 0x8c32f8, 0x8c397d, 0x8c58e7, 0x8d4b85, 0x8da186, 0x8f15ec, 0x8f1a23, 0x8f1e01, 0x8f21df, 0x8f2686, 0x8f2a64, 0x8f2f59, 0x8f344e, 0x8f3943, 0x8f3e38, 0x8f428a, 0x8f4712, 0x8f4b9a, 0x8f5022, 0x8f54aa, 0x8f5932, 0x8f5dba, 0x8f6242, 0x913b20, 0x913e10, 0x915a05, 0x91cacf, 0x920b0a, 0x939ca0, 0x939fe5, 0x93b7d7, 0x93ba3c, 0x93c216, 0x946e27, 0x96c70d, 0x96c8bc, 0x96e847, 0x96e9f6, 0x97540d, 0x9755cd, 0x97e09f, 0x97e24e, 0x99fcb5, 0x9aa4b9, 0x9aa70d, 0x9bdefb, 0x9be44f, 0x9c3eca, 0x9c6df8, 0x9c7df3, 0x9cc93e, 0x9cca4e, 0x9dbe9e, 0x9dd0cc, 0x9de3f9, 0x9deb5b, 0x9e06e8, 0x9e3c1c, 0x9e4281, 0x9e4a9d, 0x9e64cc, 0x9e67d5, 0x9fec47, 0x9fec68, 0xa02f17, 0xa0ad3b, 0xa0b4f9, 0xa0b61e, 0xa0b7c0, 0xa1f44c, 0xa24489, 0xa29999, 0xa29d18, 0xa2ec8a, 0xa2f14b, 0xa30274, 0xa3146e, 0xa32aaa, 0xa45c03, 0xa45ff7, 0xa49b2d, 0xa520c6, 0xa5295f, 0xa55bb3, 0xa56a3d, 0xa56a8b, 0xa56b54, 0xa56cbd, 0xa56db9, 0xa5bd5d, 0xa5d570, 0xa5f39f, 0xa61178, 0xa61265, 0xa614c5, 0xa8ba1e, 0xa8cb69, 0xa905fd, 0xa906e8, 0xae597d, 0xae5a21, 0xae5ac7, 0xae5b41, 0xae5cd4, 0xb0bb51, 0xb20302, 0xb208d8, 0xb214ef, 0x000000 };
static ResizableGameArray array_GM_CONF_WIDTH_obj((void**)0xb25ba8, 301, 2, array_GM_CONF_WIDTH_patches);
static const uint32_t array_GM_CONF_HEIGHT_patches[] = { 0x8c3317, 0x8c5929, 0x8d4b1f, 0x8da1b2, 0x8f166c, 0x8f1aa8, 0x8f1e86, 0x8f2264, 0x8f270b, 0x8f2ae9, 0x8f2fdf, 0x8f34d4, 0x8f39c9, 0x8f3ebe, 0x8f4310, 0x8f4798, 0x8f4c20, 0x8f50a8, 0x8f5530, 0x8f59b8, 0x8f5e40, 0x8f62c8, 0x912b01, 0x912d73, 0x913001, 0x913038, 0x9132f3, 0x91332a, 0x913615, 0x913887, 0x913ae9, 0x913dd9, 0x91411a, 0x914390, 0x9154de, 0x9159a5, 0x915fc6, 0x916187, 0x91c599, 0x91ca70, 0x91d094, 0x91d253, 0x91e3c3, 0x91f909, 0x920b6f, 0x939e20, 0x939f43, 0x93b90c, 0x93b9e5, 0x93c171, 0x946e8c, 0x96c6f3, 0x96c8a2, 0x96e82d, 0x96e9dc, 0x9753ec, 0x9755b2, 0x97e085, 0x97e234, 0x99213a, 0x992253, 0x992395, 0x99fc24, 0x9aa48c, 0x9aa6dc, 0x9bded6, 0x9be112, 0x9be4da, 0x9c369e, 0x9c3b96, 0x9c3eef, 0x9c6d67, 0x9c7d6d, 0x9cc91d, 0x9cca6e, 0x9dbebf, 0x9dd72b, 0x9dd849, 0x9debec, 0x9e070d, 0x9e3c58, 0x9e42be, 0x9e4a71, 0x9e64a5, 0x9e67aa, 0x9fecb5, 0x9fecd6, 0xa01fe4, 0xa02f42, 0xa0ad68, 0xa0b4c8, 0xa0b64e, 0xa1f41b, 0xa2178b, 0xa21f0d, 0xa21f70, 0xa21fc8, 0xa29978, 0xa29d3d, 0xa2ec65, 0xa2f127, 0xa30299, 0xa313d5, 0xa32c12, 0xa45c33, 0xa483a1, 0xa486e9, 0xa520fd, 0xa558ab, 0xa5597e, 0xa55b83, 0xa560d9, 0xa562d3, 0xa56789, 0xa5679e, 0xa56839, 0xa5688c, 0xa568f3, 0xa5d53f, 0xa5f36e, 0xa5f6cb, 0xa5f803, 0xa5f99e, 0xa5fa73, 0xa5fbc4, 0xa5fdec, 0xa610b7, 0xa612cc, 0xa613fe, 0xa8ba68, 0xa8cbb3, 0xa905d9, 0xa90735, 0xae59a7, 0xae59f7, 0xae5af1, 0xae5b6b, 0xae5caa, 0xb20328, 0xb208fe, 0xb215c8, 0x000000 };
static ResizableGameArray array_GM_CONF_HEIGHT_obj((void**)0xb25bc4, 301, 2, array_GM_CONF_HEIGHT_patches);
static const uint32_t array_GM_CONF_GFXWIDTH_patches[] = { 0x8c399c, 0x9140b4, 0x914696, 0x9147ce, 0x914a8f, 0x914bc7, 0x915536, 0x91558f, 0x915710, 0x9157e2, 0x9158cf, 0x915bd4, 0x91c5f2, 0x91c64a, 0x91c7de, 0x91c8ad, 0x91c99a, 0x91cc9e, 0x91e41c, 0x91e474, 0x91e5b7, 0x91eb50, 0x91ec88, 0x91ef66, 0x91f09e, 0x91f962, 0x91f9ba, 0x920580, 0x920aaf, 0x920bd8, 0x921207, 0x92133f, 0x92161d, 0x921755, 0x9281a6, 0x92873f, 0x928877, 0x928b52, 0x928c8a, 0x92a18b, 0x92a724, 0x92a85c, 0x92ab3a, 0x92ac72, 0x92ed6d, 0x92f2ec, 0x92f424, 0x92f6cc, 0x92f804, 0x939bab, 0x93a67e, 0x93a9c5, 0x93ac5d, 0x93ae9f, 0x93b2c0, 0x93b501, 0x93c0d6, 0x93cb88, 0x93d0a7, 0x93d2f3, 0x93d722, 0x93d970, 0x9468b7, 0x946dcd, 0x946ef6, 0x947523, 0x94765b, 0x947903, 0x947a3b, 0x98fdfb, 0x9902e5, 0x9903f1, 0x990675, 0x990781, 0x992161, 0x992187, 0xa8b49c, 0xa8b9da, 0xa8bab8, 0xa8cb25, 0xa8cc03, 0xa90627, 0xa90782, 0xa907e1, 0xae593d, 0xae5dfe, 0xae5ea3, 0xae5f49, 0xae5fc4, 0xae6157, 0xb2034d, 0xb20924, 0xb216a1, 0x000000 };
static ResizableGameArray array_GM_CONF_GFXWIDTH_obj((void**)0xb25be0, 301, 2, array_GM_CONF_GFXWIDTH_patches);
static const uint32_t array_GM_CONF_GFXHEIGHT_patches[] = { 0x8c39bd, 0x9145ee, 0x91465f, 0x914732, 0x9149e7, 0x914a58, 0x914b2b, 0x9156d9, 0x915782, 0x915841, 0x915b9d, 0x915c44, 0x915e02, 0x91c7a7, 0x91c84e, 0x91c90d, 0x91cc67, 0x91cd10, 0x91ced1, 0x91eaa8, 0x91eb19, 0x91ebec, 0x91eebe, 0x91ef2f, 0x91f002, 0x920c3d, 0x92115f, 0x9211d0, 0x9212a3, 0x921575, 0x9215e6, 0x9216b9, 0x922ded, 0x928697, 0x928708, 0x9287db, 0x928aaa, 0x928b1b, 0x928bee, 0x92a67c, 0x92a6ed, 0x92a7c0, 0x92aa92, 0x92ab03, 0x92abd6, 0x92f244, 0x92f2b5, 0x92f388, 0x92f624, 0x92f695, 0x92f768, 0x93a7ff, 0x93a923, 0x93aaee, 0x93abe4, 0x93ad7e, 0x93b151, 0x93b247, 0x93b3e0, 0x93cae3, 0x93cf2d, 0x93d02a, 0x93d1cd, 0x93d5ab, 0x93d6a6, 0x93d84a, 0x946f5a, 0x94747b, 0x9474ec, 0x9475bf, 0x94785b, 0x9478cc, 0x94799f, 0x990258, 0x9902b7, 0x99036b, 0x9905e8, 0x990647, 0x9906fb, 0x9bc363, 0x9bc410, 0xa06e9e, 0xa06ee1, 0xa06f79, 0xa070cc, 0xa0716e, 0xa21e77, 0xa21e99, 0xa8bb03, 0xa8cc4e, 0xa90651, 0xa9082e, 0xa9088e, 0xae5e28, 0xae5e79, 0xae5f73, 0xae5fee, 0xae612d, 0xb20373, 0xb20949, 0xb2177a, 0x000000 };
static ResizableGameArray array_GM_CONF_GFXHEIGHT_obj((void**)0xb25bfc, 301, 2, array_GM_CONF_GFXHEIGHT_patches);
static const uint32_t array_npc_speed_patches[] = { 0x8c2783, 0xa0a377, 0xa155ec, 0xb20792, 0xb20d68, 0xb22358, 0x000000 };
static ResizableGameArray array_npc_speed_obj((void**)0xb25c18, 301, 4, array_npc_speed_patches);
static const uint32_t array_npc_isShell_patches[] = { 0x9a7eeb, 0x9a885f, 0x9a8d96, 0x9a900e, 0x9a9084, 0x9abbc8, 0x9ac64e, 0x9acd92, 0x9ace87, 0x9ad1df, 0x9ad460, 0x9ad65d, 0x9ad90b, 0x9b2744, 0x9bee50, 0x9beee6, 0x9bfc43, 0x9c3770, 0x9cc573, 0x9cdf0c, 0x9cf0ec, 0x9cf8ec, 0xa0a15d, 0xa0caf4, 0xa0cd29, 0xa0cec1, 0xa1361e, 0xa1577b, 0xa15cbe, 0xa15d4a, 0xa1620b, 0xa1643d, 0xa16a6d, 0xa16c54, 0xa1706d, 0xa17199, 0xa17e53, 0xa19e39, 0xa1ab31, 0xa1ac7b, 0xa1addd, 0xa1b420, 0xa1b466, 0xa1d039, 0xa29dce, 0xa2f305, 0xa30615, 0xa40142, 0xa5c157, 0xb20399, 0xb2096f, 0x000000 };
static ResizableGameArray array_npc_isShell_obj((void**)0xb25c34, 301, 2, array_npc_isShell_patches);
static const uint32_t array_npc_npcblock_patches[] = { 0x8f116b, 0x99844a, 0xa090ea, 0xa0945b, 0xa0950f, 0xa0a2b4, 0xa11437, 0xa1680c, 0xb203be, 0xb20995, 0xb21ade, 0x000000 };
static ResizableGameArray array_npc_npcblock_obj((void**)0xb25c50, 301, 2, array_npc_npcblock_patches);
static const uint32_t array_npc_npcblocktop_patches[] = { 0xa09107, 0xa0942b, 0xa094f4, 0xa11469, 0xb203e4, 0xb209ba, 0xb21bb7, 0x000000 };
static ResizableGameArray array_npc_npcblocktop_obj((void**)0xb25c6c, 301, 2, array_npc_npcblocktop_patches);
static const uint32_t array_isInteractableNPC_ptr_patches[] = { 0x8d1f99, 0x8d245e, 0x8d294b, 0x8d2d0d, 0x8eac8f, 0x8f11ab, 0x9a7c2f, 0x9ac5ec, 0x9ad35f, 0x9ae031, 0x9c3d98, 0x9c4064, 0x9c4312, 0x9dca1a, 0x9f0fb7, 0xa0ba81, 0xa11f2f, 0xa16636, 0xa1951b, 0xa1966c, 0xa19d9a, /*0xa1aa5c (overwritten by ignoreThrownNPCs hook),*/ 0xa2d035, 0xa3057d, 0xa3085c, 0xa30d77, 0xa360e6, 0xa3bc89, /*0xa3c67e, overwritten by fixup_NativeFuncs() */ 0xa425a0, 0xaa2c6b, 0xafdee0, 0xb2040a, 0xb209e0, 0x000000 };
static ResizableGameArray array_isInteractableNPC_ptr_obj((void**)0xb25c88, 301, 2, array_isInteractableNPC_ptr_patches);
static const uint32_t array_isCoin_ptr_patches[] = { 0x8d654c, 0x8f11ec, 0x91e2ce, 0x9203ee, 0x92a0e4, 0x9bc1fb, 0x9bd83c, 0x9bd969, 0x9cdda6, 0x9cde70, 0x9cf1be, 0x9e2aa7, 0x9e2bb5, 0x9e3465, 0x9e47e1, 0x9f0ff2, 0xa07ec9, 0xa084b8, 0xa0a3a5, 0xa0a5d4, 0xa0f710, 0xa10d48, 0xa141bb, 0xa17f98, 0xa1813a, 0xa24e41, 0xa26096, 0xa29e4f, 0xa2ccf5, 0xa30ad0, 0xa30df1, 0xa3613c, 0xa410b7, 0xa45cfe, 0xa53f81, 0xa54561, 0xa5d80b, 0xa8b319, 0xa996b4, 0xa99728, 0xaa7198, 0xae9c09, 0xae9f67, 0xafdf60, 0xb2042f, 0xb20a06, 0x000000 };
static ResizableGameArray array_isCoin_ptr_obj((void**)0xb25ca4, 301, 2, array_isCoin_ptr_patches);
static const uint32_t array_isVineNPC_ptr_patches[] = { 0x8c38e6, 0x912807, 0x92011d, 0x99ac7f, 0x9aaf6c, 0x9ad0e4, 0x9d12e5, 0x9ef889, 0xa0a0c4, 0xa18435, 0xa22f7e, /*0xa3c738, overwritten by fixup_NativeFuncs() */ 0xa3d1f4, 0xa45221, 0xa523c1, 0xaa595e, 0xaa73a4, 0xaa75b5, 0xb20455, 0xb20a2b, 0x000000 };
static ResizableGameArray array_isVineNPC_ptr_obj((void**)0xb25cc0, 301, 2, array_isVineNPC_ptr_patches);
static const uint32_t array_isCollectableGoalNPC_ptr_patches[] = { 0x8d44cb, 0x8f1227, 0x9bc1e6, 0x9bda50, 0x9bdf2b, 0x9cc8b4, 0xa08d71, 0xa0b458, 0xa10961, 0xa10df3, 0xa26556, 0xa2990e, 0xa2cbdb, 0xa32763, 0xa3ba49, /*0xa3c617, overwritten by fixup_NativeFuncs() */ 0xa8b341, 0xae9c90, 0xae9fac, 0xb2047b, 0xb20a51, 0x000000 };
static ResizableGameArray array_isCollectableGoalNPC_ptr_obj((void**)0xb25cdc, 301, 2, array_isCollectableGoalNPC_ptr_patches);
static const uint32_t array_npc_isflying_patches[] = { 0x8d7a9a, 0x8d9f7f, 0x9a8a31, 0x9f506a, 0x9fe4ea, 0xa0f672, 0xa109aa, /*0xa12059 overwritten by runtimeHookSemisolidInteractionHook_Raw's hook, */ 0xa12b59, 0xa161b2, 0xa163e2, 0xa16510, 0xa170e0, 0xa1b094, 0xa1b0d7, 0xa1cfad, 0xa2beac, 0xa57b53, 0xa919b0, 0xa92b0e, 0xb204a0, 0xb20a77, 0x000000 };
static ResizableGameArray array_npc_isflying_obj((void**)0xb25cf8, 301, 2, array_npc_isflying_patches);
static const uint32_t array_npc_isWaterNPC_patches[] = { 0x8ca12c, 0x8d7af5, 0x8d9fc8, 0x9a9bbe, 0x9a9cb2, 0x9ac870, 0x9f51f5, 0x9fe586, 0xa09137, 0xa09d03, 0xa09e42, 0xa09ffa, 0xa0a709, 0xa0aa14, 0xa0aa54, 0xa0ab29, 0xa0affc, 0xa0b88e, 0xa0d08f, 0xa0f1ab, 0xa0f2d3, 0xa0f417, 0xa0f509, 0xa0f5bd, 0xa0f85c, 0xa0f8e7, 0xa0fcce, 0xa10f4c, 0xa11586, 0xa11697, 0xa133cb, 0xa17ff4, 0xa1baff, 0xa28fd0, 0xa54d11, 0xa92e08, 0xb204c6, 0xb20a9c, 0x000000 };
static ResizableGameArray array_npc_isWaterNPC_obj((void**)0xb25d14, 301, 2, array_npc_isWaterNPC_patches);
static const uint32_t array_npc_jumphurt_patches[] = { 0x8c393f, 0x9ac832, 0x9ae675, 0xa29135, 0xa2d8a9, 0xb204ec, 0xb20ac2, 0xb21e42, 0x000000 };
static ResizableGameArray array_npc_jumphurt_obj((void**)0xb25d30, 301, 2, array_npc_jumphurt_patches);
static const uint32_t array_npc_noblockcollision_patches[] = { 0x8c3920, 0x9e2a6b, 0xa089ac, 0xa0ab81, 0xa10e98, 0xa113a2, 0xa175fc, 0xa1b332, 0xa1b3c5, 0xb20511, 0xb20ae8, 0xb21ff4, 0x000000 };
static ResizableGameArray array_npc_noblockcollision_obj((void**)0xb25d4c, 301, 2, array_npc_noblockcollision_patches);
static const uint32_t array_npc_score_patches[] = { 0x8c32dd, 0x9ae845, 0x9c4394, 0xa263e5, 0xa304e5, 0xa30538, 0xa30780, 0xa309c9, 0xa30c24, 0xa30cf4, 0xa30dc2, 0xa30f4e, 0xa31bee, 0xb20537, 0xb20b0d, 0xb21853, 0x000000 };
static ResizableGameArray array_npc_score_obj((void**)0xb25d68, 301, 2, array_npc_score_patches);
static const uint32_t array_npc_playerblocktop_patches[] = { 0x8c594e, 0x9a7f60, 0x9a802c, 0x9a80b5, 0x9a937e, 0x9a9715, 0x9abb8a, 0xa0911c, 0xa09440, 0xa094d9, 0xa0a29f, 0xa0d1c9, 0xa0f25c, 0xa0f27e, 0xa1149d, 0xa2289d, 0xa2295a, 0xa2d894, 0xb2055d, 0xb20b33, 0xb21a05, 0x000000 };
static ResizableGameArray array_npc_playerblocktop_obj((void**)0xb25d84, 301, 2, array_npc_playerblocktop_patches);
static const uint32_t array_npc_grabtop_patches[] = { 0x8c596d, 0x8d2234, 0x9cc3cf, 0xb20582, 0xb20b59, 0xb21d69, 0x000000 };
static ResizableGameArray array_npc_grabtop_obj((void**)0xb25da0, 301, 2, array_npc_grabtop_patches);
static const uint32_t array_npc_cliffturn_patches[] = { 0xa1bfb3, 0xb205a8, 0xb20b7e, 0xb220cd, 0x000000 };
static ResizableGameArray array_npc_cliffturn_obj((void**)0xb25dbc, 301, 2, array_npc_cliffturn_patches);
static const uint32_t array_npc_nohurt_patches[] = { 0x8c3901, 0x8c598c, 0x8d2090, 0x8d2555, 0x8d2a42, 0x8d2e04, 0x8f112b, 0x9a8c78, 0x9ac458, 0x9ac968, 0x9ae46e, 0xa195d6, 0xa30d62, 0xaa2c31, 0xb205ce, 0xb20ba4, 0xb21f1b, 0x000000 };
static ResizableGameArray array_npc_nohurt_obj((void**)0xb25dd8, 301, 2, array_npc_nohurt_patches);
static const uint32_t array_npc_playerblock_patches[] = { 0x9a7fcb, 0x9ae8b5, 0xa15b7a, 0xb205f3, 0xb20bca, 0xb2192c, 0x000000 };
static ResizableGameArray array_npc_playerblock_obj((void**)0xb25df4, 301, 2, array_npc_playerblock_patches);
static const uint32_t array_b25e10_patches[] = { 0x998490, 0xa11636, 0xa15747, 0xb20619, 0xb20bef, 0x000000 };
static ResizableGameArray array_b25e10_obj((void**)0xb25e10, 301, 2, array_b25e10_patches);
static const uint32_t array_npc_grabside_patches[] = { 0x8c59ab, 0x9ad2fb, 0x9c372c, 0xb2063f, 0xb20c15, 0xb21c90, 0x000000 };
static ResizableGameArray array_npc_grabside_obj((void**)0xb25e2c, 301, 2, array_npc_grabside_patches);
static const uint32_t array_isShoeNPC_ptr_patches[] = { 0x8d47ea, 0x922869, 0x9abe4c, 0x9abf31, 0x9accbe, 0x9dd992, 0xa064db, /*0xa3c669, overwritten by fixup_NativeFuncs() */ 0xa60e7a, 0xa616da, 0xae9d9e, 0xaea0ba, 0xb20664, 0xb20c3b, 0x000000 };
static ResizableGameArray array_isShoeNPC_ptr_obj((void**)0xb25e48, 301, 2, array_isShoeNPC_ptr_patches);
static const uint32_t array_isYoshiNPC_ptr_patches[] = { 0x8d4502, 0x92052a, 0x922832, 0x92813c, 0x98fda0, 0x9abe0e, 0x9ac134, 0x9acd54, 0x9bdb21, 0x9bde86, 0x9cc870, 0x9dcac4, 0x9dd2e4, 0x9dd535, 0x9dd97d, 0x9e6363, 0xa03e62, 0xa064f0, 0xa0b07f, 0xa0b372, 0xa0b415, 0xa0d7c0, 0xa0efed, 0xa17ddf, 0xa18c82, 0xa298d6, 0xa2ca3d, /*0xa3c658, overwritten by fixup_NativeFuncs() */ 0xa41704, 0xa60d16, 0xa616a6, 0xa8b4c4, 0xa8b979, 0xae9d6d, 0xaea089, 0xb2068a, 0xb20c60, 0x000000 };
static ResizableGameArray array_isYoshiNPC_ptr_obj((void**)0xb25e64, 301, 2, array_isYoshiNPC_ptr_patches);
static const uint32_t array_b25e80_patches[] = { 0x8d42ab, 0xa17ec8, 0xa18274, 0xa19589, 0xa1b064, 0xa259de, 0xa2b929, 0xa34377, 0xb206b0, 0xb20c86, 0x000000 };
static ResizableGameArray array_b25e80_obj((void**)0xb25e80, 301, 2, array_b25e80_patches);
static const uint32_t array_npc_noyoshi_patches[] = { 0x8c395e, 0x9bd8c3, 0x9bde0e, 0xb206d5, 0xb20cac, 0xb221a6, 0x000000 };
static ResizableGameArray array_npc_noyoshi_obj((void**)0xb25e9c, 301, 2, array_npc_noyoshi_patches);
static const uint32_t array_npc_foreground_patches[] = { 0x8ca14b, 0x920344, 0x92a05e, 0xb206fb, 0xb20cd1, 0xb2227f, 0x000000 };
static ResizableGameArray array_npc_foreground_obj((void**)0xb25eb8, 301, 2, array_npc_foreground_patches);
static const uint32_t array_npc_isBot_patches[] = { 0x9bef2c, 0x9c3824, 0xa0cf11, 0xa20644, 0xa2b121, 0xa31ca1, 0xa33d32, 0xb20721, 0xb20cf7, 0x000000 };
static ResizableGameArray array_npc_isBot_obj((void**)0xb25ed4, 301, 2, array_npc_isBot_patches);
static const uint32_t array_b25ef0_patches[] = { 0xa0d074, 0xa0f194, 0xb20746, 0xb20d1d, 0x000000 };
static ResizableGameArray array_b25ef0_obj((void**)0xb25ef0, 301, 2, array_b25ef0_patches);
static const uint32_t array_isVegetableNPC_ptr_patches[] = { 0x8c59ca, 0x9a889c, 0x9a8ac9, 0x9ad51b, 0x9ad6b8, 0x9b107b, 0x9ccb9d, 0x9cdbfe, 0x9ce586, 0x9cf7af, 0xa077f6, 0xa0cf62, 0xa0d692, 0xa10ef1, 0xa16742, 0xa18543, 0xa18559, 0xa296f9, 0xa29af1, 0xa2a8fc, 0xa2b16f, 0xa2d0e9, 0xa336d7, 0xa3beb8, /*0xa3c6f4, overwritten by fixup_NativeFuncs() */ 0xa3d0fb, 0xb2076c, 0xb20d42, 0x000000 };
static ResizableGameArray array_isVegetableNPC_ptr_obj((void**)0xb25f0c, 301, 2, array_isVegetableNPC_ptr_patches);
static const uint32_t array_npc_nofireball_patches[] = { 0xa29113, 0xb207b7, 0xb20d8c, 0xb22430, 0x000000 };
static ResizableGameArray array_npc_nofireball_obj((void**)0xb25f28, 301, 2, array_npc_nofireball_patches);
static const uint32_t array_npc_noiceball_patches[] = { 0x8c7c5f, 0x8c7caa, 0x8c7d0f, 0x8c7d67, 0x8c7e70, 0x8c7fb5, 0x8eac09, 0xa284dc, 0xb207db, 0xb20db1, 0xb22509, 0x000000 };
static ResizableGameArray array_npc_noiceball_obj((void**)0xb25f44, 301, 2, array_npc_noiceball_patches);
static const uint32_t array_npc_nogravity_patches[] = { 0xa0f4ba, 0xb20801, 0xb20dd7, 0xb225e2, 0x000000 };
static ResizableGameArray array_npc_nogravity_obj((void**)0xb25f60, 301, 2, array_npc_nogravity_patches);
static const uint32_t array_npc_frames_patches[] = { 0xa3ca84, 0xa3cbf7, 0xa3cc33, 0xa3cc9a, 0xa3ccd5, 0xa3ccff, 0xa3cd30, 0xa3cd56, 0xa3cd82, 0xa3cdff, 0xa3ce3b, 0xa3ce73, 0xa3cea5, 0xa3ceca, 0xa3cef5, 0xa3cf4b, 0xa3cf7c, 0xa3cfac, 0xa3cfe2, 0xa3d01f, 0xa3d051, 0xa3d081, 0xa3d0b6, 0xb20def, 0xb226bb, 0x000000 };
static ResizableGameArray array_npc_frames_obj((void**)0xb25f7c, 301, 2, array_npc_frames_patches);
static const uint32_t array_npc_framespeed_patches[] = { 0xa3cb16, 0xb20826, 0xb20e03, 0xb22794, 0x000000 };
static ResizableGameArray array_npc_framespeed_obj((void**)0xb25f98, 301, 2, array_npc_framespeed_patches);
static const uint32_t array_npc_framestyle_patches[] = { 0xa3cac5, 0xa3cb59, 0xa3cbd6, 0xa3cc67, 0xa3cdb2, 0xb20e18, 0xb22871, 0x000000 };
static ResizableGameArray array_npc_framestyle_obj((void**)0xb25fb4, 301, 2, array_npc_framestyle_patches);
static const uint32_t array_npcdef_gfxoffsetx_patches[] = { 0xb202cd, 0xb20883, 0x000000 };
static const uint32_t array_npcdef_gfxoffsetx_ptrs[] = { 0xb202e8, 0xb2089f, 0x000000 };
static ResizableGameArray array_npcdef_gfxoffsetx_obj(nullptr, 301, 2, array_npcdef_gfxoffsetx_patches, array_npcdef_gfxoffsetx_ptrs);
static const uint32_t array_npcdef_gfxoffsety_patches[] = { 0xb202f2, 0xb208a5, 0x000000 };
static const uint32_t array_npcdef_gfxoffsety_ptrs[] = { 0xb2030e, 0xb208c4, 0x000000 };
static ResizableGameArray array_npcdef_gfxoffsety_obj(nullptr, 301, 2, array_npcdef_gfxoffsety_patches, array_npcdef_gfxoffsety_ptrs);
static const uint32_t array_npcdef_width_patches[] = { 0xb20318, 0xb208ca, 0x000000 };
static const uint32_t array_npcdef_width_ptrs[] = { 0xb20334, 0xb208ea, 0x000000 };
static ResizableGameArray array_npcdef_width_obj(nullptr, 301, 2, array_npcdef_width_patches, array_npcdef_width_ptrs);
static const uint32_t array_npcdef_height_patches[] = { 0xb2033e, 0xb208f0, 0x000000 };
static const uint32_t array_npcdef_height_ptrs[] = { 0xb20359, 0xb20910, 0x000000 };
static ResizableGameArray array_npcdef_height_obj(nullptr, 301, 2, array_npcdef_height_patches, array_npcdef_height_ptrs);
static const uint32_t array_npcdef_gfxwidth_patches[] = { 0xb20363, 0xb20916, 0x000000 };
static const uint32_t array_npcdef_gfxwidth_ptrs[] = { 0xb2037f, 0xb20935, 0x000000 };
static ResizableGameArray array_npcdef_gfxwidth_obj(nullptr, 301, 2, array_npcdef_gfxwidth_patches, array_npcdef_gfxwidth_ptrs);
static const uint32_t array_npcdef_gfxheight_patches[] = { 0xb20389, 0xb2093b, 0x000000 };
static const uint32_t array_npcdef_gfxheight_ptrs[] = { 0xb203a5, 0xb2095b, 0x000000 };
static ResizableGameArray array_npcdef_gfxheight_obj(nullptr, 301, 2, array_npcdef_gfxheight_patches, array_npcdef_gfxheight_ptrs);
static const uint32_t array_npcdef_isShell_patches[] = { 0xb203af, 0xb20961, 0x000000 };
static const uint32_t array_npcdef_isShell_ptrs[] = { 0xb203ca, 0xb20981, 0x000000 };
static ResizableGameArray array_npcdef_isShell_obj(nullptr, 301, 2, array_npcdef_isShell_patches, array_npcdef_isShell_ptrs);
static const uint32_t array_npcdef_npcblock_patches[] = { 0xb203d4, 0xb20987, 0x000000 };
static const uint32_t array_npcdef_npcblock_ptrs[] = { 0xb203f0, 0xb209a6, 0x000000 };
static ResizableGameArray array_npcdef_npcblock_obj(nullptr, 301, 2, array_npcdef_npcblock_patches, array_npcdef_npcblock_ptrs);
static const uint32_t array_npcdef_npcblocktop_patches[] = { 0xb203fa, 0xb209ac, 0x000000 };
static const uint32_t array_npcdef_npcblocktop_ptrs[] = { 0xb20416, 0xb209cc, 0x000000 };
static ResizableGameArray array_npcdef_npcblocktop_obj(nullptr, 301, 2, array_npcdef_npcblocktop_patches, array_npcdef_npcblocktop_ptrs);
static const uint32_t array_npcdef_isInteractableNPC_patches[] = { 0xb20420, 0xb209d2, 0x000000 };
static const uint32_t array_npcdef_isInteractableNPC_ptrs[] = { 0xb2043b, 0xb209f2, 0x000000 };
static ResizableGameArray array_npcdef_isInteractableNPC_obj(nullptr, 301, 2, array_npcdef_isInteractableNPC_patches, array_npcdef_isInteractableNPC_ptrs);
static const uint32_t array_npcdef_isCoin_patches[] = { 0xb20445, 0xb209f8, 0x000000 };
static const uint32_t array_npcdef_isCoin_ptrs[] = { 0xb20461, 0xb20a17, 0x000000 };
static ResizableGameArray array_npcdef_isCoin_obj(nullptr, 301, 2, array_npcdef_isCoin_patches, array_npcdef_isCoin_ptrs);
static const uint32_t array_npcdef_isVine_patches[] = { 0xb2046b, 0xb20a1d, 0x000000 };
static const uint32_t array_npcdef_isVine_ptrs[] = { 0xb20487, 0xb20a3d, 0x000000 };
static ResizableGameArray array_npcdef_isVine_obj(nullptr, 301, 2, array_npcdef_isVine_patches, array_npcdef_isVine_ptrs);
static const uint32_t array_npcdef_isCollectableGoal_patches[] = { 0xb20491, 0xb20a43, 0x000000 };
static const uint32_t array_npcdef_isCollectableGoal_ptrs[] = { 0xb204ac, 0xb20a63, 0x000000 };
static ResizableGameArray array_npcdef_isCollectableGoal_obj(nullptr, 301, 2, array_npcdef_isCollectableGoal_patches, array_npcdef_isCollectableGoal_ptrs);
static const uint32_t array_npcdef_isFlyingNPC_patches[] = { 0xb204b6, 0xb20a69, 0x000000 };
static const uint32_t array_npcdef_isFlyingNPC_ptrs[] = { 0xb204d2, 0xb20a88, 0x000000 };
static ResizableGameArray array_npcdef_isFlyingNPC_obj(nullptr, 301, 2, array_npcdef_isFlyingNPC_patches, array_npcdef_isFlyingNPC_ptrs);
static const uint32_t array_npcdef_isWaterNPC_patches[] = { 0xb204dc, 0xb20a8e, 0x000000 };
static const uint32_t array_npcdef_isWaterNPC_ptrs[] = { 0xb204f8, 0xb20aae, 0x000000 };
static ResizableGameArray array_npcdef_isWaterNPC_obj(nullptr, 301, 2, array_npcdef_isWaterNPC_patches, array_npcdef_isWaterNPC_ptrs);
static const uint32_t array_npcdef_jumphurt_patches[] = { 0xb20502, 0xb20ab4, 0x000000 };
static const uint32_t array_npcdef_jumphurt_ptrs[] = { 0xb2051d, 0xb20ad4, 0x000000 };
static ResizableGameArray array_npcdef_jumphurt_obj(nullptr, 301, 2, array_npcdef_jumphurt_patches, array_npcdef_jumphurt_ptrs);
static const uint32_t array_npcdef_noblockcollision_patches[] = { 0xb20527, 0xb20ada, 0x000000 };
static const uint32_t array_npcdef_noblockcollision_ptrs[] = { 0xb20543, 0xb20af9, 0x000000 };
static ResizableGameArray array_npcdef_noblockcollision_obj(nullptr, 301, 2, array_npcdef_noblockcollision_patches, array_npcdef_noblockcollision_ptrs);
static const uint32_t array_npcdef_score_patches[] = { 0xb2054d, 0xb20aff, 0x000000 };
static const uint32_t array_npcdef_score_ptrs[] = { 0xb20569, 0xb20b1f, 0x000000 };
static ResizableGameArray array_npcdef_score_obj(nullptr, 301, 2, array_npcdef_score_patches, array_npcdef_score_ptrs);
static const uint32_t array_npcdef_playerblocktop_patches[] = { 0xb20573, 0xb20b25, 0x000000 };
static const uint32_t array_npcdef_playerblocktop_ptrs[] = { 0xb2058e, 0xb20b45, 0x000000 };
static ResizableGameArray array_npcdef_playerblocktop_obj(nullptr, 301, 2, array_npcdef_playerblocktop_patches, array_npcdef_playerblocktop_ptrs);
static const uint32_t array_npcdef_grabtop_patches[] = { 0xb20598, 0xb20b4b, 0x000000 };
static const uint32_t array_npcdef_grabtop_ptrs[] = { 0xb205b4, 0xb20b6a, 0x000000 };
static ResizableGameArray array_npcdef_grabtop_obj(nullptr, 301, 2, array_npcdef_grabtop_patches, array_npcdef_grabtop_ptrs);
static const uint32_t array_npcdef_cliffturn_patches[] = { 0xb205be, 0xb20b70, 0x000000 };
static const uint32_t array_npcdef_cliffturn_ptrs[] = { 0xb205da, 0xb20b90, 0x000000 };
static ResizableGameArray array_npcdef_cliffturn_obj(nullptr, 301, 2, array_npcdef_cliffturn_patches, array_npcdef_cliffturn_ptrs);
static const uint32_t array_npcdef_nohurt_patches[] = { 0xb205e4, 0xb20b96, 0x000000 };
static const uint32_t array_npcdef_nohurt_ptrs[] = { 0xb205ff, 0xb20bb6, 0x000000 };
static ResizableGameArray array_npcdef_nohurt_obj(nullptr, 301, 2, array_npcdef_nohurt_patches, array_npcdef_nohurt_ptrs);
static const uint32_t array_npcdef_playerblock_patches[] = { 0xb20609, 0xb20bbc, 0x000000 };
static const uint32_t array_npcdef_playerblock_ptrs[] = { 0xb20625, 0xb20bdb, 0x000000 };
static ResizableGameArray array_npcdef_playerblock_obj(nullptr, 301, 2, array_npcdef_playerblock_patches, array_npcdef_playerblock_ptrs);
static const uint32_t array_b295d6_patches[] = { 0xb2062f, 0xb20be1, 0x000000 };
static const uint32_t array_b295d6_ptrs[] = { 0xb2064b, 0xb20c01, 0x000000 };
static ResizableGameArray array_b295d6_obj(nullptr, 301, 2, array_b295d6_patches, array_b295d6_ptrs);
static const uint32_t array_npcdef_grabside_patches[] = { 0xb20655, 0xb20c07, 0x000000 };
static const uint32_t array_npcdef_grabside_ptrs[] = { 0xb20670, 0xb20c27, 0x000000 };
static ResizableGameArray array_npcdef_grabside_obj(nullptr, 301, 2, array_npcdef_grabside_patches, array_npcdef_grabside_ptrs);
static const uint32_t array_npcdef_isShoeNPC_patches[] = { 0xb2067a, 0xb20c2d, 0x000000 };
static const uint32_t array_npcdef_isShoeNPC_ptrs[] = { 0xb20696, 0xb20c4c, 0x000000 };
static ResizableGameArray array_npcdef_isShoeNPC_obj(nullptr, 301, 2, array_npcdef_isShoeNPC_patches, array_npcdef_isShoeNPC_ptrs);
static const uint32_t array_npcdef_isYoshiNPC_patches[] = { 0xb206a0, 0xb20c52, 0x000000 };
static const uint32_t array_npcdef_isYoshiNPC_ptrs[] = { 0xb206bc, 0xb20c72, 0x000000 };
static ResizableGameArray array_npcdef_isYoshiNPC_obj(nullptr, 301, 2, array_npcdef_isYoshiNPC_patches, array_npcdef_isYoshiNPC_ptrs);
static const uint32_t array_b29f3e_patches[] = { 0xb206c6, 0xb20c78, 0x000000 };
static const uint32_t array_b29f3e_ptrs[] = { 0xb206e1, 0xb20c98, 0x000000 };
static ResizableGameArray array_b29f3e_obj(nullptr, 301, 2, array_b29f3e_patches, array_b29f3e_ptrs);
static const uint32_t array_npcdef_noYoshi_patches[] = { 0xb206eb, 0xb20c9e, 0x000000 };
static const uint32_t array_npcdef_noYoshi_ptrs[] = { 0xb20707, 0xb20cbd, 0x000000 };
static ResizableGameArray array_npcdef_noYoshi_obj(nullptr, 301, 2, array_npcdef_noYoshi_patches, array_npcdef_noYoshi_ptrs);
static const uint32_t array_npcdef_foreground_patches[] = { 0xb20711, 0xb20cc3, 0x000000 };
static const uint32_t array_npcdef_foreground_ptrs[] = { 0xb2072d, 0xb20ce3, 0x000000 };
static ResizableGameArray array_npcdef_foreground_obj(nullptr, 301, 2, array_npcdef_foreground_patches, array_npcdef_foreground_ptrs);
static const uint32_t array_npcdef_isBot_patches[] = { 0xb20737, 0xb20ce9, 0x000000 };
static const uint32_t array_npcdef_isBot_ptrs[] = { 0xb20752, 0xb20d09, 0x000000 };
static ResizableGameArray array_npcdef_isBot_obj(nullptr, 301, 2, array_npcdef_isBot_patches, array_npcdef_isBot_ptrs);
static const uint32_t array_b2a8a6_patches[] = { 0xb2075c, 0xb20d0f, 0x000000 };
static const uint32_t array_b2a8a6_ptrs[] = { 0xb20778, 0xb20d2e, 0x000000 };
static ResizableGameArray array_b2a8a6_obj(nullptr, 301, 2, array_b2a8a6_patches, array_b2a8a6_ptrs);
static const uint32_t array_npcdef_isVegetableNPC_patches[] = { 0xb20782, 0xb20d34, 0x000000 };
static const uint32_t array_npcdef_isVegetableNPC_ptrs[] = { 0xb2079e, 0xb20d54, 0x000000 };
static ResizableGameArray array_npcdef_isVegetableNPC_obj(nullptr, 301, 2, array_npcdef_isVegetableNPC_patches, array_npcdef_isVegetableNPC_ptrs);
static const uint32_t array_npcdef_speed_patches[] = { 0xb207a8, 0xb20d5a, 0x000000 };
static const uint32_t array_npcdef_speed_ptrs[] = { 0xb207c1, 0xb20d79, 0x000000 };
static ResizableGameArray array_npcdef_speed_obj(nullptr, 301, 4, array_npcdef_speed_patches, array_npcdef_speed_ptrs);
static const uint32_t array_npcdef_nofireball_patches[] = { 0xb207cb, 0xb20d7f, 0x000000 };
static const uint32_t array_npcdef_nofireball_ptrs[] = { 0xb207e7, 0xb20d9d, 0x000000 };
static ResizableGameArray array_npcdef_nofireball_obj(nullptr, 301, 2, array_npcdef_nofireball_patches, array_npcdef_nofireball_ptrs);
static const uint32_t array_npcdef_noiceball_patches[] = { 0xb207f1, 0xb20da3, 0x000000 };
static const uint32_t array_npcdef_noiceball_ptrs[] = { 0xb2080d, 0xb20dc3, 0x000000 };
static ResizableGameArray array_npcdef_noiceball_obj(nullptr, 301, 2, array_npcdef_noiceball_patches, array_npcdef_noiceball_ptrs);
static const uint32_t array_npcdef_nogravity_patches[] = { 0xb20817, 0xb20dc9, 0x000000 };
static const uint32_t array_npcdef_nogravity_ptrs[] = { 0xb20832, 0xb20de9, 0x000000 };
static ResizableGameArray array_npcdef_nogravity_obj(nullptr, 301, 2, array_npcdef_nogravity_patches, array_npcdef_nogravity_ptrs);
static const uint32_t array_b2ca7c_patches[] = { 0xacdf68, 0xad41d5, 0xad430f, 0xadab00, 0x000000 };
static ResizableGameArray array_b2ca7c_obj((void**)0xb2ca7c, 300, 2, array_b2ca7c_patches);
static const uint32_t array_GM_GFX_NPC_PTR_patches[] = { 0x912dad, 0x913364, 0x9138c1, 0x913e4a, 0x9143ca, 0x914a21, 0x915e3c, 0x9161c1, 0x91cf0b, 0x91d28d, 0x91e866, 0x91eef8, 0x920832, 0x920fa7, 0x9215af, 0x928458, 0x928ae4, 0x92a43a, 0x92aacc, 0x92f005, 0x92f65e, 0x93a3dc, 0x93b1cd, 0x93bcc0, 0x93c895, 0x93d62a, 0x946b4d, 0x9472c1, 0x947895, 0x96c88e, 0x96e9c8, 0x97559e, 0x97e220, 0x989f4b, 0x98a209, 0x99005a, 0x990619, 0x9923af, 0x99262d, 0x9928d9, 0x992b7a, 0x992e0d, 0xa8c47b, 0xa8d5c6, 0xac1c47, 0xac1cc5, 0xacb80c, 0xacb84a, 0xace12c, 0xad4761, 0xadacc4, 0xae5d01, 0xae6184, 0x000000 };
static ResizableGameArray array_GM_GFX_NPC_PTR_obj((void**)0xb2ca98, 300, 4, array_GM_GFX_NPC_PTR_patches);
static const uint32_t array_GM_GFX_NPC_MASK_PTR_patches[] = { 0x912b3b, 0x913072, 0x91364f, 0x913b5a, 0x914154, 0x914628, 0x915c7e, 0x916000, 0x91cd4a, 0x91d0ce, 0x91e610, 0x91eae2, 0x9205d9, 0x920de5, 0x921199, 0x9281ff, 0x9286d1, 0x92a1e4, 0x92a6b6, 0x92edc5, 0x92f27e, 0x93a111, 0x93ab6b, 0x93bad4, 0x93c5be, 0x93cfad, 0x946910, 0x947101, 0x9474b5, 0x96c6df, 0x96e819, 0x9753d8, 0x97e071, 0x989de5, 0x98a0a3, 0x98fe48, 0x990289, 0x99226d, 0x9924cc, 0x992783, 0x992a24, 0x992cc5, 0xa8c214, 0xa8d35f, 0xac2316, 0xac2393, 0xacb8f3, 0xace5f8, 0xad4ad7, 0xadb18e, 0x000000 };
static ResizableGameArray array_GM_GFX_NPC_MASK_PTR_obj((void**)0xb2cab4, 300, 4, array_GM_GFX_NPC_MASK_PTR_patches);
static const uint32_t array_IPictureDisp_NPC_patches[] = { 0xac1771, 0xac17fa, 0xac19c7, 0xac1c94, 0xacb88d, 0xace0ca, 0xace11a, 0xace18a, 0xace298, 0xad44c9, 0xad4524, 0xad4639, 0xad474f, 0xadac62, 0xadacb2, 0xadad22, 0xadae30, 0x000000 };
static ResizableGameArray array_IPictureDisp_NPC_obj((void**)0xb2cadc, 300, 4, array_IPictureDisp_NPC_patches);
static const uint32_t array_IPictureDisp_NPC_Mask_patches[] = { 0xac21db, 0xac2362, 0xacb936, 0xace585, 0xace5e0, 0xad4a6a, 0xad4ac5, 0xadb11b, 0xadb176, 0x000000 };
static ResizableGameArray array_IPictureDisp_NPC_Mask_obj((void**)0xb2cb04, 300, 4, array_IPictureDisp_NPC_Mask_patches);
static const uint32_t array_b2cb20_patches[] = { 0x8c5917, 0xac1b38, 0xace379, 0xad471d, 0xadaf11, 0x000000 };
static ResizableGameArray array_b2cb20_obj((void**)0xb2cb20, 300, 2, array_b2cb20_patches);
static const uint32_t array_b2cb3c_patches[] = { 0x8c58ce, 0xac196a, 0xace269, 0xad4607, 0xadae01, 0x000000 };
static ResizableGameArray array_b2cb3c_obj((void**)0xb2cb3c, 300, 2, array_b2cb3c_patches);

static uint32_t currentNpcIdLimit = 300;

uint32_t GetNpcIdLimit()
{
    return currentNpcIdLimit;
}

void PatchNpcIdLimit()
{
    uint32_t newLimit = NPC::MAX_ID;

    array_npc_gfxoffsetx_obj.Patch(newLimit + 1);
    array_npc_gfxoffsety_obj.Patch(newLimit + 1);
    array_GM_CONF_WIDTH_obj.Patch(newLimit + 1);
    array_GM_CONF_HEIGHT_obj.Patch(newLimit + 1);
    array_GM_CONF_GFXWIDTH_obj.Patch(newLimit + 1);
    array_GM_CONF_GFXHEIGHT_obj.Patch(newLimit + 1);
    array_npc_speed_obj.Patch(newLimit + 1);
    array_npc_isShell_obj.Patch(newLimit + 1);
    array_npc_npcblock_obj.Patch(newLimit + 1);
    array_npc_npcblocktop_obj.Patch(newLimit + 1);
    array_isInteractableNPC_ptr_obj.Patch(newLimit + 1);
    array_isCoin_ptr_obj.Patch(newLimit + 1);
    array_isVineNPC_ptr_obj.Patch(newLimit + 1);
    array_isCollectableGoalNPC_ptr_obj.Patch(newLimit + 1);
    array_npc_isflying_obj.Patch(newLimit + 1);
    array_npc_isWaterNPC_obj.Patch(newLimit + 1);
    array_npc_jumphurt_obj.Patch(newLimit + 1);
    array_npc_noblockcollision_obj.Patch(newLimit + 1);
    array_npc_score_obj.Patch(newLimit + 1);
    array_npc_playerblocktop_obj.Patch(newLimit + 1);
    array_npc_grabtop_obj.Patch(newLimit + 1);
    array_npc_cliffturn_obj.Patch(newLimit + 1);
    array_npc_nohurt_obj.Patch(newLimit + 1);
    array_npc_playerblock_obj.Patch(newLimit + 1);
    array_b25e10_obj.Patch(newLimit + 1);
    array_npc_grabside_obj.Patch(newLimit + 1);
    array_isShoeNPC_ptr_obj.Patch(newLimit + 1);
    array_isYoshiNPC_ptr_obj.Patch(newLimit + 1);
    array_b25e80_obj.Patch(newLimit + 1);
    array_npc_noyoshi_obj.Patch(newLimit + 1);
    array_npc_foreground_obj.Patch(newLimit + 1);
    array_npc_isBot_obj.Patch(newLimit + 1);
    array_b25ef0_obj.Patch(newLimit + 1);
    array_isVegetableNPC_ptr_obj.Patch(newLimit + 1);
    array_npc_nofireball_obj.Patch(newLimit + 1);
    array_npc_noiceball_obj.Patch(newLimit + 1);
    array_npc_nogravity_obj.Patch(newLimit + 1);
    array_npc_frames_obj.Patch(newLimit + 1);
    array_npc_framespeed_obj.Patch(newLimit + 1);
    array_npc_framestyle_obj.Patch(newLimit + 1);
    array_npcdef_gfxoffsetx_obj.Patch(newLimit + 1);
    array_npcdef_gfxoffsety_obj.Patch(newLimit + 1);
    array_npcdef_width_obj.Patch(newLimit + 1);
    array_npcdef_height_obj.Patch(newLimit + 1);
    array_npcdef_gfxwidth_obj.Patch(newLimit + 1);
    array_npcdef_gfxheight_obj.Patch(newLimit + 1);
    array_npcdef_isShell_obj.Patch(newLimit + 1);
    array_npcdef_npcblock_obj.Patch(newLimit + 1);
    array_npcdef_npcblocktop_obj.Patch(newLimit + 1);
    array_npcdef_isInteractableNPC_obj.Patch(newLimit + 1);
    array_npcdef_isCoin_obj.Patch(newLimit + 1);
    array_npcdef_isVine_obj.Patch(newLimit + 1);
    array_npcdef_isCollectableGoal_obj.Patch(newLimit + 1);
    array_npcdef_isFlyingNPC_obj.Patch(newLimit + 1);
    array_npcdef_isWaterNPC_obj.Patch(newLimit + 1);
    array_npcdef_jumphurt_obj.Patch(newLimit + 1);
    array_npcdef_noblockcollision_obj.Patch(newLimit + 1);
    array_npcdef_score_obj.Patch(newLimit + 1);
    array_npcdef_playerblocktop_obj.Patch(newLimit + 1);
    array_npcdef_grabtop_obj.Patch(newLimit + 1);
    array_npcdef_cliffturn_obj.Patch(newLimit + 1);
    array_npcdef_nohurt_obj.Patch(newLimit + 1);
    array_npcdef_playerblock_obj.Patch(newLimit + 1);
    array_b295d6_obj.Patch(newLimit + 1);
    array_npcdef_grabside_obj.Patch(newLimit + 1);
    array_npcdef_isShoeNPC_obj.Patch(newLimit + 1);
    array_npcdef_isYoshiNPC_obj.Patch(newLimit + 1);
    array_b29f3e_obj.Patch(newLimit + 1);
    array_npcdef_noYoshi_obj.Patch(newLimit + 1);
    array_npcdef_foreground_obj.Patch(newLimit + 1);
    array_npcdef_isBot_obj.Patch(newLimit + 1);
    array_b2a8a6_obj.Patch(newLimit + 1);
    array_npcdef_isVegetableNPC_obj.Patch(newLimit + 1);
    array_npcdef_speed_obj.Patch(newLimit + 1);
    array_npcdef_nofireball_obj.Patch(newLimit + 1);
    array_npcdef_noiceball_obj.Patch(newLimit + 1);
    array_npcdef_nogravity_obj.Patch(newLimit + 1);
    array_b2ca7c_obj.Patch(newLimit);
    array_GM_GFX_NPC_PTR_obj.Patch(newLimit);
    array_GM_GFX_NPC_MASK_PTR_obj.Patch(newLimit);
    array_IPictureDisp_NPC_obj.Patch(newLimit);
    array_IPictureDisp_NPC_Mask_obj.Patch(newLimit);
    array_b2cb20_obj.Patch(newLimit);
    array_b2cb3c_obj.Patch(newLimit);

    PATCH(0x8c27a2).word(newLimit).Apply();  // Loop in GF_INIT_DEF_VALS for npc_speed
    PATCH(0x8c3340).word(newLimit).Apply();  // Loop for score/width/height
    PATCH(0x8ca171).word(newLimit).Apply();  // Loop for isWaterNPC/foreground
    PATCH(0xa03667).word(newLimit).Apply();  // Skip GF_INIT_NPC if ID is > 300
    PATCH(0xacb7d7).word(newLimit).Apply();  // Loop to delete all NPC graphics HDCs
    PATCH(0xacdeb3).dword(newLimit).Apply(); // Loop to load all NPC GIFs
    PATCH(0xadaa4b).dword(newLimit).Apply(); // Loop to load all NPC GIFs
    PATCH(0xb20849).word(newLimit).Apply();  // Loop over NPC IDs for saveNPCDefaults
    PATCH(0xb20e35).word(newLimit).Apply();  // Loop over NPC IDs for loadDefaultNPCConfiguration
    PATCH(0xb20ef9).dword(newLimit).Apply(); // Loop to load all NPC TXTs
    PATCH(0xb2108d).dword(newLimit).Apply(); // Loop to load all NPC TXTs

    currentNpcIdLimit = newLimit;
}

//==============//
// BGO ID Limit //
//==============//

static const uint32_t array_b25b54_patches[] = { 0x8c27bb, 0x99adc2, 0x9a74a8, 0x000000 };
static ResizableGameArray array_b25b54_obj((void**)0xb25b54, 201, 2, array_b25b54_patches);
static const uint32_t array_b2be30_patches[] = { 0x90f690, 0x90f832, 0x90fa83, 0x90fe23, 0x910060, 0x911140, 0x911379, 0x9115b7, 0x91193c, 0x911b79, 0x911dbb, 0x912177, 0x9123b0, 0x9125ee, 0x9293dc, 0x929615, 0x929be6, 0x929e23, 0x9460f8, 0x94633a, 0x946577, 0x9fdefe, 0xac5a72, 0xad59b6, 0xae664a, 0xae6695, 0xae6731, 0xae6890, 0xaf9f9d, 0x000000 };
static ResizableGameArray array_b2be30_obj((void**)0xb2be30, 200, 2, array_b2be30_patches);
static const uint32_t array_b2be4c_patches[] = { 0x8d9dcc, 0x90f659, 0x90f7fb, 0x90fa4c, 0x90fd47, 0x90fdec, 0x90ff84, 0x910029, 0x9101fd, 0x9102a2, 0x911064, 0x911109, 0x91129d, 0x911342, 0x9114db, 0x911580, 0x911860, 0x911905, 0x911a9d, 0x911b42, 0x911cdf, 0x911d84, 0x91209b, 0x912140, 0x9122d4, 0x912379, 0x912512, 0x9125b7, 0x9290c2, 0x929167, 0x929300, 0x9293a5, 0x929539, 0x9295de, 0x9298c8, 0x92996d, 0x929b0a, 0x929baf, 0x929d47, 0x929dec, 0x94601c, 0x9460c1, 0x94625e, 0x946303, 0x94649b, 0x946540, 0x9fdf2f, 0xac5afa, 0xae665c, 0xae6683, 0xae6743, 0xae687e, 0xafa073, 0x000000 };
static ResizableGameArray array_b2be4c_obj((void**)0xb2be4c, 200, 2, array_b2be4c_patches);
static const uint32_t array_b2cc34_patches[] = { 0xacf152, 0xad5540, 0xad567a, 0xadbcdd, 0x000000 };
static ResizableGameArray array_b2cc34_obj((void**)0xb2cc34, 200, 2, array_b2cc34_patches);
static const uint32_t array_b2ccd8_patches[] = { 0xac59e4, 0xac5ac9, 0xacf563, 0xacf5d1, 0xacf5e3, 0xad5acc, 0xadc0ee, 0xadc15c, 0xadc16e, 0x000000 };
static ResizableGameArray array_b2ccd8_obj((void**)0xb2ccd8, 200, 2, array_b2ccd8_patches);
static const uint32_t array_b2ccf4_patches[] = { 0x8d9d98, 0x9102d9, 0x92919e, 0x9299a4, 0xac5816, 0xac5a41, 0xacf453, 0xacf592, 0xacf5a4, 0xad5972, 0xad59a4, 0xadbfde, 0xadc11d, 0xadc12f, 0x000000 };
static ResizableGameArray array_b2ccf4_obj((void**)0xb2ccf4, 200, 2, array_b2ccf4_patches);
static const uint32_t array_GM_GFX_BACKGROUND_MASK_PTR_patches[] = { 0x90f622, 0x90fdb5, 0x9110d2, 0x9118ce, 0x912109, 0x92936e, 0x929b78, 0x9462cc, 0xac62d0, 0xac634e, 0xacbcc9, 0xacf85e, 0xad5e86, 0xadc3e7, 0x000000 };
static ResizableGameArray array_GM_GFX_BACKGROUND_MASK_PTR_obj((void**)0xb2cc6c, 200, 4, array_GM_GFX_BACKGROUND_MASK_PTR_patches);
static const uint32_t array_GM_GFX_BACKGROUND_PTR_patches[] = { 0x90f7c4, 0x90fa15, 0x90fff2, 0x91026b, 0x91130b, 0x911549, 0x911b0b, 0x911d4d, 0x912342, 0x912580, 0x929130, 0x9295a7, 0x929936, 0x929db5, 0x94608a, 0x946509, 0xac5c04, 0xac5c81, 0xacbbcf, 0xacbc14, 0xacf316, 0xad5b10, 0xadbea1, 0xae68a5, 0x000000 };
static ResizableGameArray array_GM_GFX_BACKGROUND_PTR_obj((void**)0xb2cc50, 200, 4, array_GM_GFX_BACKGROUND_PTR_patches);
static const uint32_t array_IPictureDisp_Background_patches[] = { 0xac561d, 0xac56a6, 0xac5873, 0xac5c50, 0xacbc5e, 0xacf2b4, 0xacf304, 0xacf374, 0xacf482, 0xad5834, 0xad588f, 0xad59e9, 0xad5afe, 0xadbe3f, 0xadbe8f, 0xadbeff, 0xadc00d, 0x000000 };
static ResizableGameArray array_IPictureDisp_Background_obj((void**)0xb2cc94, 200, 4, array_IPictureDisp_Background_patches);
static const uint32_t array_IPictureDisp_Background_Mask_patches[] = { 0xac6196, 0xac631d, 0xacbd13, 0xacf7ec, 0xacf846, 0xad5e19, 0xad5e74, 0xadc375, 0xadc3cf, 0x000000 };
static ResizableGameArray array_IPictureDisp_Background_Mask_obj((void**)0xb2ccbc, 200, 4, array_IPictureDisp_Background_Mask_patches);
static const uint32_t array_unkBGOSetting_patches[] = { 0x928fe0, 0xa99451, 0x000000 };
static ResizableGameArray array_unkBGOSetting_obj((void**)0xb2be14, 201, 2, array_unkBGOSetting_patches);

static uint32_t currentBgoIdLimit = 200;

uint32_t GetBgoIdLimit()
{
    return currentBgoIdLimit;
}

void PatchBgoIdLimit()
{
    uint32_t newLimit = SMBX_BGO::MAX_ID;

    array_b25b54_obj.Patch(newLimit + 1);
    array_b2be30_obj.Patch(newLimit);
    array_b2be4c_obj.Patch(newLimit);
    array_b2cc34_obj.Patch(newLimit);
    array_b2ccd8_obj.Patch(newLimit);
    array_b2ccf4_obj.Patch(newLimit);
    array_GM_GFX_BACKGROUND_MASK_PTR_obj.Patch(newLimit);
    array_GM_GFX_BACKGROUND_PTR_obj.Patch(newLimit);
    array_IPictureDisp_Background_obj.Patch(newLimit);
    array_IPictureDisp_Background_Mask_obj.Patch(newLimit);
    array_unkBGOSetting_obj.Patch(newLimit+1);

    currentBgoIdLimit = newLimit;
}

//================//
// Block ID Limit //
//================//

static const uint32_t array_b2b94c_patches[] = { 0x8d5861, 0x997b66, 0x997b0f, 0x997dfd, 0x9a11c1, 0x9a12a5, 0x9a1c5c, 0x9a24ac, 0x9a24fd, 0x9a2557, 0x9a286b, 0x9a2952, 0x9a2bc4, 0x9a2bf8, 0x9a2db5, 0x9a3055, 0x9a314b, 0x9a3267, 0x9a359f, 0x9a3740, 0x9a3698, 0x9a3921, 0x9a3ae4, 0x9a3c05, 0x9a4354, 0x9a483e, 0x9a4c76, 0x9a4e3e, 0x9a5156, 0x9c0a13, 0x9eeddb, 0x9ef5b6, 0xa0a24d, 0xa0a1fa, 0xa12e8d, 0xa13032, 0xa13110, 0xa13508, 0xa134b2, 0xa136cc, 0xa137b3, 0xa15fae, 0xa1a747, 0xa1bbfd, 0xa1bdfd, 0xa1c5a1, 0xa1cdd2, 0xa1cebb, 0xa1d0be, 0xa4fa63, 0x000000 };
static ResizableGameArray array_b2b94c_obj((void**)0xb2b94c, 701, 2, array_b2b94c_patches);
static const uint32_t array_b2b968_patches[] = { 0x9a1df7, 0x9a1eac, 0x9a1f94, 0x9a237e, 0x9c0a38, 0x9eee03, 0x9ef5e7, 0xa123cd, 0xa124dd, 0xa125c8, 0xa12756, 0xa12835, 0xa12d4f, 0xa50aee, 0xa50ee0, 0xa51336, 0x000000 };
static ResizableGameArray array_b2b968_obj((void**)0xb2b968, 701, 2, array_b2b968_patches);
static const uint32_t array_b2c048_patches[] = { 0x8d593e, 0x8d5d84, 0x9982ee, 0x9a17bc, 0x9aeebe, 0x9c0aea, 0x9dbb4f, 0x9dc306, 0x9e2d67, 0x9e3044, 0x9eeda7, 0xa1201f, 0xa1236e, 0xa1a6a4, 0xa4f9da, 0xa4fdc9, 0xa5029e, 0xa5061c, 0xa50a63, 0xa50e32, 0xa512ad, 0xa5167c, 0xa526c1, 0xa5b2b4, 0x000000 };
static ResizableGameArray array_b2c048_obj((void**)0xb2c048, 701, 2, array_b2c048_patches);
static const uint32_t array_b2c064_patches[] = { 0x8c982c, 0x91d755, 0x92b4db, 0x9a20cc, 0x9a38ed, 0x9a3a5e, 0xa139ca, 0x000000 };
static ResizableGameArray array_b2c064_obj((void**)0xb2c064, 701, 2, array_b2c064_patches);
static const uint32_t array_b2c080_patches[] = { 0xa31330, 0x000000 };
static ResizableGameArray array_b2c080_obj((void**)0xb2c080, 701, 2, array_b2c080_patches);
static const uint32_t array_b2c09c_patches[] = { 0x9a1837, 0x9bbcf7, 0x9bbd46, 0x000000 };
static ResizableGameArray array_b2c09c_obj((void**)0xb2c09c, 701, 2, array_b2c09c_patches);
static const uint32_t array_b2c0b8_patches[] = { 0x9e39cc, 0x000000 };
static ResizableGameArray array_b2c0b8_obj((void**)0xb2c0b8, 701, 2, array_b2c0b8_patches);
static const uint32_t array_b2c0d4_patches[] = { 0x8f7067, 0x998322, 0x9a1748, 0x9bba16, 0x9c03b1, 0x9c0b28, 0x9eed7a, 0xa11309, 0xa1c3cf, 0xa1c884, 0xa1cc2a, 0xa3bfba, 0xa4f927, 0xa4fd12, 0xa501e1, 0xa50563, 0xa509ac, 0xa50d7f, 0xa511fa, 0xa515c5, 0xa5260b, 0xa5b23d, 0xa5d3ea, 0x000000 };
static ResizableGameArray array_b2c0d4_obj((void**)0xb2c0d4, 701, 2, array_b2c0d4_patches);
static const uint32_t array_blockdef_isResizeableBlock_patches[] = { 0x8d58cf, 0x8d5c86, 0x8f70de, 0x91055a, 0x91d6c8, 0x943714, 0x943f2e, 0x99827c, 0x9a1788, 0x9a2a03, 0x9aee88, 0x9bb9ba, 0x9c0430, 0x9c09dd, 0x9dbb7c, 0x9dc333, 0x9e2d2e, 0x9e300b, 0x9ed6d9, 0x9ed6c5, 0x9ed80c, 0x9eed4d, 0x9ef511, 0x9f10b4, 0x9f1823, 0x9f59ca, 0x9f974a, 0x9fd68c, 0xa06a48, 0xa11fa7, 0xa11fec, 0xa1233a, 0xa131bc, 0xa14aca, 0xa1a669, 0xa1c462, 0xa1c927, 0xa1cccd, 0xa2261d, 0xa4f9a1, 0xa4fd8f, 0xa50260, 0xa505df, 0xa50a29, 0xa50df9, 0xa51274, 0xa51642, 0xa52689, 0xa5b278, 0xa87089, 0xa998be, 0xaf7159, 0x000000 };
static ResizableGameArray array_blockdef_isResizeableBlock_obj((void**)0xb2b930, 701, 2, array_blockdef_isResizeableBlock_patches);
static const uint32_t array_blockdef_height_patches[] = { 0x8c81d8, 0x8c85d0, 0x9de91f, 0x9e387e, 0x9e4000, 0x9fd637, 0x9fd655, 0xae52ca, 0xae52f0, 0xae5365, 0xae53ad, 0xae54e7, 0xae66f9, 0xae6c89, 0xae720d, 0x000000 };
static ResizableGameArray array_blockdef_height_obj((void**)0xb2ba14, 701, 2, array_blockdef_height_patches);
static const uint32_t array_blockdef_width_patches[] = { 0x8c81bd, 0x8c8588, 0x9de94b, 0x9e385d, 0x9e3fd2, 0x9fd5d5, 0x9fd5f3, 0xae52b9, 0xae5301, 0xae5376, 0xae539c, 0xae54f8, 0xae670a, 0xae6c9a, 0xae721e, 0x000000 };
static ResizableGameArray array_blockdef_width_obj((void**)0xb2b9f8, 701, 2, array_blockdef_width_patches);
static const uint32_t array_b2bebc_patches[] = { 0x000000 };
static ResizableGameArray array_b2bebc_obj((void**)0xb2bebc, 700, 2, array_b2bebc_patches);
static const uint32_t array_b2bea0_patches[] = { 0x91d844, 0x91d9f3, 0x91db9d, 0x92b5bd, 0x92b76d, 0x92b918, 0x943ffd, 0x9441ac, 0x944356, 0x9da934, 0x000000 };
static ResizableGameArray array_b2bea0_obj((void**)0xb2bea0, 700, 2, array_b2bea0_patches);
static const uint32_t array_HasNoBlockMask_patches[] = { 0x91d7f0, 0x92b568, 0x943fa9, 0xac0780, 0xacb5c8, 0x000000 };
static ResizableGameArray array_HasNoBlockMask_obj((void**)0xb2c010, 700, 2, array_HasNoBlockMask_patches);
static const uint32_t array_unkLoadedCustomBlocks_patches[] = { 0xacd3f5, 0xad3495, 0xad35cc, 0xad9f9f, 0x000000 };
static ResizableGameArray array_unkLoadedCustomBlocks_obj((void**)0xb2c940, 700, 2, array_unkLoadedCustomBlocks_patches);
static const uint32_t array_GM_GFX_BLOCKS_MASK_PTR_patches[] = { 0x910997, 0x91da2a, 0x92b7a4, 0x943a41, 0x9441e3, 0xac067d, 0xac06fa, 0xacb608, 0xacd86f, 0xad3b69, 0xada419, 0x000000 };
static ResizableGameArray array_GM_GFX_BLOCKS_MASK_PTR_obj((void**)0xb2c978, 700, 4, array_GM_GFX_BLOCKS_MASK_PTR_patches);
static const uint32_t array_GM_GFX_BLOCKS_PTR_patches[] = { 0x910b27, 0x910cb9, 0x91d87b, 0x91dbd4, 0x92b5f4, 0x92b94f, 0x943bce, 0x943d62, 0x944034, 0x94438d, 0xabffae, 0xac002c, 0xacb4e1, 0xacb51f, 0xacd5c1, 0xad37f3, 0xada16b, 0xae550d, 0x000000 };
static ResizableGameArray array_GM_GFX_BLOCKS_PTR_obj((void**)0xb2c95c, 700, 4, array_GM_GFX_BLOCKS_PTR_patches);
static const uint32_t array_IPictureDisp_Blocks_patches[] = { 0xabfe73, 0xabfffb, 0xacb562, 0xacd54f, 0xacd5a9, 0xad3786, 0xad37e1, 0xada0f9, 0xada153, 0x000000 };
static ResizableGameArray array_IPictureDisp_Blocks_obj((void**)0xb2c9a0, 700, 4, array_IPictureDisp_Blocks_patches);
static const uint32_t array_IPictureDisp_Blocks_Mask_patches[] = { 0xac0542, 0xac06c9, 0xacb64c, 0xacd7fd, 0xacd857, 0xad3afc, 0xad3b57, 0xada3a7, 0xada401, 0x000000 };
static ResizableGameArray array_IPictureDisp_Blocks_Mask_obj((void**)0xb2c9c8, 700, 4, array_IPictureDisp_Blocks_Mask_patches);

static uint32_t currentBlockIdLimit = 700;

uint32_t GetBlockIdLimit()
{
    return currentBlockIdLimit;
}

void PatchBlockIdLimit()
{
    uint32_t newLimit = Block::MAX_ID;

    array_b2b94c_obj.Patch(newLimit + 1);
    array_b2b968_obj.Patch(newLimit + 1);
    array_b2c048_obj.Patch(newLimit + 1);
    array_b2c064_obj.Patch(newLimit + 1);
    array_b2c080_obj.Patch(newLimit + 1);
    array_b2c09c_obj.Patch(newLimit + 1);
    array_b2c0b8_obj.Patch(newLimit + 1);
    array_b2c0d4_obj.Patch(newLimit + 1);
    array_blockdef_isResizeableBlock_obj.Patch(newLimit + 1);
    array_blockdef_height_obj.Patch(newLimit + 1);
    array_blockdef_width_obj.Patch(newLimit + 1);

    array_b2bebc_obj.Patch(newLimit);
    array_b2bea0_obj.Patch(newLimit);
    array_HasNoBlockMask_obj.Patch(newLimit);
    array_unkLoadedCustomBlocks_obj.Patch(newLimit);
    array_GM_GFX_BLOCKS_MASK_PTR_obj.Patch(newLimit);
    array_GM_GFX_BLOCKS_PTR_obj.Patch(newLimit);
    array_IPictureDisp_Blocks_obj.Patch(newLimit);
    array_IPictureDisp_Blocks_Mask_obj.Patch(newLimit);

    // Some others that were missed...
    PATCH(0x9DC913).dword(newLimit).Apply();
    PATCH(0x9DC92B).dword(newLimit).Apply();
    PATCH(0x9DC9B9).dword(newLimit).Apply();
    PATCH(0x9DC9D7).dword(newLimit).Apply();
    PATCH(0x9DDA85).dword(newLimit).Apply();
    PATCH(0x9DDA94).dword(newLimit).Apply();
    PATCH(0x9DE1C5).dword(newLimit).Apply();
    PATCH(0x9DE1E2).dword(newLimit).Apply();
    PATCH(0x9DF2C0).dword(newLimit).Apply();
    PATCH(0x9DF2DD).dword(newLimit).Apply();
    PATCH(0x9DF6DC).dword(newLimit).Apply();
    PATCH(0x9DF6FA).dword(newLimit).Apply();
    PATCH(0x9DFBF2).dword(newLimit).Apply();
    PATCH(0x9DFC10).dword(newLimit).Apply();
    PATCH(0x9E0104).dword(newLimit).Apply();
    PATCH(0x9E0122).dword(newLimit).Apply();
    PATCH(0xA1257A).dword(newLimit).Apply();
    PATCH(0xA52740).word(newLimit).Apply();

    // This wasn't actually mixxed though: PATCH(0xAD3495).dword(newLimit).Apply();
    PATCH(0xACD342).dword(newLimit).Apply();
    PATCH(0xAD9EE9).dword(newLimit).Apply();

    currentBlockIdLimit = newLimit;
}
