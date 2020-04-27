#ifndef LunaImage_hhhh
#define LunaImage_hhhh

#include <cstdint>
#include <mutex>
#include <memory>
#include <atomic>

#include "../Misc/ResourceFileMapper.h"

// Forward declerations
struct HDC__;
typedef HDC__ *HDC;
struct HBITMAP__;
typedef HBITMAP__ *HBITMAP;
class GLSprite;

class LunaImage : public std::enable_shared_from_this<LunaImage>
{
public:
    static std::atomic<uint32_t> totalRawMem;
    static std::atomic<uint32_t> totalCompMem;

public:
    static std::shared_ptr<LunaImage> fromData(int width, int height, const uint8_t* data);
    static std::shared_ptr<LunaImage> getBlank();
    static std::shared_ptr<LunaImage> fromHDC(HDC hdc);
    static std::shared_ptr<LunaImage> fromFile(const wchar_t* filename, const ResourceFileInfo* metadata=nullptr);
public:
    static void holdCachedImages(bool isWorld);
    static void releaseCachedImages(bool isWorld);
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

    void* compressedDataPtr; // Pointer to compressed PNG data
    uint32_t compressedDataSize;
    bool mustKeepData;
    bool isPngImage;
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
        mask(nullptr),
        compressedDataPtr(nullptr),
        compressedDataSize(0),
        mustKeepData(false),
        isPngImage(false)
    {
    }
    virtual ~LunaImage();
    
    HBITMAP asHBITMAP();
    inline uint64_t getUID() { return uid; }
    void* getDataPtr();
    uint32_t getDataPtrAsInt();

    void notifyTextureified();

    inline uint32_t getW() { return w; }
    inline uint32_t getH() { return h; }
    inline void lock() { mut.lock(); }
    inline void unlock() { mut.unlock(); }
    inline std::shared_ptr<LunaImage> getMask() { return mask; }
    inline void setMask(std::shared_ptr<LunaImage> _mask) { mask = std::move(_mask); }

    // Function to attempt to convert an image with mask, into RGBA without
    // mask, but only if it's completely safe to do so without inaccuracy.
    bool tryMaskToRGBA();

    // Function to make a color transparent, for old-style BMP loading things
    void makeColorTransparent(unsigned int rgb);

    // Basic draw operation
    void draw(int dx, int dy, int w, int h, int sx, int sy, bool drawMask = true, bool drawMain = true, float opacity = 1.0f);

private:
    void drawMasked(int dx, int dy, int w, int h, int sx, int sy, bool drawMask = true, bool drawMain = true);
    void drawRGBA(int dx, int dy, int w, int h, int sx, int sy, bool maskOnly, float opacity = 1.0f);
};

#endif
