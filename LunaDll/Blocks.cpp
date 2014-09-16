#include "Blocks.h"

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
	double playerX = demo->CurXPos - 0.20;
	double playerY = demo->CurYPos - 0.20;
	double playerX2 = demo->CurXPos + demo->Width + 0.20;
	double playerY2 = demo->CurYPos + demo->Height + 0.20;

	for(int i = 1; i <= GM_BLOCK_COUNT; i++) {
		if(blocks[i].BlockType == type) {
			block = &blocks[i];

			if(playerX > block->XPos + block->W ||
				playerX2 < block->XPos  ||
				playerY > block->YPos + block->H ||
				playerY2 < block->YPos)
				continue;

			if(TestCollision(demo, block) == sought)
				return true;
		}
	}
	return false; // no collision
}

// TEST COLLISION (SMBX BUILTIN)
int Blocks::TestCollision(PlayerMOB* pMobPOS, Block* pBlockPOS) {	
	typedef int colfunc(void*, void*);
	colfunc* f = (colfunc*)GF_MOB_BLOCK_COL;	
	return f(&pMobPOS->CurXPos, &pBlockPOS->XPos);
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
			blocks[i].IsHidden = 0;
		}		
	}
}

void Blocks::HideAll(int type) {
	Block* blocks = Blocks::GetBase();	
	for(int i = 1; i <= GM_BLOCK_COUNT; i++) {
		if(blocks[i].BlockType == type) {
			blocks[i].IsHidden = 0xFFFF;
		}		
	}
}