#ifndef FreeImageGifWriter_hhhhh
#define FreeImageGifWriter_hhhhh
#include <FreeImage.h>
#include <string>

class FreeImageGifWriter
{
private:
    std::string m_filename;
    FIMULTIBITMAP* m_gifHandle;

public:
    FreeImageGifWriter(const std::string& filename);
    ~FreeImageGifWriter();

    void closeAndCleanup();
    inline bool isOpen() { return m_gifHandle != nullptr; }

    void add24bitBGRDataPage(int width, int height, BYTE* pData);
};

#endif


