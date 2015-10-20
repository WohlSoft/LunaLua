#include "Hitbox.h"
#include "../CSprite.h"
#include "../../Globals.h"
#include "../../Misc/MiscFuncs.h"

// TEST -- fast rectangle
bool Hitbox::Test(int Left2, int Up2, int _W, int _H) {
	if(pParent != NULL) {
		if(this->CollisionType == 0) { // square aabb collision detection
			bool rightcol = true;
			bool leftcol = true;
			bool upcol = true;
			bool downcol = true;

			if(CalcRight() < Left2)
				return false;
			if(CalcLeft() > Left2 + _W)
				return false;
			if(CalcTop() > Up2 + _H)
				return false;
			if(CalcBottom() < Up2)
				return false;

			if(rightcol == false || leftcol == false || upcol == false || downcol == false)
				return false;
			return true;
		}
		else { // circle vs aaab -> convert to circle
			double halfwidth = _W / 2;
			double halfheight = _H / 2;
			return Test((int)(Left2 + halfwidth), (int)(Up2 + halfheight), (int)(halfwidth <= halfheight ? halfwidth : halfheight));
		}
	}
	return false;
}

// TEST -- fast circle/distance
bool Hitbox::Test(int cx, int cy, int radius) {
	int radi_total = radius + (W <= H ? W / 2 : H / 2); // Other obj radius + my radius
	radi_total *= radi_total;
	int x_dist = (int)CenterX() - cx;
	int y_dist = (int)CenterY() - cy;
	int sqr_dist = x_dist*x_dist + y_dist*y_dist;

	if(sqr_dist > radi_total)
		return false;
	return true;
}

double Hitbox::CalcLeft() {
	if(pParent)
		return pParent->m_Xpos + Left_off;
	return 0;
}

double Hitbox::CalcTop() {
	if(pParent)
		return pParent->m_Ypos + Top_off;
	return 0;
}

double Hitbox::CalcBottom() {
	if(pParent)
		return pParent->m_Ypos + H;
	return 0;
}

double Hitbox::CalcRight() {
	if(pParent)
		return pParent->m_Xpos + W;
	return 0;
}

double Hitbox::CenterX() {
	return CalcLeft() + (W / 2);	
}

double Hitbox::CenterY() {
	return CalcTop() + (H / 2);
}