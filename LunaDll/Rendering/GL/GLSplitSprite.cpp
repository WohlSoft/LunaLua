#include "GLSplitSprite.h"

using namespace gl;

GLSplitSprite::GLSplitSprite(void* data, GLenum format, int32_t width, int32_t height) :
valid(false), segsTall(0), segsWide(0), subSprites(), width(width), height(height)
{
    int32_t maxTextureSize = getMaxTextureSize();
    if (width <= 0 || height <= 0) return;

    segsWide = (width + maxTextureSize - 1) / maxTextureSize;
    segsTall = (height + maxTextureSize - 1) / maxTextureSize;

    if (segsWide == 0 || segsTall == 0) return;

    for (int32_t i = 0; i < segsTall; i++) {
        int32_t yOff = i * maxTextureSize;
        int32_t subHeight = (height - yOff < maxTextureSize) ? height - yOff : maxTextureSize;

        for (int32_t j = 0; j < segsWide; j++) {
            int32_t xOff = j * maxTextureSize;
            int32_t subWidth = (width - xOff < maxTextureSize) ? width - xOff : maxTextureSize;

            GLBasicSprite* sprite;

            sprite = new GLBasicSprite(data, format, width, height, xOff, yOff, subWidth, subHeight);
            if (sprite == NULL || !sprite->IsValid())
            {
                if (sprite) delete sprite;
                return;
            }
            subSprites.push_back(sprite);
        }
    }
    
    valid = true;
}

GLSplitSprite::~GLSplitSprite() {
    for (uint32_t i = 0; i < subSprites.size(); i++) {
        delete subSprites[i];
        subSprites[i] = NULL;
    }
    subSprites.clear();
}

void GLSplitSprite::Draw(const SRect<double>& dest, const SRect<double>& src, float opacity, GLDraw::RenderMode mode) const {
    if (!valid) return;

    if (dest.isEmpty() || src.isEmpty()) return;
    
    // Trim the coordinates to fit the texture
    SRect<double> tSrc = src.intersection(SRect<double>::fromXYWH(0, 0, width, height));
    if (tSrc.isEmpty()) return;
    SRect<double> tDest = dest.shrinkProportionately(src, tSrc);
    if (tDest.isEmpty()) return;

    int32_t maxTextureSize = getMaxTextureSize();

    int32_t initXSeg = (int32_t)std::floor(tSrc.x1 / maxTextureSize);
    int32_t initYSeg = (int32_t)std::floor(tSrc.y1 / maxTextureSize);
    int32_t maxXSeg = (int32_t)std::ceil(tSrc.x2 / maxTextureSize);
    int32_t maxYSeg = (int32_t)std::ceil(tSrc.y2 / maxTextureSize);
    if (initXSeg < 0) initXSeg = 0;
    if (initYSeg < 0) initYSeg = 0;
    if (maxXSeg > segsWide - 1) maxXSeg = segsWide - 1;
    if (maxYSeg > segsTall - 1) maxYSeg = segsTall - 1;
    for (int32_t i = initYSeg; i < (int32_t)segsTall && i <= maxYSeg; i++) {
        double yOff = i * (double)maxTextureSize;
        
        for (int32_t j = initXSeg; j < (int32_t)segsWide && j <= maxXSeg; j++) {
            double xOff = j * (double)maxTextureSize;

            // Calculate intersecting of this subtexture with the src rect, and get corrosponding dest rect
            SRect<double> subRect = SRect<double>::fromXYWH(xOff, yOff, maxTextureSize, maxTextureSize);
            SRect<double> subSrc = tSrc.intersection(subRect);
            if (subSrc.isEmpty()) continue;
            SRect<double> subDest = tDest.shrinkProportionately(tSrc, subSrc);
            if (subDest.isEmpty()) continue;
            
            // Subtract offset of the sub-texture...
            SRect<double> relSubSrc(
                subSrc.x1 - xOff,
                subSrc.y1 - yOff,
                subSrc.x2 - xOff,
                subSrc.y2 - yOff
                );
            
            const GLBasicSprite* subSprite = subSprites[i*segsWide+j];
            if (subSprite) {
                subSprite->Draw(subDest, relSubSrc, opacity, mode);
            }
        }
    }
}

void GLSplitSprite::BindTexture() const {
    if (!valid) return;

    subSprites[0]->BindTexture();
}