#ifndef A_FFMPEGMPHDR
#define A_FFMPEGMPHDR

#include "FFmpeg.h"
#include "FFmpegMedia.h"
#include "FFmpegDecodeQueue.h"
#include "FFmpegThread.h"
#include <SDL2/SDL.h>
#include <string>
#include <functional>

#define FFMP_AUDIO_BUFFER_SIZE 176400
#define SDL_AUDIO_BUFFER_SIZE 4096
/* about 0.09 sec */

struct FFmpegVideoDecodeSetting {
	AVPixelFormat pixFmt;
	int width;
	int height;
	int resampling_mode;
};

struct FFmpegAudioDecodeSetting {
	int channelLayout;
	int sample_rate;
	AVSampleFormat sample_format;
	int channel_num;
};

struct FFmpegDecodeSetting {
	FFmpegAudioDecodeSetting audio;
	FFmpegVideoDecodeSetting video;
};

class FFmpegMediaPlayer {
public:
	FFmpegDecodeSetting decodeSetting;
	FFmpegMedia media;
	FFmpegMediaPlayer();
	FFmpegMediaPlayer(std::wstring filePath, FFmpegDecodeSetting desiredDecodeSetting);
	void setVideoBufferDest(void* videoBuffer);
	bool isVideoPlayable() const;
	bool isAudioPlayable() const;
	static FFmpegThread* videoOutputThread;
private:
	void init();
	bool initVideo(FFmpegMedia media,FFmpegVideoDecodeSetting *vSet);
	bool initAudio(FFmpegMedia media, FFmpegAudioDecodeSetting *aSet);
	AVFrame *vidSrcFrame, *vidDestFrame,*audFrame;
	uint8_t* vBuffer;
	SwsContext* swsCtx;
	SwrContext* swrCtx;
	uint8_t* outBuffer;
	AVPacket pkt,vPkt,aPkt;
	int got_picture,got_sound;
	int audDestMaxSamples, audDestSamples;
	uint8_t **audDestData;
	uint8_t *audPktData;
	int audPktSize;
	int audDestLSize;
	uint8_t* audBuf; // [FFMP_AUDIO_BUFFER_SIZE];
	int audBufSize;
	int audBufOffset;
	FFmpegDecodeQueue soundQueue;
	FFmpegDecodeQueue videoQueue;
	void SDLCallback(void* userdata, uint8_t* stream, int len);
	static void SDLCALL __SDLCallbackWrapper(void* userdata, uint8_t* stream, int len);
	bool isSettingValid(FFmpegDecodeSetting s);
	bool _videoPlayable, _audioPlayable;
	FFmpegThreadFunc __queue,__outputVideoBuffer;
	//FFmpegThreadFuncController queueController, outputVideoController;
	int resampleAudio(AVFrame* decodedFrame,uint8_t ***dest_data);
	volatile int decodedFrameCount;
	int decodeAudioFrame(uint8_t* buffer,int buffer_size);
	SDL_AudioSpec sdlSpec;
	bool setSDLAudioDevice(FFmpegAudioDecodeSetting* as);
};

#endif