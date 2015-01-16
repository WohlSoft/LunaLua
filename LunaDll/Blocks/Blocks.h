#ifndef Blocks_Hhhhh
#define Blocks_Hhhhh

#include "../Defines.h"
#include "../Globals.h"
#include "../MOBs/PlayerMOB.h"

struct Block {

	short				Slippery; //0xFFFF = yes
	short				Unknown02;

	short				Unknown04;
	short				Unknown06;
	int					Unknown08;

	wchar_t*			pUnknown0C;
	wchar_t*			pUnknown10;
	wchar_t*			pUnknown14;
	wchar_t*			pLayerName;		// 0x18	

	short				IsInvisible;	// 0x1C	
	short				BlockType;		// 0x1E	

	double				XPos;			// 0x20	
	double				YPos;			// 0x28	
	double				H;				// 0x30	
	double				W;				// 0x38	
	double				XSpeed;			// 0x40	
	double				YSpeed;			// 0x48	

	short				ContentsID;
	short				BeingHitStatus1;	//50
	short				BeingHitTimer;		//52
	short				BeingHitStatus2;	//54

	short				Unknown56;			
	short				Unknown58;	
	unsigned short		IsHidden;			//5A
	short				Unknown5C;
	short				Unknown5E;

	//int				Unknown60;
	//int				Unknown64;

};

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
