#include "LunaImage.h"
#include <Windows.h>
#include <string>
#include <cstdlib>
#include <atomic>
#include "../Misc/FreeImageUtils/FreeImageHelper.h"
#include "../Misc/FreeImageUtils/FreeImageData.h"
#include "../Misc/FreeImageUtils/FreeImageGifData.h"

uint64_t LunaImage::getNewUID()
{
    static std::atomic<uint64_t> uidCounter = 1;
    return uidCounter.fetch_add(1, std::memory_order_relaxed);
}

void LunaImage::load(const wchar_t* file)
{
    std::lock_guard<std::mutex> lock(mut);

    clearInternal();

    FreeImageData bitmapData;
    if ((file != nullptr) && (file[0] != '\0') && bitmapData.loadFile(file))
    {
        w = bitmapData.getWidth();
        h = bitmapData.getHeight();
        if ((w == 0) || (h == 0))
        {
            // Invalid size
            clearInternal();
            return;
        }
        data = std::malloc(4 * w*h);
        if (data == nullptr)
        {
            // Unable to allocate?
            clearInternal();
            return;
        }
        // Convert/Copy image data
        if (!bitmapData.toRawBGRA(data))
        {
            // Unable to convert?
            clearInternal();
            return;
        }
    }
}

void LunaImage::clearInternal()
{
    if (hbmp != nullptr)
    {
        // If we hae a hbmp, then it's where data is allocated so we only need
        // to deallocate the hbmp
        ::DeleteObject(hbmp);
        hbmp = nullptr;
        data = nullptr;
    }
    else if (data != nullptr)
    {
        // otherwise if we have data, directly deallocate that
        std::free(data);
        data = nullptr;
    }
    w = 0;
    h = 0;
}

void LunaImage::clear()
{
    std::lock_guard<std::mutex> lock(mut);

    clearInternal();
}

HBITMAP LunaImage::asHBITMAP()
{
    std::lock_guard<std::mutex> lock(mut);

    if (hbmp != nullptr) return hbmp;
    if (data == nullptr) return nullptr;

    // Convert this to a HBITMAP
    void* newData = nullptr;
    hbmp = FreeImageHelper::CreateEmptyBitmap(w, h, 32, (void**)&newData);

    // Copy data and replace with the HBITMAP's pointer
    memcpy(newData, data, 4 * w*h);
    std::free(data);
    data = newData;

    return hbmp;
}
