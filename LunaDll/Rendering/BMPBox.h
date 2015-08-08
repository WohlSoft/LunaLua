#ifndef BMPBox_hhhhhh
#define BMPBox_hhhhhh

#include "../Defines.h"
#include <string>
#include <functional>

#define DEFAULT_TRANS_COLOR 0xFF00DC

// A user-loaded bitmap container
class BMPBox {
public:

    /// Functions ///
    BMPBox();
    ~BMPBox();
    BMPBox(std::wstring filename, HDC screen_dc);
    void Init();
    void MakeColorTransparent(int transparency_color);
    bool ImageLoaded();

    void forEachPixelValue(std::function<void(BYTE)> forEachFunc);


    /// Members ///
    int m_H;				// Height of bitmap
    int m_W;				// Width of bitmap
    HBITMAP m_hbmp;			// Handle to bitmap data
    HDC m_hdc;				// handle to compatible DC for this bitmap
};

#endif
