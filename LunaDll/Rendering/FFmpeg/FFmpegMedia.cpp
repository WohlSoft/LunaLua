#include "FFmpegMedia.h"
#include "../../GlobalFuncs.h"
#include <math.h>
#include "../../Globals.h"

FFmpegMedia::~FFmpegMedia() {
	
	if (vidCodecCtx) {
		avcodec_close(vidCodecCtx);
		//avcodec_free_context(&vidCodecCtx);
	}
	if (audCodecCtx) {
		avcodec_close(audCodecCtx);
		//avcodec_free_context(&audCodecCtx);
	}
	if (maskCodecCtx) {
		avcodec_close(maskCodecCtx);
	}
	
	if (fmtCtx) {
		avformat_close_input(&fmtCtx);
	}
	if (mFmtCtx) {
		avformat_close_input(&mFmtCtx);
	}
}

void FFmpegMedia::init() {
	vidStreamIdx = audStreamIdx = maskStreamIdx=-1;
	_videoAvailable = _audioAvailable = false;
	fmtCtx = mFmtCtx=NULL;
	FPS = { 0,1 };
	video = NULL; audio = NULL;
	vidCodecCtx = audCodecCtx = maskCodecCtx=NULL;
	//rawBitmapBuffer = NULL;
	//rawBufferFrameNum = 0;
}

FFmpegMedia::FFmpegMedia() {
	init();
}
FFmpegMedia::FFmpegMedia(std::wstring filepath):FFmpegMedia() {

	if (filepath.length() < 1)return;
	int ret = 0;

	/* open file */
    ret = avformat_open_input(
				&fmtCtx,					//get format context
                WStr2StrA(filepath).c_str(), //filepath(cast from wstring to char)
				NULL,						//nothing to specify about input format
				NULL);						//no options

	if (ret != 0)return;	//return when failed to open

	/* set stream info */
	ret = avformat_find_stream_info(fmtCtx, NULL);

	if (ret < 0)return;		//return when no stream found
	
	/* find most appropriate video stream */
	AVCodec *vidCodec,*audCodec,*maskCodec;				//get codec info in this
	vidCodec = audCodec = maskCodec = NULL;
	vidCodec = audCodec = NULL;
	vidStreamIdx = audStreamIdx = -1;

	vidStreamIdx = av_find_best_stream(
					fmtCtx,					//file info
					AVMEDIA_TYPE_VIDEO,		//media type we want
					-1,						//no stream specified
					-1,						//no related stream spiecified
					&vidCodec,				//get codec(decoder) info
					0);						//(no flags available)

	audStreamIdx = av_find_best_stream(		//same as video
					fmtCtx,					
					AVMEDIA_TYPE_AUDIO,		
					-1,					
					-1,						
					&audCodec,				
					0);
	

	if (vidStreamIdx >= 0 && vidCodec != NULL) { // has video stream
		/* init codec */
		vidCodecCtx = fmtCtx->streams[vidStreamIdx]->codec;
		_videoAvailable = (avcodec_open2(vidCodecCtx, vidCodec, NULL) >=0);
		if (_videoAvailable) {
			video = fmtCtx->streams[vidStreamIdx];
			FPS = video->avg_frame_rate;
		}
	}

	if (audStreamIdx >= 0 && audCodec != NULL) { 
		audCodecCtx = fmtCtx->streams[audStreamIdx]->codec;
		_audioAvailable = (avcodec_open2(audCodecCtx, audCodec, NULL) >= 0);
		if (_audioAvailable)audio = fmtCtx->streams[audStreamIdx];
	}

	

	//mask open
	int extPos = filepath.find_last_of('.');
	std::wstring ext = extPos==std::wstring::npos?L"":filepath.substr(extPos, filepath.length() - extPos);
	std::wstring mainName = extPos == std::wstring::npos ? filepath : filepath.substr(0, extPos);
	ret = avformat_open_input(
		&mFmtCtx,					
        WStr2StrA(mainName+L"_m"+ext).c_str(),
		NULL,						
		NULL);
	
	if (ret != 0)return;
	ret = avformat_find_stream_info(mFmtCtx, NULL);
	if (ret < 0)return;
	

	maskStreamIdx = av_find_best_stream(
		mFmtCtx,					//file info
		AVMEDIA_TYPE_VIDEO,		//media type we want
		-1,						//no stream specified
		-1,						//no related stream spiecified
		&maskCodec,				//get codec(decoder) info
		0);

	if (maskStreamIdx >= 0 && maskCodec != NULL) {
		maskCodecCtx = mFmtCtx->streams[maskStreamIdx]->codec;
		_maskAvailable = (avcodec_open2(maskCodecCtx, maskCodec, NULL) >= 0);
		if (_maskAvailable)mask = mFmtCtx->streams[maskStreamIdx];
		
	}
	
	
end:
	return;
}

bool FFmpegMedia::isVideoAvailable() const {
	return _videoAvailable;
}

bool FFmpegMedia::isAudioAvailable() const {
	return _audioAvailable;
}

bool FFmpegMedia::isMaskAvailable() const{
	return _maskAvailable;
}

int FFmpegMedia::width() const {
	return isVideoAvailable()?vidCodecCtx->width:-1;
}

int FFmpegMedia::height() const {
	return isVideoAvailable() ? vidCodecCtx->height : -1;
}

double FFmpegMedia::duration() const {
	return max(
		isVideoAvailable() ? fmtCtx->streams[vidStreamIdx]->duration : -1,
		isAudioAvailable() ? fmtCtx->streams[audStreamIdx]->duration : -1
		);
}
/*
void FFmpegMedia::rawBufferAlloc(int size, int frameNum, uint8_t** buf) {
	buf = new uint8_t*[frameNum];
	rawBitmapBuffer[0] = (uint8_t*)malloc(frameNum*size);
	for (int i = 1; i < frameNum; i++) {
		rawBitmapBuffer[i] = ((uint8_t*)rawBitmapBuffer[i - 1]) + size;
	}
}

void FFmpegMedia::rawBufferFree(uint8_t** buf) {
	free(buf[0]);
}
*/
