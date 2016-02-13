#ifndef BMPBox_hhhhhh
#define BMPBox_hhhhhh

#include "../GlobalFuncs.h"
#include "../Defines.h"
#include "../../SdlMusic/SdlMusPlayer.h"
#include <SDL2/SDL.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <atomic>

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
	static std::unordered_map<std::wstring, std::shared_ptr<Mix_Chunk>> audList;
	static const enum PixelFormat pixFmt = PIX_FMT_BGRA; 
    
    void Init();
    void MakeColorTransparent(int transparency_color);
    bool ImageLoaded();
	bool VideoLoaded();
	bool AudioLoaded();
	void updateVideoFrame(bool forceReset = false);
	std::shared_ptr<Mix_Chunk> decodeAudioOnce();
	void playAudio();

    void forEachPixelValue(std::function<void(BYTE)> forEachFunc);
    void* getBits();
	static void chunkDeleter(Mix_Chunk* m);

	/// Members ///
	std::wstring m_Filename;  // Original filename
	int m_H;				// Height of bitmap
	int m_W;				// Width of bitmap
	HBITMAP m_hbmp;			// Handle to bitmap data
	HDC m_hdc;				// handle to compatible DC for this bitmap
	double FPS;
	std::atomic<bool> m_modified;       // If the image was modified and needs a reload
	std::shared_ptr<Mix_Chunk> audioDataPtr;
	// Static function
	static BMPBox* loadIfExist(const std::wstring& filename, HDC screen_dc);
	uint32_t lastAudioPlayedTick; //msec
	uint32_t audioDuration; //msec
	int audioSDLChannel;
	int lastDecodedGFrame;
	int lastRenderRequestedGFrame;
	void stopPlay(bool noFlush = false);
	bool musicPlayed;
	static void initMovieCache();

private:
	AVFormatContext* vFmtCont;
	AVCodecContext* vCdcCont, *aCdcCont;
	AVFrame* srcFrame, *destFrame;
	AVPacket vPkt;
	AVRational rFPS;
	SwsContext* swsCont;
	int vStrIdx;
	int aStrIdx;
	int frameFinished;
	uint8_t* vBuffer;
	void* bmpPtr;
	int aL[8];
	int vEndFlag;
	int fCount;
	int fUpd;
	int lfUpd;
	bool audioLoadByCache;

	bool avFrameAlloc();
	SDL_AudioSpec specD, specH;
	SDL_AudioDeviceID dev;
	uint32_t decodedFrameCount;

};

#endif
