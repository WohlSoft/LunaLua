#ifndef LunaImage_hhhh
#define LunaImage_hhhh

#include <cstdint>
#include <mutex>
#include <memory>

// Forward declerations
struct HBITMAP__;
typedef HBITMAP__ *HBITMAP;
class GLSprite;

class LunaImage
{
private:
    static uint64_t getNewUID();
private:
    uint64_t   uid;  // Unique identifier for the currently loaded image state
    std::mutex mut;  // Mutex for loading/unloading
    void*      data; // Pointer to BGRA image data
    uint32_t   w;    // Image width
    uint32_t   h;    // Image height
    HBITMAP    hbmp; // Cached HBITMAP if this
private:
    // Pointer to an associated mask image
    std::shared_ptr<LunaImage> mask;
private:
    void clearInternal();
public:
    LunaImage & operator=(const LunaImage&) = delete;
    LunaImage(const LunaImage&) = delete;
    LunaImage() :
        uid(getNewUID()),

        data(nullptr), w(0), h(0),
        hbmp(nullptr),
        mask(nullptr)
    {
    }
    ~LunaImage()
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
    std::shared_ptr<LunaImage> getMask() { return mask; }
    void setMask(std::shared_ptr<LunaImage> _mask) { mask = std::move(_mask); }
};

#endif
