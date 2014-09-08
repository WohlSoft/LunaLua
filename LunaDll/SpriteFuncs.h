#pragma once

#include "CSprite.h"

void Activate(int code, CSprite* spr);	// Formally add the components specified by "code" to sprite behaviors

namespace SpriteFunc {	

	/////////////////////
	/// Behavior funcs///
	/////////////////////


	// Collision funcs
	void OnPlayerCollide(CSprite* me, SpriteComponent* obj);		// Activate given behavior when colliding with player
	void OnPlayerDistance(CSprite* me, SpriteComponent* obj);
	void PlayerCollectible(CSprite* me, SpriteComponent* obj);		// Calls Die() if detecting collision with player	

	// Wait-on-condition funcs
	void WaitForPlayer(CSprite* me, SpriteComponent* obj);			// Activate linked component on a player memory condition
	void PlayerHoldingSprite(CSprite* me, SpriteComponent* obj);	// Activate linked component when player holding certain sprite

	// Speed funcs
	void Deccelerate(CSprite* me, SpriteComponent* obj);		// Subtract sprite's speed towards minimum for each active frame
	void Accelerate(CSprite* me, SpriteComponent* obj);			// Add to sprite's speed towards maximum for each active frame
	void AccelToPlayer(CSprite* me, SpriteComponent* obj);		// Push towards player for each active frame
	void TeleportNearPlayer(CSprite* me, SpriteComponent* obj);	// Direct set x/y position to somewhere in a circle around the player

	// Move funcs
	void PhaseMove(CSprite* me, SpriteComponent* obj);			// Move directly according to speed, ignoring collisions	
	void BumpMove(CSprite* me, SpriteComponent* obj);			// Move according to speed, bump into and reverse speed when colliding

	// Set funcs
	void SetXSpeed(CSprite* me, SpriteComponent* obj);
	void SetYSpeed(CSprite* me, SpriteComponent* obj);
	void SetHitbox(CSprite* me, SpriteComponent* obj);

	// Action funcs
	void TriggerLunaEvent(CSprite* me, SpriteComponent* obj);	// Trigger a lunadll eventAT_BumpMove
	void HarmPlayer(CSprite* me, SpriteComponent* obj);

	// Animation funcs
	void BasicAnimate(CSprite* me, SpriteComponent* obj);		// Simple animation func (moves drawing rect down the spritesheet)


	//////////////////
	/// Draw funcs ///
	//////////////////

	void StaticDraw(CSprite* me);		// Draw sprite to absolute position on the screen
	void RelativeDraw(CSprite* me);		// Draw sprite inside level, relative to camera position
}