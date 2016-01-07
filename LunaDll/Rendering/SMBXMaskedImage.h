#ifndef SMBXMaskedImage_hhhh
#define SMBXMaskedImage_hhhh

#include <Windows.h>
#include <cstdint>
#include <unordered_map>
#include <memory>

class SMBXMaskedImage;
class BMPBox;

class SMBXMaskedImage
{
// Private static
private:
    // Use shared_ptr not to return, but because multiple HDCs will refer to
    // the same SMBXMaskedImage.
    static std::unordered_map<HDC, std::shared_ptr<SMBXMaskedImage>> lookupTable;

// Public static methods
public:
    static SMBXMaskedImage* get(HDC maskHdc, HDC mainHdc);

    // IMPORTANT: Before this is called, ensure no SMBXMaskedImage remain in use!
    static void clearLookupTable(void);

// Private data storage
private:
    HDC maskHdc;
    HDC mainHdc;
    std::shared_ptr<BMPBox> loadedPngImage;
    std::shared_ptr<BMPBox> rgbaOverrideImage;
    SMBXMaskedImage* maskedOverrideImage;

// Public methods
public:
    SMBXMaskedImage();
    ~SMBXMaskedImage() = default;

    void Draw(int dx, int dy, int w, int h, int sx, int sy, bool drawMask = true, bool drawMain = true);
    void DrawWithOverride(int dx, int dy, int w, int h, int sx, int sy, bool drawMask = true, bool drawMain = true);

    void SetLoadedPng(const std::shared_ptr<BMPBox>& img);
    void SetOverride(const std::shared_ptr<BMPBox>& img);
    void SetOverride(SMBXMaskedImage* img);
    void UnsetOverride();
    std::shared_ptr<BMPBox> GetLoadedPng() { return loadedPngImage; };
    std::shared_ptr<BMPBox> GetRGBAOverride() { return rgbaOverrideImage; };
    SMBXMaskedImage* GetMaskedOverride() { return maskedOverrideImage; };
};

#endif