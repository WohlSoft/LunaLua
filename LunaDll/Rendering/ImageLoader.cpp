#include <cstring>
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "ImageLoader.h"
#include "SMBXImageCategories.h"
#include "RenderUtils.h"
#include "../Misc/ResourceFileMapper.h"

void ImageLoaderCategory::getImageResourceInfo(ResourceFileInfo** imgFileInfoOut, ResourceFileInfo** maskFileInfoOut) const
{
    uint32_t arrLen = m_Category.getArrayLength();
    std::wstring prefix = m_Category.getPrefix();

    std::wstring basegamePath = gAppPathWCHAR + L"/graphics/" + m_Category.getFolderPrefix() + L"/";
    std::wstring levelPath = getCustomFolderPath();
    std::wstring episodePath = GM_FULLDIR;
    
    if (imgFileInfoOut != nullptr)
    {
        ResourceFileInfo* imgFileInfo = new ResourceFileInfo[arrLen];
        if (levelPath != episodePath) {
            FillResourceFileInfo((levelPath + prefix + L"-").c_str(), L".gif", arrLen, imgFileInfo);
        }
        FillResourceFileInfo((episodePath + prefix + L"-").c_str(), L".gif", arrLen, imgFileInfo);
        FillResourceFileInfo((basegamePath + prefix + L"-").c_str(), L".gif", arrLen, imgFileInfo);
        (*imgFileInfoOut) = imgFileInfo;
    }

    if ((maskFileInfoOut != nullptr) && m_Category.haveMaskPtrArray())
    {
        ResourceFileInfo* maskFileInfo = new ResourceFileInfo[arrLen];
        if (levelPath != episodePath) {
            FillResourceFileInfo((levelPath + prefix + L"-").c_str(), L"m.gif", arrLen, maskFileInfo);
        }
        FillResourceFileInfo((episodePath + prefix + L"-").c_str(), L"m.gif", arrLen, maskFileInfo);
        FillResourceFileInfo((basegamePath + prefix + L"-").c_str(), L"m.gif", arrLen, maskFileInfo);
        (*maskFileInfoOut) = maskFileInfo;
    }
}

void ImageLoaderCategory::updateLoadedImages()
{
    uint32_t arrLen = m_Category.getArrayLength();
    ResourceFileInfo* newImgFileInfo = nullptr;
    ResourceFileInfo* newMaskFileInfo = nullptr;

    // Get new image resource file infomration
    getImageResourceInfo(&newImgFileInfo, &newMaskFileInfo);

    for (uint32_t i = 1; i <= arrLen; i++)
    {
        ResourceFileInfo* newImg = (newImgFileInfo != nullptr) ? &newImgFileInfo[i - 1] : nullptr;
        ResourceFileInfo* oldImg = (m_ImgFileInfo != nullptr) ? &m_ImgFileInfo[i - 1] : nullptr;
        ResourceFileInfo* newMask = (newMaskFileInfo != nullptr) ? &newMaskFileInfo[i - 1] : nullptr;
        ResourceFileInfo* oldMask = (m_MaskFileInfo != nullptr) ? &m_MaskFileInfo[i - 1] : nullptr;
        // If anything has changed, reload
        if (
            ((newImg == nullptr) != (oldImg == nullptr)) || ((newImg != nullptr) && (oldImg != nullptr) && (*newImg != *oldImg)) ||
            ((newMask == nullptr) != (oldMask == nullptr)) || ((newMask != nullptr) && (oldMask != nullptr) && (*newMask != *oldMask))
            )
        {
            short width = 0, height = 0;

            if (m_Category.haveImagePtrArray())
            {
                // Clear old HBITMAP if any
                HDC hdcPtr = m_Category.getImagePtr(i);
                if (hdcPtr)
                {
                    HGDIOBJ oldImg = GetCurrentObject(hdcPtr, OBJ_BITMAP);
                    if (oldImg) DeleteObject(oldImg);
                    DeleteDC(hdcPtr);
                    m_Category.setMaskPtr(i, nullptr);
                }

                HBITMAP img = nullptr;
                if ((newImg != nullptr) && (newImg->path.length() > 0))
                {
                    img = LoadGfxAsBitmap(newImg->path);
                }
                if (img != nullptr)
                {
                    hdcPtr = CreateCompatibleDC(NULL);
                    m_Category.setImagePtr(i, hdcPtr);
                    SelectObject(hdcPtr, img);

                    BITMAP bmp;
                    GetObject(img, sizeof(BITMAP), &bmp);
                    if (width < bmp.bmWidth) width = (int16_t)min(bmp.bmWidth, 0x7FFF);
                    if (height < bmp.bmHeight) height = (int16_t)min(bmp.bmHeight, 0x7FFF);
                }
            }

            if (m_Category.haveMaskPtrArray())
            {
                // Clear old HBITMAP if any
                HDC hdcPtr = m_Category.getMaskPtr(i);
                if (hdcPtr)
                {
                    HGDIOBJ oldImg = GetCurrentObject(hdcPtr, OBJ_BITMAP);
                    if (oldImg) DeleteObject(oldImg);
                    DeleteDC(hdcPtr);
                    m_Category.setMaskPtr(i, nullptr);
                }

                HBITMAP img = nullptr;
                if ((newMask != nullptr) && (newMask->path.length() > 0))
                {
                    img = LoadGfxAsBitmap(newMask->path);
                }
                if (img != nullptr)
                {
                    hdcPtr = CreateCompatibleDC(NULL);
                    m_Category.setMaskPtr(i, hdcPtr);
                    SelectObject(hdcPtr, img);

                    BITMAP bmp;
                    GetObject(img, sizeof(BITMAP), &bmp);
                    if (width < bmp.bmWidth) width = (int16_t)min(bmp.bmWidth, 0x7FFF);
                    if (height < bmp.bmHeight) height = (int16_t)min(bmp.bmHeight, 0x7FFF);
                }
            }

            m_Category.setWidth(i, width);
            m_Category.setHeight(i, height);
        }
    }

    // Replace old file information
    if (m_ImgFileInfo) { delete m_ImgFileInfo; m_ImgFileInfo = nullptr; };
    if (m_MaskFileInfo) { delete m_MaskFileInfo; m_MaskFileInfo = nullptr; };
    m_ImgFileInfo = newImgFileInfo;
    m_MaskFileInfo = newMaskFileInfo;
}


static ImageLoaderCategory smbxImageLoaderBlock(smbxImageCategoryBlock);
static ImageLoaderCategory smbxImageLoaderBackground2(smbxImageCategoryBackground2);
static ImageLoaderCategory smbxImageLoaderNpc(smbxImageCategoryNpc);
static ImageLoaderCategory smbxImageLoaderEffect(smbxImageCategoryEffect);
static ImageLoaderCategory smbxImageLoaderBackground(smbxImageCategoryBackground);
static ImageLoaderCategory smbxImageLoaderMario(smbxImageCategoryMario);
static ImageLoaderCategory smbxImageLoaderLuigi(smbxImageCategoryLuigi);
static ImageLoaderCategory smbxImageLoaderPeach(smbxImageCategoryPeach);
static ImageLoaderCategory smbxImageLoaderToad(smbxImageCategoryToad);
static ImageLoaderCategory smbxImageLoaderLink(smbxImageCategoryLink);
static ImageLoaderCategory smbxImageLoaderYoshiB(smbxImageCategoryYoshiB);
static ImageLoaderCategory smbxImageLoaderYoshiT(smbxImageCategoryYoshiT);
static ImageLoaderCategory smbxImageLoaderTile(smbxImageCategoryTile);
static ImageLoaderCategory smbxImageLoaderLevel(smbxImageCategoryLevel);
static ImageLoaderCategory smbxImageLoaderScene(smbxImageCategoryScene);
static ImageLoaderCategory smbxImageLoaderPath(smbxImageCategoryPath);
static ImageLoaderCategory smbxImageLoaderPlayer(smbxImageCategoryPlayer);

void ImageLoader::Run(bool initialLoad)
{
    // Clear special case mapping
    GM_GFX_LEVEL_PTR[0] = nullptr;
    GM_GFX_LEVEL_MASK_PTR[0] = nullptr;
    GM_GFX_LEVEL_W_PTR[0] = 0;
    GM_GFX_LEVEL_H_PTR[0] = 0;

    smbxImageLoaderBlock.updateLoadedImages();
    smbxImageLoaderBackground2.updateLoadedImages();
    smbxImageLoaderNpc.updateLoadedImages();
    smbxImageLoaderEffect.updateLoadedImages();
    smbxImageLoaderBackground.updateLoadedImages();
    smbxImageLoaderMario.updateLoadedImages();
    smbxImageLoaderLuigi.updateLoadedImages();
    smbxImageLoaderPeach.updateLoadedImages();
    smbxImageLoaderToad.updateLoadedImages();
    smbxImageLoaderLink.updateLoadedImages();
    smbxImageLoaderYoshiB.updateLoadedImages();
    smbxImageLoaderYoshiT.updateLoadedImages();
    smbxImageLoaderTile.updateLoadedImages();
    smbxImageLoaderLevel.updateLoadedImages();
    smbxImageLoaderScene.updateLoadedImages();
    smbxImageLoaderPath.updateLoadedImages();
    smbxImageLoaderPlayer.updateLoadedImages();

    // Set special case mapping...
    GM_GFX_LEVEL_PTR[0] = GM_GFX_PATH_PTR[2];
    GM_GFX_LEVEL_MASK_PTR[0] = GM_GFX_PATH_MASK_PTR[2];
    GM_GFX_LEVEL_W_PTR[0] = GM_GFX_PATH_W_PTR[2];
    GM_GFX_LEVEL_H_PTR[0] = GM_GFX_PATH_H_PTR[2];

    if (initialLoad)
    {
        for (int i = 0; i < smbxImageCategoryBackground.getArrayLength(); i++)
        {
            GM_GFX_BACKGROUND_W_UNK_PTR[i] = GM_GFX_BACKGROUND_W_PTR[i];
            GM_GFX_BACKGROUND_H_UNK_PTR[i] = GM_GFX_BACKGROUND_H_PTR[i];
        }
    }

    for (int i = 0; i < smbxImageCategoryBlock.getArrayLength(); i++)
    {
        GM_GFX_BLOCKS_NO_MASK[i] = (smbxImageCategoryBlock.getMaskPtr(i + 1) == nullptr) ? -1 : 0;
    }
}
