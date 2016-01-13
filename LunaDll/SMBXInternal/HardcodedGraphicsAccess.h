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
class HardcodedGraphicsItem;
class HardcodedGraphicsItem {
public:
    enum HardcodedGraphicsItemState {
        HITEMSTATE_NORMAL,          // A normal hardcoded graphic
        HITEMSTATE_NORMAL_MASK,     // A mask hardcoded graphic
        HITEMSTATE_ARRAY,           // An array of normal hardcoded graphics
        HITEMSTATE_ARRAY_MASK,      // An array of mask hardcoded graphics
        HITEMSTATE_INVALID          // Other form elements which are not a VB6 Picture Box
    };

    int index;                          // The own index value
    HardcodedGraphicsItemState state;   // The type/state of the graphic/form
    int maskIndex;                      // The mask of that image. If no mask, then the value is -1
    int minItem;                        // The min value of the array (lower bound). -1 if none
    int maxItem;                        // The max value of the array (upper bound). -1 if none
    
    // Constructor for constructing normal or invalid items
    HardcodedGraphicsItem(int indexParam, HardcodedGraphicsItemState stateParam) : 
        HardcodedGraphicsItem(indexParam, stateParam, -1) {}
    // Constructor for constructing normal items with mask
    HardcodedGraphicsItem(int indexParam, HardcodedGraphicsItemState stateParam, int maskIndexParam) :
        HardcodedGraphicsItem(indexParam, stateParam, maskIndexParam, -1, -1) {}
    // Constructor for constructing array items with and without mask
    HardcodedGraphicsItem(int indexParam, HardcodedGraphicsItemState stateParam, int maskIndexParam, int minItemParam, int maxItemParam) :
        index(indexParam), 
        state(stateParam), 
        maskIndex(maskIndexParam), 
        minItem(minItemParam), 
        maxItem(maxItemParam)  {}

    inline bool isMask() { return state == HITEMSTATE_ARRAY_MASK || state == HITEMSTATE_NORMAL_MASK; }
    inline bool hasMask() { return maskIndex != -1; }
    inline bool isArray() { return state == HITEMSTATE_ARRAY || state == HITEMSTATE_ARRAY_MASK; }
    
    bool getHDC(int arrayIndex, HDC* colorHDC, HDC* maskHDC);
    HardcodedGraphicsItem* getMaskObj();

    // Static stuff
    static HardcodedGraphicsItem& Get(int index);
    static int Size();
    static bool GetHDCByName(std::string text, HDC* colorHDC, HDC* maskHDC);
};

#endif