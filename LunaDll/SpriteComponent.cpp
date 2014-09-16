#include "SpriteComponent.h"

void SpriteComponent::Tick() {
	if(org_time == 0) { // endless component
		return;
	}	
	run_time--;			// normal behavior	
	if(run_time == 0) { // time to delete
		expired = true;		
	}
}