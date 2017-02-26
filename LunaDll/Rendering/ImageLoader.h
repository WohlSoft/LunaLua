#ifndef ImageLoader_hhhh
#define ImageLoader_hhhh

#include <cstdint>
#include <unordered_map>
#include "SMBXImageCategories.h"
#include "../Misc/ResourceFileMapper.h"

class ImageLoaderCategory
{
private:
    const SMBXImageCategory& m_Category;
    ResourceFileInfo* m_ImgFileInfo;
    ResourceFileInfo* m_MaskFileInfo;
public:
    ImageLoaderCategory & operator=(const ImageLoaderCategory&) = delete;
    ImageLoaderCategory(const ImageLoaderCategory&) = delete;
    ImageLoaderCategory() = delete;

    ImageLoaderCategory(const SMBXImageCategory& category) :
        m_Category(category), m_ImgFileInfo(nullptr), m_MaskFileInfo(nullptr)
    {
    }

    void updateLoadedImages(const std::unordered_map<std::wstring, ResourceFileInfo>* fileData, const std::unordered_map<std::wstring, ResourceFileInfo>* oldFileData);
    void resolveResources(std::unordered_map<std::wstring, ResourceFileInfo>& levelFiles, std::unordered_map<std::wstring, ResourceFileInfo>& episodeFiles, std::unordered_map<std::wstring, ResourceFileInfo>& outData) const;
private:
};

class ImageLoader
{
public:
    static std::unordered_map<uintptr_t, std::shared_ptr<LunaImage>> loadedImages;
public:
    ImageLoader & operator=(const ImageLoader&) = delete;
    ImageLoader(const ImageLoader&) = delete;
    ImageLoader() = delete;
public:
    static void Run(bool initialLoad = false);
    static void ResolveHardcodedGfx(std::unordered_map<std::wstring, ResourceFileInfo>& levelFiles, std::unordered_map<std::wstring, ResourceFileInfo>& episodeFiles, std::unordered_map<std::wstring, ResourceFileInfo>& outData);
    static void LoadHardcodedGfx(const std::unordered_map<std::wstring, ResourceFileInfo>* fileData, const std::unordered_map<std::wstring, ResourceFileInfo>* oldFileData);
    static std::shared_ptr<LunaImage> GetByHDC(HDC hdc);

    // TODO: Implement override system
    // TODO: Implement get-by-* queries similar to SMBXMaskedImage
};

#endif
