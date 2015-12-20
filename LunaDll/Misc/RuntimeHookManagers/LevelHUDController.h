#ifndef LevelHUDController_hhhhh
#define LevelHUDController_hhhhh

#include <utility>
#include <map>
#include "../../Defines.h"
#include "../../Misc/AsmPatch.h"

class LevelHUDController
{
    std::map<LEVEL_HUD_CONTROL, Patchable*> m_patching;
public:
    LevelHUDController();
    LevelHUDController(const LevelHUDController& other) = delete;
    
};
#endif



