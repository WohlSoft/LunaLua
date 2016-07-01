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

    // Registration of custom images that can be overridden
    static std::unordered_map<std::string, std::shared_ptr<SMBXMaskedImage>> customOverridable;

// Public static methods
public:
    static SMBXMaskedImage* Get(HDC maskHdc, HDC mainHdc);

    // IMPORTANT: Before this is called, ensure no SMBXMaskedImage remain in use!
    static void ClearLookupTable(void);

    static SMBXMaskedImage* GetCharacterSprite(short charId, short powerup);
    static SMBXMaskedImage* GetHardcoded(const std::string& name);
    static SMBXMaskedImage* GetByName(const std::string& t, int index);

    static SMBXMaskedImage* RegisterCustomOverridable(const std::string& t, int index, const std::shared_ptr<BMPBox>& img);
    static void UnregisterCustomOverridable(const std::string& t, int index);

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
    bool HasLoadedPng() { return (bool)loadedPngImage; };
    std::shared_ptr<BMPBox> GetLoadedPng() { return loadedPngImage; };
    std::shared_ptr<BMPBox> GetRGBAOverride() { return rgbaOverrideImage; };
    SMBXMaskedImage* GetMaskedOverride() { return maskedOverrideImage; };

    void getSize(int& w, int& h, bool followOverride = true);

    void QueueDraw(double x, double y, double w, double h, double sx, double sy, bool sceneCoords, double priority=2.0);
};

#endif