#pragma once
#include "Globals.h"
#include "Defines.h"

namespace Level {

	double GetBoundary(int section, int which_boundary_UDLR);
	void GetBoundary(RECT* rectangle, int section);
	void PushSectionBoundary(int section, int which_boundary_UDLR, double push_val);
	void SetSectionBounds(int section, double left_bound, double top_bound, double right_bound, double bot_bound);

	std::wstring GetName();
}