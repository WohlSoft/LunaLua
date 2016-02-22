#include "FFmpeg.h"

//don't put vars that cause crash when zero init
struct FFmpegVideoDecodeComponent {
	AVFrame* vidSrcFrame, *vidDestFrame, *booked, *srcPtr;
	uint8_t* vBuffer;
	SwsContext* swsCtx;
	CustomAVPacket vPkt;
	int got_picture;
	FFmpegVideoDecodeComponent();
	~FFmpegVideoDecodeComponent();
};

struct FFmpegAudioDecodeComponent {
	AVFrame* audFrame;
	SwrContext* swrCtx;
	CustomAVPacket aPkt;
	int got_sound;
	int audDestMaxSamples, audDestSamples;
	uint8_t **audDestData;
	uint8_t *audPktData;
	int audPktSize;
	int audDestLSize;
	uint8_t* audBuf;
	int audBufSize;
	int audBufOffset;
	FFmpegAudioDecodeComponent();
	~FFmpegAudioDecodeComponent();
};
