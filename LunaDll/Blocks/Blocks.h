#ifndef Blocks_Hhhhh
#define Blocks_Hhhhh

#include "../Defines.h"
#include "../Globals.h"
#include "../MOBs/PlayerMOB.h"
#include "../Misc/VB6StrPtr.h"

#pragma pack(push, 4)
struct Block {

	short				Slippery; //0xFFFF = yes
	short				Unknown02;

	short				Unknown04;
	short				Unknown06;
	int					Unknown08;

	wchar_t*			pUnknown0C;
	wchar_t*			pUnknown10;
	wchar_t*			pUnknown14;
	VB6StrPtr			pLayerName;		// 0x18	

	short				IsInvisible;	// 0x1C	
	short				BlockType;		// 0x1E	

	double				XPos;			// 0x20	
	double				YPos;			// 0x28	
	double				H;				// 0x30	
	double				W;				// 0x38	
	double				XSpeed;			// 0x40	
	double				YSpeed;			// 0x48	

	short				ContentsID;     // 0x50 (todo: verify!)
	short				BeingHitStatus1;// 0x52 (todo: verify!)
	short				BeingHitTimer;	// 0x54 (todo: verify!)
	short				BeingHitStatus2;// 0x56 (todo: verify!)

	short				Unknown58;
	short				Unknown5A;
	unsigned short		IsHidden;		// 0x5C (todo: verify!)
	short				Unknown5E;
	short				Unknown60;

	short				Unknown62;
	short				Unknown64;
	short				Unknown66;
};
#pragma pack(pop)

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
