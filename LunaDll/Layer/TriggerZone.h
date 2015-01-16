#ifndef TriggerZone_hhh
#define TriggerZone_hhh

struct TriggerZone {
	// The trigger zone boundaries
	double L;
	double R;
	double U;
	double D;
	int Section;

	// Settings
	bool PlayersOnly;	// Only players can trigger the event
	bool OnceOnly;		// The event only triggers once

private:
	bool IsValid;

	// Quick Cunstructor
	TriggerZone() {
		L = 0;
		R = 0;
		U = 0;
		D = 0;
		Section = 0;
		PlayersOnly = true;
		OnceOnly = true;
		IsValid = true;
	}

};
#endif
