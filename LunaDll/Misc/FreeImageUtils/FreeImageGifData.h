#ifndef FreeImageGifWriter_hhhhh
#define FreeImageGifWriter_hhhhh
#include <Windows.h>
#include <FreeImage.h>
#include <string>

class FreeImageGifData
{
private:
    std::string m_filename;
    FIMULTIBITMAP* m_gifHandle;

public:
    FreeImageGifData(const std::string& filename, bool newInit = true);
    ~FreeImageGifData();

    void closeAndCleanup();
    inline bool isOpen() { return m_gifHandle != nullptr; }

    void add24bitBGRDataPage(int width, int height, BYTE* pData);
    
    inline int count() 
    { 
        if (isOpen()) 
            return FreeImage_GetPageCount(m_gifHandle);  
        return 0;
    };
    HBITMAP getFrame(int index);
    int getDelayValue(int index);

};

#endif


