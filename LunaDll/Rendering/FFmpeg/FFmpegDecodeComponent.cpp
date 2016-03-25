#pragma once
#include "FFmpegDecodeComponent.h"
#include "../../Defines.h"
FFmpegVideoDecodeComponent2::FFmpegVideoDecodeComponent2() {
	initVar();
}

FFmpegVideoDecodeComponent2::FFmpegVideoDecodeComponent2(AVCodecContext* src):FFmpegVideoDecodeComponent2() {
	initVar();
	init(src);
}

FFmpegVideoDecodeComponent2::~FFmpegVideoDecodeComponent2() {
	if (vsrc)av_frame_free(&vsrc);
	if (vdest)av_frame_free(&vdest);
	if (swsctx)sws_freeContext(swsctx);
	if (vbuf)av_freep(&vbuf);
	DeleteCriticalSection(&crSect);
}
void FFmpegVideoDecodeComponent2::initVar() {
	vsrc = vdest = NULL;
	vbuf = NULL;
	swsctx = NULL;
	cdcctx = NULL;
	reqW = reqH = -1;
	InitializeCriticalSectionEx(&crSect,4000, CRITICAL_SECTION_NO_DEBUG_INFO);
}
bool FFmpegVideoDecodeComponent2::init(AVCodecContext* src) {
	int w, h;
	getAlignedSize(src, &w, &h);
	if (!setSwsCtx(src, w, h, DEFALUT_OUT_PIX_FMT))return false;
	if (!setVideoBuffer(w, h, DEFALUT_OUT_PIX_FMT))return false;
	if (!allocVideoFrame())return false;
	avpicture_fill((AVPicture*)vdest, vbuf, DEFALUT_OUT_PIX_FMT, w, h);
	cdcctx = src;
	reqW = w;
	reqH = h;
}

void FFmpegVideoDecodeComponent2::getAlignedSize(AVCodecContext* videoCodec, int* alW, int* alH) {
	int _w, _h, al[8];
	_w = videoCodec->width;
	_h = videoCodec->height;
	avcodec_align_dimensions2(videoCodec, &_w, &_h, al);
	*alW = _w;
	*alH = _h;
}

inline bool FFmpegVideoDecodeComponent2::setSwsCtx(AVCodecContext* src, int outW, int outH, AVPixelFormat outPixFmt) {
	return NULL != (swsctx = sws_getContext(
		src->width,src->height,src->pix_fmt,
		outW,outH,outPixFmt,
		SWS_BILINEAR,
		NULL,NULL,NULL
		));
}

inline bool FFmpegVideoDecodeComponent2::setVideoBuffer(int outW, int outH, AVPixelFormat outPixFmt) {
	return NULL != (vbuf = (uint8_t*)av_malloc(avpicture_get_size(outPixFmt, outW, outH)*sizeof(uint8_t)));
}

inline bool FFmpegVideoDecodeComponent2::allocVideoFrame() {
	return ((vsrc = av_frame_alloc()) != NULL) && ((vdest = av_frame_alloc()) != NULL);
}

int FFmpegVideoDecodeComponent2::decode(AVPacket* pkt, AVFrame** decodeUsed) {
	*decodeUsed = NULL;
	int got_picture = 0;
	EnterCriticalSection(&crSect);
	avcodec_decode_video2(cdcctx, vsrc, &got_picture, pkt);
	LeaveCriticalSection(&crSect);
	*decodeUsed = vsrc;
	return got_picture;
	//exception for bad pointer
}

int FFmpegVideoDecodeComponent2::resize(AVFrame* decoded,AVFrame** out_resized) {
	int height = sws_scale(swsctx, decoded->data, decoded->linesize, 0, decoded->height, vdest->data, vdest->linesize);
	*out_resized = vdest;
	return height;
}

void FFmpegVideoDecodeComponent2::flush() {
	EnterCriticalSection(&crSect);
	avcodec_flush_buffers(cdcctx);
	LeaveCriticalSection(&crSect);
	/*
	if (vsrc)av_frame_free(&vsrc);
	if (vdest)av_frame_free(&vdest);
	allocVideoFrame();
	*/
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
FFmpegAudioDecodeComponent2::FFmpegAudioDecodeComponent2() {
	memset(this, 0, sizeof(FFmpegAudioDecodeComponent2));
}


FFmpegAudioDecodeComponent2::FFmpegAudioDecodeComponent2(AVCodecContext* src):FFmpegAudioDecodeComponent2() {
	init(src);
}


FFmpegAudioDecodeComponent2::~FFmpegAudioDecodeComponent2() {

	if (audFrame)av_frame_free(&audFrame);
	if (swrCtx)swr_free(&swrCtx);
	if (audDestData) {
		av_freep(&audDestData[0]);
		av_freep(&audDestData);
	}
	audPktData = NULL;//this is copied from AVPacket
	if (audBuf)free(audBuf);
	DeleteCriticalSection(&crSect);
}

bool FFmpegAudioDecodeComponent2::init(AVCodecContext* src) {
	if (!setSwrCtx(src, DEFAULT_OUT_CHANNEL_LAYOUT, DEFAULT_OUT_SAMPLE_RATE, DEFAULT_OUT_SAMPLE_FMT))return false;
	if (!allocAudioFrame())return false;
	if (!setAudioBuffer())return false;
	cdcctx = src;
	av_init_packet(&internalPkt);
	InitializeCriticalSectionEx(&crSect,4000, CRITICAL_SECTION_NO_DEBUG_INFO);
}

inline bool FFmpegAudioDecodeComponent2::setSwrCtx(AVCodecContext* src, int outChLayout, int outSRate, enum AVSampleFormat outSampleFmt) {
	swrCtx =swr_alloc_set_opts(
		NULL,
		outChLayout,
		outSampleFmt,
		outSRate,
		src->channel_layout ? src->channel_layout : DEFAULT_OUT_CHANNEL_LAYOUT,
		src->sample_fmt == AV_SAMPLE_FMT_NONE ? DEFAULT_OUT_SAMPLE_FMT : src->sample_fmt,
		src->sample_rate ? src->sample_rate : DEFAULT_OUT_SAMPLE_RATE,
		NULL, NULL);
	return (swr_init(swrCtx) >= 0);
}

inline bool FFmpegAudioDecodeComponent2::allocAudioFrame() {
	return NULL != (audFrame = av_frame_alloc());
}

inline bool FFmpegAudioDecodeComponent2::setAudioBuffer() {
	return NULL != (audBuf = (uint8_t*)malloc(DEFAULT_BUFFER_SIZE*sizeof(uint8_t)));
}

int FFmpegAudioDecodeComponent2::resample(AVFrame* decodedFrame, uint8_t***dest_data) {
	int ret = 0;
	int srcSRate = cdcctx->sample_rate ? cdcctx->sample_rate : DEFAULT_OUT_SAMPLE_RATE;
	audDestMaxSamples = audDestSamples = (int)av_rescale_rnd(
		decodedFrame->nb_samples,
		DEFAULT_OUT_SAMPLE_RATE,
		srcSRate,
		AV_ROUND_UP);

	ret = av_samples_alloc_array_and_samples(
		dest_data,
		&audDestLSize,
		DEFAULT_OUT_CHANNELS,
		audDestSamples,
		DEFAULT_OUT_SAMPLE_FMT,
		0);

	if (ret < 0)return -1;
	audDestSamples = (int)av_rescale_rnd(
		swr_get_delay(swrCtx, srcSRate) + decodedFrame->nb_samples,
		DEFAULT_OUT_SAMPLE_RATE,
		srcSRate,
		AV_ROUND_UP);

	if (audDestSamples > audDestMaxSamples) {
		//av_freep((&(*dest_data)[0]));
		//av_freep(dest_data);
		ret = av_samples_alloc(
			*dest_data,
			&audDestLSize,
			DEFAULT_OUT_CHANNELS,
			audDestSamples,
			DEFAULT_OUT_SAMPLE_FMT,
			1);
		if (ret < 0)return -1;
		audDestMaxSamples = audDestSamples;
	}
	ret = swr_convert(
		swrCtx,
		*dest_data,
		audDestSamples,
		(const uint8_t**)decodedFrame->data,
		decodedFrame->nb_samples
		);
	if (ret < 0)return -1;
	return av_samples_get_buffer_size(
		&audDestLSize,
		DEFAULT_OUT_CHANNELS,
		ret,
		DEFAULT_OUT_SAMPLE_FMT,
		1);
}

bool FFmpegAudioDecodeComponent2::needPacket() {
	return audPktSize <= 0;
}

//NOTE:this method frees the given packet later for performance
int FFmpegAudioDecodeComponent2::decode(AVPacket* pkt, uint8_t* out) {
	int len1=0,got_sound=0,audDestBufSize=0;

	if (pkt) {
		
		av_dup_packet(pkt);
		internalPkt = *pkt;
		audPktSize = internalPkt.size;
		audPktData = internalPkt.data;
	}
	
		while (audPktSize > 0) {
			av_frame_unref(audFrame);
			EnterCriticalSection(&crSect);
			len1 = avcodec_decode_audio4(cdcctx, audFrame, &got_sound, &internalPkt);
			LeaveCriticalSection(&crSect);
			if (len1 < 0) {
				audPktSize = 0;
				LeaveCriticalSection(&crSect);
				return -1;
			}
			
			audPktData += len1;
			audPktSize -= len1;
			audDestBufSize = 0;
			if (got_sound) {
				audDestBufSize = resample(audFrame, &audDestData);
				
				if (audDestBufSize > 0) {
					memcpy(out, audDestData[0], audDestBufSize);
				}
				av_freep(&audDestData[0]);
				av_freep(&audDestData);
			}
			if (audDestBufSize <= 0) {
				continue;
			}
			if (audPktSize <= 0) {
				av_free_packet(&internalPkt);
			}
			return audDestBufSize;
		}
	
		return 0;
}


void FFmpegAudioDecodeComponent2::flush() {
	EnterCriticalSection(&crSect);
	avcodec_flush_buffers(cdcctx);
	LeaveCriticalSection(&crSect);
	/*
	if (audFrame)av_frame_free(&audFrame);
	allocAudioFrame();
	*/
}