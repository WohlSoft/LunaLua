#include "GLSplitSprite.h"

GLSplitSprite::GLSplitSprite(void* data, GLint format, int32_t width, int32_t height) :
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

void GLSplitSprite::Draw(int xDest, int yDest, int dWidth, int dHeight, int xSrc, int ySrc, GLDraw::RenderMode mode) const {
    if (!valid) return;

    if (dWidth <= 0) return;
    if (dHeight <= 0) return;
    if (xSrc >= (int32_t)width) return;
    if (ySrc >= (int32_t)height) return;
    if (xSrc + dWidth <= 0) return;
    if (ySrc + dHeight <= 0) return;
    if ((int32_t)width - xSrc < dWidth) dWidth = (int32_t)width - xSrc;
    if ((int32_t)height - ySrc < dHeight) dHeight = (int32_t)height - ySrc;

    int32_t maxTextureSize = getMaxTextureSize();

    int32_t initXSeg = xSrc / maxTextureSize;
    int32_t initYSeg = ySrc / maxTextureSize;
    int32_t maxXSeg = (xSrc + dWidth + maxTextureSize - 1) / maxTextureSize;
    int32_t maxYSeg = (ySrc + dHeight + maxTextureSize - 1) / maxTextureSize;
    for (int32_t i = initYSeg; i < (int32_t)segsTall && i <= maxYSeg; i++) {
        int32_t yOff = i * maxTextureSize;
        int32_t ySrcRel = (int32_t)ySrc - (int32_t)yOff;
        int32_t ySrcSub = (ySrcRel >= 0) ? ySrcRel : 0;
        int32_t yDestOffset = ySrcSub - ySrcRel;

        int32_t subHeight = dHeight - yDestOffset;
        subHeight = (subHeight < (int32_t)maxTextureSize - ySrcSub) ? subHeight : (int32_t)maxTextureSize - ySrcSub;

        if (subHeight <= 0) break;
        
        for (int32_t j = initXSeg; j < (int32_t)segsWide && j <= maxXSeg; j++) {
            int32_t xOff = j * maxTextureSize;
            
            int32_t xSrcRel = (int32_t)xSrc - (int32_t)xOff;
            int32_t xSrcSub = (xSrcRel >= 0) ? xSrcRel : 0;
            int32_t xDestOffset = xSrcSub - xSrcRel;

            int32_t subWidth = dWidth - xDestOffset;
            subWidth = (subWidth < (int32_t)maxTextureSize - xSrcSub) ? subWidth : (int32_t)maxTextureSize - xSrcSub;

            if (subWidth <= 0) break;

            const GLBasicSprite* subSprite = subSprites[i*segsWide+j];
            if (subSprite) {
                subSprite->Draw(xDest + xDestOffset, yDest + yDestOffset, subWidth, subHeight, xSrcSub, ySrcSub, mode);
            }
        }
    }
}

void GLSplitSprite::BindTexture() const {
    if (!valid) return;

    subSprites[0]->BindTexture();
}