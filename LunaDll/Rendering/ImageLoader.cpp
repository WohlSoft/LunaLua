#include <cstring>
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "ImageLoader.h"
#include "SMBXImageCategories.h"
#include "RenderUtils.h"
#include "../Misc/ResourceFileMapper.h"
#include "LunaImage.h"
#include "../SMBXInternal/HardcodedGraphicsAccess.h"

// Loaded image map decleration
std::unordered_map<uintptr_t, std::shared_ptr<LunaImage>> ImageLoader::loadedImages;

void ImageLoaderCategory::getImageResourceInfo(ResourceFileInfo** imgFileInfoOut, ResourceFileInfo** maskFileInfoOut) const
{
    uint32_t firstIdx = m_Category.getFirstIdx();
    uint32_t lastIdx = m_Category.getLastIdx();
    uint32_t arrLen = m_Category.getArrayLength();
    std::wstring prefix = m_Category.getPrefix();

    std::wstring basegamePath = gAppPathWCHAR + L"/graphics/" + m_Category.getFolderPrefix() + L"/";
    std::wstring levelPath = getCustomFolderPath();
    std::wstring episodePath = GM_FULLDIR;
    
    ResourceFileInfo* imgFileInfo = nullptr;
    if (imgFileInfoOut != nullptr)
    {
        imgFileInfo = new ResourceFileInfo[arrLen];
        if (levelPath != episodePath) {
            FillResourceFileInfo((levelPath + prefix + L"-").c_str(), L".png", firstIdx, lastIdx, imgFileInfo);
            FillResourceFileInfo((levelPath + prefix + L"-").c_str(), L".gif", firstIdx, lastIdx, imgFileInfo);
        }
        FillResourceFileInfo((episodePath + prefix + L"-").c_str(), L".png", firstIdx, lastIdx, imgFileInfo);
        FillResourceFileInfo((episodePath + prefix + L"-").c_str(), L".gif", firstIdx, lastIdx, imgFileInfo);
        FillResourceFileInfo((basegamePath + prefix + L"-").c_str(), L".png", firstIdx, lastIdx, imgFileInfo);
        FillResourceFileInfo((basegamePath + prefix + L"-").c_str(), L".gif", firstIdx, lastIdx, imgFileInfo);
        (*imgFileInfoOut) = imgFileInfo;
    }

    if ((maskFileInfoOut != nullptr) && m_Category.haveMaskPtrArray())
    {
        ResourceFileInfo* maskFileInfo = new ResourceFileInfo[arrLen];

        // Don't load masks when a PNG is loaded in the slot
        if (imgFileInfo != nullptr)
        {
            for (uint32_t idx = firstIdx; idx <= lastIdx; idx++)
            {
                uint32_t i = idx - firstIdx;
                if (imgFileInfo[i].done && imgFileInfo[i].extension == L".png")
                {
                    maskFileInfo[i].done = true;
                }
            }
        }

        if (levelPath != episodePath) {
            FillResourceFileInfo((levelPath + prefix + L"-").c_str(), L"m.gif", firstIdx, lastIdx, maskFileInfo);
        }
        FillResourceFileInfo((episodePath + prefix + L"-").c_str(), L"m.gif", firstIdx, lastIdx, maskFileInfo);
        FillResourceFileInfo((basegamePath + prefix + L"-").c_str(), L"m.gif", firstIdx, lastIdx, maskFileInfo);
        (*maskFileInfoOut) = maskFileInfo;
    }
}

void ImageLoaderCategory::updateLoadedImages()
{
    uint32_t firstIdx = m_Category.getFirstIdx();
    uint32_t lastIdx = m_Category.getLastIdx();
    ResourceFileInfo* newImgFileInfo = nullptr;
    ResourceFileInfo* newMaskFileInfo = nullptr;

    // Get new image resource file infomration
    getImageResourceInfo(&newImgFileInfo, &newMaskFileInfo);

    for (uint32_t i = firstIdx; i <= lastIdx; i++)
    {
        ResourceFileInfo* newImg = (newImgFileInfo != nullptr) ? &newImgFileInfo[i - firstIdx] : nullptr;
        ResourceFileInfo* oldImg = (m_ImgFileInfo != nullptr) ? &m_ImgFileInfo[i - firstIdx] : nullptr;
        ResourceFileInfo* newMask = (newMaskFileInfo != nullptr) ? &newMaskFileInfo[i - firstIdx] : nullptr;
        ResourceFileInfo* oldMask = (m_MaskFileInfo != nullptr) ? &m_MaskFileInfo[i - firstIdx] : nullptr;
        // If anything has changed, reload
        if (
            ((newImg == nullptr) != (oldImg == nullptr)) || ((newImg != nullptr) && (oldImg != nullptr) && (*newImg != *oldImg)) ||
            ((newMask == nullptr) != (oldMask == nullptr)) || ((newMask != nullptr) && (oldMask != nullptr) && (*newMask != *oldMask))
            )
        {
            uint32_t width = 0, height = 0;

            HDC mainImgHdc = nullptr;
            std::shared_ptr<LunaImage> mainImg = nullptr;
            if (m_Category.haveImagePtrArray())
            {
                // Make HDC if-needed
                mainImgHdc = m_Category.getImagePtr(i);
                if (mainImgHdc == nullptr)
                {
                    mainImgHdc = CreateCompatibleDC(NULL);
                    m_Category.setImagePtr(i, mainImgHdc);
                }

                // Try to load image
                if ((newImg != nullptr) && (newImg->path.length() > 0))
                {
                    mainImg = LunaImage::fromFile(newImg->path.c_str());
                }

                // Assign image, note size
                ImageLoader::loadedImages[(uintptr_t)mainImgHdc] = mainImg;
                if (mainImg != nullptr)
                {
                    if (width < mainImg->getW()) width = mainImg->getW();
                    if (height < mainImg->getH()) height = mainImg->getH();
                }
            }
            else
            {
                // That's required...
                throw;
            }

            if (m_Category.haveMaskPtrArray() && mainImg)
            {
                // Match HDC to non-mask version
                m_Category.setMaskPtr(i, mainImgHdc);

                // Try to load image
                std::shared_ptr<LunaImage> maskImg = nullptr;
                if ((newMask != nullptr) && (newMask->path.length() > 0))
                {
                    maskImg = LunaImage::fromFile(newMask->path.c_str());
                }

                // Assign image, note size
                mainImg->setMask(maskImg);
                if (maskImg != nullptr)
                {
                    if (width < maskImg->getW()) width = maskImg->getW();
                    if (height < maskImg->getH()) height = maskImg->getH();
                }
            }

            m_Category.setWidth(i, (int16_t)min(width, 0x7FFF));
            m_Category.setHeight(i, (int16_t)min(height, 0x7FFF));
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

    // Read 'hardcoded' GFX
    static bool haveLoadedHardcoded = false;
    if ((!haveLoadedHardcoded) && (GM_FORM_GFX != nullptr))
    {
        LoadHardcodedGfx();
        haveLoadedHardcoded = true;
    }

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
        GM_GFX_BLOCKS_NO_MASK[i] = (smbxImageCategoryBlock.getMaskPtr(i + smbxImageCategoryBlock.getFirstIdx()) == nullptr) ? -1 : 0;
    }
}

void ImageLoader::LoadHardcodedGfx()
{


    for (int idx1 = 1; idx1 <= HardcodedGraphicsItem::Size(); idx1++)
    {
        HardcodedGraphicsItem& hItemInfo = HardcodedGraphicsItem::Get(idx1);
        
        // No processing invalid or mask items here
        if ((hItemInfo.state != HardcodedGraphicsItem::HITEMSTATE_NORMAL) && (hItemInfo.state != HardcodedGraphicsItem::HITEMSTATE_ARRAY))
            continue;

        int minItem = hItemInfo.isArray() ? hItemInfo.minItem : -1;
        int maxItem = hItemInfo.isArray() ? hItemInfo.maxItem : -1;
        for (int idx2 = minItem; idx2 <= maxItem; idx2++)
        {
            if (hItemInfo.isArray() && !hItemInfo.isValidArrayIndex(idx2))
            {
                // If this index isn't valid, skip it
                continue;
            }
            HDC colorHDC = nullptr;
            HDC maskHDC = nullptr;
            hItemInfo.getHDC(idx2, &colorHDC, &maskHDC);

            if (colorHDC == nullptr)
            {
                // No such thing as mask-only graphics
                continue;
            }

            std::shared_ptr<LunaImage> img  = LunaImage::fromHDC(colorHDC);
            std::shared_ptr<LunaImage> mask = LunaImage::fromHDC(maskHDC);
            if (img && mask)
            {
                img->setMask(mask);
            }
            if (img)  ImageLoader::loadedImages[(uintptr_t)colorHDC] = img;
            if (mask) ImageLoader::loadedImages[(uintptr_t)maskHDC]  = mask;

            // TODO: Consider special case for maskless hardcoded graphics, where black should be made transparent
        }
    }

    // TODO: Support loading replacement hardcoded graphics
}

std::shared_ptr<LunaImage> ImageLoader::GetByHDC(HDC hdc) {
    auto it = loadedImages.find((uintptr_t)hdc);
    if (it != loadedImages.end())
    {
        return it->second;
    }
    return nullptr;
}