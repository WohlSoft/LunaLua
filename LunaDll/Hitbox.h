#pragma once

#include <stddef.h>

class CSprite;

enum COLLISION_TYPE {
	COLTYPE_NONE = 0,
	COLTYPE_LEFT,
	COLTYPE_RIGHT,
	COLTYPE_TOP,
	COLTYPE_BOT,
};

struct Hitbox {
	Hitbox() : Left_off(0), Top_off(0), W(0), H(0), CollisionType(0), pParent(NULL) {  };

	double CalcLeft();		// Get the left absolute position (with parent base coords)
	double CalcRight();		// Get the right absolute position (with parent base coords)
	double CalcTop();		// Get the top absolute position (with parent base coords)
	double CalcBottom();	// Get the bottom absolute position (with parent base coords)
	double CenterX();		// Get the center X position (from parent base coords)
	double CenterY();		// Get the center Y position (from parent base coords)

	bool Test(int left, int up, int width, int height);	// Test hitbox against given rect
	bool Test(int cx, int cy, int radius);				// Test hitbox against given circle

	//COLLISION_TYPE GetCollisionDir(int left, int up, int right, int down); // Get the direction this hitbox is colliding with given rect
	
	CSprite* pParent;	// Pointer to parent sprite (for calculating actual coordinates + collision area)
	short Left_off;			// Offset from 0,0 on sprite
	short Top_off;			// Offset from 0,0 on sprite
	short W;				// Width
	short H;				// Height
	char CollisionType;	// TODO
};