#include "BitBltEmulation.h"
#include "../Defines.h"
#include "../Globals.h"
#include "../Rendering/RenderOverrideManager.h"
#include "../Rendering/GLEngineProxy.h"

// Global instance
BitBltEmulation g_BitBltEmulation;

////////////////////
// Public Methods //
////////////////////

BitBltEmulation::BitBltEmulation() :
    m_LastMask()
{}

BitBltEmulation::~BitBltEmulation()
{}

void BitBltEmulation::onBitBlt(HDC src, int dx, int dy, int w, int h, int sx, int sy, DWORD rop)
{

    if (m_LastMask.present)
    {

        if (rop == SRCPAINT &&
            m_LastMask.dx == dx && m_LastMask.dy == dy &&
            m_LastMask.w == w && m_LastMask.h == h &&
            m_LastMask.sx == sx && m_LastMask.sy == sy)
        {
            // If the last operation was a SRCAND mask, and this operation is a
            // matching SRCPAINT, draw as a unified operation.
            drawMasked(m_LastMask.src, src, dx, dy, w, h, sx, sy);
            m_LastMask.present = false;

            // We've handled the current operation plus the last one, so exit simply
            return;
        }
        else
        {
            // If we're getting a new drawing operating that is not a matching
            // SRCPAINT, flush the mask drawing before handling the new
            flushPendingBlt();
        }
    }

    if (rop == SRCAND)
    {
        // If this is a mask operation, record it
        m_LastMask.src = src;
        m_LastMask.dx = dx;
        m_LastMask.dy = dy;
        m_LastMask.w = w;
        m_LastMask.h = h;
        m_LastMask.sx = sx;
        m_LastMask.sy = sy;
        m_LastMask.rop = rop;
        m_LastMask.present = true;
    }
    else if (rop == SRCPAINT)
    {
        // If this is for masked drawing but we don't have a mask... oh well
        drawMasked(nullptr, src, dx, dy, w, h, sx, sy);
    }
    else if (rop == SRCCOPY)
    {
        // If this is for opaque drawing, handle appropriately
        drawOpaque(src, dx, dy, w, h, sx, sy);
    }
    else if (src == nullptr && (rop == BLACKNESS || rop == 0x10))
    {
        // If this is for masked drawing but we don't have a mask... oh well
        drawBlackRectangle(dx, dy, w, h);
    }
}

void BitBltEmulation::flushPendingBlt()
{
    if (m_LastMask.present)
    {
        drawMasked(
            m_LastMask.src, nullptr,
            m_LastMask.dx, m_LastMask.dy,
            m_LastMask.w, m_LastMask.h,
            m_LastMask.sx, m_LastMask.sy
            );
        m_LastMask.present = false; 
    }
}

//////////////////////////////////
// Drawing Type Implementations //
//////////////////////////////////

void BitBltEmulation::drawMasked(HDC maskSrc, HDC src, int dx, int dy, int w, int h, int sx, int sy)
{
    if (maskSrc == nullptr && src == nullptr) return;

    // TODO: In the future, based on maskSrc and src, get an "image object"
    //       that will handle rendering.

    // TODO: Modify RenderOverrideManager to handle the case of mask-only rendering
    if ((src != nullptr) && gRenderOverride.renderOverrideBitBlt(dx, dy, w, h, src, sx, sy))
    {
    }
    else if (g_GLEngine.IsEnabled())
    {
        // TODO: Implement GLEngine masked image rendering as a single call of some sort
        if (maskSrc != nullptr)
            g_GLEngine.EmulatedBitBlt(dx, dy, w, h, maskSrc, sx, sy, SRCAND);
        if (src != nullptr)
            g_GLEngine.EmulatedBitBlt(dx, dy, w, h, src, sx, sy, SRCPAINT);
    }
    else
    {
        if (maskSrc != nullptr)
            BitBlt((HDC)GM_SCRN_HDC, dx, dy, w, h, maskSrc, sx, sy, SRCAND);
        if (src != nullptr)
            BitBlt((HDC)GM_SCRN_HDC, dx, dy, w, h, src, sx, sy, SRCPAINT);
    }
}

void BitBltEmulation::drawOpaque(HDC src, int dx, int dy, int w, int h, int sx, int sy)
{
    if (src == nullptr) return;

    // TODO: In the future, based on src, get an "image object" that will
    //       handle rendering.

    // TODO: Modify RenderOverrideManager to handle the case of mask-only rendering
    if ((src != nullptr) && gRenderOverride.renderOverrideBitBlt(dx, dy, w, h, src, sx, sy))
    {
    }
    else if (g_GLEngine.IsEnabled())
    {
        g_GLEngine.EmulatedBitBlt(dx, dy, w, h, src, sx, sy, SRCCOPY);
    }
    else
    {
        BitBlt((HDC)GM_SCRN_HDC, dx, dy, w, h, src, sx, sy, SRCCOPY);
    }
}

void BitBltEmulation::drawBlackRectangle(int dx, int dy, int w, int h)
{
    // TODO: Consider some more proper form of rectangle drawing

    if (g_GLEngine.IsEnabled())
    {
        g_GLEngine.EmulatedBitBlt(dx, dy, w, h, nullptr, 0, 0, 0x10);
    }
    else
    {
        BitBlt((HDC)GM_SCRN_HDC, dx, dy, w, h, nullptr, 0, 0, 0x10);
    }
}

