#include "Level.h"

void Level::PushSectionBoundary(int section, int which_boundary_UDLR, double push_val) {
    Bounds* boundarray = (Bounds*)GM_LVL_BOUNDARIES;

	switch(which_boundary_UDLR) {
	case 0:		// U
        boundarray[section].top += push_val;
		break;

	case 1:		// D
        boundarray[section].bottom += push_val;
		break;

	case 2:		// L
        boundarray[section].left += push_val;
		break;

	case 3:		// R
        boundarray[section].right += push_val;
		break;
	}
}

void Level::SetSectionBounds(int section, double left_bound, double top_bound, double right_bound, double bot_bound) {
    Bounds* boundarray = (Bounds*)GM_LVL_BOUNDARIES;
    boundarray[section].left = left_bound;
    boundarray[section].top = top_bound;
    boundarray[section].bottom = bot_bound;
    boundarray[section].right = right_bound;
}

double Level::GetBoundary(int section, int which_boundary_UDLR) {
    Bounds* boundarray = (Bounds*)GM_LVL_BOUNDARIES;

	switch(which_boundary_UDLR) {
	case 0:		// U
        return boundarray[section].top;
		break;

	case 1:		// D
        return boundarray[section].bottom;
		break;

	case 2:		// L
        return boundarray[section].left;
		break;

	case 3:		// R
        return boundarray[section].right;
		break;
	}
	return 0;
}

void Level::GetBoundary(RECT* rect, int section) {
    Bounds* boundarray = (Bounds*)GM_LVL_BOUNDARIES;

	if(rect != NULL) {
        rect->top = (LONG)boundarray->top;
        rect->bottom = (LONG)boundarray->bottom;
        rect->left = (LONG)boundarray->left;
        rect->right = (LONG)boundarray->right;
	}
}

std::wstring Level::GetName() {
	return (std::wstring)GM_LVLFILENAME_PTR;
}