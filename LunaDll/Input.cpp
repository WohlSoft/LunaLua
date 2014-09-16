#include "Input.h"
#include "Sound.h"
#include "DeathCounter.h"
#include "Autocode.h"

using namespace std;

#define FULL_LUNA_TOGGLE_CHT L"thouartdamned"
#define TOGGLE_DEMO_COUNTER_CHT L"toggledemocounter"
#define DELETE_ALL_RECORDS_CHT L"formatcdrive"
#define LUNA_DEBUG_CHT L"lunadebug"
#define DUMP_DEATH_COUNT_FILE L"countdemos"

// CHECK SPECIAL CHEATS -- Check for special lunadll management strings in the buffer
void Input::CheckSpecialCheats() {	
	wstring curbuf = Input::GetInputStringCopy();

	// thouartdamned
	int org_len = curbuf.length();
	int sought_len = wcslen(FULL_LUNA_TOGGLE_CHT);
	if(org_len >= sought_len && curbuf.substr(org_len - sought_len, sought_len) == FULL_LUNA_TOGGLE_CHT) {
		if(gLunaEnabled)
			gLunaEnabled = 0;
		else
			gLunaEnabled = 1;
		SMBXSound::PlaySFX(36);
		ClearInputStringBuffer();
		return;
	}

	// toggledemocounter
	org_len = curbuf.length();
	sought_len = wcslen(TOGGLE_DEMO_COUNTER_CHT);
	if(org_len >= sought_len && curbuf.substr(org_len - sought_len, sought_len) == TOGGLE_DEMO_COUNTER_CHT) {
		if(gShowDemoCounter)
			gShowDemoCounter = 0;
		else
			gShowDemoCounter = 1;
		SMBXSound::PlaySFX(36);
		ClearInputStringBuffer();
		return;
	}

	// formatcdrive
	org_len = curbuf.length();
	sought_len = wcslen(DELETE_ALL_RECORDS_CHT);
	if(org_len >= sought_len && curbuf.substr(org_len - sought_len, sought_len) == DELETE_ALL_RECORDS_CHT) {
		gDeathCounter.ClearRecords();
		gDeathCounter.TrySave();
		gDeathCounter.Recount();
		SMBXSound::PlaySFX(36);
		ClearInputStringBuffer();
		return;
	}

	//lunadebug
	org_len = curbuf.length();
	sought_len = wcslen(LUNA_DEBUG_CHT);
	if(org_len >= sought_len && curbuf.substr(org_len - sought_len, sought_len) == LUNA_DEBUG_CHT) {
		std::wstring none = L"__null";
		Autocode* ac = new Autocode(AT_DebugPrint, 0, 0, 0, 0, none, 600, 0, none);
		gAutoMan.m_CustomCodes.push_back(ac);
		SMBXSound::PlaySFX(14);
		ClearInputStringBuffer();
		return;
	}

	//death count dump
	org_len = curbuf.length();
	sought_len = wcslen(DUMP_DEATH_COUNT_FILE);
	if(org_len >= sought_len && curbuf.substr(org_len - sought_len, sought_len) == DUMP_DEATH_COUNT_FILE) {
		gDeathCounter.DumpAllToFile(L"democount.txt");
		SMBXSound::PlaySFX(15);
		ClearInputStringBuffer();
		return;
	}
}

// UPDATE INPUT TASKS -- Update key presses, etc
void Input::UpdateInputTasks() {
	wchar_t* dbg = L"Input tasks debug";
	ResetTaps();
	UpdateKeyRecords(Player::Get(1));
}

// RESET TAPS -- Reset the tapped key states info
void Input::ResetTaps() {
	gDownTapped = 0;
	gUpTapped = 0;
	gLeftTapped = 0;
	gRightTapped = 0;
	gRunTapped = 0;
	gJumpTapped = 0;
}

// RESET ALL -- Reset taps, presses, etc
void Input::ResetAll() {
	gLastDownPress = 0;
	gDownTapped = 0;
	gLastUpPress = 0;
	gUpTapped = 0;
	gLastLeftPress = 0;
	gLeftTapped = 0;
	gLastRightPress = 0;
	gRightTapped = 0;
	gLastJumpPress = 0;
	gJumpTapped = 0;
	gLastRunPress = 0;
	gRunTapped = 0;
}

// UPDATE KEY RECORDS -- Update the global key press info
void Input::UpdateKeyRecords(PlayerMOB* pPlayer) {
	if(pPlayer == 0)
		return;

	wchar_t* dbg = L"Update keys debug";

	// Up
	if(Player::PressingUp(pPlayer)) {
		// If still holding from last frame
		if(gFrames - 1 == gLastUpPress) {
			gLastUpPress = gFrames;
		}
		else { //else set tapped this frame
			gLastUpPress = gFrames;
			gUpTapped = gFrames;
		}
	}

	// Down
	if(Player::PressingDown(pPlayer)) {
		// If still holding from last frame
		if(gFrames - 1 == gLastDownPress) {
			gLastDownPress = gFrames;
		}
		else { //else set tapped this frame
			gLastDownPress = gFrames;
			gDownTapped = gFrames;
		}
	}

	// Left
	if(Player::PressingLeft(pPlayer)) {
		// If still holding from last frame
		if(gFrames - 1 == gLastLeftPress) {
			gLastLeftPress = gFrames;
		}
		else { //else set tapped this frame
			gLastLeftPress = gFrames;
			gLeftTapped = gFrames;
		}
	}

	// Right
	if(Player::PressingRight(pPlayer)) {
		// If still holding from last frame
		if(gFrames - 1 == gLastRightPress) {
			gLastRightPress = gFrames;
		}
		else { //else set tapped this frame
			gLastRightPress = gFrames;
			gRightTapped = gFrames;
		}
	}

	// Jump
	if(Player::PressingJump(pPlayer)) {
		// If still holding from last frame
		if(gFrames - 1 == gLastJumpPress) {
			gLastJumpPress = gFrames;
		}
		else { //else set tapped this frame
			gLastJumpPress = gFrames;
			gJumpTapped = gFrames;
		}
	}

	// Run
	if(Player::PressingRun(pPlayer)) {
		// If still holding from last frame
		if(gFrames - 1 == gLastRunPress) {
			gLastRunPress = gFrames;
		}
		else { //else set tapped this frame
			gLastRunPress = gFrames;
			gRunTapped = gFrames;
		}
	}
}

bool Input::PressingUp() { return gLastUpPress == gFrames ? true : false; }
bool Input::PressingDown() { return gLastDownPress == gFrames ? true : false; }
bool Input::PressingLeft() { return gLastLeftPress == gFrames ? true : false; }
bool Input::PressingRight() { return gLastRightPress == gFrames ? true : false; }
bool Input::PressingRun() { return gLastRunPress == gFrames ? true : false; }
bool Input::PressingJump() { return gLastJumpPress == gFrames ? true : false; }

bool Input::UpThisFrame() { return gUpTapped != 0 ? true : false; }
bool Input::DownThisFrame() { return gDownTapped != 0 ? true : false; }
bool Input::LeftThisFrame() { return gLeftTapped != 0 ? true : false; }
bool Input::RightThisFrame() { return gRightTapped != 0 ? true : false; }
bool Input::RunThisFrame() { return gRunTapped != 0 ? true : false; }
bool Input::JumpThisFrame() { return gJumpTapped != 0 ? true : false; }

// GET INPUT STRING COPY -- Get a copy of the current state of the input string buffer (for cheats)
wstring Input::GetInputStringCopy() {
	if(GM_INPUTSTR_BUF_PTR == 0)
		return wstring(L"");
	return wstring((wchar_t*)GM_INPUTSTR_BUF_PTR);
}

// CLEAR INPUT STRING BUFFER -- 
void Input::ClearInputStringBuffer() {
	int len = GetInputStringCopy().length();
	if(GM_INPUTSTR_BUF_PTR != 0) {
		*(((short*)GM_INPUTSTR_BUF_PTR) - 2) = (int)0; // zero out VBA string len
		*(short*)GM_INPUTSTR_BUF_PTR = (short)0; // null out first character
	}
}