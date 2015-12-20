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

			if(playerX > block->mometum.x + block->mometum.width ||
				playerX2 < block->mometum.x  ||
				playerY > block->mometum.y + block->mometum.height ||
				playerY2 < block->mometum.y)
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
        if (Blocks::Get(i)->mometum.x > Blocks::Get(i - 1)->mometum.x) {
            WORD prev = i - 1;
            native_sortY((short*)&i, (short*)&prev);
        }
    }

    native_sort_finalize1();
    native_sort_finalize2();
}

void Blocks::SetNextFrameSorting()
{
    isBlocksSortingRequired = true;
}

// TEST COLLISION (SMBX BUILTIN)
int Blocks::TestCollision(PlayerMOB* pMobPOS, Block* pBlockPOS) {	
	typedef int __stdcall colfunc(void*, void*);
	colfunc* f = (colfunc*)GF_MOB_BLOCK_COL;	
	return f(&pMobPOS->momentum.x, &pBlockPOS->mometum.x);
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