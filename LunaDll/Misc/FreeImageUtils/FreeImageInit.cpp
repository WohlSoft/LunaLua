#include "FreeImageInit.h"
#include <FreeImage.h>
#include <iostream>

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
    std::cout << "[FreeImage Error] ";
    if (fif != FIF_UNKNOWN) {
        std::cout << "[" << FreeImage_GetFormatFromFIF(fif) << "] ";
    }
    else{
        std::cout << "[General] ";
    }
    std::cout << message << std::endl;
}

FreeImageInit::FreeImageInit()
{}

void FreeImageInit::init()
{
    if (m_isInit)
        return;

    FreeImage_Initialise();
    FreeImage_SetOutputMessage(FreeImageErrorHandler);
}

FreeImageInit::~FreeImageInit()
{
    FreeImage_DeInitialise();
}


