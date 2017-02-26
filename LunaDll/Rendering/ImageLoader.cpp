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

static void resolveImageResource(
    const std::wstring& basegamePath,
    const std::wstring& fileRoot,
    std::unordered_map<std::wstring, ResourceFileInfo>& levelFiles,
    std::unordered_map<std::wstring, ResourceFileInfo>& episodeFiles,
    std::unordered_map<std::wstring, ResourceFileInfo>& outData)
{
    std::wstring pngName = fileRoot + L".png";
    std::wstring gifName = fileRoot + L".gif";

    bool mainIsGif;
    {
        ResourceFileInfo resource;
        auto it = levelFiles.find(pngName);
        if (it == levelFiles.end())
            it = levelFiles.find(gifName);
        if (it == levelFiles.end())
            it = episodeFiles.find(pngName);
        if (it == episodeFiles.end())
            it = episodeFiles.find(gifName);
        if (it != episodeFiles.end())
            resource = it->second;
        if (resource.path.length() == 0)
            resource = GetResourceFileInfo(basegamePath, fileRoot, L"png");
        if (resource.path.length() == 0)
            resource = GetResourceFileInfo(basegamePath, fileRoot, L"gif");
        mainIsGif = (resource.extension == L"gif");
        if (resource.path.length() > 0)
        {
            outData[fileRoot] = std::move(resource);
        }
    }

    if (mainIsGif)
    {
        std::wstring maskName = fileRoot + L"m.gif";
        ResourceFileInfo maskResource;
        auto it = levelFiles.find(maskName);
        if (it == levelFiles.end())
            it = episodeFiles.find(maskName);
        if (it != episodeFiles.end())
            maskResource = it->second;
        if (maskResource.path.length() == 0)
            maskResource = GetResourceFileInfo(basegamePath, fileRoot + L"m", L"gif");
        if (maskResource.path.length() > 0) {
            outData[fileRoot + L"m"] = std::move(maskResource);
        }
    }
}

void ImageLoaderCategory::resolveResources(
    std::unordered_map<std::wstring, ResourceFileInfo>& levelFiles,
    std::unordered_map<std::wstring, ResourceFileInfo>& episodeFiles,
    std::unordered_map<std::wstring, ResourceFileInfo>& outData) const
{
    uint32_t firstIdx = m_Category.getFirstIdx();
    uint32_t lastIdx = m_Category.getLastIdx();
    std::wstring prefix = m_Category.getPrefix();

    std::wstring basegamePath = gAppPathWCHAR + L"/graphics/" + m_Category.getFolderPrefix() + L"/";

    for (uint32_t idx = firstIdx; idx <= lastIdx; idx++)
    {
        std::wstring fileRoot = prefix + L"-" + std::to_wstring(idx);
        resolveImageResource(basegamePath, fileRoot, levelFiles, episodeFiles, outData);
    }
}

static void getImageResource(const std::wstring& imageName,
    const std::unordered_map<std::wstring, ResourceFileInfo>& fileData,
    ResourceFileInfo& mainResource,
    ResourceFileInfo& maskResource)
{
    auto it = fileData.find(imageName);
    if (it == fileData.end()) return;
    mainResource = it->second;

    it = fileData.find(imageName + L"m");
    if (it == fileData.end()) return;
    maskResource = it->second;
}

void ImageLoaderCategory::updateLoadedImages(const std::unordered_map<std::wstring, ResourceFileInfo>* fileData, const std::unordered_map<std::wstring, ResourceFileInfo>* oldFileData)
{
    uint32_t firstIdx = m_Category.getFirstIdx();
    uint32_t lastIdx = m_Category.getLastIdx();

    std::wstring prefix = m_Category.getPrefix();
    for (uint32_t i = firstIdx; i <= lastIdx; i++)
    {
        std::wstring imageName = prefix + L"-" + std::to_wstring(i);
        ResourceFileInfo newMain, newMask;
        ResourceFileInfo oldMain, oldMask;
        getImageResource(imageName, *fileData, newMain, newMask);
        if (oldFileData != nullptr)
        {
            getImageResource(imageName, *oldFileData, oldMain, oldMask);
        }

        // If anything has changed, reload
        if ((oldFileData == nullptr) || (newMain != oldMain) || (newMask != oldMask))
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
                if (newMain.path.length() > 0)
                {
                    mainImg = LunaImage::fromFile(newMain.path.c_str());
                }

                // Assign image, note size
                ImageLoader::loadedImages[(uintptr_t)mainImgHdc] = mainImg;
                if (mainImg)
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
                if (newMask.path.length() > 0)
                {
                    maskImg = LunaImage::fromFile(newMask.path.c_str());
                }

                // Assign image, note size
                mainImg->setMask(maskImg);
                if (maskImg)
                {
                    if (width < maskImg->getW()) width = maskImg->getW();
                    if (height < maskImg->getH()) height = maskImg->getH();
                }
            }

            m_Category.setWidth(i, (int16_t)min(width, 0x7FFF));
            m_Category.setHeight(i, (int16_t)min(height, 0x7FFF));
        }
    }
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
static ImageLoaderCategory* smbxImageLoaderCategories[] = {
    &smbxImageLoaderBlock,
    &smbxImageLoaderBackground2,
    &smbxImageLoaderNpc,
    &smbxImageLoaderEffect,
    &smbxImageLoaderBackground,
    &smbxImageLoaderMario,
    &smbxImageLoaderLuigi,
    &smbxImageLoaderPeach,
    &smbxImageLoaderToad,
    &smbxImageLoaderLink,
    &smbxImageLoaderYoshiB,
    &smbxImageLoaderYoshiT,
    &smbxImageLoaderTile,
    &smbxImageLoaderLevel,
    &smbxImageLoaderScene,
    &smbxImageLoaderPath,
    &smbxImageLoaderPlayer,
    nullptr
};

void ImageLoader::Run(bool initialLoad)
{
    static std::unordered_map<std::wstring, ResourceFileInfo>* lastResources = nullptr;
    std::unordered_map<std::wstring, ResourceFileInfo>* foundResources = nullptr;
    foundResources = new std::unordered_map<std::wstring, ResourceFileInfo>();

    // Read level directory listing
    std::wstring levelPath = getCustomFolderPath();
    std::unordered_map<std::wstring, ResourceFileInfo> levelFiles;
    ListResourceFilesFromDir(levelPath.c_str(), levelFiles);

    // Read episode directory listing
    std::wstring episodePath = GM_FULLDIR;
    std::unordered_map<std::wstring, ResourceFileInfo> episodeFiles;
    ListResourceFilesFromDir(episodePath.c_str(), episodeFiles);

    // Resolve correct resource file info for each category
    for (int i = 0; smbxImageLoaderCategories[i] != nullptr; i++)
    {
        smbxImageLoaderCategories[i]->resolveResources(
            levelFiles, episodeFiles, *foundResources
            );
    }

    // Load each normal category's GFX
    for (int i = 0; smbxImageLoaderCategories[i] != nullptr; i++)
    {
        smbxImageLoaderCategories[i]->updateLoadedImages(foundResources, lastResources);
    }

    // Read 'hardcoded' GFX
    static bool loadedHardcoded = false; // TODO: Won't need this odd 'loadedHardcoded' shenanigans when the initial run of ImageLoader::Run is moved to after GM_FORM_GFX is initialized
    if (GM_FORM_GFX != nullptr)
    {
        ResolveHardcodedGfx(levelFiles, episodeFiles, *foundResources);
        LoadHardcodedGfx(foundResources, loadedHardcoded ? lastResources : nullptr);
        loadedHardcoded = true;
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

    delete lastResources;
    lastResources = foundResources;
}

void ImageLoader::ResolveHardcodedGfx(std::unordered_map<std::wstring, ResourceFileInfo>& levelFiles, std::unordered_map<std::wstring, ResourceFileInfo>& episodeFiles, std::unordered_map<std::wstring, ResourceFileInfo>& outData)
{
    for (int idx1 = 1; idx1 <= HardcodedGraphicsItem::Size(); idx1++)
    {
        std::wstring basegamePath = gAppPathWCHAR + L"/graphics/hardcoded/";
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

            std::wstring hardcodedName = L"hardcoded-" + std::to_wstring(idx1);
            if (hItemInfo.isArray())
            {
                hardcodedName += L"-" + std::to_wstring(idx2);
            }

            resolveImageResource(basegamePath, hardcodedName, levelFiles, episodeFiles, outData);
        }
    }
}

void ImageLoader::LoadHardcodedGfx(const std::unordered_map<std::wstring, ResourceFileInfo>* fileData, const std::unordered_map<std::wstring, ResourceFileInfo>* oldFileData)
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

            std::wstring hardcodedName = L"hardcoded-" + std::to_wstring(idx1);
            if (hItemInfo.isArray())
            {
                hardcodedName +=  L"-" + std::to_wstring(idx2);
            }

            ResourceFileInfo newMain, newMask;
            ResourceFileInfo oldMain, oldMask;
            getImageResource(hardcodedName, *fileData, newMain, newMask);
            if (oldFileData != nullptr)
            {
                getImageResource(hardcodedName, *oldFileData, oldMain, oldMask);
            }

            // If anything has changed, reload
            if ((oldFileData == nullptr) || (newMain != oldMain) || (newMask != oldMask))
            {
                std::shared_ptr<LunaImage> img = nullptr;
                std::shared_ptr<LunaImage> mask = nullptr;

                if (newMain.done)
                {
                    // If we found a file, load from it
                    img = LunaImage::fromFile(newMain.path.c_str());
                    mask = LunaImage::fromFile(newMask.path.c_str());
                }
                else
                {
                    // Otherwise, load from HDC
                    img = LunaImage::fromHDC(colorHDC);
                    mask = LunaImage::fromHDC(maskHDC);

                    // TODO: Consider special case for maskless hardcoded graphics from HDC, where black should be made transparent
                }

                if (img && mask)
                {
                    img->setMask(mask);
                }

                if (img)
                {
                    if (colorHDC != nullptr) ImageLoader::loadedImages[(uintptr_t)colorHDC] = img;
                    if (maskHDC != nullptr)  ImageLoader::loadedImages[(uintptr_t)maskHDC] = img;
                }
            }
        }
    }
}

std::shared_ptr<LunaImage> ImageLoader::GetByHDC(HDC hdc) {
    auto it = loadedImages.find((uintptr_t)hdc);
    if (it != loadedImages.end())
    {
        return it->second;
    }
    return nullptr;
}