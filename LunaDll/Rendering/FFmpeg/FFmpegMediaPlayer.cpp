#include "FFmpegMediaPlayer.h"
#include <SDL2/SDL.h>

FFmpegThread* FFmpegMediaPlayer::videoOutputThread = new FFmpegThread();

void FFmpegMediaPlayer::init() {
	vidSrcFrame = NULL; vidDestFrame = NULL;
	vBuffer = NULL;
	swsCtx = NULL; swrCtx = NULL;
	outBuffer = NULL;
	got_picture = 0; got_sound = 0;
	audDestMaxSamples = 0; audDestSamples = 0;
	audDestData = NULL;
	audPktData = NULL;
	audPktSize = 0;
	audDestLSize = 0;
	audBuf = (uint8_t*)malloc(FFMP_AUDIO_BUFFER_SIZE*sizeof(uint8_t));
	audBufSize = 0;
	audBufOffset = 0;
	decodedFrameCount = 0;
	SDL_zero(sdlSpec);
	_videoPlayable = _audioPlayable = false;

	/* queue size will be changed while decoding */
	/* default size is assuming raw data */
	soundQueue = FFmpegDecodeQueue(44100*2); //0.5sec
	videoQueue = FFmpegDecodeQueue(800*600*4); //whole screen

	audPktSize = 0;
	audDestData = NULL;
	audPktData = NULL;

	__queue.func = [=](FFmpegThreadFuncController* ctrl) {
		int end = 0;
		while (!ctrl->quit) {
			if (soundQueue.dataSize() >= soundQueue.MAX_SIZE || videoQueue.dataSize() >= videoQueue.MAX_SIZE) continue;
			av_init_packet(&pkt);
			while ((end = av_read_frame(media.fmtCtx, &pkt)) == 0) {
				if (pkt.stream_index == media.audStreamIdx) {
					soundQueue.push(pkt);
				}
				else if (pkt.stream_index == media.vidStreamIdx) {
					videoQueue.push(pkt);
				}
				else {
					av_free_packet(&pkt);
				}
			}
			if (end == AVERROR_EOF)break;
		}
	};
	__outputVideoBuffer.func = [=](FFmpegThreadFuncController* ctrl) {
		if (ctrl->quit || ctrl->pause)return;
		if (!videoQueue.pop(vPkt))return;
		avcodec_decode_video2(media.vidCodecCtx, vidSrcFrame, &got_picture, &vPkt);
		if (got_picture) {
			sws_scale(
				swsCtx,
				vidSrcFrame->data,
				vidSrcFrame->linesize,
				0,
				vidSrcFrame->height,
				vidDestFrame->data,
				vidDestFrame->linesize
				);
			if (outBuffer != NULL) {
				for (int h = 0; h < decodeSetting.video.height; h++) {
					memcpy(
						(uint32_t*)outBuffer + h*decodeSetting.video.width,
						((uint8_t*)vidDestFrame->data[0]) + h*vidDestFrame->linesize[0],
						vidDestFrame->linesize[0]
						);
				}
			}
			decodedFrameCount++;
		}
	};
}

FFmpegMediaPlayer::FFmpegMediaPlayer() {
	init();
}
bool FFmpegMediaPlayer::initVideo(FFmpegMedia m, FFmpegVideoDecodeSetting* vs) {
	if (!m.isVideoAvailable())return false;
	/* make size aligned */
	avcodec_align_dimensions2(
		m.vidCodecCtx,
		&vs->width,
		&vs->height,
		NULL);

	/* use short variable name */
	AVPixelFormat pixFmt = vs->pixFmt;
	int _w = vs->width;
	int _h = vs->height;

	/* init resampling settings */
	swsCtx = sws_getContext(
		media.width(),				//src info
		media.height(),
		media.vidCodecCtx->pix_fmt,
		_w,	//dest info
		_h,
		pixFmt,
		decodeSetting.video.resampling_mode,
		NULL,
		NULL,
		NULL);
	if (swsCtx == NULL)return false;
	/* malloc decode buffer */
	vBuffer = (uint8_t*)av_malloc(avpicture_get_size(pixFmt, _w, _h)*sizeof(uint8_t));
	if (vBuffer == NULL)return false;
	/* assign buffer to frame */
	avpicture_fill((AVPicture*)vidDestFrame, vBuffer, pixFmt, _w, _h);
	return true;
}

bool FFmpegMediaPlayer::initAudio(FFmpegMedia m, FFmpegAudioDecodeSetting* as) {
	if (!m.isAudioAvailable())return false;
	swrCtx = swr_alloc();
	if (swrCtx == NULL)return false;
	av_opt_set_int(swrCtx, "in_channel_layout", m.audCodecCtx->channel_layout, 0);
	av_opt_set_int(swrCtx, "in_sample_rate", m.audCodecCtx->sample_rate, 0);
	av_opt_set_sample_fmt(swrCtx, "in_sample_fmt", m.audCodecCtx->sample_fmt, 0);

	av_opt_set_int(swrCtx, "out_channel_layout", as->channelLayout, 0);
	av_opt_set_int(swrCtx, "out_sample_rate", as->sample_rate, 0);
	av_opt_set_sample_fmt(swrCtx, "out_sample_fmt", as->sample_format, 0);
	if (swr_init(swrCtx) < 0)return false;
	as->channel_num= av_get_channel_layout_nb_channels(as->channelLayout);
	return setSDLAudioDevice(as);
}
FFmpegMediaPlayer::FFmpegMediaPlayer(std::wstring filePath, FFmpegDecodeSetting dSet) :FFmpegMediaPlayer() {
	if (!isSettingValid(dSet))return;
	media = FFmpegMedia(filePath);
	decodeSetting = dSet;
	_videoPlayable = initVideo(media, &decodeSetting.video);
	_audioPlayable = initAudio(media, &decodeSetting.audio);
	
	if (!isAudioPlayable() && !isVideoPlayable())return;
	FFmpegDecodeQueue::queueThread->addWork(&__queue);
	if (isVideoPlayable())
		FFmpegMediaPlayer::videoOutputThread->addWork(&__outputVideoBuffer);

}
void FFmpegMediaPlayer::setVideoBufferDest(void* dest) {
	outBuffer = (uint8_t*)dest;
}


int FFmpegMediaPlayer::decodeAudioFrame(uint8_t* buffer, int buffer_size) {
	int audDestBufSize = 0, ret = 0;
	while (1) {
		while (audPktSize > 0) {
			int len1 = avcodec_decode_audio4(media.audCodecCtx, audFrame, &got_sound, &aPkt);
			if (len1 < 0) {
				audPktSize = 0;
				break;
			}
			audPktData += len1;
			audPktSize -= len1;
			audDestBufSize = 0;
			if (got_sound) {
				audDestBufSize = resampleAudio(audFrame,&audDestData);

				/* seg fault point*/
				if(audDestBufSize>0) memcpy(buffer, audDestData[0], audDestBufSize);
				av_freep(&audDestData[0]);
				av_freep(&audDestData);
			}
			if (audDestBufSize <= 0) {
				continue;
			}

			return audDestBufSize;
		}
		if (aPkt.data)av_free_packet(&aPkt);

		if (!soundQueue.pop(aPkt))return -1;
		audPktData = aPkt.data;
		audPktSize = aPkt.size;

	}
}
void FFmpegMediaPlayer::SDLCallback(void* userdata, uint8_t *stream, int len) {
	int len1, audio_size;
	while (len > 0) {
		if (audBufOffset >= audBufSize) {
			audio_size = decodeAudioFrame(audBuf, FFMP_AUDIO_BUFFER_SIZE);
			if (audio_size < 0) {
				audBufSize = 1024;
				memset(audBuf, 0, audBufSize);
			}
			else {
				audBufSize = audio_size;
			}
			audBufOffset = 0;
		}
		len1 = audBufSize - audBufOffset;
		if (len1 > len)len1 = len;
		memcpy(stream, (uint8_t*)audBuf + audBufOffset, len1);
		len -= len1;
		stream += len1;
		audBufOffset += len1;
	}
}
void FFmpegMediaPlayer::__SDLCallbackWrapper(void* userdata, uint8_t *stream, int len) {
	((FFmpegMediaPlayer* const)userdata)->SDLCallback(userdata, stream, len);
}
int FFmpegMediaPlayer::resampleAudio(AVFrame* decodedFrame,uint8_t ***dest_data) {
	int ret = 0;
	audDestMaxSamples = audDestSamples = av_rescale_rnd(
		decodedFrame->nb_samples,
		decodeSetting.audio.sample_rate,
		media.audCodecCtx->sample_rate,
		AV_ROUND_UP);

	ret = av_samples_alloc_array_and_samples(
		dest_data,
		&audDestLSize,
		decodeSetting.audio.channel_num,
		audDestSamples,
		decodeSetting.audio.sample_format,
		0);

	if (ret < 0)return -1;
	audDestSamples = av_rescale_rnd(
		swr_get_delay(swrCtx, media.audCodecCtx->sample_rate) + decodedFrame->nb_samples,
		decodeSetting.audio.sample_rate,
		media.audCodecCtx->sample_rate,
		AV_ROUND_UP);

	if (audDestSamples > audDestMaxSamples) {
		av_freep((&(*dest_data)[0]));
		av_freep(dest_data);
		ret = av_samples_alloc(
			*dest_data,
			&audDestLSize,
			decodeSetting.audio.channel_num,
			audDestSamples,
			decodeSetting.audio.sample_format,
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
		decodeSetting.audio.channel_num,
		ret,
		decodeSetting.audio.sample_format,
		1);
}

bool FFmpegMediaPlayer::setSDLAudioDevice(FFmpegAudioDecodeSetting *as) {
	sdlSpec.freq = as->sample_rate;
	switch (as->sample_format) {
	case AV_SAMPLE_FMT_NONE:
		sdlSpec.format = AUDIO_S16SYS; //most ordinary one?
		break;
	case AV_SAMPLE_FMT_U8:
		sdlSpec.format = AUDIO_U8;
		break;
	case AV_SAMPLE_FMT_S16:
		sdlSpec.format = AUDIO_S16SYS;
		break;
	case AV_SAMPLE_FMT_S32:
		sdlSpec.format = AUDIO_S32SYS;
		break;
	case AV_SAMPLE_FMT_FLT:
		sdlSpec.format = AUDIO_F32SYS;
		break;
	default:
		/* SDL cannot deal the formats that not listed above.*/
		return false;
		break;
	}
	sdlSpec.channels = as->channel_num;
	sdlSpec.silence = 0;
	sdlSpec.samples = SDL_AUDIO_BUFFER_SIZE;
	sdlSpec.userdata = this;
	sdlSpec.callback = __SDLCallbackWrapper;

	if (SDL_OpenAudio(&sdlSpec, NULL)<0) { //force
		return false;
	}
	return true;
}
bool FFmpegMediaPlayer::isAudioPlayable() const {
	return _audioPlayable;
}
bool FFmpegMediaPlayer::isVideoPlayable() const {
	return _videoPlayable;
}

bool FFmpegMediaPlayer::isSettingValid(FFmpegDecodeSetting s) {
	return s.video.height > 0 && s.video.width > 0
		&& s.audio.sample_rate > 0;
}