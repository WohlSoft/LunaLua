#ifndef SMBXMaskedImage_hhhh
#define SMBXMaskedImage_hhhh

#include <Windows.h>
#include <cstdint>
#include <unordered_map>
#include <memory>

class SMBXMaskedImage;

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

// Public methods
public:
    SMBXMaskedImage();
    ~SMBXMaskedImage() = default;

    void Draw(int dx, int dy, int w, int h, int sx, int sy, bool maskOnly = false);
};

#endif