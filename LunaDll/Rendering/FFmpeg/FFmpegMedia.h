#ifndef A_FFMPEGMHDR
#define A_FFMPEGMHDR

#include "FFmpeg.h"
#include "FFmpegDecodeQueue.h"
#include <Windows.h>

#include <string>

class FFmpegMedia {
public:
	FFmpegMedia();
	FFmpegMedia(std::wstring filepath);
	~FFmpegMedia();
	bool isVideoAvailable() const;
	bool isAudioAvailable() const;
	bool isMaskAvailable() const;
	int width() const;
	int height() const;
	AVRational FPS;
	AVStream *video, *audio,*mask;
	double duration() const;
	AVFormatContext* fmtCtx,*mFmtCtx;
	AVCodecContext* vidCodecCtx, *audCodecCtx,*maskCodecCtx;
	int vidStreamIdx, audStreamIdx,maskStreamIdx;
	void DebugMsgBox(LPCSTR pszFormat, ...)
	{
		va_list	argp;
		char pszBuf[256];
		va_start(argp, pszFormat);
		vsprintf(pszBuf, pszFormat, argp);
		va_end(argp);
		MessageBoxA(NULL, pszBuf, "debug info", MB_OK);
	}
private:
	void init();
	
	
	bool _videoAvailable, _audioAvailable,_maskAvailable;
};

#endif