#ifndef FrameCapture_H
#define FrameCapture_H

#include <memory>
class GLFramebuffer;

class CaptureBuffer : public std::enable_shared_from_this<CaptureBuffer> {
public:
    CaptureBuffer(int w, int h);
    ~CaptureBuffer();
    void captureAt(double priority);
public:
    int mW, mH;
    GLFramebuffer* mFramebuffer;
};

#endif
