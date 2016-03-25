#pragma once
#include "FFmpeg.h"
#include <Windows.h>
struct FFmpegVideoDecodeComponent2{
public:
	static const AVPixelFormat DEFALUT_OUT_PIX_FMT = AV_PIX_FMT_BGRA;
	AVFrame* vsrc, *vdest;
	uint8_t* vbuf;
	SwsContext* swsctx;
	AVCodecContext* cdcctx;
	int reqW, reqH;
	FFmpegVideoDecodeComponent2();
	FFmpegVideoDecodeComponent2(AVCodecContext* src);
	~FFmpegVideoDecodeComponent2();

	int decode(AVPacket* pkt,AVFrame** decodeUsed);
	int resize(AVFrame* decoded, AVFrame** out_resized);
	virtual void flush();
private:
	void initVar();
	bool init(AVCodecContext* srcCodec);
	bool setSwsCtx(AVCodecContext* srcCodec,int outW,int outH, AVPixelFormat outPixFmt);
	bool setVideoBuffer(int outW, int outH, AVPixelFormat outPixFmt);
	bool allocVideoFrame();
	void getAlignedSize(AVCodecContext* videoCodec, int* alW, int* alH);
	CRITICAL_SECTION  crSect;
};

struct FFmpegAudioDecodeComponent2{
	static const int DEFAULT_OUT_CHANNELS = 2;
	static const int DEFAULT_OUT_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
	static const AVSampleFormat DEFAULT_OUT_SAMPLE_FMT = AV_SAMPLE_FMT_S16;
	static const int DEFAULT_OUT_SAMPLE_RATE = 44100;
	static const int DEFAULT_BUFFER_SIZE = DEFAULT_OUT_SAMPLE_RATE * DEFAULT_OUT_CHANNELS * 2;
	AVFrame* audFrame;
	SwrContext* swrCtx;
	int audDestMaxSamples, audDestSamples;
	uint8_t **audDestData;
	uint8_t *audPktData;
	int audPktSize;
	int audDestLSize;
	uint8_t* audBuf;
	AVPacket internalPkt;
	int audBufSize;
	int audBufOffset;
	AVCodecContext* cdcctx;
	FFmpegAudioDecodeComponent2();
	FFmpegAudioDecodeComponent2(AVCodecContext* src);
	~FFmpegAudioDecodeComponent2();
	bool needPacket();
	//negative == fail by error,0==decoded but no sound, >1 == success got sound,size
	int decode(AVPacket* pkt, uint8_t* out);
	virtual void flush();

private:
	bool init(AVCodecContext* srcCodec);
	bool setSwrCtx(AVCodecContext* src, int outChLayout, int outSRate, enum AVSampleFormat outSampleFmt);
	bool allocAudioFrame();
	bool setAudioBuffer();
	int resample(AVFrame* decodedF, uint8_t ***dest_data);
	CRITICAL_SECTION  crSect;
};