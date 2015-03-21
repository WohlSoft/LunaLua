#include "LuaProxy.h"
#include "LuaHelper.h"
#include "../Rendering/Rendering.h"
#include "../Globals.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../SMBXInternal/NPCs.h"
#include "../Misc/MiscFuncs.h"
#include "../SMBXInternal/SMBXEvents.h"
#include "../SMBXInternal/Sound.h"
#include "../SMBXInternal/Level.h"
#include "../CustomSprites/CSprite.h"
#include "../CustomSprites/CSpriteManager.h"
#include "../SMBXInternal/Blocks.h"
#include "../SMBXInternal/Layer.h"
#include "../SMBXInternal/Animation.h"
#include "../SMBXInternal/Overworld.h"
#include "../SMBXInternal/WorldLevel.h"

#include "../SdlMusic/SdlMusPlayer.h"
#include "../GlobalFuncs.h"

/*
 * The LuaProxy codes are split in LuaProxyComponenet.
 * This file should only used temporary.
 */

