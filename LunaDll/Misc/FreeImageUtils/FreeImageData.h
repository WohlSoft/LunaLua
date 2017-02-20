#ifndef FreeImagePngWriter_hhhhh
#define FreeImagePngWriter_hhhhh

#include <string>
#define FREEIMAGE_LIB
#include <FreeImageLite.h>

// Forward declerations
struct HBITMAP__;
typedef HBITMAP__ *HBITMAP;

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
    bool loadFile(const std::wstring& filename);
    bool loadMem(unsigned char *data, unsigned long size, const std::string& filename);
    bool saveFile(const std::string& filename) const;
    bool isValid() const;

    void init(int width, int height, BYTE* pData, ColorMode color = ColorMode::COLORMODE_BGR, int bpp = -1);
    void reset();
    HBITMAP toHBITMAP();
    uint32_t getWidth();
    uint32_t getHeight();
    bool toRawBGRA(void* out);
};

#endif
