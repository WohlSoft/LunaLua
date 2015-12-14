#ifndef LevelHUDController_hhhhh
#define LevelHUDController_hhhhh

#include "../../Defines.h"
#include <utility>
#include <map>
#include "../../Misc/AsmPatch.h"

class LevelHUDController
{
    std::map<LEVEL_HUD_CONTROL, Patchable*> m_patching;
public:
    LevelHUDController();
    LevelHUDController(const LevelHUDController& other) = delete;
    
};
#endif



