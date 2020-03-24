#include "Blocks.h"
#include "PlayerMOB.h"

bool isBlocksSortingRequired = false;

Block* Blocks::Get(int index) {
    if(GM_BLOCKS_PTR == 0 || index < 0 || index > GM_BLOCK_COUNT) {
        return NULL;
    } else {
        return &((GetBase())[index]);
    }
}

bool Blocks::IsPlayerTouchingType(int type, int sought, PlayerMOB* demo) {	
    Block* blocks = Blocks::GetBase();
    Block* block = 0;
    double playerX = demo->momentum.x - 0.20;
    double playerY = demo->momentum.y - 0.20;
    double playerX2 = demo->momentum.x + demo->momentum.width + 0.20;
    double playerY2 = demo->momentum.y + demo->momentum.height + 0.20;

    for(int i = 1; i <= GM_BLOCK_COUNT; i++) {
        if(blocks[i].BlockType == type) {
            block = &blocks[i];

            if(playerX > block->momentum.x + block->momentum.width ||
                playerX2 < block->momentum.x  ||
                playerY > block->momentum.y + block->momentum.height ||
                playerY2 < block->momentum.y)
                continue;

            if(TestCollision(demo, block) == sought)
                return true;
        }
    }
    return false; // no collision
}

void Blocks::DoSortingIfRequired()
{
    if (!isBlocksSortingRequired)
        return;

    WORD beginIndex = 1;
    WORD blockCount = GM_BLOCK_COUNT;
    native_sortX((short*)&beginIndex, (short*)&blockCount);

    for (WORD i = 2; i <= blockCount; i++) {
        if (Blocks::Get(i)->momentum.x > Blocks::Get(i - 1)->momentum.x) {
            WORD prev = i - 1;
            native_sortY((short*)&i, (short*)&prev);
        }
    }

    native_sort_finalize1();
    native_sort_finalize2();

    isBlocksSortingRequired = false;
}

void Blocks::SetNextFrameSorting()
{
    isBlocksSortingRequired = true;
}

// TEST COLLISION (SMBX BUILTIN)
int Blocks::TestCollision(PlayerMOB* pMobPOS, Block* pBlockPOS) {	
    typedef int __stdcall colfunc(void*, void*);
    colfunc* f = (colfunc*)GF_MOB_BLOCK_COL;	
    return f(&pMobPOS->momentum.x, &pBlockPOS->momentum.x);
}

// SET ALL
void Blocks::SetAll(int type1, int type2) {
    Block* blocks = Blocks::GetBase();	
    for(int i = 1; i <= GM_BLOCK_COUNT; i++) {
        if(blocks[i].BlockType == type1) {
            blocks[i].BlockType = type2;
        }
    }
}

// SWAP ALL
void Blocks::SwapAll(int type1, int type2) {
    Block* blocks = Blocks::GetBase();	
    for(int i = 1; i <= GM_BLOCK_COUNT; i++) {
        if(blocks[i].BlockType == type1) {
            blocks[i].BlockType = type2;
        }
        else if(blocks[i].BlockType == type2) {
            blocks[i].BlockType = type1;
        }
    }
}

void Blocks::ShowAll(int type) {
    Block* blocks = Blocks::GetBase();	
    for(int i = 1; i <= GM_BLOCK_COUNT; i++) {
        if(blocks[i].BlockType == type) {
            blocks[i].IsInvisible3 = 0;
        }		
    }
}

void Blocks::HideAll(int type) {
    Block* blocks = Blocks::GetBase();	
    for(int i = 1; i <= GM_BLOCK_COUNT; i++) {
        if(blocks[i].BlockType == type) {
            blocks[i].IsInvisible3 = 0xFFFF;
        }		
    }
}

static int16_t blockprop_bumpable[Block::MAX_ID + 1] = { 0 };
static int16_t blockprop_playerfilter[Block::MAX_ID + 1] = { 0 };
static int16_t blockprop_npcfilter[Block::MAX_ID + 1] = { 0 };

void Blocks::InitProperties() {
    for (int id = 1; id <= Block::MAX_ID; id++)
    {
        SetBlockBumpable(id, false);
        SetBlockPlayerFilter(id, 0);
    }

    // Default game config
    SetBlockBumpable(0x4, true);
    SetBlockBumpable(0x267, true);
    SetBlockBumpable(0x37, true);
    SetBlockBumpable(0x3C, true);
    SetBlockBumpable(0x5A, true);
    SetBlockBumpable(0x9F, true);
    SetBlockBumpable(0xA9, true);
    SetBlockBumpable(0xAA, true);
    SetBlockBumpable(0xAD, true);
    SetBlockBumpable(0xB0, true);
    SetBlockBumpable(0xB3, true);
    SetBlockBumpable(0xBC, true);
    SetBlockBumpable(0xE2, true);
    SetBlockBumpable(0x119, true);
    SetBlockBumpable(0x11A, true);
    SetBlockBumpable(0x11B, true);
    for (int id = 0x26E; id <= 0x271; id++)
    {
        SetBlockBumpable(id, true);
    }

    SetBlockPlayerFilter(626, 1);
    SetBlockPlayerFilter(627, 2);
    SetBlockPlayerFilter(628, 3);
    SetBlockPlayerFilter(629, 4);
    SetBlockPlayerFilter(632, 5);
}

bool Blocks::GetBlockBumpable(int id) {
    if ((id < 1) || (id > Block::MAX_ID)) return false;
    return (blockprop_bumpable[id] != 0);
}

void Blocks::SetBlockBumpable(int id, bool bumpable) {
    if ((id < 1) || (id > Block::MAX_ID)) return;
    blockprop_bumpable[id] = bumpable ? -1 : 0;
}

short Blocks::GetBlockPlayerFilter(int id)
{
    if ((id < 1) || (id > Block::MAX_ID)) return 0;
    return blockprop_playerfilter[id];
}

void Blocks::SetBlockPlayerFilter(int id, short characterId)
{
    if ((id < 1) || (id > Block::MAX_ID)) return;
    blockprop_playerfilter[id] = characterId;
}

short Blocks::GetBlockNPCFilter(int id)
{
    if ((id < 1) || (id > Block::MAX_ID)) return 0;
    return blockprop_npcfilter[id];
}

void Blocks::SetBlockNPCFilter(int id, short npcId)
{
    if ((id < 1) || (id > Block::MAX_ID)) return;
    blockprop_npcfilter[id] = npcId;
}

// Getter for address of Block property arrays
uintptr_t Blocks::GetPropertyTableAddress(const std::string& s)
{
    if (s == "bumpable")
    {
        return reinterpret_cast<uintptr_t>(blockprop_bumpable);
    }
    else if (s == "playerfilter")
    {
        return reinterpret_cast<uintptr_t>(blockprop_playerfilter);
    }
    else if (s == "npcfilter")
    {
        return reinterpret_cast<uintptr_t>(blockprop_npcfilter);
    }
    else
    {
        return 0;
    }
}
