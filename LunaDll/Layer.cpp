#include <windows.h>
#include "Layer.h"
#include "Defines.h"

// Get a pointer to a layer based on index
LayerControl* Layer::Get(int index) {
	if(index > 100 && index >= 0)
		return 0;

	LayerControl* layers = (LayerControl*)GM_LAYER_ARRAY_PTR;
	return &(layers[index]);
}

// Stop a layer
void Layer::Stop(LayerControl* layer) {
	layer->IsStopped = (short)0;
	layer->xSpeed = 0.0001f;
	layer->ySpeed = 0.0001f;
}

void Layer::SetYSpeed(LayerControl* layer, float setY) {
	setY = (setY == 0 ?  0.0001f : setY);
	layer->ySpeed = setY;
	layer->IsStopped = (short)0xFFFF;
}

void Layer::SetXSpeed(LayerControl* layer, float setX) {
	setX = (setX == 0 ?  0.0001f : setX);
	layer->xSpeed = setX;
	layer->IsStopped = (short)0xFFFF;
}

