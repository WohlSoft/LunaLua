#include "../Keyboard/Keyboard.h"
#include "../Defines.h"



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

void Input::PressUp() {
	return;

	BYTE keybuf[256];
	GetKeyboardState(keybuf);

	short* pVKeys = (short*)GM_VKEY_TABLE_PTR;
	short vkey = pVKeys[0];

}

void Input::PressSJ() {
}