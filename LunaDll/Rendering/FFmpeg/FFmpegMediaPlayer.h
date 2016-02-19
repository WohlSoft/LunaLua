#ifndef A_FFMPEGMPHDR
#define A_FFMPEGMPHDR

#include "FFmpeg.h"
#include "FFmpegMedia.h"
#include "FFmpegDecodeQueue.h"
#include "FFmpegThread.h"
#include "FFmpegComponent.h"
#include "../../SdlMusic/SdlMusPlayer.h"
#include <SDL2/SDL.h>
#include <string>
#include <functional>
#include <atomic>
#include <chrono>
#include <mutex>
#include <Windows.h>

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


class FFmpegPlayerStateManager {
public:
	//request these operations
	void play();
	void pause();
	void stop();
	void seek(double sec);

	//get position(msec)
	double playPosition();
	double pausePosition();
	
	//know whether these actions should be done
	bool audioShouldPlay();
	bool videoShouldPlay();
	bool shouldInitVideo();
	bool shouldInitAudio();
	bool shouldBegin();
	bool shouldPause();
	bool shouldSeek();
	bool shouldPauseByVideo();
	bool shouldBookPause();
	bool videoShouldKeepUp();
	bool shouldRenderVideo();

	//plan future actions
	void pauseBooking(double currentTime, double remainTime);
	void stopBooking();

	//misc proc
	void startProc(double currentTime, double delay);
	void seekProc();
	void pauseProc();

	bool isSeeking();

	void updateState(double currentTime);
	void refreshVideoState(double currentFrameTime);
	
	//states; "w" means "wait"
	bool playing;
	bool wPlay;
	bool wPause;
	bool wSeek;
	bool wStop;
	bool pauseBooked;
	bool stopBooked;

	bool needAudioPosReset;
	bool needVideoPosReset;

	bool videoRendered;
	bool videoMemResetFlag;

	bool audioMemResetFlag;

	bool audioInitReq;
	bool videoInitReq;

	//Time:absolute time
	//Pos:relative time from start
	//unit=msec
	double startTime;
	double playTime;
	double pauseTime;
	double plannedStartTime;

	double videoPos;
	double seekPos;
	double lastAudioDelay;

	double videoDelay;

	int seekCount;
	void init();
	FFmpegPlayerStateManager() {
		init();
	};

};

class FFmpegMediaPlayerOperation {
public:
	
	std::function<void(void*)> initOp;
	void* data;
	std::function<bool(void)> endCond;
	FFmpegMediaPlayerOperation():data(NULL){}
};

class FFmpegMediaPlayerOpQueue {
public:
	void push(FFmpegMediaPlayerOperation*& op);

	bool pop(FFmpegMediaPlayerOperation*& op);

	void clear();

	void proc();

	FFmpegMediaPlayerOperation* wait;

	FFmpegMediaPlayerOpQueue() :wait(NULL){}
	std::mutex mtx1;
private:

	std::deque<FFmpegMediaPlayerOperation*> Ops;
	int size_;
};

class FFmpegMediaPlayer {
public:
	static const int MAX_QUEUE_ONCE = 10;
	void play();
	void pause();
	void seek(double sec);
	void stop();
	void setVolume(int volume);
	bool loop;
	enum OffScreenMode {
		CONTINUE,PAUSE,STOP
	};
	
	bool onScreen;
	
	bool isVideoPlayable() const;
	bool isAudioPlayable() const;
	FFmpegDecodeSetting getAppliedSetting();
	void setVideoBufferDest(void* videoBuffer);

	FFmpegMediaPlayer();
	FFmpegMediaPlayer(std::wstring filePath, FFmpegDecodeSetting desiredDecodeSetting);
	~FFmpegMediaPlayer();

	static FFmpegThread* videoOutputThread;
	void DebugMsgBox(LPCSTR pszFormat, ...);
	void setOffScreenMode(OffScreenMode mode);
	void setOnScreen(bool onScreen);
	int maskMode;
	uint8_t* collisionMap;
	void setVideoDelay(double d);
private:
	int aL[8]; //garbage
	int _SDLSoundFormat;
	double __seekVal;
	uint8_t* outBuffer;//no init needed
	AVPacket pkt,mPkt,tmpMPkt;
	bool mPktConsumed;
	bool _videoPlayable, _audioPlayable;
	bool loadViaPlayer;
	bool loadCompleted;
	FFmpegMedia* media;
	FFmpegDecodeQueue *soundQueue;
	FFmpegDecodeQueue *videoQueue;
	FFmpegDecodeQueue *maskQueue;
	PGE_PostMixFunc* postMixCallback;
	FFmpegMediaPlayerOpQueue* opq;
	FFmpegMediaPlayerOperation* _play;
	FFmpegMediaPlayerOperation* _pause;
	FFmpegMediaPlayerOperation* _seek;
	FFmpegPlayerStateManager sman;
	FFmpegDecodeSetting decodeSetting;
	FFmpegAudioDecodeComponent FFADC;
	FFmpegVideoDecodeComponent FFVDC;
	FFmpegVideoDecodeComponent MaskVDC;
	FFmpegThreadFunc *__queue, *__outputVideoBuffer;
	CRITICAL_SECTION crSectionA,crSectionV,crSectionM;

	
	void init();
	void coreInit();
	bool initVideo(FFmpegMedia* media,FFmpegVideoDecodeSetting *vSet);
	bool initAudio(FFmpegMedia* media, FFmpegAudioDecodeSetting *aSet);
	bool isSettingValid(FFmpegDecodeSetting s);
	int resampleAudio(AVFrame* decodedFrame,uint8_t ***dest_data);
	int decodeAudioFrame(uint8_t* buffer,int buffer_size,double* head_time);
	bool setSDLAudioDevice(FFmpegAudioDecodeSetting* as);
	
	void seek_internal(double sec);
	bool needMute();
	bool shouldEnd();
	
	bool waitEnd;
	int _volume;
	bool offScreenProcessed;
	double lastOnScreenTime;
	OffScreenMode OScrMode;
};

#endif