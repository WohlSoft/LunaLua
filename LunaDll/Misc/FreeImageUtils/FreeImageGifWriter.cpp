#include "FreeImageGifWriter.h"
#include <iostream>


FreeImageGifWriter::FreeImageGifWriter(const std::string& filename) :
    m_filename(filename),
    m_gifHandle(nullptr)
{
    m_gifHandle = FreeImage_OpenMultiBitmap(FIF_GIF, filename.c_str(), true, false);
}


FreeImageGifWriter::~FreeImageGifWriter()
{
    closeAndCleanup();
}

void FreeImageGifWriter::closeAndCleanup()
{
    if (m_gifHandle){
        FreeImage_CloseMultiBitmap(m_gifHandle);
        m_gifHandle = nullptr;
    }
}

void FreeImageGifWriter::add24bitBGRDataPage(int width, int height, BYTE* pData)
{
    FIBITMAP* newBitmap = FreeImage_Allocate(width, height, 24, 0x0000FF, 0x00FF00, 0xFF0000);
    BYTE* bitmapData = FreeImage_GetBits(newBitmap);
    memcpy(bitmapData, pData, width * height * 3);
    
    //Set metadata
    FIBITMAP* convBitmap = FreeImage_ColorQuantize(newBitmap, FIQ_LFPQUANT);
    FITAG* delayTag = FreeImage_CreateTag();
    
    FreeImage_SetMetadata(FIMD_ANIMATION, convBitmap, NULL, NULL);

    LONG delayVal = 20;
    if (delayTag) {
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
    FreeImage_Unload(newBitmap);
    FreeImage_Unload(convBitmap);
}
