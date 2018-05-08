// Child for autocode.cpp
#include "../Autocode.h"
#include "../../Globals.h"

void Autocode::LunaControl(LunaControlAct act, int val) {
	switch(act) {

	case LCA_DemoCounter:
		break;

	case LCA_SMBXHUD:
		if(val == 1)
			gSMBXHUDSettings.skip = true;
		else
			gSMBXHUDSettings.skip = false;
		break;
	
	case LCA_Invalid:
	default:
		return;
	}
}