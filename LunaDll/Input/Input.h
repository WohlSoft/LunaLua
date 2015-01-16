// Input.h -- Easily accesible functions for the input of the main player
#ifndef Input_hhhhh
#define Input_hhhhh

#include "../Globals.h"
#include "../MOBs/PlayerMOB.h"

namespace Input {

	void CheckSpecialCheats();

	void UpdateInputTasks();

	void ResetTaps();
	void ResetAll();
	void UpdateKeyRecords(PlayerMOB* player);

	std::wstring GetInputStringCopy();
	void ClearInputStringBuffer();

	bool PressingUp();
	bool PressingDown(); 
	bool PressingLeft(); 
	bool PressingRight(); 
	bool PressingRun(); 
	bool PressingJump();

	bool UpThisFrame(); 
	bool DownThisFrame(); 
	bool LeftThisFrame();
	bool RightThisFrame(); 
	bool RunThisFrame(); 
	bool JumpThisFrame(); 
}

#endif
