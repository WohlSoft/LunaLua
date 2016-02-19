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
#include "FFmpeg/FFmpegMediaPlayer.h"

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

	void DebugMsgBox(LPCSTR pszFormat, ...);
    
    void Init();
    void MakeColorTransparent(int transparency_color);
    bool ImageLoaded();

    void forEachPixelValue(std::function<void(BYTE)> forEachFunc);
    void* getBits();

	/// Members ///
	std::wstring m_Filename;  // Original filename
	int m_H;				// Height of bitmap
	int m_W;				// Width of bitmap
	HBITMAP m_hbmp;			// Handle to bitmap data
	HDC m_hdc;				// handle to compatible DC for this bitmap
	std::atomic<bool> m_modified;       // If the image was modified and needs a reload
	// Static function
	static BMPBox* loadIfExist(const std::wstring& filename, HDC screen_dc);
	FFmpegMediaPlayer* mp;
	FFmpegDecodeSetting ffdset;
	FFmpegDecodeSetting realffdset;
	bool hasVideo;
	int scaleUpMode;
	int scaleDownMode;
	void setOnScreen(bool onScreen);
	void setScaleUpMode(int m);
	void setScaleDownMode(int m);
	void play();
	void stop();
	void pause();
	void seek(double src);
	void colTest(int scrX, int scrY, int destWidth, int destHeight);
	static void procPendingHarm();
	void setHurtMode(int m);
	int getHurtMode() const;
	//int hurtMode;//0= no hurt,1=use alphachannel,2=use custom hurt mask
	//int hurtMaskIndex;//stream index
	int maskThreshold[8];//when value >= maskThreshold it is hurt area
	//luabind::object* callbacks[8];
	void procCallback();
	void setVideoDelay(double d);
	void setCallback(void(*fn)(int));
	void(*clbc)(int);
private:
	void* bmpPtr;
	static bool pendingHarm;
	bool pendingHarmArr[8];
};

#endif
