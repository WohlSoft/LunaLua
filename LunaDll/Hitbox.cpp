#include "Hitbox.h"
#include "CSprite.h"
#include "Globals.h"
#include "MiscFuncs.h"

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

			if(false) { // debug
				Render::Print(to_wstring((long long)CalcLeft()), 3, 300,420);
				Render::Print(to_wstring((long long)CalcTop()), 3, 300,440);
				Render::Print(to_wstring((long long)CalcRight()), 3, 300,460);
				Render::Print(to_wstring((long long)CalcBottom()), 3, 300,480);
				Render::Print(to_wstring((long long)Left2), 3, 300,500);
				Render::Print(to_wstring((long long)Up2), 3, 300,520);
				Render::Print(to_wstring((long long)Left2 + W), 3, 300,540);
				Render::Print(to_wstring((long long)Up2 + H), 3, 300,560);
				Render::Print(BoolToString(leftcol), 3, 300,580);
				Render::Print(BoolToString(upcol), 3, 410,580);
				Render::Print(BoolToString(rightcol), 3, 520,580);
				Render::Print(BoolToString(downcol), 3, 630,580);
			}

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