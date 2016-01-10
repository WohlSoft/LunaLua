#ifndef FreeImagePngWriter_hhhhh
#define FreeImagePngWriter_hhhhh

#include <string>
#define FREEIMAGE_LIB
#include <FreeImageLite.h>

class FreeImageData
{
    FIBITMAP* m_bitmap;
public:
    
    enum class ColorMode {
        COLORMODE_BGR,
        COLORMODE_RGB
    };
    
    FreeImageData();
    ~FreeImageData();

    bool loadFile(const std::string& filename);
    bool saveFile(const std::string& filename) const;
    bool isValid() const;

    void init(int width, int height, BYTE* pData, ColorMode color = ColorMode::COLORMODE_BGR, int bpp = -1);
    void reset();
    HBITMAP toHBITMAP();
};

#endif
