#pragma once
#include "FFmpegMedia2.h"
#include "FFmpegDecodeQueue.h"
#include <functional>
#include "../../SdlMusic/SdlMusPlayer.h"
#include "../GL/GLDraw.h"

class FFmpegMediaPlayer2 {
public:
	enum AlphaType {
		PMUL=0,STR=1
	};
	void init();
	void setVideoOutput(uint8_t* out,int w,int h);
	void setMaskOutput(uint8_t* out, int w, int h);
	void play();
	void pause();
	void stop();
	void seek(double sec);
	~FFmpegMediaPlayer2();
	FFmpegMediaPlayer2();
	FFmpegMediaPlayer2(std::wstring filename);
	int getWidth()const;
	int getHeight()const;
	int getMaskWidth()const;
	int getMaskHeight()const;
	void setAltAlpha(int altCh);
	int getAltAlpha() const;
	void setAlphaType(int mode);
	int getAlphaType()const;
	void setVideoDelay(double d);
	double getVideoDelay()const;
	void setMaskDelay(double d);
	double getMaskDelay()const;
	bool playable()const;
	bool maskExist()const;
	bool loop;
	static FFmpegThread* videoOutputThread;
private:
	
	FFmpegAudioDecodeComponent2* audioDecoder;
	FFmpegVideoDecodeComponent2* vMediaDecoder[FMEDIA_NUM];
	FFmpegDecodeQueue* queues[FMEDIA_NUM];

	uint8_t* aBuf=NULL;
	CustomAVPacket apkt;
	int audDataSize;
	int audDataIndex;
	static const int LOOP_OVERWRAP = 12;
	static const int LOOP_CLEAN_PADDING = 4;
	double audioAbsTime[LOOP_OVERWRAP];
	double audioTime[LOOP_OVERWRAP];
	double audioNextTime;
	//update loopcount before nexttime
	int audioLoopCount;
	bool nextLoopInit;

	double vMediaTime[FMEDIA_NUM];
	double vMediaDelay[FMEDIA_NUM];
	uint8_t* vMediaOutput[FMEDIA_NUM];
	int vMediaOutW[FMEDIA_NUM];
	int vMediaOutH[FMEDIA_NUM];
	int vMediaLoopCount[FMEDIA_NUM];
	int vMediaLastRemain[FMEDIA_NUM];

	bool shouldPlay;

	bool mediaSeekReq[FMEDIA_NUM];
	/*
	bool audioSeekReq;
	bool videoSeekReq;
	bool maskSeekReq;
	*/
	double seekPos;

	bool loadCompleted[FMEDIA_NUM];

	

	bool waitEnd;
	FFmpegThreadFunc* videoOutputLoop;
	FFmpegThreadFunc* maskOutputLoop;
	FFmpegThreadFunc* queueThreadLoop;
	PGE_PostMixFunc* SDLCallback;
	void initCallback();
	void initQueue();
	void initVars();
	void initWorks();
	void initDecoder();
	bool shouldEnd()const;
	bool needPlay()const;
	void mixIntoSDLBuffer(uint8_t* sdlBuf, uint8_t* dataPtr, int dataLen);
	double calcAudioSampleDuration(int bytes)const;
	void setCurrentAudioTime(double time,int loop);
	double getCurrentAudioTime(int loop)const;
	void writeVideoIntoBuffer(uint8_t* buffer, int bufW, int bufH, uint8_t* vData, int dataW, int dataH, uint8_t* altAlpha, int alphaW, int alphaH,int altCh);
	bool setAudioDecodePktPtr(FFmpegAudioDecodeComponent2* decoder,CustomAVPacket** pktPtr);
	double getTimestampFromVideoFrame(FMEDIA vmt, AVFrame* vframe,double lastFrameTime) const;
	int __mediaSeek(FMEDIA mt, double pos);
	void __mediaFlush(FMEDIA mt);
	void __setVMediaOutput(FMEDIA mt, uint8_t* out, int w, int h);
	int __getVMediaHeight(FMEDIA vmt)const;
	int __getVMediaWidth(FMEDIA vmt)const;
	FFmpegThreadFunc* __videoOutputFuncGen(FMEDIA vmt);
	void packetQueue(FMEDIA mt);
	void setAlphaByChannel(uint8_t* dest, int destW, int destH, uint8_t* src, int srcW, int srcH, int srcCh);
	FFmpegMedia2* media;
	
	AlphaType alphaType;

	int altAlpha;

	SwsContext* alphaSws;
	uint8_t* swsAlphaBuf;
};

