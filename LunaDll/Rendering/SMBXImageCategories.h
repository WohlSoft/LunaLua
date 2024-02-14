#ifndef SMBXImageCategories_hhhh
#define SMBXImageCategories_hhhh

#include <cstdint>
#include <string>
#include "../Defines.h"
#include "../SMBXInternal/NPCs.h"
#include "../SMBXInternal/BGOs.h"
#include "../SMBXInternal/Animation.h"
#include "../SMBXInternal/Blocks.h"
#include "../SMBXInternal/Tile.h"
#include "../SMBXInternal/Path.h"
#include "../SMBXInternal/Scenery.h"
#include "../SMBXInternal/WorldLevel.h"

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
    uint16_t       _firstIndex;
public:
    const wchar_t* getPrefix() const {
        return _prefix;
    }
    const wchar_t* getFolderPrefix() const {
        if (_folderPrefix == nullptr) return _prefix;
        return _folderPrefix;
    }
    uint16_t getFirstIdx() const {
        return _firstIndex;
    }
    uint16_t getLastIdx() const {
        return _firstIndex + (_arrayLength - 1);
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
        if ((idx < _firstIndex) || ((idx - _firstIndex) > _arrayLength) || (_imagePtrArray == nullptr) || (*_imagePtrArray == nullptr)) return nullptr;
        return (*_imagePtrArray)[idx - _firstIndex];
    }
    void setImagePtr(uint16_t idx, HDC ptr) const {
        if ((idx < _firstIndex) || ((idx - _firstIndex) > _arrayLength) || (_imagePtrArray == nullptr) || (*_imagePtrArray == nullptr)) return;
        (*_imagePtrArray)[idx - _firstIndex] = ptr;
    }
    HDC getMaskPtr(uint16_t idx) const {
        if ((idx < _firstIndex) || ((idx - _firstIndex) > _arrayLength) || (_maskPtrArray == nullptr) || (*_maskPtrArray == nullptr)) return nullptr;
        return (*_maskPtrArray)[idx - _firstIndex];
    }
    void setMaskPtr(uint16_t idx, HDC ptr) const {
        if ((idx < _firstIndex) || ((idx - _firstIndex) > _arrayLength) || (_maskPtrArray == nullptr) || (*_maskPtrArray == nullptr)) return;
        (*_maskPtrArray)[idx - _firstIndex] = ptr;
    }
    uint16_t getWidth(uint16_t idx) const {
        if ((idx < _firstIndex) || ((idx - _firstIndex) > _arrayLength) || (_widthArray == nullptr) || (*_widthArray == nullptr)) return 0;
        return (*_widthArray)[idx - _firstIndex];
    }
    void setWidth(uint16_t idx, uint16_t val) const {
        if ((idx < _firstIndex) || ((idx - _firstIndex) > _arrayLength) || (_widthArray == nullptr) || (*_widthArray == nullptr)) return;
        (*_widthArray)[idx - _firstIndex] = val;
    }
    uint16_t getHeight(uint16_t idx) const {
        if ((idx < _firstIndex) || ((idx - _firstIndex) > _arrayLength) || (_heightArray == nullptr) || (*_heightArray == nullptr)) return 0;
        return (*_heightArray)[idx - _firstIndex];
    }
    void setHeight(uint16_t idx, uint16_t val) const {
        if ((idx < _firstIndex) || ((idx - _firstIndex) > _arrayLength) || (_heightArray == nullptr) || (*_heightArray == nullptr)) return;
        (*_heightArray)[idx - _firstIndex] = val;
    }
};

static const SMBXImageCategory smbxImageCategoryBlock =       { L"block",                 Block::MAX_ID, GM_GFX_BLOCKS_PTR_POINTER, GM_GFX_BLOCKS_MASK_PTR_POINTER, nullptr, nullptr, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryBackground2 = { L"background2",                     100, GM_GFX_BACKGROUND2_PTR_POINTER, nullptr, GM_GFX_BACKGROUND2_W_PTR_POINTER, GM_GFX_BACKGROUND2_H_PTR_POINTER, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryNpc =         { L"npc",                     NPC::MAX_ID, GM_GFX_NPC_PTR_POINTER, GM_GFX_NPC_MASK_PTR_POINTER, nullptr, nullptr, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryEffect =      { L"effect",        SMBXAnimation::MAX_ID, GM_GFX_EFFECTS_PTR_POINTER, GM_GFX_EFFECTS_MASK_PTR_POINTER, GM_GFX_EFFECTS_W_PTR_POINTER, GM_GFX_EFFECTS_H_PTR_POINTER, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryBackground =  { L"background",         SMBX_BGO::MAX_ID, GM_GFX_BACKGROUND_PTR_POINTER, GM_GFX_BACKGROUND_MASK_PTR_POINTER, GM_GFX_BACKGROUND_W_PTR_POINTER, GM_GFX_BACKGROUND_H_PTR_POINTER, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryMario =       { L"mario",                            10, GM_GFX_MARIO_PTR_POINTER, GM_GFX_MARIO_MASK_PTR_POINTER, GM_GFX_MARIO_W_PTR_POINTER, GM_GFX_MARIO_H_PTR_POINTER, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryLuigi =       { L"luigi",                            10, GM_GFX_LUIGI_PTR_POINTER, GM_GFX_LUIGI_MASK_PTR_POINTER, GM_GFX_LUIGI_W_PTR_POINTER, GM_GFX_LUIGI_H_PTR_POINTER, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryPeach =       { L"peach",                            10, GM_GFX_PEACH_PTR_POINTER, GM_GFX_PEACH_MASK_PTR_POINTER, GM_GFX_PEACH_W_PTR_POINTER, GM_GFX_PEACH_H_PTR_POINTER, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryToad =        { L"toad",                             10, GM_GFX_TOAD_PTR_POINTER, GM_GFX_TOAD_MASK_PTR_POINTER, GM_GFX_TOAD_W_PTR_POINTER, GM_GFX_TOAD_H_PTR_POINTER, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryLink =        { L"link",                             10, GM_GFX_LINK_PTR_POINTER, GM_GFX_LINK_MASK_PTR_POINTER, GM_GFX_LINK_W_PTR_POINTER, GM_GFX_LINK_H_PTR_POINTER, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryYoshiB =      { L"yoshib",                           10, GM_GFX_YOSHIB_PTR_POINTER, GM_GFX_YOSHIB_MASK_PTR_POINTER, nullptr, nullptr, L"yoshi", 1 };
static const SMBXImageCategory smbxImageCategoryYoshiT =      { L"yoshit",                           10, GM_GFX_YOSHIT_PTR_POINTER, GM_GFX_YOSHIT_MASK_PTR_POINTER, nullptr, nullptr, L"yoshi", 1 };
static const SMBXImageCategory smbxImageCategoryTile =        { L"tile",               SMBXTile::MAX_ID, GM_GFX_TILES_PTR_POINTER, nullptr, GM_GFX_TILES_W_PTR_POINTER, GM_GFX_TILES_H_PTR_POINTER, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryLevel =       { L"level",          WorldLevel::MAX_ID+1, GM_GFX_LEVEL_PTR_POINTER, GM_GFX_LEVEL_MASK_PTR_POINTER, GM_GFX_LEVEL_W_PTR_POINTER, GM_GFX_LEVEL_H_PTR_POINTER, nullptr, 0 };
static const SMBXImageCategory smbxImageCategoryScene =       { L"scene",           SMBXScenery::MAX_ID, GM_GFX_SCENE_PTR_POINTER, GM_GFX_SCENE_MASK_PTR_POINTER, GM_GFX_SCENE_W_PTR_POINTER, GM_GFX_SCENE_H_PTR_POINTER, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryPath =        { L"path",               SMBXPath::MAX_ID, GM_GFX_PATH_PTR_POINTER, GM_GFX_PATH_MASK_PTR_POINTER, GM_GFX_PATH_W_PTR_POINTER, GM_GFX_PATH_H_PTR_POINTER, nullptr, 1 };
static const SMBXImageCategory smbxImageCategoryPlayer =      { L"player",                            5, GM_GFX_PLAYER_PTR_POINTER, GM_GFX_PLAYER_MASK_PTR_POINTER, GM_GFX_PLAYER_W_PTR_POINTER, GM_GFX_PLAYER_H_PTR_POINTER, nullptr, 1 };

#endif
