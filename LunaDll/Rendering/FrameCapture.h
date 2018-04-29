#ifndef FrameCapture_H
#define FrameCapture_H

#include <memory>
class GLFramebuffer;

class CaptureBuffer : public std::enable_shared_from_this<CaptureBuffer> {
public:
    CaptureBuffer(int w, int h, bool nonskippable);
    ~CaptureBuffer();
    void CaptureAt(double priority);
    void Clear(double priority);

    // Only call this from the GL thread
    void EnsureFramebufferExists();
public:
    int mW, mH;
	bool mNonskippable;
    GLFramebuffer* mFramebuffer;
};

#endif
