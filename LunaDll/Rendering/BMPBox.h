#ifndef BMPBox_hhhhhh
#define BMPBox_hhhhhh


#include "FFmpeg/FFmpeg.h"
#include "../../GlobalFuncs.h"
#include "../Defines.h"
#include "../../SdlMusic/SdlMusPlayer.h"
#include <SDL2/SDL.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <atomic>
#include "FFmpeg/FFmpegMediaPlayer2.h"

#define DEFAULT_TRANS_COLOR 0xFF00DC

// A user-loaded bitmap container
class BMPBox {
public:

    /// Functions ///
    BMPBox();
    ~BMPBox();
    BMPBox(std::wstring filename, HDC screen_dc);
    BMPBox(HBITMAP bitmapData, HDC screen_dc);
    BMPBox(BMPBox&) = default;

    void Init();
    void MakeColorTransparent(int transparency_color);
    bool ImageLoaded()const;

    void forEachPixelValue(std::function<void(BYTE)> forEachFunc);
    void* getBits();

    enum OffScreenMode {
        CONTINUE = 0,
        PAUSE = 1,
        STOP = 2
    };
    enum OnScreenMode {
        NOTHING = 0,
        PLAY = 1
    };

    /// Members ///
    std::wstring m_Filename;        // Original filename
    int m_H;                        // Height of bitmap
    int m_W;                        // Width of bitmap
    HBITMAP m_hbmp;                 // Handle to bitmap data
    HDC m_hdc;                      // handle to compatible DC for this bitmap
    std::atomic<bool> m_modified;   // If the image was modified and needs a reload
    
    // Static function
    static BMPBox* loadIfExist(const std::wstring& filename, HDC screen_dc);
    
    std::unique_ptr<FFmpegMediaPlayer2> mp;
    int maskW, maskH;
    bool hasVideo;
    bool hasMask;
    int scaleUpMode;
    int scaleDownMode;
    bool nowOnScreen;
    void setOnScreen(bool onScreen);
    void setScaleUpMode(int m);
    void setScaleDownMode(int m);
    int getScaleUpMode() const;
    int getScaleDownMode() const;
    void setOffScreenMode(int m);
    int getOffScreenMode() const;
    void setOnScreenMode(int m);
    int getOnScreenMode() const;
    void setVideoDelay(double d);
    double getVideoDelay() const;
    void setMaskDelay(double d);
    void setLoop(bool enable);
    bool getLoop() const;
    void setAltAlpha(int altCh);
    int getAltAlpha() const;
    void setAlphaType(int m);
    int getAlphaType() const;
    void setVolume(int m);
    int getVolume() const;

    double getMaskDelay() const;
    void setCallback(void(*fn)(int));
    void setOnScreenCallback(void(*fn)());
    void setOffScreenCallback(void(*fn)());

    void play();
    void stop();
    void pause();
    void seek(double src);
    void colTest(int scrX, int scrY, int destWidth, int destHeight);
    int maskThreshold[8]; //when value >= maskThreshold it is hurt area
    void procCallback();
    
    void(*clbc)(int);

    //used for offscreen check
    int lastDecodedFrame;
    OffScreenMode offScrMode;
    OnScreenMode onScrMode;
    uint8_t* maskOutput;
    void(*onScrClbk)();
    void(*offScrClbk)();
    bool shouldCallOnScrClbk;
    bool shouldCallOffScrClbk;
private:
    void* bmpPtr;
    bool pendingHarmArr[8];
};

#endif
