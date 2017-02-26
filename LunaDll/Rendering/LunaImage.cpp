#include "LunaImage.h"
#include <Windows.h>
#include <string>
#include <cstdlib>
#include <atomic>
#include "../Misc/FreeImageUtils/FreeImageHelper.h"
#include "../Misc/FreeImageUtils/FreeImageData.h"
#include "../Misc/FreeImageUtils/FreeImageGifData.h"
#include "GL/GLEngineProxy.h"

uint64_t LunaImage::getNewUID()
{
    static std::atomic<uint64_t> uidCounter = 1;
    return uidCounter.fetch_add(1, std::memory_order_relaxed);
}

std::shared_ptr<LunaImage> LunaImage::fromHDC(HDC hdc)
{
    if (hdc == nullptr) return nullptr;

    HBITMAP convHBMP = CopyBitmapFromHdc(hdc);
    if (convHBMP == nullptr) return nullptr;

    BITMAP bmp;
    GetObject(convHBMP, sizeof(BITMAP), &bmp);

    // Allocate and copy into LunaImage
    std::shared_ptr<LunaImage> img = std::make_shared<LunaImage>();
    img->w = bmp.bmWidth;
    img->h = bmp.bmHeight;
    img->data = std::malloc(4 * img->w*img->h);
    if (img->data != nullptr)
    {
        memcpy(img->data, bmp.bmBits, 4 * img->w*img->h);
    }
    else
    {
        img = nullptr;
    }

    // Deallocate temporary conversion memory
    DeleteObject(convHBMP);
    convHBMP = NULL;

    return img;
}

std::shared_ptr<LunaImage> LunaImage::fromFile(const wchar_t* filename)
{
    if ((filename == nullptr) || (filename[0] == L'\0')) return nullptr;
    std::shared_ptr<LunaImage> img = std::make_shared<LunaImage>();
    img->load(filename);
    if ((img->getW() == 0) && (img->getH() == 0))
    {
        return nullptr;
    }

    return std::move(img);
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

void LunaImage::draw(int dx, int dy, int w, int h, int sx, int sy, bool drawMask, bool drawMain)
{
    if (g_GLEngine.IsEnabled())
    {
        if (mask && drawMask) {
            auto obj = std::make_shared<GLEngineCmd_DrawSprite>();
            obj->mXDest = dx;
            obj->mYDest = dy;
            obj->mWidthDest = w;
            obj->mHeightDest = h;
            obj->mXSrc = sx;
            obj->mYSrc = sy;
            obj->mWidthSrc = w;
            obj->mHeightSrc = h;

            obj->mImg = mask;
            obj->mOpacity = 1.0f;
            obj->mMode = GLDraw::RENDER_MODE_MULTIPLY; //  GLDraw::RENDER_MODE_AND
            g_GLEngine.QueueCmd(obj);
        }

        if (mask && drawMain)
        {
            auto obj = std::make_shared<GLEngineCmd_DrawSprite>();
            obj->mXDest = dx;
            obj->mYDest = dy;
            obj->mWidthDest = w;
            obj->mHeightDest = h;
            obj->mXSrc = sx;
            obj->mYSrc = sy;
            obj->mWidthSrc = w;
            obj->mHeightSrc = h;

            obj->mImg = shared_from_this();
            obj->mOpacity = 1.0f;
            obj->mMode = GLDraw::RENDER_MODE_MAX; //  GLDraw::RENDER_MODE_OR
            g_GLEngine.QueueCmd(obj);
        }

        if (!mask && drawMain)
        {
            auto obj = std::make_shared<GLEngineCmd_DrawSprite>();
            obj->mXDest = dx;
            obj->mYDest = dy;
            obj->mWidthDest = w;
            obj->mHeightDest = h;
            obj->mXSrc = sx;
            obj->mYSrc = sy;
            obj->mWidthSrc = w;
            obj->mHeightSrc = h;

            obj->mImg = shared_from_this();
            obj->mOpacity = 1.0f;
            obj->mMode = GLDraw::RENDER_MODE_ALPHA;
            g_GLEngine.QueueCmd(obj);
        }
    }
}