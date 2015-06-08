#ifndef Blocks_Hhhhh
#define Blocks_Hhhhh

#include "../Defines.h"
#include "../Globals.h"
#include "PlayerMOB.h"
#include "../Misc/VB6StrPtr.h"

#pragma pack(push, 4)
struct Block {

    short               Slippery;                           // 0x00 0xFFFF = yes
    short               Unknown02;                          // 0x02

    short               Unknown04;                          // 0x04
    short               BlockType2;                         // 0x06
    short               ContentIDRelated;                   // 0x08
    short               Unknwon10;                          // 0x0A

    VB6StrPtr           pHitEventName;                      // 0x0C
    VB6StrPtr           pDestroyEventName;                  // 0x10
    VB6StrPtr           pNoMoreObjInLayerEventName;         // 0x14
    VB6StrPtr           pLayerName;                         // 0x18

    short               IsHidden;                           // 0x1C
    short               BlockType;                          // 0x1E

    Momentum            mometum;                            // 0x20

    short               ContentsID;                         // 0x50
    short               BeingHitStatus1;                    // 0x52
    short               BeingHitTimer;                      // 0x54
    short               BeingHitStatus2;                    // 0x56

    short               Unknown58;                          // 0x58
    short               IsInvisible2;                       // 0x5A
    unsigned short      IsInvisible3;                       // 0x5C (todo: verify!)
    short               Unknown5E;                          // 0x5E
    short               Unknown60;                          // 0x60

    short               Unknown62;                          // 0x62
    short               Unknown64;                          // 0x64
    short               Unknown66;                          // 0x66

    // Note, 0-base indexed in SMBX code, and keep 0-base indexed here
    static inline ::Block* Get(unsigned short index) {
        if (index >= GM_BLOCK_COUNT) return NULL;
        return &((Block*)GM_BLOCKS_PTR)[index];
    }

    static inline unsigned short Count() {
        return GM_BLOCK_COUNT;
    }

    static const short MAX_ID = 638;
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(Block) == 0x68, "sizeof(Block) must be 0x68");
#endif

namespace Blocks {

	Block* Get(int index);			// Get ptr to a block
	inline Block* GetBase();		// Get base of block array
	inline short Count();			// Get block count
	int TestCollision(PlayerMOB* pMobPOS, Block* pBlockPOS);

	void SetAll(int type1, int type2);  // Set ID of all blocks of type 1 to type 2
	void SwapAll(int type1, int type2); // Swap ID of all blocks of type 1 to type 2, and vice versa
	void ShowAll(int type);				// Show all blocks of type
	void HideAll(int type);				// Hide all blocks of type

	bool IsPlayerTouchingType(int BlockType, int sought_collision, PlayerMOB* pMobPOS); // See if player touching block of BlockType
}



 /// Inlines ///
inline Block* Blocks::GetBase() { return (Block*)GM_BLOCKS_PTR; }

inline short Blocks::Count() { return GM_BLOCK_COUNT; }

#endif
