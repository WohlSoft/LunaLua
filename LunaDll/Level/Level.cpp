#include "Level.h"

void Level::PushSectionBoundary(int section, int which_boundary_UDLR, double push_val) {
	double* boundtable = (double*)GM_LVL_BOUNDARIES;
	boundtable = boundtable + (section * 6);

	switch(which_boundary_UDLR) {
	case 0:		// U
		boundtable[1] += push_val;
		break;

	case 1:		// D
		boundtable[2] += push_val;
		break;

	case 2:		// L
		boundtable[0] += push_val;
		break;

	case 3:		// R
		boundtable[3] += push_val;
		break;
	}
}

void Level::SetSectionBounds(int section, double left_bound, double top_bound, double right_bound, double bot_bound) {
	double* boundtable = (double*)GM_LVL_BOUNDARIES;
	boundtable = boundtable + (section * 6);
	boundtable[0] = left_bound;
	boundtable[1] = top_bound;
	boundtable[2] = bot_bound;
	boundtable[3] = right_bound;
}

double Level::GetBoundary(int section, int which_boundary_UDLR) {
	double* boundtable = (double*)GM_LVL_BOUNDARIES;
	boundtable = boundtable + (section * 6);

	switch(which_boundary_UDLR) {
	case 0:		// U
		return boundtable[1];
		break;

	case 1:		// D
		return boundtable[2];
		break;

	case 2:		// L
		return boundtable[0];
		break;

	case 3:		// R
		return boundtable[3];
		break;
	}
	return 0;
}

void Level::GetBoundary(RECT* rect, int section) {
	double* boundtable = (double*)GM_LVL_BOUNDARIES;
	boundtable = boundtable + (section * 6);

	if(rect != NULL) {
		rect->top = (LONG)boundtable[1];
		rect->bottom = (LONG)boundtable[2];
		rect->left = (LONG)boundtable[0];
		rect->right = (LONG)boundtable[3];		
	}
}

std::wstring Level::GetName() {
	if(GM_LVLNAME_PTR == 0) return 0;
	return std::wstring((wchar_t*)GM_LVLNAME_PTR);
}