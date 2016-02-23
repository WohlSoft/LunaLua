#pragma once
#include "FFmpegMediaPlayer2.h"
#include <SDL2/SDL.h>
#include "../../SdlMusic/SdlMusPlayer.h"
#include "../../Globals.h"
#include <emmintrin.h>

#define THDEC (std::function<void(FFmpegThreadFuncController*)>)[&](FFmpegThreadFuncController* a)

FFmpegThread* FFmpegMediaPlayer2::videoOutputThread = new FFmpegThread();
void FFmpegMediaPlayer2::initVars() {
	audDataSize = 0;
	audDataIndex = 0;
	for (int i = 0; i < LOOP_OVERWRAP; i++) {
		audioAbsTime[i] = 0;
		audioTime[i] = 0;
	}
	audioNextTime = 0;
	audioLoopCount = 0;
	nextLoopInit = false;
	videoOutputLoop = NULL;
	maskOutputLoop = NULL;
	queueThreadLoop = NULL;
	SDLCallback = NULL;
	media = NULL;
	av_init_packet(&apkt);
	audioDecoder = NULL;
	aBuf = NULL;
	shouldPlay = false;
	for (int i = 0; i < FMEDIA_NUM; i++) {
		FMEDIA m = (FMEDIA)i;
		queues[m] = NULL;
		mediaSeekReq[m] = false;
		loadCompleted[m] = false;
		vMediaDecoder[m] = NULL;
		vMediaTime[m] = 0;
		vMediaDelay[m] = 0;
		vMediaLoopCount[m] = 0;
		vMediaLastRemain[m] = -1;
		vMediaOutH[m] = 0;
		vMediaOutW[m] = 0;
		vMediaOutput[m] = NULL;
	}
	seekPos = -DBL_MAX;
	loop = true;
	waitEnd = false;
	altAlpha=-1;
	alphaSws = NULL;
	swsAlphaBuf = NULL;
	alphaType = PMUL;
}

FFmpegMediaPlayer2::~FFmpegMediaPlayer2() {
	if (videoOutputLoop)FFmpegMediaPlayer2::videoOutputThread->delWork(videoOutputLoop);
	if (maskOutputLoop)FFmpegMediaPlayer2::videoOutputThread->delWork(maskOutputLoop);
	if (queueThreadLoop)FFmpegDecodeQueue::queueThread->delWork(queueThreadLoop);
	if (SDLCallback)PGE_MusPlayer::removePostMixFunc(SDLCallback);

	if (videoOutputLoop)delete videoOutputLoop;
	if (maskOutputLoop)delete maskOutputLoop;
	if (queueThreadLoop)delete queueThreadLoop;
	if (SDLCallback)delete SDLCallback;

	if (audioDecoder)delete audioDecoder;
	for (int i = 0; i < 3; i++) {
		if (vMediaDecoder[i])delete vMediaDecoder[i];
		if (queues[i])delete queues[i];
	}
	if (media)delete media;
	if (aBuf)free(aBuf);
	if (alphaSws)sws_freeContext(alphaSws);
	if (swsAlphaBuf)av_freep(&swsAlphaBuf);
}

void FFmpegMediaPlayer2::initDecoder() {
	if (media->mediaAvailable(FVIDEO)) {
		vMediaDecoder[FVIDEO] = media->getVideoDecodeComponent();
	}

	if (media->mediaAvailable(FAUDIO)) {
		audioDecoder = media->getAudioDecodeComponent();
		aBuf = (uint8_t*)malloc(44100 * 4 * sizeof(uint8_t));
	}

	if (media->mediaAvailable(FMASK)) {
		vMediaDecoder[FMASK] = media->getMaskDecodeComponent();
	}
}

void FFmpegMediaPlayer2::initWorks() {

	if (maskOutputLoop)videoOutputThread->addWork(maskOutputLoop);

	//render main video after mask for alpha substituion
	if (videoOutputLoop)videoOutputThread->addWork(videoOutputLoop);

	

	FFmpegDecodeQueue::queueThread->addWork(queueThreadLoop);

	PGE_MusPlayer::addPostMixFunc(SDLCallback);
}



void FFmpegMediaPlayer2::initQueue() {
	queues[FAUDIO] = new FFmpegDecodeQueue(44100 * 2, 128);
	queues[FVIDEO] = new FFmpegDecodeQueue(800 * 600 * 4 * 65, 65); //consider H.264 max frameref (16)
	queues[FMASK] = new FFmpegDecodeQueue(800 * 600 * 4 * 65, 65);
}

void FFmpegMediaPlayer2::init() {
	initDecoder(); 
	initQueue(); 
	initCallback(); 
	initWorks(); 
}

FFmpegMediaPlayer2::FFmpegMediaPlayer2() {
	initVars();
}

FFmpegMediaPlayer2::FFmpegMediaPlayer2(std::wstring filename) :FFmpegMediaPlayer2() {
	media = new FFmpegMedia2(filename);
	init();
	
}

bool FFmpegMediaPlayer2::shouldEnd()const {
	return !gLunaLua.isReady() && (GM_EPISODE_MODE || GM_LEVEL_MODE);
}


bool FFmpegMediaPlayer2::needPlay()const {
	return gLunaLua.isReady() && gFrames > 0 && shouldPlay;
}

void FFmpegMediaPlayer2::play() {
	shouldPlay = true;
}

void FFmpegMediaPlayer2::pause() {
	shouldPlay = false;
}

void FFmpegMediaPlayer2::stop() {
	seek(0);
	pause();
}

int FFmpegMediaPlayer2::getWidth()const {
	return __getVMediaWidth(FVIDEO);
}

int FFmpegMediaPlayer2::getMaskWidth()const {
	return __getVMediaWidth(FMASK);
}

int FFmpegMediaPlayer2::getHeight()const {
	return __getVMediaHeight(FVIDEO);
}

int FFmpegMediaPlayer2::getMaskHeight()const {
	return __getVMediaHeight(FMASK);
}

void FFmpegMediaPlayer2::setAltAlpha(int altCh) {
	altAlpha = altCh;
}

int FFmpegMediaPlayer2::getAltAlpha()const {
	return altAlpha;
}

void FFmpegMediaPlayer2::setAlphaType(int mode) {
	alphaType = (AlphaType)mode;
}

int FFmpegMediaPlayer2::getAlphaType()const {
	return alphaType;
}

void FFmpegMediaPlayer2::setVideoDelay(double d) {
	vMediaDelay[FVIDEO] = d;
}

double FFmpegMediaPlayer2::getVideoDelay() const {
	return vMediaDelay[FVIDEO];
}

void FFmpegMediaPlayer2::setMaskDelay(double d) {
	vMediaDelay[FMASK] = d;
}

double FFmpegMediaPlayer2::getMaskDelay()const {
	return vMediaDelay[FMASK];
}

void FFmpegMediaPlayer2::setVideoOutput(uint8_t* out, int w, int h) {
	__setVMediaOutput(FVIDEO, out, w, h);
}

void FFmpegMediaPlayer2::setMaskOutput(uint8_t* out, int w, int h) {
	__setVMediaOutput(FMASK, out, w, h);
}
bool FFmpegMediaPlayer2::playable()const {
	return media->mediaAvailable(FVIDEO);
}

bool FFmpegMediaPlayer2::maskExist()const {
	return media->mediaAvailable(FMASK);
}

void FFmpegMediaPlayer2::seek(double pos) {
	if (pos > 0) {
		seekPos = pos;
		for (int i = 0; i < FMEDIA_NUM; i++) {
			if (media->mediaAvailable((FMEDIA)i))mediaSeekReq[i] = true;
		}
		
	}
	else {
		if (media->mediaAvailable(FAUDIO)) {
			__mediaSeek(FAUDIO, 0);

			audioAbsTime[0] = 0;
			audioTime[0] = 0;
			audioNextTime = 0;
			audioLoopCount = 0;
			nextLoopInit = false;
		}
		if (media->mediaAvailable(FVIDEO)) {
			__mediaSeek(FVIDEO, 0);
			vMediaTime[FVIDEO] = 0;
			vMediaLoopCount[FVIDEO] = 0;
			vMediaLastRemain[FVIDEO] = -1;
		}

		if (media->mediaAvailable(FMASK)) {
			__mediaSeek(FMASK, 0);
			vMediaTime[FMASK] = 0;
			vMediaLoopCount[FMASK] = 0;
			vMediaLastRemain[FMASK] = -1;
		}
	}
	if (media->mediaAvailable(FVIDEO) || media->mediaAvailable(FMASK))videoOutputThread->boost = true;
	FFmpegDecodeQueue::queueThread->boost = true;
}


int FFmpegMediaPlayer2::__getVMediaWidth(FMEDIA vmt)const {
	if (media->mediaAvailable(vmt)) {
		return vMediaDecoder[vmt]->reqW;
	}
	else {
		return -1;
	}
}

int FFmpegMediaPlayer2::__getVMediaHeight(FMEDIA vmt)const {
	if (media->mediaAvailable(vmt)) {
		return vMediaDecoder[vmt]->reqH;
	}
	else {
		return -1;
	}
}


void FFmpegMediaPlayer2::__setVMediaOutput(FMEDIA mt, uint8_t* out, int w, int h) {
	vMediaOutput[mt] = out;
	vMediaOutW[mt] = w;
	vMediaOutH[mt] = h;
}

bool FFmpegMediaPlayer2::setAudioDecodePktPtr(FFmpegAudioDecodeComponent2* decoder, CustomAVPacket** pktPtr) {
	if (decoder->needPacket()) {
		if (!queues[FAUDIO]->pop(apkt)) {
			
			return false;
		}
		
		*pktPtr = &apkt;
	}
	else {
		*pktPtr = NULL;
	}
	return true;
}

inline void FFmpegMediaPlayer2::mixIntoSDLBuffer(uint8_t* sdlBuf, uint8_t* dataPtr, int dataLen) {
	SDL_MixAudioFormat(
		sdlBuf,
		dataPtr,
		AUDIO_S16,
		dataLen,
		100); //set volume
}

inline double FFmpegMediaPlayer2::calcAudioSampleDuration(int bytes)const {
	return 1000 * bytes / (double)(FFmpegAudioDecodeComponent2::DEFAULT_OUT_CHANNELS * 2 * FFmpegAudioDecodeComponent2::DEFAULT_OUT_SAMPLE_RATE);
}

inline void FFmpegMediaPlayer2::setCurrentAudioTime(double msec,int loop) {
	audioTime[loop%LOOP_OVERWRAP] = msec;
	audioAbsTime[loop%LOOP_OVERWRAP] = SDL_GetTicks();
}

inline double FFmpegMediaPlayer2::getCurrentAudioTime(int loop)const {
	if (loop > audioLoopCount)return 0;
	int q_loop = loop%LOOP_OVERWRAP;
	double currentTime = SDL_GetTicks() - audioAbsTime[q_loop] + audioTime[q_loop];
	if (loop == audioLoopCount) {
		return min(audioNextTime, max(audioTime[q_loop], currentTime));
		
	}
	else {
		return max(audioTime[q_loop], currentTime);
	}
}

inline double FFmpegMediaPlayer2::getTimestampFromVideoFrame(FMEDIA vmt, AVFrame* fr, double lastFrameTime)const {
	return 1000 * ((av_frame_get_best_effort_timestamp(fr) + fr->repeat_pict / 2)*media->getTimeBase(vmt));
}

void FFmpegMediaPlayer2::writeVideoIntoBuffer(uint8_t* buffer, int bufW, int bufH, uint8_t* vData, int dataW, int dataH,uint8_t* altAlphaData,int alphaW,int alphaH,int altCh) {
	if (buffer == NULL || vData == NULL)return;
	int h = min(bufH, dataH);
	int l_dataW = min(dataW, bufW);

	if (altAlphaData == NULL || altCh<0 || altCh>3) {
		
		if (bufW == dataW) {
			memcpy(buffer, vData, dataW*h);
		}
		else {

			for (int i = 0; i < h; i++) {
				memcpy(buffer + i*bufW, vData + i*dataW, l_dataW);
			}
		}
	}
	else {
		int l_alphaW = min(alphaW, bufW);
		int l_loop = min(l_alphaW, l_dataW);
			switch (alphaType) {
			case STR:
					for (int i = 0; i < h; i++) {
						for (int j = 0; j < l_loop; j += 4) {
							buffer[i*bufW + j + 0] = vData[i*dataW + j + 0];
							buffer[i*bufW + j + 1] = vData[i*dataW + j + 1];
							buffer[i*bufW + j + 2] = vData[i*dataW + j + 2];
							buffer[i*bufW + j + 3] = altAlphaData[i*alphaW + j + altCh];
						}
						for (int j = l_loop; j < l_dataW; j += 4) {
							*(uint32_t*)&(buffer[i*bufW + j]) = *(uint32_t*)&(vData[i*dataW + j]);
						}
					}
				
				break;


			case PMUL:
			default:
					for (int i = 0; i < h; i++) {
						for (int j = 0; j < l_loop; j += 4) {
							if (vData[i*dataW + j + 3] == 255) {
								buffer[i*bufW + j + 0] = (vData[i*dataW + j + 0] * altAlphaData[i*alphaW + j + altCh] * 257 + 257) >> 16;
								buffer[i*bufW + j + 1] = (vData[i*dataW + j + 1] * altAlphaData[i*alphaW + j + altCh] * 257 + 257) >> 16;
								buffer[i*bufW + j + 2] = (vData[i*dataW + j + 2] * altAlphaData[i*alphaW + j + altCh] * 257 + 257) >> 16;
								buffer[i*bufW + j + 3] = altAlphaData[i*alphaW + j + altCh];
							}else if (vData[i*dataW + j + 3] == 0) {
								buffer[i*bufW + j + 0] = buffer[i*bufW + j + 1] = buffer[i*bufW + j + 2] = 0;
								buffer[i*bufW + j + 3] = altAlphaData[i*alphaW + j + altCh];
							}
							else {
								buffer[i*bufW + j + 0] = vData[i*dataW + j + 0] * altAlphaData[i*alphaW + j + altCh] / vData[i*dataW + j + 3];
								buffer[i*bufW + j + 1] = vData[i*dataW + j + 1] * altAlphaData[i*alphaW + j + altCh] / vData[i*dataW + j + 3];
								buffer[i*bufW + j + 2] = vData[i*dataW + j + 2] * altAlphaData[i*alphaW + j + altCh] / vData[i*dataW + j + 3];
								buffer[i*bufW + j + 3] = altAlphaData[i*alphaW + j + altCh];
							}
							
						}
						for (int j = l_loop; j < l_dataW; j += 4) {
							*(uint32_t*)&(buffer[i*bufW + j]) = *(uint32_t*)&(vData[i*dataW + j]);
						}
					}
				
				break;


			}
	}
}

void FFmpegMediaPlayer2::packetQueue(FMEDIA mt) {
	CustomAVPacket p;
	av_init_packet(&p);
	int end = 0;
	while (media->mediaAvailable(mt) && queues[mt]->queueable()) {

		if ((end = media->readMediaFrame(mt, &p)) >= 0) {
			if (queues[mt]->dataCount()>0) {
				
				CustomAVPacket& lp = queues[mt]->lastPacket();
				lp.userdata = 1;
				lp.decodeReady = true;
			}
			queues[mt]->push(p);
			loadCompleted[mt] = false;
		}
		else {
			loadCompleted[mt] = (end == AVERROR_EOF);
			if (queues[mt]->dataCount() > 0) {
				CustomAVPacket& lp = queues[mt]->lastPacket();
				lp.userdata = loadCompleted[mt] ? 2 : -1;
				lp.decodeReady = true;
			}
			if (loadCompleted[mt] && loop) {
				media->seek(mt, 0);
				if (media->readMediaFrame(mt, &p) >= 0) {
					queues[mt]->push(p);
					loadCompleted[mt] = false;
				}
			}
			return;
		}

	}
}
int FFmpegMediaPlayer2::__mediaSeek(FMEDIA mt, double pos) {
	if (!media->mediaAvailable(mt))return -1;
	int ret = media->seek(mt, (int64_t)round(pos*AV_TIME_BASE));
	if (ret < 0)return ret;
	__mediaFlush(mt);
	queues[mt]->clear();
	switch (mt) {
	case FAUDIO:
		audioTime[0] = audioNextTime = pos * 1000;
		audioAbsTime[0] = SDL_GetTicks();
		audioLoopCount = 0;
		nextLoopInit = false;
		break;
	case FVIDEO:
		vMediaTime[mt] = pos * 1000;
		vMediaLoopCount[mt] = 0;
		vMediaLastRemain[mt] = -1;
		break;
	case FMASK:
		vMediaTime[mt] = pos * 1000;
		vMediaLoopCount[mt] = 0;
		vMediaLastRemain[mt] = -1;
		break;
	default:
		break;
	}
	return ret;

}

void FFmpegMediaPlayer2::__mediaFlush(FMEDIA mt) {
	switch (mt) {
	case FAUDIO:
		audioDecoder->flush();
		break;
	case FVIDEO:
		vMediaDecoder[mt]->flush();
		break;
	case FMASK:
		vMediaDecoder[mt]->flush();
		break;
	default:
		break;
	}
}

void FFmpegMediaPlayer2::setAlphaByChannel(uint8_t* dest,int destW,int destH,uint8_t* src,int srcW,int srcH, int srcCh) {
	if (srcCh<0 || srcCh>3 ||(dest == src && srcCh == 3))return; //id. map
	uint8_t* srcPtr = src;
	if (!(destW == srcW && destH == srcH)) {
		uint8_t* tmpDataArr[1] = { src };
		int tmpLSizeArr[1] = { srcW*4 };
		int tmpDestLSizeArr[1] = { destW*4 };
		if (!alphaSws) {
			alphaSws = sws_getContext(srcW, srcH, AV_PIX_FMT_BGRA, destW, destH, AV_PIX_FMT_BGRA, NULL, NULL, NULL, NULL);
			swsAlphaBuf = (uint8_t*)av_malloc(avpicture_get_size(AV_PIX_FMT_BGRA, destW, destH));
		}
		uint8_t* tmpDestDataArr[1] = { swsAlphaBuf };
		sws_scale(alphaSws, tmpDataArr, tmpLSizeArr, 0, srcH, tmpDestDataArr, tmpDestLSizeArr);
		srcPtr = tmpDestDataArr[0];
	}
	for (int i = 0; i < destH; i++) {
		for (int j = 0; j < destW; j++) {
			dest[i*destW * 4 + j * 4 + 3] = srcPtr[i*destW * 4 + j * 4 + srcCh];
		}
	}
}

FFmpegThreadFunc* FFmpegMediaPlayer2::__videoOutputFuncGen(FMEDIA vmt) {
	return new FFmpegThreadFunc((std::function<void(FFmpegThreadFuncController*)>)[&, vmt](FFmpegThreadFuncController* a) {
		if (waitEnd)return;
		if (mediaSeekReq[vmt]) {
			__mediaSeek(vmt, seekPos);
			mediaSeekReq[vmt] = false;
		}

		if (!mediaSeekReq[vmt] && !mediaSeekReq[FAUDIO] && videoOutputThread->boost) {
			videoOutputThread->boost = false;
		}
		if (!media->mediaAvailable(vmt) || !needPlay())return;

		double currentAudioTime = getCurrentAudioTime(vMediaLoopCount[vmt]);
		
		CustomAVPacket vPkt;
		AVFrame* usedFrame = NULL, *outputFrame = NULL;
		int got_picture = 0, height = 0;
		//GM_STAR_COUNT = vMediaTime[vmt] / 10;

		while (currentAudioTime >= vMediaTime[vmt] + vMediaDelay[vmt] && !mediaSeekReq[vmt] && !mediaSeekReq[FAUDIO]) {
			got_picture = 0; usedFrame = NULL;
			
			if (!queues[vmt]->pop(vPkt)) {
				return;
			}
			got_picture = vMediaDecoder[vmt]->decode(&vPkt, &usedFrame);
			
			if (vMediaLastRemain[vmt]>=0) {
				if(got_picture)vMediaLastRemain[vmt]--;
				
				if (vMediaLastRemain[vmt] == -1) {
					vMediaLoopCount[vmt]++;
					currentAudioTime = getCurrentAudioTime(vMediaLoopCount[vmt]);
					
				}
			}
			av_free_packet(&vPkt);
			if (vPkt.userdata == 2) {
				vMediaLastRemain[vmt] = max(vMediaDecoder[vmt]->cdcctx->delay,0)+1;
				
			}
			vMediaTime[vmt] = getTimestampFromVideoFrame(vmt, usedFrame, vMediaTime[vmt]) + 15.6 * 2;
		}
		if (got_picture && vMediaOutput[vmt]) {
			height = vMediaDecoder[vmt]->resize(usedFrame, &outputFrame);
			if (vmt == FVIDEO) {
				if (0 <= altAlpha && altAlpha <= 2) { //3==id
					writeVideoIntoBuffer(vMediaOutput[vmt], vMediaOutW[vmt] * 4, vMediaOutH[vmt], outputFrame->data[0], outputFrame->linesize[0], height,
						outputFrame->data[0], outputFrame->linesize[0], height, altAlpha);
				}
				else if (vMediaOutput[FMASK] && 4 <= altAlpha && altAlpha <= 7) {
					writeVideoIntoBuffer(vMediaOutput[vmt], vMediaOutW[vmt] * 4, vMediaOutH[vmt], outputFrame->data[0], outputFrame->linesize[0], height,
						vMediaOutput[FMASK], vMediaOutW[FMASK]*4, vMediaOutH[FMASK], altAlpha - 4);
				}
				else {
					writeVideoIntoBuffer(vMediaOutput[vmt], vMediaOutW[vmt] * 4, vMediaOutH[vmt], outputFrame->data[0], outputFrame->linesize[0], height,
						NULL, 0, 0, -1);
				}
			}
			else {
				writeVideoIntoBuffer(vMediaOutput[vmt], vMediaOutW[vmt] * 4, vMediaOutH[vmt], outputFrame->data[0], outputFrame->linesize[0], height,
					NULL, 0,0,-1);
			}
		}

	});
}
void FFmpegMediaPlayer2::initCallback() {
	videoOutputLoop = __videoOutputFuncGen(FVIDEO);
	maskOutputLoop = __videoOutputFuncGen(FMASK);
	queueThreadLoop = new FFmpegThreadFunc((std::function<void(FFmpegThreadFuncController*)>)[&](FFmpegThreadFuncController* a) {
		if (waitEnd)return;
		if (shouldEnd()) {
			waitEnd = true;
		}
		bool nsreq = true;
		for (int i = 0; i < FMEDIA_NUM; i++) {
			nsreq = nsreq&&!mediaSeekReq[i];
		}
		if (nsreq && FFmpegDecodeQueue::queueThread->boost) {
			FFmpegDecodeQueue::queueThread->boost = false;
		}
		for (int i = 0; i < FMEDIA_NUM; i++) {
			packetQueue((FMEDIA)i);
		}
	});

	SDLCallback = new PGE_PostMixFunc((std::function<void(void *udata, uint8_t *stream, int len)>)[&](void *udata, uint8_t *stream, int len) {
		if (waitEnd)return;
		if (mediaSeekReq[FAUDIO]) {
			__mediaSeek(FAUDIO, seekPos);
			mediaSeekReq[FAUDIO] = false;
		}
		if (!media->mediaAvailable(FAUDIO) || !needPlay())return;
		CustomAVPacket* pktPtr;
		int len1 = 0, initLen = len;
		int loopCount = 0; bool audioTimeUpdated = false; double currentAudioTime = -1;

		while (len > 0) {
			loopCount++;
			if (audDataIndex >= audDataSize) {
				
				if (!setAudioDecodePktPtr(audioDecoder, &pktPtr))break;
				if (pktPtr) {
					if (nextLoopInit) {
						nextLoopInit = false;
						for (int k = 0; k < LOOP_CLEAN_PADDING; k++) {
							audioAbsTime[(k + audioLoopCount + 1) % LOOP_OVERWRAP] = 0;
							audioTime[(k + audioLoopCount + 1) % LOOP_OVERWRAP] = 0;
						}
						audioLoopCount++;
					}
					if (pktPtr->pts != AV_NOPTS_VALUE) {

						setCurrentAudioTime(
							1000 * media->getTimeBase(FAUDIO)*pktPtr->pts
							- calcAudioSampleDuration(initLen - len),
							audioLoopCount
							);
						audioTimeUpdated = true;
					}
					if (pktPtr->userdata == 2) {
						nextLoopInit = true;
					}
				}
				

				
				audDataSize = audioDecoder->decode(pktPtr, aBuf);
				if (audDataSize == 0)continue;

				if (audDataSize < 0) {
					return; //error
				}
				else if (audDataSize>0) {
					audDataIndex = 0;
				}
			}
			if (!audioTimeUpdated) {
				setCurrentAudioTime(audioNextTime, audioLoopCount);
				audioTimeUpdated = true;
			}
			len1 = min(len, audDataSize - audDataIndex);
			mixIntoSDLBuffer(stream, aBuf + audDataIndex, len1);
			len -= len1;
			stream += len1;
			audDataIndex += len1;
		}
		audioNextTime = audioTime[audioLoopCount%LOOP_OVERWRAP] + calcAudioSampleDuration(initLen - len);

	});
}