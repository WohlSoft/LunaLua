#ifndef SMBXImageCategories_hhhh
#define SMBXImageCategories_hhhh

#include <cstdint>
#include <string>
#include "../Defines.h"

struct SMBXImageCategory
{
public:
    const wchar_t* _prefix;
    uint16_t       _arrayLength;
    HDC          **_imagePtrArray;
    HDC          **_maskPtrArray;
    int16_t      **_widthArray;
    int16_t      **_heightArray;
    const wchar_t* _folderPrefix;
    uint16_t       _baseIndex;
public:
    const wchar_t* getPrefix() const {
        return _prefix;
    }
    const wchar_t* getFolderPrefix() const {
        if (_folderPrefix == nullptr) return _prefix;
        return _folderPrefix;
    }
    uint16_t getArrayLength() const {
        return _arrayLength;
    }
    bool haveImagePtrArray() const {
        return (_imagePtrArray != nullptr) && (*_imagePtrArray != nullptr);
    }
    bool haveMaskPtrArray() const {
        return (_maskPtrArray != nullptr) && (*_maskPtrArray != nullptr);
    }
    HDC getImagePtr(uint16_t idx) const {
        if ((idx == 0) || (idx > _arrayLength) || (_imagePtrArray == nullptr) || (*_imagePtrArray == nullptr)) return nullptr;
        return (*_imagePtrArray)[idx - 1 + _baseIndex];
    }
    void setImagePtr(uint16_t idx, HDC ptr) const {
        if ((idx == 0) || (idx > _arrayLength) || (_imagePtrArray == nullptr) || (*_imagePtrArray == nullptr)) return;
        (*_imagePtrArray)[idx - 1 + _baseIndex] = ptr;
    }
    HDC getMaskPtr(uint16_t idx) const {
        if ((idx == 0) || (idx > _arrayLength) || (_maskPtrArray == nullptr) || (*_maskPtrArray == nullptr)) return nullptr;
        return (*_maskPtrArray)[idx - 1 + _baseIndex];
    }
    void setMaskPtr(uint16_t idx, HDC ptr) const {
        if ((idx == 0) || (idx > _arrayLength) || (_maskPtrArray == nullptr) || (*_maskPtrArray == nullptr)) return;
        (*_maskPtrArray)[idx - 1 + _baseIndex] = ptr;
    }
    uint16_t getWidth(uint16_t idx) const {
        if ((idx == 0) || (idx > _arrayLength) || (_widthArray == nullptr) || (*_widthArray == nullptr)) return 0;
        return (*_widthArray)[idx - 1 + _baseIndex];
    }
    void setWidth(uint16_t idx, uint16_t val) const {
        if ((idx == 0) || (idx > _arrayLength) || (_widthArray == nullptr) || (*_widthArray == nullptr)) return;
        (*_widthArray)[idx - 1 + _baseIndex] = val;
    }
    uint16_t getHeight(uint16_t idx) const {
        if ((idx == 0) || (idx > _arrayLength) || (_heightArray == nullptr) || (*_heightArray == nullptr)) return 0;
        return (*_heightArray)[idx - 1 + _baseIndex];
    }
    void setHeight(uint16_t idx, uint16_t val) const {
        if ((idx == 0) || (idx > _arrayLength) || (_heightArray == nullptr) || (*_heightArray == nullptr)) return;
        (*_heightArray)[idx - 1 + _baseIndex] = val;
    }
};

static const SMBXImageCategory smbxImageCategoryBlock = { L"block",       700, GM_GFX_BLOCKS_PTR_CONSTPTR, GM_GFX_BLOCKS_MASK_PTR_CONSTPTR, nullptr, nullptr, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryBackground2 = { L"background2", 100, GM_GFX_BACKGROUND2_PTR_CONSTPTR, nullptr, GM_GFX_BACKGROUND2_W_PTR_CONSTPTR, GM_GFX_BACKGROUND2_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryNpc = { L"npc",         300, GM_GFX_NPC_PTR_CONSTPTR, GM_GFX_NPC_MASK_PTR_CONSTPTR, GM_GFX_NPC_W_PTR_CONSTPTR, GM_GFX_NPC_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryEffect = { L"effect",      200, GM_GFX_EFFECTS_PTR_CONSTPTR, GM_GFX_EFFECTS_MASK_PTR_CONSTPTR, GM_GFX_EFFECTS_W_PTR_CONSTPTR, GM_GFX_EFFECTS_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryBackground = { L"background",  200, GM_GFX_BACKGROUND_PTR_CONSTPTR, GM_GFX_BACKGROUND_MASK_PTR_CONSTPTR, GM_GFX_BACKGROUND_W_PTR_CONSTPTR, GM_GFX_BACKGROUND_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryMario = { L"mario",        10, GM_GFX_MARIO_PTR_CONSTPTR, GM_GFX_MARIO_MASK_PTR_CONSTPTR, GM_GFX_MARIO_W_PTR_CONSTPTR, GM_GFX_MARIO_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryLuigi = { L"luigi",        10, GM_GFX_LUIGI_PTR_CONSTPTR, GM_GFX_LUIGI_MASK_PTR_CONSTPTR, GM_GFX_LUIGI_W_PTR_CONSTPTR, GM_GFX_LUIGI_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryPeach = { L"peach",        10, GM_GFX_PEACH_PTR_CONSTPTR, GM_GFX_PEACH_MASK_PTR_CONSTPTR, GM_GFX_PEACH_W_PTR_CONSTPTR, GM_GFX_PEACH_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryToad = { L"toad",         10, GM_GFX_TOAD_PTR_CONSTPTR, GM_GFX_TOAD_MASK_PTR_CONSTPTR, GM_GFX_TOAD_W_PTR_CONSTPTR, GM_GFX_TOAD_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryLink = { L"link",         10, GM_GFX_LINK_PTR_CONSTPTR, GM_GFX_LINK_MASK_PTR_CONSTPTR, GM_GFX_LINK_W_PTR_CONSTPTR, GM_GFX_LINK_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryYoshiB = { L"yoshib",       10, GM_GFX_YOSHIB_PTR_CONSTPTR, GM_GFX_YOSHIB_MASK_PTR_CONSTPTR, nullptr, nullptr, L"yoshi", 0 };
static const SMBXImageCategory smbxImageCategoryYoshiT = { L"yoshit",       10, GM_GFX_YOSHIT_PTR_CONSTPTR, GM_GFX_YOSHIT_MASK_PTR_CONSTPTR, nullptr, nullptr, L"yoshi", 0 };
static const SMBXImageCategory smbxImageCategoryTile = { L"tile",        400, GM_GFX_TILES_PTR_CONSTPTR, nullptr, GM_GFX_TILES_W_PTR_CONSTPTR, GM_GFX_TILES_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryLevel = { L"level",       100, GM_GFX_LEVEL_PTR_CONSTPTR, GM_GFX_LEVEL_MASK_PTR_CONSTPTR, GM_GFX_LEVEL_W_PTR_CONSTPTR, GM_GFX_LEVEL_H_PTR_CONSTPTR, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryScene = { L"scene",       100, GM_GFX_SCENE_PTR_CONSTPTR, GM_GFX_SCENE_MASK_PTR_CONSTPTR, GM_GFX_SCENE_W_PTR_CONSTPTR, GM_GFX_SCENE_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryPath = { L"path",        100, GM_GFX_PATH_PTR_CONSTPTR, GM_GFX_PATH_MASK_PTR_CONSTPTR, GM_GFX_PATH_W_PTR_CONSTPTR, GM_GFX_PATH_H_PTR_CONSTPTR, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryPlayer = { L"player",        5, GM_GFX_PLAYER_PTR_CONSTPTR, GM_GFX_PLAYER_MASK_PTR_CONSTPTR, GM_GFX_PLAYER_W_PTR_CONSTPTR, GM_GFX_PLAYER_H_PTR_CONSTPTR, nullptr, 0 };

#endif
