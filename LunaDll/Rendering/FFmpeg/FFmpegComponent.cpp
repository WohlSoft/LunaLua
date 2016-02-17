#include "FFmpegComponent.h"

FFmpegVideoDecodeComponent::FFmpegVideoDecodeComponent() {
	memset(this, 0, sizeof(FFmpegVideoDecodeComponent));
}

FFmpegVideoDecodeComponent::~FFmpegVideoDecodeComponent() {
	if (vBuffer)av_freep(&vBuffer);
	if (vidSrcFrame)av_frame_free(&vidSrcFrame);
	if (vidDestFrame)av_frame_free(&vidDestFrame);
	if (booked)av_frame_free(&booked);
	srcPtr = NULL; //this is copied
	if (swsCtx)sws_freeContext(swsCtx);
	if (vPkt.data)av_free_packet(&vPkt);
}

FFmpegAudioDecodeComponent::FFmpegAudioDecodeComponent() {
	memset(this, 0, sizeof(FFmpegAudioDecodeComponent));
}

FFmpegAudioDecodeComponent::~FFmpegAudioDecodeComponent() {

	if (audFrame)av_frame_free(&audFrame);
	if (swrCtx)swr_free(&swrCtx);
	if (audDestData) {
		av_freep(&audDestData[0]);
		av_freep(&audDestData);
	}
	audPktData = NULL;//this is copied from AVPacket
	if (audBuf)free(audBuf);
	if (aPkt.data)av_free_packet(&aPkt);

}