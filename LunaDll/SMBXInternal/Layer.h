//*** Layer.h - Definition of known Layer structures and Layer-related framework functions ***
#ifndef layer__cpp
#define layer__cpp

#include "../Defines.h"
#include "../Misc/VB6StrPtr.h"
#include "BaseItemArray.h"

#pragma pack(push, 4)
struct LayerControl : SMBX_StaticBaseItemArray<LayerControl, 100, GM_LAYER_ARRAY_PTR_CONSTPTR> {
    short       IsStopped;		// 0x00 0xFFFF when moving, event ended?
    short       Unknown1;       // 0x02
    VB6StrPtr	ptLayerName;	// 0x04 ptr to double zero terminated 2byte wide char string
    short       isHidden;       // 0x08
    short       unknown;        // 0x0A

	float		xSpeed;         // 0x0C
	float		ySpeed;         // 0x10
};
#pragma pack(pop)

/* Verify struct is correctly sized */
#ifndef __INTELLISENSE__
static_assert(sizeof(LayerControl) == 0x14, "sizeof(LayerControl) must be 0x14");
#endif

namespace Layer{

	/// Functions ///
	LayerControl* Get(int LayerIndex);

	void Stop(LayerControl* layercontrol);

	void SetXSpeed(LayerControl* layercontrol, float xSpeed);
	void SetYSpeed(LayerControl* layercontrol, float ySpeed);
}

#endif
