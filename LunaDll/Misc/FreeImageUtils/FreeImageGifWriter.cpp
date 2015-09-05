#include "FreeImageGifWriter.h"
#include <iostream>


FreeImageGifWriter::FreeImageGifWriter(const std::string& filename) :
    m_filename(filename),
    m_gifHandle(nullptr)
{
    std::cout << "Create new gif: " << filename << std::endl;
    m_gifHandle = FreeImage_OpenMultiBitmap(FIF_GIF, filename.c_str(), true, false);
    std::cout << "Gif handle ptr: " << (int)m_gifHandle << std::endl;
}


FreeImageGifWriter::~FreeImageGifWriter()
{
    closeAndCleanup();
}

void FreeImageGifWriter::closeAndCleanup()
{
    std::cout << "Close gif" << std::endl;
    if (m_gifHandle){
        FreeImage_CloseMultiBitmap(m_gifHandle);
        m_gifHandle = nullptr;
    }
}

void FreeImageGifWriter::add24bitBGRDataPage(int width, int height, BYTE* pData)
{
    std::cout << "Add new page!" << std::endl;
    FIBITMAP* newBitmap = FreeImage_Allocate(width, height, 24, 0x0000FF, 0x00FF00, 0xFF0000);
    BYTE* bitmapData = FreeImage_GetBits(newBitmap);
    memcpy(bitmapData, pData, width * height * 3);
    
    //Set metadata
    FIBITMAP* convBitmap = FreeImage_ColorQuantize(newBitmap, FIQ_LFPQUANT);
    FITAG* delayTag = FreeImage_CreateTag();
    
    FreeImage_SetMetadata(FIMD_ANIMATION, convBitmap, NULL, NULL);

    LONG delayVal = 20;
    std::cout << "New tag is..." << std::endl;
    if (delayTag) {
        std::cout << "Doing stuff" << std::endl;
        FreeImage_SetTagKey(delayTag, "FrameTime");
        FreeImage_SetTagType(delayTag, FIDT_LONG);
        FreeImage_SetTagCount(delayTag, 1);
        FreeImage_SetTagLength(delayTag, 4);
        FreeImage_SetTagValue(delayTag, &delayVal);
        FreeImage_SetMetadata(FIMD_ANIMATION, convBitmap, FreeImage_GetTagKey(delayTag), delayTag);
        FreeImage_DeleteTag(delayTag);
    }

    FreeImage_AppendPage(m_gifHandle, convBitmap);
    
    int pCount = FreeImage_GetPageCount(m_gifHandle);
    std::cout << "Current pages: " << pCount << std::endl;
    FreeImage_Unload(newBitmap);
    FreeImage_Unload(convBitmap);
}
