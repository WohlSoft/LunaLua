#include "Playground.h"
#include "../Defines.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../Misc/VB6StrPtr.h"

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

    if (localCounter % 100 == 0){
        VB6StrPtr defLayer = "Default";
        short smoke = 0;
        native_hideLayer(&defLayer, &smoke);
    }
}
#endif