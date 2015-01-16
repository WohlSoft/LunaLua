#include "WorldLevel.h"
#include "../Defines.h"


WorldLevel* SMBXLevel::get(int index)
{
	if(index < 0 || index > 400)
		return 0;

	return &(((WorldLevel*)GM_LEVEL_BASE)[index]);
}
