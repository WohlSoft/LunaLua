#include "Playground.h"
#include "../Defines.h"
#include "../MOBs/PlayerMOB.h"

//Here you can add random crap if you want to test something
#if COMPILE_PLAYGROUND

int localCounter = 0;
void Playground::doPlaygroundStuff()
{
    localCounter++;
    //1. Arg score factor
    //2. Arg Momentum struct
    //3. Unknown

    typedef void scoreFunc(short*, Momentum*, short*);
    scoreFunc* theScoreFunc = (scoreFunc*)GF_SCORE_RELEATED;
    if (localCounter % 100 == 0){
        Momentum customMomentum;
        ::PlayerMOB* pl = ::Player::Get(1);
        memcpy((void*)&customMomentum, (void*)&pl->momentum, sizeof(Momentum));
        customMomentum.y -= 64;

        short tmp1 = 0;
        short scoreFactor = 10;
        theScoreFunc(&scoreFactor, &customMomentum, &tmp1);

    }

    typedef void spawnEffect(short* id, Momentum* theMomentum, float* animationFrame, short* unknown1, short* unknown2);
    spawnEffect* theSpawnEffectFunc = (spawnEffect*)GF_RUN_ANIM;
    if (localCounter % 100 == 0){
        Momentum customMomentum;
        ::PlayerMOB* pl = ::Player::Get(1);
        memcpy((void*)&customMomentum, (void*)&pl->momentum, sizeof(Momentum));
        customMomentum.y -= 100;
        customMomentum.x += 100;

        short the_id = 0x38;
        float the_animationFrame = 5.0f;
        short the_unknown1 = 20;
        short the_unknown2 = -1;
        theSpawnEffectFunc(&the_id, &customMomentum, &the_animationFrame, &the_unknown1, &the_unknown2);
    }
}
#endif