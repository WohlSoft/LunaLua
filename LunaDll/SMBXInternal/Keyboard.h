#ifndef Keyboard_hhhh
#define Keyboard_hhhh

// Vkey table reference
// 0 - Up
// 1 - Down
// 2 - Left
// 3 - Right
// 4 - Run
// 5 - Alt run
// 6 - Jump
// 7 - Alt jump
// 8 - Drop item
// 9 - Pause

struct SMBXNativeKeyboard{
	short up;
	short down;
	short left;
	short right;
	short run;
	short altrun;
	short jump;
	short altjump;
	short dropitem;
	short pause;
};

namespace Input {
	void PressUp();
	void PressSJ();
	SMBXNativeKeyboard* getNativeInput();
}

#endif
