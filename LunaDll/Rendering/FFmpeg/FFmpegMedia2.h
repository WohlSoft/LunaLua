#pragma once
#include "FFmpeg.h"
#include "FFmpegDecodeComponent.h"

#include <string>
#define FAUDIO FFmpegMedia2::AUDIO
#define FVIDEO FFmpegMedia2::VIDEO
#define FMASK FFmpegMedia2::MASK
#define FMEDIA_NUM 3



class FFmpegMedia2 {
	

	
private:
	
	void init();

	
	

	void loadVideo(std::wstring filepath);
	void loadAudio(std::wstring filepath);
	void loadMask(std::wstring filepath);


	int loadMedia(std::wstring filepath,AVFormatContext*& dest, enum AVMediaType mediaType);

	bool openFile(std::wstring filepath,AVFormatContext** dest);
	bool setStreamInfo(AVFormatContext* fmt);
	int getBestStreamIndex(AVFormatContext* fmt, AVCodec** codec, enum AVMediaType mediaType);
	bool openCodec(AVFormatContext* fmt, int streamidx,AVCodec* codec);
	int readFrame(AVFormatContext* fmt, int streamIdx, AVPacket* pkt);
	void getAlignedSize(AVCodecContext* videoCodec, int* alW, int* alH);

	AVFormatContext* fmtCtx[3];
	int streamIdx[3];
	AVCodecContext* codecCtx[3];
	std::wstring findMask(std::wstring mainFilePath);
public:
	enum MediaType {
		INVALID = -1, AUDIO = 0, VIDEO = 1, MASK = 2
	};
	FFmpegMedia2(std::wstring mainVid, std::wstring mainAud, std::wstring mask);

	//FFmpegMedia2(std::wstring mainVid, std::wstring mainAud) :FFmpegMedia2(mainVid, mainAud, NULL) {};
	FFmpegMedia2(std::wstring main, std::wstring mask) :FFmpegMedia2(main, main, mask) {};

	FFmpegMedia2(std::wstring main);

	int readMediaFrame(MediaType mt, AVPacket* pkt);
	FFmpegMedia2();
	~FFmpegMedia2();
	FFmpegVideoDecodeComponent2* getVideoDecodeComponent();
	FFmpegVideoDecodeComponent2* getMaskDecodeComponent();
	FFmpegAudioDecodeComponent2* getAudioDecodeComponent();

	//double getAvgFrameRate(MediaType mt);
	//double getTimeBase(MediaType mt);
	//bool mediaAvailable(MediaType mt);

	inline bool mediaAvailable(MediaType mt) {
		return streamIdx[mt] >= 0;
	}

	inline double getAvgFrameRate(MediaType mt) {
		return av_q2d(fmtCtx[mt]->streams[streamIdx[mt]]->avg_frame_rate);
	}

	inline double getTimeBase(MediaType mt) {
		return av_q2d(fmtCtx[mt]->streams[streamIdx[mt]]->time_base);
	}

	inline AVRational getTimeBaseQ(MediaType mt) {
		return fmtCtx[mt]->streams[streamIdx[mt]]->time_base;
	}

	int FFmpegMedia2::seek(MediaType mt, int64_t pos);
};

typedef  FFmpegMedia2::MediaType FMEDIA;