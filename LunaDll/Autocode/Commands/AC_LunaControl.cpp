// Child for autocode.cpp
#include "../Autocode.h"
#include "../../Globals.h"

void Autocode::LunaControl(LunaControlAct act, int val) {
	switch(act) {

	case LCA_DemoCounter:
		if(val == 1)
			gShowDemoCounter = true;
		else
			gShowDemoCounter = false;
		break;

	case LCA_SMBXHUD:
		if(val == 1)
			gSkipSMBXHUD = true;
		else
			gSkipSMBXHUD = false;
		break;
	
	case LCA_Invalid:
	default:
		return;
	}
}