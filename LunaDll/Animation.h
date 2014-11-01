#pragma once

#pragma pack(push, 4)
struct SMBXAnimation{
    short AnimationID;
    short Unknown02;
    double XPos;
    double YPos;
    double Height;
    double Width;
    double XSpeed;
    double YSpeed;
    short Unknown34; //Subtimer?
    short Unknown36;
    short Unknown38;
    short Unknown3A;
    short Timer;
    short Unknown3E;
    short Unknown40;
};
#pragma pack(pop)


namespace Animations {

    SMBXAnimation* Get(int AnimationIndex);

}
