#ifndef ImageLoader_hhhh
#define ImageLoader_hhhh

#include <cstdint>
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

    void updateLoadedImages();

private:
    void getImageResourceInfo(ResourceFileInfo** imgFileInfoOut, ResourceFileInfo** maskFileInfoOut) const;
};

class ImageLoader
{
public:
    ImageLoader & operator=(const ImageLoader&) = delete;
    ImageLoader(const ImageLoader&) = delete;
    ImageLoader() = delete;
public:
    static void Run(bool initialLoad = false);
};

#endif
