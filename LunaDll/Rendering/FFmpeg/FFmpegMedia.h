#ifndef A_FFMPEGMHDR
#define A_FFMPEGMHDR

#include "FFmpeg.h"
#include "FFmpegDecodeQueue.h"

#include <string>

class FFmpegMedia {
public:
	FFmpegMedia();
	FFmpegMedia(std::wstring filepath);
	
	bool isVideoAvailable() const;
	bool isAudioAvailable() const;
	int width() const;
	int height() const;
	double duration() const;
	AVFormatContext* fmtCtx;
	AVCodecContext* vidCodecCtx, *audCodecCtx;
	int vidStreamIdx, audStreamIdx;
private:
	void init();
	
	
	bool _videoAvailable, _audioAvailable;
};

#endif