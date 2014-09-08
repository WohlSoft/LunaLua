#include "SpriteComponent.h"

void SpriteComponent::Tick() {
	if(org_time == 0) { // endless component
		return;
	}
	else if(run_time == 1) { // time to delete
		expired = true;		
	}
	run_time--;			// normal behavior	
}