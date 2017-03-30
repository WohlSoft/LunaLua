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
    if (mask)
    {
        mask = nullptr;
    }
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

bool LunaImage::tryMaskToRGBA()
{
    if (data == nullptr) return false;
    if ((!mask) || (mask->data == nullptr)) return false;
    if ((h != mask->h) || (w != mask->w)) return false;

    uint32_t byteCount = 4*w*h;
    for (uint32_t idx = 0; idx < byteCount; idx += 4)
    {
        uint32_t mainPix = (
            ((uint32_t)(((uint8_t*)data)[idx + 0]) << 16) |
            ((uint32_t)(((uint8_t*)data)[idx + 1]) << 8) |
            ((uint32_t)(((uint8_t*)data)[idx + 2]) << 0)
            );
        uint32_t maskPix = (
            ((uint32_t)(((uint8_t*)mask->data)[idx + 0]) << 16) |
            ((uint32_t)(((uint8_t*)mask->data)[idx + 1]) << 8) |
            ((uint32_t)(((uint8_t*)mask->data)[idx + 2]) << 0)
            );

        // Transparent
        if ((mainPix == 0x000000) && (maskPix == 0xFFFFFF)) continue;

        // Dark bits in the image, that the mask doesn't mask, are bad
        if (((mainPix ^ 0xFFFFFF) & maskPix) != 0) return false;
    }

    // Set up alpha channel correctly
    for (uint32_t idx = 0; idx < byteCount; idx += 4)
    {
        uint32_t mainPix = (
            ((uint32_t)(((uint8_t*)data)[idx + 0]) << 16) |
            ((uint32_t)(((uint8_t*)data)[idx + 1]) << 8) |
            ((uint32_t)(((uint8_t*)data)[idx + 2]) << 0)
            );
        uint32_t maskPix = (
            ((uint32_t)(((uint8_t*)mask->data)[idx + 0]) << 16) |
            ((uint32_t)(((uint8_t*)mask->data)[idx + 1]) << 8) |
            ((uint32_t)(((uint8_t*)mask->data)[idx + 2]) << 0)
            );

        // Transparent
        if ((mainPix == 0x000000) && (maskPix == 0xFFFFFF))
        {
            ((uint8_t*)data)[idx + 0] = 0x00;
            ((uint8_t*)data)[idx + 1] = 0x00;
            ((uint8_t*)data)[idx + 2] = 0x00;
            ((uint8_t*)data)[idx + 3] = 0x00;
        }
        else
        {
            ((uint8_t*)data)[idx + 3] = 0xFF;
        }
    }

    // Toss out the mask
    mask = nullptr;

    return true;
}

void LunaImage::drawMasked(int dx, int dy, int w, int h, int sx, int sy, bool drawMask, bool drawMain)
{
    if (g_GLEngine.IsEnabled())
    {
        if (drawMask) {
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

        if (drawMain)
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
    }
    else
    {
        // LUNAIMAGE_TODO: Support GDI renderer
    }
}

void LunaImage::drawRGBA(int dx, int dy, int w, int h, int sx, int sy, bool maskOnly, float opacity)
{
    if (g_GLEngine.IsEnabled())
    {
        // LUNAIMAGE_TODO: Implement silhouette drawing of RGBA images for the case of mask-only drawing

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
        obj->mOpacity = opacity;
        obj->mMode = GLDraw::RENDER_MODE_ALPHA;
        g_GLEngine.QueueCmd(obj);
    }
    else
    {
        // LUNAIMAGE_TODO: Support GDI renderer
    }
}

void LunaImage::draw(int dx, int dy, int w, int h, int sx, int sy, bool drawMask, bool drawMain, float opacity)
{
    if (opacity <= 0.0f) return;
    if ((!drawMask) && (!drawMain)) return;

    if (mask)
    {
        // LUNAIMAGE_TODO: If opacity is not 1.0, do a forced conversion to RGBA
        drawMasked(dx, dy, w, h, sx, sy, drawMask, drawMain);
    }
    else
    {
        drawRGBA(dx, dy, w, h, sx, sy, !drawMain, opacity);
    }
}

LunaImage::~LunaImage()
{
    if (g_GLEngine.IsEnabled())
    {
        g_GLEngine.NotifyTextureDeletion(uid);
    }
    clearInternal();
}

uint32_t LunaImage::getDataPtrAsInt() {
    return (uint32_t)data;
}