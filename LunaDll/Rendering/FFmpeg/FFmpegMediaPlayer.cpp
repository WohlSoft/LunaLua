#include "FFmpegMediaPlayer.h"
#include <SDL2/SDL.h>
#include "../../Defines.h"
#include <chrono>
#include <mutex>
#include <Windows.h>

//wrap回避
#define UINT32SUB(a,b) ((uint32_t)max(0,(int64_t)a-(int64_t)b))

/*
	操作の逐次化
*/
void FFmpegMediaPlayerOpQueue::push(FFmpegMediaPlayerOperation*& op) {
	std::lock_guard<std::mutex> lock(mtx1);
	Ops.push_back(op);
}

bool FFmpegMediaPlayerOpQueue::pop(FFmpegMediaPlayerOperation*& op) {
	std::lock_guard<std::mutex> lock(mtx1);
	if (Ops.empty()) return false;
	op = Ops.front();
	Ops.pop_front();
	return true;
}

void FFmpegMediaPlayerOpQueue::clear() {
	
}

void FFmpegMediaPlayerOpQueue::proc() {
	if (wait == NULL) {
		if (!pop(wait))return;
		wait->initOp(wait->data);
	}
	else {
		if (wait->endCond()) {
			
			wait = NULL;
		}
	}
}



/*
	適当に変数まとめたかっただけのクラス　ひどいわこれ
*/
void FFmpegPlayerStateManager::play() {
	//プレイ中でない場合のみ再生を待つ
	if(!playing)wPlay = true;
}

void FFmpegPlayerStateManager::pause() {
	//プレイ中のみポーズを待つ
	if(playing)wPause= true;
}

void FFmpegPlayerStateManager::stop() {
	//プレイ中のみ停止を待つ
	if(playing)wStop = true;
}

void FFmpegPlayerStateManager::seek(double sec) {
	//いつでもシークは出来る
	seekPos = sec;
	wSeek = true;
}
bool FFmpegPlayerStateManager::shouldSeek() {
	//シーク待ちならいつでもシークすべき
	return wSeek;
}
void FFmpegPlayerStateManager::seekProc() {
	//シーク時の操作特に無し
	wSeek = false;
}

bool FFmpegPlayerStateManager::audioShouldPlay() {
	//再生中なら当然再生(=継続)すべきで、さらにオーディオが再生の起点になるので、再生待ちの場合も再生(=開始)すべき
	return playing || wPlay;
}

bool FFmpegPlayerStateManager::videoShouldPlay() {
	//オーディオと違って再生の起点ではないので再生中に続行するのみ
	return playing;
}
bool FFmpegPlayerStateManager::shouldBegin() {
	//再生中でないが再生待ちなら再生開始すべき
	return !playing && wPlay;
}

//time:absolute time
//pos:relative time
void FFmpegPlayerStateManager::startProc(double cur, double delay) {
	//オーディオが再生の起点なのでSDLのスレッドで呼ばれる

	//順番に注意(他のスレッドが間に入る事を考えよ)

	//1.開始時間=現在の時間+遅れから前回の再生位置を差し引いた時間
	startTime = cur +delay-playPosition();
	//2."再生の開始"を終えるので再生待ち
	wPlay = false; 
	//3.再生する
	playing = true;
	//開始時間設定前に再生開始フラグを立てると他のスレッドが不正な開始時間を読み取るので注意
}

//使わない
bool FFmpegPlayerStateManager::shouldPause() {
	
	//ポーズすべき
	return playing && pauseTime <= playTime && wPause && pauseBooked;
}
bool FFmpegPlayerStateManager::shouldPauseByVideo() {
	//ポーズ待ちかつポーズが予定されていて、再生中かつポーズ(予定)位置を再生位置がオーバーしている時
	//ポーズすべき
	return playing && pausePosition() <= videoPos && wPause && pauseBooked && !isSeeking();
}
bool FFmpegPlayerStateManager::shouldBookPause() {
	//再生中かつ再生待ちの時ポーズの予定をする
	return playing && wPause;
}
void FFmpegPlayerStateManager::pauseBooking(double cur, double remain) {

	//1.ポーズ予定したということにする(他のスレッドの牽制)
	pauseBooked = true;
	//2.ポーズ予定時刻の設定
	pauseTime = cur + remain;
	
}
void FFmpegPlayerStateManager::stopBooking() {
	//停止の予定
	//先頭へのシークの試行
	seek(0);
	//予定済みにする
	stopBooked = true;
}
void FFmpegPlayerStateManager::pauseProc() {
	//1.先に停止を宣言しておく
	playing = false;
	//2.ポーズを終えるので予定は消える
	pauseBooked = false;
	//3.ポーズ待ちも終わり
	wPause = false;
	//4.ポーズ時刻まで再生したことにする
	playTime = pauseTime;
}
void FFmpegPlayerStateManager::updateState(double cur) {
	//現在時刻の更新
	playTime = cur;
}

double FFmpegPlayerStateManager::playPosition() {
	//再生位置
	return playTime - startTime;
}
double FFmpegPlayerStateManager::pausePosition() {
	//ポーズ位置
	return pauseTime - startTime;
}
bool FFmpegPlayerStateManager::isSeeking() {
	//シーク中かどうか

	//シーク待ちまたは映像か音声の位置リセット待ちならまだシーク中
	return wSeek||needAudioPosReset || needVideoPosReset;
}
bool FFmpegPlayerStateManager::videoShouldKeepUp() {
	//映像を進めるべきかどうか
	//再生中かつ、映像の位置が現在位置より前にあるなら進めるべき
	//または、映像だけ再生位置のリセットが必要な場合も進めるべき
	//(映像だけ再生位置のリセットが必要 
	// == シーク位置からのパケット読み込み済み&&音声によるリセット位置決定済み 
	// == (ちゃんとバッファの類を消せば)次の読み込みで映像のリセット位置が決定できる)
	return ((videoPos <= playPosition()) && playing) || (needVideoPosReset && !needAudioPosReset) ;
}

void FFmpegPlayerStateManager::refreshVideoState(double curFT) {
	//映像の状態の更新
	//レンダリングされていない状態にする
	//再生位置の更新
	videoRendered = false;
	videoPos = curFT;
}

bool FFmpegPlayerStateManager::shouldRenderVideo() {
	//再生中かつまだレンダリングされてなくて映像位置が再生位置を上回ってるならレンダリングすべき
	//(初めて上回った時にレンダリングさせる)
	return videoPos >= playPosition() && !videoRendered && playing;
}

FFmpegThread* FFmpegMediaPlayer::videoOutputThread = new FFmpegThread();

void FFmpegMediaPlayer::DebugMsgBox(LPCSTR pszFormat, ...)
{
	va_list	argp;
	char pszBuf[256];
	va_start(argp, pszFormat);
	vsprintf(pszBuf, pszFormat, argp);
	va_end(argp);
	MessageBoxA(NULL, pszBuf, "debug info", MB_OK);
}

FFmpegMediaPlayer::~FFmpegMediaPlayer() {
	/* まず他のスレッドにある処理を全て停止 */
	if(__outputVideoBuffer)FFmpegMediaPlayer::videoOutputThread->delWork(__outputVideoBuffer);
	if(__queue)FFmpegDecodeQueue::queueThread->delWork(__queue);
	if (postMixCallback)PGE_MusPlayer::removePostMixFunc(postMixCallback);

	/* あれば削除 */
	if (__outputVideoBuffer)delete __outputVideoBuffer;
	if (__queue)delete __queue;
	if (postMixCallback)delete postMixCallback;

	if (_play)delete _play;
	if (_pause)delete _pause;
	if (_seek)delete _seek;

	/* 中にあるAVPacketはデストラクタにfreeされる */
	if (soundQueue) delete soundQueue;
	if (videoQueue) delete videoQueue;

	/* このplayerのコンストラクタで読み込まれたmediaなら削除 */
	if (media && loadViaPlayer)delete media;

	//structのデストラクタはスタックトレースに載らなかったりする
}

//主にオブジェクトの生成を伴わない初期化
void FFmpegMediaPlayer::init() {
	loadViaPlayer = false;
	outBuffer = NULL;
	_videoPlayable = _audioPlayable = false;
	_SDLSoundFormat = AUDIO_S16SYS;
	av_init_packet(&pkt);
	media = NULL;
	_play = _pause = _seek = NULL;
	soundQueue = videoQueue = NULL;
	__queue = __outputVideoBuffer = NULL;
	postMixCallback = NULL;
	loop = true;
	loadCompleted = false;
	_volume = 100;
	waitEnd = false;

	//treat it as out-of-screen initially
	onScreen = false;
	offScreenProcessed = true;
	lastOnScreenTime = -1;

	OScrMode = STOP;
}

//映像に関する初期化
bool FFmpegMediaPlayer::initVideo(FFmpegMedia* m, FFmpegVideoDecodeSetting* vs) {
	if (!m->isVideoAvailable())return false;
	/* make size aligned */
	vs->width = vs->width >0 ? vs->width : m->vidCodecCtx->width;
	vs->height = vs->height >0 ? vs->height : m->vidCodecCtx->height;
	avcodec_align_dimensions2(
		m->vidCodecCtx,
		&vs->width,
		&vs->height,
		aL);

	/* use short variable name */
	AVPixelFormat pixFmt = vs->pixFmt;
	int _w = vs->width;
	int _h = vs->height;

	/* init resampling settings */
	FFVDC.swsCtx = sws_getContext(
		m->width(),				//src info
		m->height(),
		m->vidCodecCtx->pix_fmt,
		_w,	//dest info
		_h,
		pixFmt,
		decodeSetting.video.resampling_mode,
		NULL,
		NULL,
		NULL);
	if (FFVDC.swsCtx == NULL)return false;
	/* malloc decode buffer */
	FFVDC.vBuffer = (uint8_t*)av_malloc(avpicture_get_size(pixFmt, _w, _h)*sizeof(uint8_t));
	if (FFVDC.vBuffer == NULL)return false;
	/* alloc frame buffer */
	FFVDC.vidDestFrame = av_frame_alloc();
	FFVDC.vidSrcFrame = av_frame_alloc();
	//FFVDC.booked = av_frame_alloc();
	if (FFVDC.vidDestFrame == NULL || FFVDC.vidSrcFrame == NULL)return false;
	/* assign buffer to frame */
	avpicture_fill((AVPicture*)FFVDC.vidDestFrame, FFVDC.vBuffer, pixFmt, _w, _h);

	//デコードとバッファへの書き込みをする関数
	__outputVideoBuffer = new FFmpegThreadFunc((std::function<void(FFmpegThreadFuncController*)>)[=](FFmpegThreadFuncController* ctrl) {
		if (waitEnd)return;
		/* 再生すべき状態かどうか確認 */
		if (!sman.videoShouldPlay())return;

		/* playerの状態の更新 */
		sman.updateState(SDL_GetTicks());

		/*
			シーク時に行われる処理
			オーディオ側でstartTimeの設定が終わった後に行われるようにする。
		*/
		bool posReset = sman.needVideoPosReset && !sman.needAudioPosReset;

		//シーク処理を行う条件を満たした最初のループのみ行う処理
		if (posReset && !sman.videoMemResetFlag) {
			/*
				次のパケットのタイムスタンプでどこにシークされたかを確認するため、シーク後の位置のパケットを確実に取得する。

				(シーク処理を行う条件を満たした時、キューにある次のパケットはシーク後の位置のものになっているので、
				ここでバッファをクリアして確実に読み込みに行けばよい。)
			*/
			av_free_packet(&FFVDC.vPkt);
			av_init_packet(&FFVDC.vPkt);
			av_frame_free(&FFVDC.vidSrcFrame);
			FFVDC.vidSrcFrame = av_frame_alloc();
			//この初期化処理は一回きり
			sman.videoMemResetFlag = true;
		}

		//現在の再生時刻に比べて動画の時刻(タイムスタンプ)が遅れているなら進める
		while (sman.videoShouldKeepUp()) {

			//外部から操作したいとき
			if (ctrl->quit || ctrl->pause)return;

			/*
				別スレッドで、posResetとここの間でシーク処理が行われた時は一旦抜ける
			*/
			if (!posReset && sman.needVideoPosReset)return;

			/*
				ポーズから復帰したときはデコードせずbookedから読みだす
				(注意:bookedの読み込みに使ったvPktをキューからの読み込みに使ったりして上書きするとbookedのフレームがこわれる)
			*/
			//使うフレームのポインタはsrcPtrに入れておく
			if (FFVDC.booked) {
				FFVDC.srcPtr = FFVDC.booked;
				FFVDC.got_picture = 1;
			}
			else {
				/* キューからの読み込み */
				if (!videoQueue->pop(FFVDC.vPkt)) {
					if (videoQueue->dataSize() == 0 && loadCompleted && loop) {
						seek(0);
					}
					return;
				}

				/* デコード */
				avcodec_decode_video2(media->vidCodecCtx, FFVDC.vidSrcFrame, &FFVDC.got_picture, &FFVDC.vPkt);
				FFVDC.srcPtr = FFVDC.vidSrcFrame;
				/* vPktはもう使わないのでfree */
				av_free_packet(&FFVDC.vPkt);
			}
			/* 
				画像を得た場合にのみバッファに書き込む。
				PフレームやBフレームを読み込んだ場合はデコーダ内部のバッファに蓄えられてすぐには出力されないので注意。
			*/
			if (FFVDC.got_picture) {
				/* 再生位置の取得 */
				sman.refreshVideoState(1000 * av_q2d(media->video->time_base)*av_frame_get_best_effort_timestamp(FFVDC.srcPtr));
				/* シーク時の処理 ここでシーク処理を終了する */
				if (posReset) {
					posReset = false;
					sman.needVideoPosReset = false;
					sman.startTime = sman.plannedStartTime;
					return;
				}
				/* 
					ポーズするべき状態だった時の処理 
					ポーズは映像を起点とする。
				*/
				if (sman.shouldPauseByVideo()) {
					//bookedはnullなはず
					FFVDC.booked = av_frame_alloc();
					//bookedに入れておいて次書き出す
					av_frame_copy(FFVDC.booked, FFVDC.vidSrcFrame);
					sman.pauseProc();
					return;
				}
				/* レンダリングすべき状態かつ書き込み先がある場合のみ書き出す*/
				if (sman.shouldRenderVideo() && outBuffer != NULL) {
					//リサンプリング
					sws_scale(
						FFVDC.swsCtx,
						FFVDC.srcPtr->data,
						FFVDC.srcPtr->linesize,
						0,
						FFVDC.srcPtr->height,
						FFVDC.vidDestFrame->data,
						FFVDC.vidDestFrame->linesize
						);
					//ビットマップをコピー
					for (int j = 0; j < decodeSetting.video.height; ++j) {
						memcpy(
							(uint32_t*)outBuffer + j*decodeSetting.video.width,
							((uint8_t*)FFVDC.vidDestFrame->data[0]) + j*FFVDC.vidDestFrame->linesize[0],
							FFVDC.vidDestFrame->linesize[0]
							);
					}
				}
			}
			//bookedにフレームをコピーした場合はここまで来ない(何かコード書き換えるならここまで来させないように)
			if (FFVDC.booked) {
				av_frame_free(&FFVDC.booked);
				FFVDC.booked = NULL;
			}

		}
	});
	return true;
}
bool FFmpegMediaPlayer::initAudio(FFmpegMedia* m, FFmpegAudioDecodeSetting* as) {
	if (!m->isAudioAvailable())return false;
	FFADC.swrCtx = swr_alloc();
	if (FFADC.swrCtx == NULL)return false;
	int o_chLayout = m->audCodecCtx->channel_layout;
	int o_sRate = m->audCodecCtx->sample_rate;
	AVSampleFormat o_sFmt = m->audCodecCtx->sample_fmt;
	av_opt_set_int(FFADC.swrCtx, "in_channel_layout", o_chLayout? o_chLayout :AV_CH_LAYOUT_STEREO, 0);
	av_opt_set_int(FFADC.swrCtx, "in_sample_rate", o_sRate? o_sRate:44100, 0);
	av_opt_set_sample_fmt(FFADC.swrCtx, "in_sample_fmt", o_sFmt == AV_SAMPLE_FMT_NONE?AV_SAMPLE_FMT_S16:o_sFmt, 0);
	av_opt_set_int(FFADC.swrCtx, "out_channel_layout", as->channelLayout, 0);
	av_opt_set_int(FFADC.swrCtx, "out_sample_rate", as->sample_rate, 0);
	av_opt_set_sample_fmt(FFADC.swrCtx, "out_sample_fmt", as->sample_format, 0);
	if (swr_init(FFADC.swrCtx) < 0)return false;
	as->channel_num = av_get_channel_layout_nb_channels(as->channelLayout);
	FFADC.audFrame = av_frame_alloc();
	if (FFADC.audFrame == NULL)return false;
	FFADC.audBuf = (uint8_t*)malloc(FFMP_AUDIO_BUFFER_SIZE*sizeof(uint8_t));
	if (FFADC.audBuf == NULL)return false;
	postMixCallback = new PGE_PostMixFunc((std::function<void(void *udata, uint8_t *stream, int len)>)[=](void *udata, uint8_t *stream, int len) {
		if (waitEnd)return;
		if (!sman.audioShouldPlay())return;
		int len1, audio_size;
		double time_stamp = 0;
		bool first_send = true;
		//Check reset request before popping packet
		bool posReset = sman.needAudioPosReset;
		if (posReset) {
			av_free_packet(&FFADC.aPkt);
			av_init_packet(&FFADC.aPkt);
			//this cause popping
			FFADC.audPktData = NULL;
			FFADC.audPktSize = 0;
		}
		while (len > 0) {

			if (FFADC.audBufOffset >= FFADC.audBufSize) {
				audio_size = decodeAudioFrame(FFADC.audBuf, FFMP_AUDIO_BUFFER_SIZE, &time_stamp);
				if (audio_size < 0) {
					FFADC.audBufSize = 1024;
					memset(FFADC.audBuf, 0, FFADC.audBufSize);
				}
				else {
					FFADC.audBufSize = audio_size;
				}
				FFADC.audBufOffset = 0;
			}
			len1 = FFADC.audBufSize - FFADC.audBufOffset;
			if (len1 > len)len1 = len;
			SDL_MixAudioFormat(stream, (uint8_t*)FFADC.audBuf + FFADC.audBufOffset, _SDLSoundFormat, len1, needMute()?0:_volume);
			len -= len1;
			stream += len1;
			FFADC.audBufOffset += len1;
			if (sman.shouldBegin()) {

				sman.startProc(SDL_GetTicks(), 2*1000 * len / ((double)PGE_MusPlayer::sampleRate()*PGE_MusPlayer::channels()));
			}
			if (sman.shouldBookPause()) {

				sman.pauseBooking(SDL_GetTicks(), 2*1000 * len / ((double)PGE_MusPlayer::sampleRate()*PGE_MusPlayer::channels()));
			}
			if (first_send && posReset) {
				posReset = false;
				//time_stamp may become huge negative value
				sman.plannedStartTime = (double)SDL_GetTicks() - max(0, time_stamp) + 2*1000 * len / ((double)PGE_MusPlayer::sampleRate()*PGE_MusPlayer::channels()) -15;
				sman.needAudioPosReset = false;
				first_send = false;
			}
		}
	});
	
	return setSDLAudioDevice(as);
}
void FFmpegMediaPlayer::coreInit() {
	soundQueue = new FFmpegDecodeQueue(44100 * 2); //0.5sec
	videoQueue = new FFmpegDecodeQueue(800 * 600 * 4); //whole screen
	_play = new FFmpegMediaPlayerOperation();
	_pause = new FFmpegMediaPlayerOperation();
	_seek = new FFmpegMediaPlayerOperation();
	opq = new FFmpegMediaPlayerOpQueue();
	_play->initOp = [=](void* data) {
		sman.play();
	};
	_play->endCond = [=]() {
		return sman.playing && !sman.wPlay;
	};

	_pause->initOp = [=](void* data) {
		sman.pause();
	};

	_pause->endCond = [=]() {
		return !sman.wPause && !sman.playing && !sman.pauseBooked;
	};

	_seek->initOp = [=](void* data) {
		sman.seek(*(double*)data);
	};
	_seek->endCond = [=]() {
		return !sman.isSeeking();
	};
	_seek->data = &__seekVal;

	__queue = new FFmpegThreadFunc((std::function<void(FFmpegThreadFuncController*)>)[=](FFmpegThreadFuncController* ctrl) {
		
		if (waitEnd)return;
		if (shouldEnd()) {
			waitEnd = true;
		}
		
		if (SDL_GetTicks() - lastOnScreenTime > 100) {
			onScreen = false;
		}
		if (!onScreen && !offScreenProcessed) {
			switch (OScrMode) {
			case CONTINUE:
				break;
			case PAUSE:
				pause();
				break;
			case STOP:
				stop();
				break;
			default:
				break;
			}
			offScreenProcessed = true;
		}

		if (onScreen && offScreenProcessed) {
			play();
			offScreenProcessed = false;
		}
		int end = 0;
		opq->proc();

		if (sman.shouldSeek()) {
			seek_internal(sman.seekPos);
			sman.seekProc();
		}			
			//av_init_packet(&pkt);
		
		
			while (!ctrl->quit && soundQueue->dataSize() < soundQueue->MAX_SIZE && videoQueue->dataSize() < videoQueue->MAX_SIZE) {
				
				end = av_read_frame(media->fmtCtx, &pkt);
				if (end < 0) {
					loadCompleted = (end == AVERROR_EOF);
					return;
				}
				if (pkt.stream_index == media->audStreamIdx) {
					soundQueue->push(pkt);
				}
				else if (pkt.stream_index == media->vidStreamIdx) {
					videoQueue->push(pkt);
				}
				else {
					av_free_packet(&pkt);
				}
				
			}
	});
}
FFmpegMediaPlayer::FFmpegMediaPlayer() {
	init();
}

FFmpegMediaPlayer::FFmpegMediaPlayer(std::wstring filePath, FFmpegDecodeSetting dSet) :FFmpegMediaPlayer() {
	if (!isSettingValid(dSet))return;
	media = new FFmpegMedia(filePath);
	loadViaPlayer = true;
	decodeSetting = dSet;
	_videoPlayable = initVideo(media, &decodeSetting.video);
	_audioPlayable = initAudio(media, &decodeSetting.audio);
	
	if (!isAudioPlayable() && !isVideoPlayable())return;
	coreInit();
	//startTime = std::chrono::system_clock::now();
	if (isVideoPlayable())FFmpegMediaPlayer::videoOutputThread->addWork(__outputVideoBuffer);
	
	FFmpegDecodeQueue::queueThread->addWork(__queue);
	//play();
}
void FFmpegMediaPlayer::play() {
	opq->push(_play);
}
void FFmpegMediaPlayer::pause() {
	opq->push(_pause);
}
void FFmpegMediaPlayer::seek(double sec) {
	__seekVal = sec;
	opq->push(_seek);
}
void FFmpegMediaPlayer::stop() {
	seek(0);
	pause();
}
void FFmpegMediaPlayer::setVolume(int vol) {
	_volume = min(127, max(0, vol));
}
bool FFmpegMediaPlayer::needMute() {
	return sman.isSeeking() || (offScreenProcessed && (OScrMode == PAUSE || OScrMode == STOP));
}
bool FFmpegMediaPlayer::shouldEnd() {
	return GM_EPISODE_MODE || GM_LEVEL_MODE;
}
void FFmpegMediaPlayer::setOffScreenMode(OffScreenMode mode) {
	OScrMode = mode;
}
void FFmpegMediaPlayer::setOnScreen(bool _onScreen) {
	//dbgboxA(_onScreen ? "ooon" : "nooon");
	onScreen = _onScreen;
	if(onScreen)lastOnScreenTime = SDL_GetTicks();
}
FFmpegDecodeSetting FFmpegMediaPlayer::getAppliedSetting() {
	return decodeSetting;
}

void FFmpegMediaPlayer::seek_internal(double sec) {
	std::lock_guard<std::mutex>lock1(soundQueue->mtx1);
	std::lock_guard<std::mutex>lock2(videoQueue->mtx1);
	soundQueue->rawClear();
	videoQueue->rawClear();
	av_seek_frame(media->fmtCtx, media->vidStreamIdx, (int64_t)round(sec / av_q2d(media->video->time_base)), AVSEEK_FLAG_BACKWARD);
	avcodec_flush_buffers(media->audCodecCtx);
	avcodec_flush_buffers(media->vidCodecCtx);
	if (FFVDC.booked) {
		av_frame_free(&FFVDC.booked);
		FFVDC.booked = NULL;
	}
	sman.videoMemResetFlag = false;
	sman.needAudioPosReset = true;
	sman.needVideoPosReset = true;
}
void FFmpegMediaPlayer::setVideoBufferDest(void* dest) {
	outBuffer = (uint8_t*)dest;
}
int FFmpegMediaPlayer::decodeAudioFrame(uint8_t* buffer, int buffer_size,double* head_time) {
	int audDestBufSize = 0, ret = 0;
	bool first_decode = true;
	while (1) {
		while (FFADC.audPktSize > 0) {
			int len1 = avcodec_decode_audio4(media->audCodecCtx, FFADC.audFrame, &FFADC.got_sound, &FFADC.aPkt);
			if (first_decode) {
				*head_time = 1000 * av_q2d(media->audio->time_base)*av_frame_get_best_effort_timestamp(FFADC.audFrame);
				first_decode = false;
			}
			if (len1 < 0) {
				FFADC.audPktSize = 0;
				break;
			}
			FFADC.audPktData += len1;
			FFADC.audPktSize -= len1;
			audDestBufSize = 0;
			if (FFADC.got_sound) {
				audDestBufSize = resampleAudio(FFADC.audFrame,&FFADC.audDestData);
				/* seg fault point*/
				if (audDestBufSize > 0) {
					memcpy(buffer, FFADC.audDestData[0], audDestBufSize);
				}
				av_freep(&FFADC.audDestData[0]);
				av_freep(&FFADC.audDestData);
			}
			if (audDestBufSize <= 0) {
				continue;
			}

			return audDestBufSize;
		}
		if (FFADC.aPkt.data)av_free_packet(&FFADC.aPkt);

		if (!soundQueue->pop(FFADC.aPkt))return -1;
		FFADC.audPktData = FFADC.aPkt.data;
		FFADC.audPktSize = FFADC.aPkt.size;

	}
}
int FFmpegMediaPlayer::resampleAudio(AVFrame* decodedFrame,uint8_t ***dest_data) {
	int ret = 0;
	FFADC.audDestMaxSamples = FFADC.audDestSamples = (int)av_rescale_rnd(
		decodedFrame->nb_samples,
		decodeSetting.audio.sample_rate,
		media->audCodecCtx->sample_rate,
		AV_ROUND_UP);

	ret = av_samples_alloc_array_and_samples(
		dest_data,
		&FFADC.audDestLSize,
		decodeSetting.audio.channel_num,
		FFADC.audDestSamples,
		decodeSetting.audio.sample_format,
		0);

	if (ret < 0)return -1;
	FFADC.audDestSamples = (int)av_rescale_rnd(
		swr_get_delay(FFADC.swrCtx, media->audCodecCtx->sample_rate) + decodedFrame->nb_samples,
		decodeSetting.audio.sample_rate,
		media->audCodecCtx->sample_rate,
		AV_ROUND_UP);

	if (FFADC.audDestSamples > FFADC.audDestMaxSamples) {
		//av_freep((&(*dest_data)[0]));
		//av_freep(dest_data);
		ret = av_samples_alloc(
			*dest_data,
			&FFADC.audDestLSize,
			decodeSetting.audio.channel_num,
			FFADC.audDestSamples,
			decodeSetting.audio.sample_format,
			1);
		if (ret < 0)return -1;
		FFADC.audDestMaxSamples = FFADC.audDestSamples;
	}
	ret = swr_convert(
		FFADC.swrCtx,
		*dest_data,
		FFADC.audDestSamples,
		(const uint8_t**)decodedFrame->data,
		decodedFrame->nb_samples
		);
	if (ret < 0)return -1;

	return av_samples_get_buffer_size(
		&FFADC.audDestLSize,
		decodeSetting.audio.channel_num,
		ret,
		decodeSetting.audio.sample_format,
		1);
}

bool FFmpegMediaPlayer::setSDLAudioDevice(FFmpegAudioDecodeSetting *as) {
	switch (as->sample_format) {
	case AV_SAMPLE_FMT_NONE:
		_SDLSoundFormat = AUDIO_S16SYS; //most ordinary one?
		break;
	case AV_SAMPLE_FMT_U8:
		_SDLSoundFormat = AUDIO_U8;
		break;
	case AV_SAMPLE_FMT_S16:
		_SDLSoundFormat = AUDIO_S16SYS;
		break;
	case AV_SAMPLE_FMT_S32:
		_SDLSoundFormat = AUDIO_S32SYS;
		break;
	case AV_SAMPLE_FMT_FLT:
		_SDLSoundFormat = AUDIO_F32SYS;
		break;
	default:
		// SDL cannot deal the formats that not listed above.
		return false;
		break;
	}
	
	//!!!clean this on delete
	//////////////////////////////////////////////////////////
	PGE_MusPlayer::addPostMixFunc(postMixCallback);
	/////////////////////////////////////////////////////////
	return true;
}
bool FFmpegMediaPlayer::isAudioPlayable() const {
	return _audioPlayable;
}
bool FFmpegMediaPlayer::isVideoPlayable() const {
	return _videoPlayable;
}

bool FFmpegMediaPlayer::isSettingValid(FFmpegDecodeSetting s) {
	return s.audio.sample_rate > 0;
}