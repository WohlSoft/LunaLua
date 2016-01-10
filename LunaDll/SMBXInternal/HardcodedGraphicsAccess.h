#ifndef HardcodedGraphicsAccess_hhhh
#define HardcodedGraphicsAccess_hhhh

#include <cstdint>
#include "../Defines.h"
#include "../Misc/MiscFuncs.h"
#include "../GlobalFuncs.h"
#include <vector>
#include <utility>
#include <array>


/*
    This struct describes internal information about a hardcoded graphic in GM_FORM_GFX
*/
struct HardcodedGraphicsItem {
    enum HardcodedGraphicsItemState {
        HITEMSTATE_NORMAL,          // A normal hardcoded graphic
        HITEMSTATE_NORMAL_MASK,     // A mask hardcoded graphic
        HITEMSTATE_ARRAY,           // An array of normal hardcoded graphics
        HITEMSTATE_ARRAY_MASK,      // An array of mask hardcoded graphics
        HITEMSTATE_INVALID          // Other form elements which are not a VB6 Picture Box
    };

    HardcodedGraphicsItemState state;   // The type/state of the graphic/form
    int maskIndex;                      // The mask of that image. If no mask, then the value is -1
    int minItem;                        // The min value of the array (lower bound). -1 if none
    int maxItem;                        // The max value of the array (upper bound). -1 if none
    
    // Constructor for constructing normal or invalid items
    HardcodedGraphicsItem(HardcodedGraphicsItemState stateParam) : HardcodedGraphicsItem(stateParam, -1) {}
    // Constructor for constructing normal items with mask
    HardcodedGraphicsItem(HardcodedGraphicsItemState stateParam, int maskIndexParam) : HardcodedGraphicsItem(stateParam, maskIndexParam, -1, -1) {}
    // Constructor for constructing array items with and without mask
    HardcodedGraphicsItem(HardcodedGraphicsItemState stateParam, int maskIndexParam, int minItemParam, int maxItemParam)
        : state(stateParam), maskIndex(maskIndexParam), minItem(minItemParam), maxItem(maxItemParam)  {}

    inline bool isMask() { return state == HITEMSTATE_ARRAY_MASK || state == HITEMSTATE_NORMAL_MASK; }
    inline bool hasMask() { return maskIndex != -1; }
    inline bool isArray() { return state == HITEMSTATE_ARRAY || state == HITEMSTATE_ARRAY_MASK; }
};

namespace LunaHardcodedGraphicsPatching 
{
// This is the actual array for the hardcoded graphics
// DON'T USE DIRECTLY, USE getHardcodedGraphicsItem INSTEAD! (Array is 1-indexed)
extern std::array<HardcodedGraphicsItem, 51> HardcodedGraphics;
static inline HardcodedGraphicsItem& getHardcodedGraphicsItem(int index) { return HardcodedGraphics[index - 1]; }
static inline int getHardcodedGraphicsItemSize() { return HardcodedGraphics.size(); }

static inline void getHDCFromIndex(int index, int arrayIndex, HDC* colorHDC, HDC* maskHDC) {
    HardcodedGraphicsItem& hItemInfo = getHardcodedGraphicsItem(index);
    HardcodedGraphicsItem* hItemInfoMask = nullptr;
    if (hItemInfo.hasMask())
        hItemInfoMask = &getHardcodedGraphicsItem(hItemInfo.maskIndex);

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
        auto get_HardcodedImageObjectMask = (void* (__stdcall *)(void*)) *(void**)(*(int32_t*)gfxForm + 0x2f8 + hItemInfo.maskIndex * 4);
        hardcodedImageObjectMask = get_HardcodedImageObjectMask(gfxForm);
    }

    // 3.3 Get the actual object:
    void* pictureBox = nullptr;
    void* pictureBoxMask = nullptr;

    // It is an array
    if (hItemInfo.isArray()) {
        auto getPictureBoxArray = (HRESULT(__stdcall *)(void*, int32_t, void**)) *(void**)(*(int32_t*)hardcodedImageObject + 0x40);
        getPictureBoxArray(hardcodedImageObject, arrayIndex, &pictureBox);
        if (hItemInfo.hasMask()) {
            getPictureBoxArray(hardcodedImageObjectMask, arrayIndex, &pictureBoxMask);
        }
    }
    else { // It is not an array
        pictureBox = hardcodedImageObject;
        pictureBoxMask = hardcodedImageObjectMask;
    }

    // 3.4 Now finally get the HDC
    auto _IPictureBox_getHDC = (void(__stdcall *)(void*, HDC*)) *(void**)(*(int32_t*)pictureBox + 0xE0);
    _IPictureBox_getHDC(pictureBox, colorHDC);
    if (hItemInfo.hasMask())
        _IPictureBox_getHDC(pictureBoxMask, maskHDC);
    return;
}

// i.e. hardcoded-numId-arrayId.bmp
static inline bool getHDCForHardcodedGraphicName(std::string text, HDC* colorHDC, HDC* maskHDC)
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
    std::vector<std::string> textPattern = split(text, '-');
    if (textPattern.size() != 2 && textPattern.size() != 3) // Size must be 2 or three (see above)
        return false;

    if (!is_number(textPattern[1])) // 2. value must be a number
        return false;
    if (textPattern.size() == 3) // If size is 3, then 3. value must be a number
        if (!is_number(textPattern[2]))
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
    HardcodedGraphicsItem& hItemInfo = getHardcodedGraphicsItem(index);
    HardcodedGraphicsItem* hItemInfoMask = nullptr;
    if (hItemInfo.hasMask())
        hItemInfoMask = &getHardcodedGraphicsItem(hItemInfo.maskIndex);

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

    getHDCFromIndex(index, arrayIndex, colorHDC, maskHDC);
    return true;
}

}


#endif