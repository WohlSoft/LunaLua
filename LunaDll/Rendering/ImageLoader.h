#ifndef ImageLoader_hhhh
#define ImageLoader_hhhh

#include <cstdint>
#include <unordered_map>
#include "SMBXImageCategories.h"
#include "../Misc/ResourceFileMapper.h"

class LunaImage;

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
    void resolveResources(const std::wstring& appGfxDir, const std::wstring& episodeGfxDir, const std::unordered_map<std::wstring, ResourceFileInfo>& levelFiles, const std::unordered_map<std::wstring, ResourceFileInfo>& episodeFiles, std::unordered_map<std::wstring, ResourceFileInfo>& outData) const;
private:
};

class ImageLoader
{
private:
    static std::unordered_map<std::string, std::shared_ptr<LunaImage>> m_ExtraGfx;
    static std::unordered_map<std::string, std::shared_ptr<LunaImage>> m_ExtraGfxOverride;
    static std::unordered_map<std::string, uintptr_t>                  m_NameToHDC;
    static std::unordered_map<uintptr_t, std::shared_ptr<LunaImage>>   m_GfxOverride;
    static std::unordered_map<uintptr_t, std::shared_ptr<LunaImage>>   m_Gfx;

    friend ImageLoaderCategory;

public:
    ImageLoader & operator=(const ImageLoader&) = delete;
    ImageLoader(const ImageLoader&) = delete;
    ImageLoader() = delete;
public:
    static void Run(bool initialLoad = false);
    static void ResolveHardcodedGfx(const std::wstring& appGfxDir, const std::wstring& episodeGfxDir, const std::unordered_map<std::wstring, ResourceFileInfo>& levelFiles, const std::unordered_map<std::wstring, ResourceFileInfo>& episodeFiles, std::unordered_map<std::wstring, ResourceFileInfo>& outData);
    static void LoadHardcodedGfx(const std::unordered_map<std::wstring, ResourceFileInfo>* fileData, const std::unordered_map<std::wstring, ResourceFileInfo>* oldFileData);
    static std::shared_ptr<LunaImage> GetByHDC(HDC hdc, bool bypassOverride=false);

    static std::shared_ptr<LunaImage> GetCharacterSprite(short charId, short powerup);

    static void RegisterExtraGfx(const std::string& folderName, const std::string& name);
    static void UnregisterExtraGfx(const std::string& name);

    static std::shared_ptr<LunaImage> GetByName(const std::string& name, bool bypassOverride=false);
    static bool OverrideByName(const std::string& name, const std::shared_ptr<LunaImage>& img);
};

#endif
