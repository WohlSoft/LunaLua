#include "HardcodedGraphicsAccess.h"

// Values for index and array-index: https://gist.github.com/KevinW1998/49417ab8b94712bee39c
std::array<HardcodedGraphicsItem, 51> LunaHardcodedGraphicsPatching::HardcodedGraphics =
{
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL),                // 1
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL_MASK),           // 2
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL,      2),        // 3
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL_MASK),           // 4
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL,      4),        // 5
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 5),  // 6 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,       6, 1, 5),  // 7 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL_MASK),           // 8
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL,      8),        // 9
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL),                // 10
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL),                // 11
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_INVALID),               // 12 "Misc"
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL),                // 13
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      -1, 1, 2),  // 14 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      -1, 1, 2),  // 15 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 1),  // 16 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      16, 1, 1),  // 17 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_INVALID),               // 18 "Yoshi"
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL,     20),        // 19
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL_MASK),           // 20
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      22, 1, 2),  // 21 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 2),  // 22 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_INVALID),               // 23 "Mounts"
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 3),  // 24 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      24, 1, 3),  // 25 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      27, 2, 2),  // 26 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 2, 2),  // 27 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_INVALID),               // 28 "Menu"
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 4),  // 29 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      29, 1, 4),  // 30 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_INVALID),               // 31 "Interface"
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 8),  // 32 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      32, 1, 8),  // 33 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      37, 0, 2),  // 34 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 2),  // 35 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      35, 1, 2),  // 36 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 0, 2),  // 37 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 0, 7),  // 38 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      38, 0, 7),  // 39 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      -1, 1, 1),  // 40 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 1, 3),  // 41 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      41, 1, 3),  // 42 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL,     44),        // 43
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL_MASK),           // 44
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      47, 1, 3),  // 45 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_NORMAL),                // 46
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 2, 2),  // 47 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      49, 0, 2),  // 48 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 0, 2),  // 49 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY,      51, 0, 2),  // 50 (array)
    HardcodedGraphicsItem(HardcodedGraphicsItem::HITEMSTATE_ARRAY_MASK, -1, 0, 2)   // 51 (array)
};