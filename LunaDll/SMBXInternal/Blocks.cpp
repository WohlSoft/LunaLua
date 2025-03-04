#include "Blocks.h"
#include "PlayerMOB.h"
#include <algorithm>

bool isBlocksSortingRequired = false;

Block* Blocks::Get(int index) {
    if(GM_BLOCKS_PTR == 0 || index < 0 || index > GM_BLOCK_COUNT) {
        return NULL;
    } else {
        return &((GetBase())[index]);
    }
}

static ExtendedBlockFields g_extendedBlockFields[20001];

ExtendedBlockFields* Blocks::GetRawExtended(int index)
{
    if (index < 0 || index >= 20001)
        return nullptr;

    return &g_extendedBlockFields[index];
}

void Blocks::ClearExtendedFields()
{
    for (int i = 0; i < 20001; i++)
    {
        g_extendedBlockFields[i].Reset();
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
static int16_t blockprop_walkpaststair[Block::MAX_ID + 1] = { 0 }; // whether to walk past semisolid slopes (like ghost house stairs)

void Blocks::InitProperties() {
    for (int id = 1; id <= Block::MAX_ID; id++)
    {
        SetBlockBumpable(id, false);
        SetBlockPlayerFilter(id, 0);
        SetBlockNPCFilter(id, 0);
        SetBlockWalkPastStair(id, false);
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
    SetBlockBumpable(0x277, true);

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

bool Blocks::GetBlockWalkPastStair(int id) {
    if ((id < 1) || (id > Block::MAX_ID)) return false;
    return (blockprop_walkpaststair[id] != 0);
}

void Blocks::SetBlockWalkPastStair(int id, bool walkpaststair) {
    if ((id < 1) || (id > Block::MAX_ID)) return;
    blockprop_walkpaststair[id] = walkpaststair ? -1 : 0;
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
    else if (s == "walkpaststair")
    {
        return reinterpret_cast<uintptr_t>(blockprop_walkpaststair);
    }
    else
    {
        return 0;
    }
}


// returns the range of block indexes to iterate over using the block lookup table
void BlockLookupQuery(double x1, double x2, int* outF, int* outL) {
    *outF = GM_BLOCK_LOOKUP_MIN[std::min(std::max((int)floor((x1) / 32 + 8000), 0), 16000)];
    *outL = GM_BLOCK_LOOKUP_MAX[std::min(std::max((int)floor((x2) / 32 + 8000), 0), 16000)];
}


double calculateSlopeAt(double refX, double offsX, Momentum* blockMomentum, short floorslope) {
    double blockCollisionPercent = ((refX + offsX) - blockMomentum->x) / blockMomentum->width;
    if (floorslope == -1) {
        // reverse direction for slopes going down left
        blockCollisionPercent = 1.0 - blockCollisionPercent;
    }
    return blockCollisionPercent;
}

// used in NPC and player collision hooks to handle collision against slopes when flagged as semisolid.
// returns whether the collision should be allowed
bool Blocks::FilterSemisolidSlopeCollision(Momentum* entityMomentum, Momentum* speed, int blockIdx, int entityBottomCollisionTimer, bool standingOnSlope)
{
    Block* block = Block::GetRaw(blockIdx);

    short floorslope = blockdef_floorslope[block->BlockType];

    if (speed->speedY < 0 && !standingOnSlope) {
        // if moving upwards
        if (speed->speedX == 0 || floorslope != (speed->speedX < 0 ? 1 : -1)) {
            // moving into the slope in the same direction of its upwards facing face:
            // collision should never be considered in this case
            return false;
        } else {
            // moving into the slope *opposite* the direction of its upwards face,
            // in this case, if the slope of our movement vector is pointed MORE upwards then the slope of the slope,
            // we shouldn't consider collisions

            if (((-speed->speedY) / speed->speedX) < (block->momentum.height / block->momentum.width)) {
                return false;
            }
        }
    }

    double playerRefX = entityMomentum->x;
    if (floorslope == -1) {
        // use player right edge for slopes going down left
        playerRefX += entityMomentum->width;
    }
    // Use whichever is more likely to put us on the slope:
    // our current position - speed, OR current position + speed
    double slopeAtA = calculateSlopeAt(playerRefX, speed->speedX *  4.0, &block->momentum, floorslope);
    double slopeAtB = calculateSlopeAt(playerRefX, speed->speedX * -4.0, &block->momentum, floorslope);
    double blockCollisionPercent = std::max(slopeAtA, slopeAtB);

    // if the object is colliding above the top edge of the slope
    if (blockCollisionPercent < 0.0) {
        // check if it's a top slope!!!!!!!
        // detect slopes ABOVE and aligned to the edge of the slope
        bool isTopSlope = true;
        {
            // ref position for a block above
            double refX = block->momentum.x;
            if (floorslope == -1) {
                refX = refX + block->momentum.width;
            }
            double refY = block->momentum.y;

            // 
            int fblock, lblock;
            BlockLookupQuery(refX - 2, refX + 2, &fblock, &lblock);

            for (int i = fblock; i <= lblock; i++) {
                auto b = Block::GetRaw(i);
                // if this is a semisolid slope of the same type
                if (abs(b->momentum.y + b->momentum.height - refY) <= 2.0 && !b->IsHidden && !b->IsInvisible2 && blockdef_semisolid[b->BlockType] && blockdef_floorslope[b->BlockType] == floorslope) {
                    // if the edge is aligned at the correct position
                    if (floorslope == -1) {
                        // left slope
                        if (abs(b->momentum.x - refX) > 2.0) { continue; }
                    }
                    else {
                        // right slope
                        if (abs(b->momentum.x + b->momentum.width - refX) > 2.0) { continue; }
                    }

                    // found a matching semisolid above!
                    // this is NOT a top slope
                    isTopSlope = false;
                    break;
                }
            }
        }
        // if this is a top slope,
        if (isTopSlope) {
            // clamp percent so player can stand at top edge
            blockCollisionPercent = 0.0;
        }
    }

    // check whether the player is sufficiently above the slope,
    double offset = 2;
    if (speed->speedY > 0) {
        // if moving downwards, add some extra leniency to collision
        offset += speed->speedY;
    }
    if (standingOnSlope || speed->speedY > 0) {
        // if already standing on a slope, make it more lenient to help stick to the slope
        offset += 4;
    }
    double playerFootY = entityMomentum->y + entityMomentum->height;
    double blockCollisionBottom = block->momentum.y + blockCollisionPercent * block->momentum.height;
    if (playerFootY > blockCollisionBottom + offset) {
        // foot is too far below to consider collision, cancel
        return false;
    }

    // check for slopes we can walk past, like stairs
    if (Blocks::GetBlockWalkPastStair(block->BlockType)) {
        // if our feet our at the bottom of the stair, and we're standing on something that isn't a slope
        if (entityMomentum->y + entityMomentum->height >= block->momentum.y + block->momentum.height - 1 && entityBottomCollisionTimer != 0 && speed->speedY >= 0) {

            // Check that there's not a platform BELOW the slope, in which case we want the entity to step up onto it anyways
            bool isEdgeSlope = true;
            {
                double x1;
                if (floorslope == -1) {
                    // Down left slope, check according to left edge
                    x1 = block->momentum.x;
                } else {
                    // Down right slope, check according to right edge
                    x1 = block->momentum.x + block->momentum.width - 32;
                }
                double x2 = x1 + 32;
                double refY = block->momentum.y + block->momentum.height;
                x1 += 1;
                x2 -= 1;

                int fblock, lblock;
                BlockLookupQuery(x1, x2, &fblock, &lblock);

                for (int i = fblock; i <= lblock; i++) {
                    auto b = Block::GetRaw(i);

                    if (abs(b->momentum.y - refY) <= 2.0) {
                        // if the block is below the slope
                        if (b->momentum.x > x2 || b->momentum.x + b->momentum.width < x1) {
                            // the block does not 'collide' horizontally
                            continue;
                        }

                        if (!b->IsHidden && !b->IsInvisible2 && blockdef_floorslope[b->BlockType] == 0 && blockdef_passthrough[b->BlockType] == 0) {
                            // found a matching solid below!
                            // this is NOT an edge slope
                            isEdgeSlope = false;
                            break;
                        }

                    }
                }
            }

            // if this slope ISN'T at the edge of a platform
            if (!isEdgeSlope) {
                // cancel collision, allow walking past the slope
                return false;
            }
        }

    }

    return true;
}
