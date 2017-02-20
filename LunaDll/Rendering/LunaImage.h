#ifndef LunaImage_hhhh
#define LunaImage_hhhh

#include <cstdint>
#include <mutex>

// Forward declerations
struct HBITMAP__;
typedef HBITMAP__ *HBITMAP;
class GLSprite;

class LunaImageData
{
private:
    static uint64_t getNewUID();
private:
    uint64_t   uid;
    std::mutex mut;
    void*      data;
    uint32_t   w;
    uint32_t   h;
    HBITMAP    hbmp;
private:
    void clearInternal();
public:
    LunaImageData & operator=(const LunaImageData&) = delete;
    LunaImageData(const LunaImageData&) = delete;
    LunaImageData() :
        uid(getNewUID()),

        data(nullptr), w(0), h(0),
        hbmp(nullptr)
    {
    }
    ~LunaImageData()
    {
        clear();
    }

    // Load an image file. If the filename is null or empty, the image will be cleared.
    void load(const wchar_t* file);
    void clear();
    HBITMAP asHBITMAP();
    uint64_t getUID() { return uid; }
    inline void* getDataPtr() { return data; }
    uint32_t getW() { return w; }
    uint32_t getH() { return h; }
    void Lock() { mut.lock(); }
    void Unlock() { mut.unlock(); }
};

class LunaImage
{
private:
    LunaImageData m_img;
    LunaImageData m_mask;
public:
    LunaImage & operator=(const LunaImage&) = delete;
    LunaImage(const LunaImage&) = delete;
    LunaImage();

    // Loads an image file. The mask is optional.
    void load(const wchar_t* imgFile, const wchar_t* maskFile);
};

#endif
