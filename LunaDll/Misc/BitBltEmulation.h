#ifndef BitBltEmulation_hhhh
#define BitBltEmulation_hhhh

#include <Windows.h>

class BitBltEmulation {
// Data storage
private:
    struct {
        bool present;
        HDC src;
        int dx;
        int dy;
        int w;
        int h;
        int sx;
        int sy;
        DWORD rop;
    } m_LastMask;

// Public methods
public:
    BitBltEmulation();
    ~BitBltEmulation();

    void onBitBlt(HDC src, int dx, int dy, int w, int h, int sx, int sy, DWORD rop);
    void flushPendingBlt();

// Drawing implementations
private:
    static void drawMasked(HDC maskSrc, HDC src, int dx, int dy, int w, int h, int sx, int sy);
    static void drawOpaque(HDC src, int dx, int dy, int w, int h, int sx, int sy);
    static void drawBlackRectangle(int dx, int dy, int w, int h);
};

extern BitBltEmulation g_BitBltEmulation;

#endif
