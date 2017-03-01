#ifndef LunaImage_hhhh
#define LunaImage_hhhh

#include <cstdint>
#include <mutex>
#include <memory>

// Forward declerations
struct HDC__;
typedef HDC__ *HDC;
struct HBITMAP__;
typedef HBITMAP__ *HBITMAP;
class GLSprite;

class LunaImage : public std::enable_shared_from_this<LunaImage>
{
public:
    static std::shared_ptr<LunaImage> fromHDC(HDC hdc);
    static std::shared_ptr<LunaImage> fromFile(const wchar_t* filename);
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
    void load(const wchar_t* file);
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
    virtual ~LunaImage();
    
    HBITMAP asHBITMAP();
    uint64_t getUID() { return uid; }
    inline void* getDataPtr() { return data; }
    uint32_t getW() { return w; }
    uint32_t getH() { return h; }
    void lock() { mut.lock(); }
    void unlock() { mut.unlock(); }
    std::shared_ptr<LunaImage> getMask() { return mask; }
    void setMask(std::shared_ptr<LunaImage> _mask) { mask = std::move(_mask); }

    // Function to attempt to convert an image with mask, into RGBA without
    // mask, but only if it's completely safe to do so without inaccuracy.
    bool tryMaskToRGBA();

    // Basic draw operation
    void draw(int dx, int dy, int w, int h, int sx, int sy, bool drawMask = true, bool drawMain = true);
};

#endif
