#include "HardcodedGraphicsAccess.h"
#include "../Utils/StringUtils.h"

// Values for index and array-index: https://gist.github.com/KevinW1998/49417ab8b94712bee39c
std::array<HardcodedGraphicsItem, 51> HardcodedGraphics =
{
    HardcodedGraphicsItem(1,  HardcodedGraphicsItem::HITEMSTATE_NORMAL),                // 1
    HardcodedGraphicsItem(2,  HardcodedGraphicsItem::HITEMSTATE_NORMAL_MASK),           // 2
    HardcodedGraphicsItem(3,  HardcodedGraphicsItem::HITEMSTATE_NORMAL,      2),        // 3
    HardcodedGraphicsItem(4,  HardcodedGraphicsItem::HITEMSTATE_NORMAL_MASK),           // 4
    HardcodedGraphicsItem(5,  HardcodedGraphicsItem::HITEMSTATE_NORMAL,      4),        // 5
    HardcodedGraphicsItem(6,  HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 5),  // 6 (array)
    HardcodedGraphicsItem(7,  HardcodedGraphicsItem::HITEMSTATE_ARRAY,       6, 1, 5),  // 7 (array)
    HardcodedGraphicsItem(8,  HardcodedGraphicsItem::HITEMSTATE_NORMAL_MASK),           // 8
    HardcodedGraphicsItem(9,  HardcodedGraphicsItem::HITEMSTATE_NORMAL,      8),        // 9
    HardcodedGraphicsItem(10, HardcodedGraphicsItem::HITEMSTATE_NORMAL),                // 10
    HardcodedGraphicsItem(11, HardcodedGraphicsItem::HITEMSTATE_NORMAL),                // 11
    HardcodedGraphicsItem(12, HardcodedGraphicsItem::HITEMSTATE_INVALID),               // 12 "Misc"
    HardcodedGraphicsItem(13, HardcodedGraphicsItem::HITEMSTATE_NORMAL),                // 13
    HardcodedGraphicsItem(14, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      -1, 1, 2),  // 14 (array)
    HardcodedGraphicsItem(15, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      -1, 1, 2),  // 15 (array)
    HardcodedGraphicsItem(16, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 1),  // 16 (array)
    HardcodedGraphicsItem(17, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      16, 1, 1),  // 17 (array)
    HardcodedGraphicsItem(18, HardcodedGraphicsItem::HITEMSTATE_INVALID),               // 18 "Yoshi"
    HardcodedGraphicsItem(19, HardcodedGraphicsItem::HITEMSTATE_NORMAL,     20),        // 19
    HardcodedGraphicsItem(20, HardcodedGraphicsItem::HITEMSTATE_NORMAL_MASK),           // 20
    HardcodedGraphicsItem(21, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      22, 1, 2),  // 21 (array)
    HardcodedGraphicsItem(22, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 2),  // 22 (array)
    HardcodedGraphicsItem(23, HardcodedGraphicsItem::HITEMSTATE_INVALID),               // 23 "Mounts"
    HardcodedGraphicsItem(24, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 3),  // 24 (array)
    HardcodedGraphicsItem(25, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      24, 1, 3),  // 25 (array)
    HardcodedGraphicsItem(26, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      27, 2, 2),  // 26 (array)
    HardcodedGraphicsItem(27, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 2, 2),  // 27 (array)
    HardcodedGraphicsItem(28, HardcodedGraphicsItem::HITEMSTATE_INVALID),               // 28 "Menu"
    HardcodedGraphicsItem(29, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 3),  // 29 (array)
    HardcodedGraphicsItem(30, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      29, 1, 4),  // 30 (array)
    HardcodedGraphicsItem(31, HardcodedGraphicsItem::HITEMSTATE_INVALID),               // 31 "Interface"
    HardcodedGraphicsItem(32, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 0, 8,   // 32 (array) NOTE: It has not a mask at array-index 4
        [](HardcodedGraphicsItem* obj, int arrayIndex) { return obj->minItem <= arrayIndex && obj->maxItem >= arrayIndex && arrayIndex != 4; } ),  
    HardcodedGraphicsItem(33, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      32, 0, 8),  // 33 (array)
    HardcodedGraphicsItem(34, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      37, 0, 2),  // 34 (array)
    HardcodedGraphicsItem(35, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 2),  // 35 (array)
    HardcodedGraphicsItem(36, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      35, 1, 2),  // 36 (array)
    HardcodedGraphicsItem(37, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 0, 2),  // 37 (array)
    HardcodedGraphicsItem(38, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 0, 7),  // 38 (array)
    HardcodedGraphicsItem(39, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      38, 0, 7),  // 39 (array)
    HardcodedGraphicsItem(40, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      -1, 1, 1),  // 40 (array)
    HardcodedGraphicsItem(41, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 3),  // 41 (array)
    HardcodedGraphicsItem(42, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      41, 1, 3),  // 42 (array)
    HardcodedGraphicsItem(43, HardcodedGraphicsItem::HITEMSTATE_NORMAL,     44),        // 43
    HardcodedGraphicsItem(44, HardcodedGraphicsItem::HITEMSTATE_NORMAL_MASK),           // 44
    HardcodedGraphicsItem(45, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      47, 1, 3),  // 45 (array)
    HardcodedGraphicsItem(46, HardcodedGraphicsItem::HITEMSTATE_NORMAL),                // 46
    HardcodedGraphicsItem(47, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 2, 2),  // 47 (array)
    HardcodedGraphicsItem(48, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      49, 0, 2),  // 48 (array)
    HardcodedGraphicsItem(49, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 0, 2),  // 49 (array)
    HardcodedGraphicsItem(50, HardcodedGraphicsItem::HITEMSTATE_ARRAY,      51, 0, 9),  // 50 (array)
    HardcodedGraphicsItem(51, HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 0, 9)   // 51 (array)
};

#include <iostream>
bool HardcodedGraphicsItem::getHDC(int arrayIndex, HDC* colorHDC, HDC* maskHDC)
{
    *colorHDC = nullptr;
    *maskHDC = nullptr;
    HardcodedGraphicsItem* hItemInfoMask = nullptr;
    if (hasMask(arrayIndex))
        hItemInfoMask = &HardcodedGraphicsItem::Get(maskIndex);

    // 3. Getting the HDC
    // Without array:   GfxForm -->                          HardcodedImageObj --> HardcodedImageObj.getHDC
    // With array:      GfxForm --> HardcodedImageObj[X] --> HardcodedImageObj --> HardcodedImageObj.getHDC


    // 3.1 First get the main GFX form
    void* gfxForm = reinterpret_cast<void*>(GM_FORM_GFX);

    // 3.2 Now get the element after the _Form vtbl.
    // 3.2.1 Do with color image
    auto get_HardcodedImageObject = (void* (__stdcall *)(void*)) *(void**)(*(int32_t*)gfxForm + 0x2f8 + index * 4);
    void* hardcodedImageObject = get_HardcodedImageObject(gfxForm);

    // 3.2.2 Same do with the mask if it exist: 
    void* hardcodedImageObjectMask = nullptr;
    if (hItemInfoMask) {
        auto get_HardcodedImageObjectMask = (void* (__stdcall *)(void*)) *(void**)(*(int32_t*)gfxForm + 0x2f8 + maskIndex * 4);
        hardcodedImageObjectMask = get_HardcodedImageObjectMask(gfxForm);
    }

    // 3.3 Get the actual object:
    void* pictureBox = nullptr;
    void* pictureBoxMask = nullptr;

    // It is an array
    if (isArray()) {
        auto getPictureBoxArray = (HRESULT(__stdcall *)(void*, int32_t, void**)) *(void**)(*(int32_t*)hardcodedImageObject + 0x40);
        getPictureBoxArray(hardcodedImageObject, arrayIndex, &pictureBox);
        if (hasMask(arrayIndex)) {
            getPictureBoxArray(hardcodedImageObjectMask, arrayIndex, &pictureBoxMask);
        }
    }
    else { // It is not an array
        pictureBox = hardcodedImageObject;
        pictureBoxMask = hardcodedImageObjectMask;
    }

    // 3.4 Now finally get the HDC
    auto _IPictureBox_getHDC = (void(__stdcall *)(void*, HDC*)) *(void**)(*(int32_t*)pictureBox + 0xE0);
    if(pictureBox != nullptr)
        _IPictureBox_getHDC(pictureBox, colorHDC);
    if (pictureBoxMask != nullptr && hasMask())
        _IPictureBox_getHDC(pictureBoxMask, maskHDC);
    return true;
}

HardcodedGraphicsItem* HardcodedGraphicsItem::getMaskObj()
{
    HardcodedGraphicsItem* possibleMaskObj = &Get(maskIndex);
    if (!possibleMaskObj->isMask())
        return nullptr;
    return possibleMaskObj;
}

HardcodedGraphicsItem& HardcodedGraphicsItem::Get(int index)
{
    return HardcodedGraphics[index - 1];
}

int HardcodedGraphicsItem::Size()
{
    return HardcodedGraphics.size();
}

bool HardcodedGraphicsItem::GetHDCByName(std::string text, HDC* colorHDC, HDC* maskHDC)
{
    // 1. PARSE TEXT
    // If it has an ending, then remove it:
    if (text.find("."))
        text = removeExtension(text);

    // First extract the name:
    if (text.find("hardcoded-") != 0)
        return false;

    // Possible outcome:

    // hardcoded-1-3.png --> 
    //      hardcoded-1-3 
    // --> [0] = hardcoded 
    // --> [1] = 1
    // --> [2] = 3

    // hardcoded-1.png
    // --> [0] = hardcoded
    // --> [1] = 1
    std::vector<std::string> textPattern = LunaLua::StringUtils::split(text, "-");
    if (textPattern.size() != 2 && textPattern.size() != 3) // Size must be 2 or three (see above)
        return false;

    if (!LunaLua::StringUtils::isNumber(textPattern[1])) // 2. value must be a number
        return false;
    if (textPattern.size() == 3) // If size is 3, then 3. value must be a number
        if (!LunaLua::StringUtils::isNumber(textPattern[2]))
            return false;

    // Now convert them to numbers
    int index = atoi(textPattern[1].c_str());
    int arrayIndex = -1;
    if (textPattern.size() == 3)
        arrayIndex = atoi(textPattern[2].c_str());


    // 2. GET HARDCODED OBJECT AND VALIDATE
    // Check if index is in bounds
    if (index > (int)HardcodedGraphics.size() || index < 1)
        return false;

    // Get the item info:
    HardcodedGraphicsItem& hItemInfo = HardcodedGraphicsItem::Get(index);
    HardcodedGraphicsItem* hItemInfoMask = hItemInfo.getMaskObj();

    if (hItemInfo.state == HardcodedGraphicsItem::HITEMSTATE_INVALID) // We cannot access invalid items
        return false;

    // If it is not an array, but the item info tells it is an array, then it is invalid!
    if (arrayIndex == -1 && hItemInfo.isArray())
        return false;

    // If it is an array, but the item info tells it is NOT an array, then it is invalid!
    if (arrayIndex != -1 && !hItemInfo.isArray())
        return false;

    // If it is an array, then do validation further:
    // TODO: Mask Validation

    return hItemInfo.getHDC(arrayIndex, colorHDC, maskHDC);
}
