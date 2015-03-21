#include "Animation.h"
#include "../Defines.h"

SMBXAnimation *Animations::Get(int AnimationIndex)
{
    if(AnimationIndex < 0 || AnimationIndex > GM_ANIM_COUNT)
        return 0;

    SMBXAnimation* animations = (SMBXAnimation*)GM_ANIM_PTR;
    return &(animations[AnimationIndex]);
}
