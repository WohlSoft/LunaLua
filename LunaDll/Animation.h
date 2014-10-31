#pragma once

#pragma pack(push, 4)
struct SMBXAnimation{
    short animationID;
    short unknown02;
    double XPos;
    double YPos;
    double Height;
    double Width;
    double XSpeed;
    double YSpeed;
    short unknown34;
    short unknown36;
    short unknown38;
    short unknown3A;
    short timer;
    short unknown3E;
    short unknown40;
};
#pragma pack(pop)


namespace Animations {

    SMBXAnimation* Get(int AnimationIndex);

}
