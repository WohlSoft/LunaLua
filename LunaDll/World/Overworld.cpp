#include "Overworld.h"
#include "../Defines.h"

Overworld* SMBXOverworld::get()
{
	return (Overworld*)GM_OVERWORLD_PTR;
}
