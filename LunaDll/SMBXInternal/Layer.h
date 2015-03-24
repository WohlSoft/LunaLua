//*** Layer.h - Definition of known Layer structures and Layer-related framework functions ***
#ifndef layer__cpp
#define layer__cpp

#include "../Defines.h"
#include "../Globals.h"
#include "../Misc/VB6StrPtr.h"

#pragma pack(push, 4)
struct LayerControl {
	short		IsStopped;		// 0xFFFF when moving, event ended?
	short		Unknown1;
	VB6StrPtr	ptLayerName;	// ptr to double zero terminated 2byte wide char string
	int			Unknown2;       // Have a hunch +0x08 is a 16-bit value representing layer visibility, need to verify.

	float		xSpeed;
	float		ySpeed;
};
#pragma pack(pop)

/* Verify struct is correctly sized */
static_assert(sizeof(LayerControl) == 0x14, "sizeof(LayerControl) must be 0x14");

namespace Layer{

	/// Functions ///
	LayerControl* Get(int LayerIndex);

	void Stop(LayerControl* layercontrol);

	void SetXSpeed(LayerControl* layercontrol, float xSpeed);
	void SetYSpeed(LayerControl* layercontrol, float ySpeed);
}

#endif
