#include "FFmpegMedia2.h"
#include "../../GlobalFuncs.h"
#include "../../Defines.h"
#include <Windows.h>

void FFmpegMedia2::init() {
	//memset(fmtCtx, 0, sizeof(AVFormatContext*) * 3);
	//memset(streamIdx, 0, sizeof(int*) * 4);
	fmtCtx[AUDIO] = fmtCtx[VIDEO] = fmtCtx[MASK] = NULL;
	streamIdx[VIDEO] = streamIdx[AUDIO] = streamIdx[MASK] = -1;
}

FFmpegMedia2::FFmpegMedia2() {
	init();
}

FFmpegMedia2::~FFmpegMedia2() {

	for (int i = 0; i < 3; i++) {
		if (fmtCtx[i]) {
			if (streamIdx[i] >= 0 && fmtCtx[i]->streams[streamIdx[i]]->codec) {
				avcodec_close(fmtCtx[i]->streams[streamIdx[i]]->codec);
			}
			avformat_close_input(&(fmtCtx[i]));
		}
	}
}

int FFmpegMedia2::seek(MediaType mt, int64_t pos) {
	if (mt == VIDEO || mt == MASK) {
		return av_seek_frame(fmtCtx[mt], -1, pos, AVSEEK_FLAG_BACKWARD);
	}
	else {
		return av_seek_frame(fmtCtx[mt], -1, pos, AVSEEK_FLAG_ANY);
	}
	
}

FFmpegMedia2::FFmpegMedia2(std::wstring mainVid, std::wstring mainAud, std::wstring mask) :FFmpegMedia2() {
	init();
	
	loadVideo(mainVid);
	loadAudio(mainAud);

	loadMask(mask);
}
std::wstring FFmpegMedia2::findMask(std::wstring main) {
		size_t pos = main.find_last_of(L".");
		size_t dirpos = main.find_last_of(L"\\/");
		if (std::wstring::npos == dirpos) return L"";
		std::wstring body = std::wstring::npos == pos?main:main.substr(0, pos);
		std::wstring dir = main.substr(0, dirpos);
		_WIN32_FIND_DATAW fw;
		HANDLE hFind;
		hFind = FindFirstFile((body + L"_m.*").c_str(), &fw);
		if (hFind == INVALID_HANDLE_VALUE) {
			FindClose(hFind);
			return L"";
		}
		FindClose(hFind);
		return dir + L"\\" + fw.cFileName;
}

FFmpegMedia2::FFmpegMedia2(std::wstring main):FFmpegMedia2(main,main,findMask(main)) {
}
/*
inline bool FFmpegMedia2::mediaAvailable(MediaType mt) {
	return streamIdx[mt] >= 0;
}
inline double FFmpegMedia2::getAvgFrameRate(MediaType mt) {
	return av_q2d(fmtCtx[mt]->streams[streamIdx[mt]]->avg_frame_rate);
}


inline double FFmpegMedia2::getTimeBase(MediaType mt) {
	return av_q2d(fmtCtx[mt]->streams[streamIdx[mt]]->time_base);
}

*/
FFmpegVideoDecodeComponent2* FFmpegMedia2::getVideoDecodeComponent() {
	return new FFmpegVideoDecodeComponent2(fmtCtx[VIDEO]->streams[streamIdx[VIDEO]]->codec);
}

FFmpegVideoDecodeComponent2* FFmpegMedia2::getMaskDecodeComponent() {
	return new FFmpegVideoDecodeComponent2(fmtCtx[MASK]->streams[streamIdx[MASK]]->codec);
}

FFmpegAudioDecodeComponent2* FFmpegMedia2::getAudioDecodeComponent() {
	return new FFmpegAudioDecodeComponent2(fmtCtx[AUDIO]->streams[streamIdx[AUDIO]]->codec);
}

int FFmpegMedia2::readMediaFrame(MediaType mt, AVPacket* pkt) {
	return mediaAvailable(mt)? readFrame(fmtCtx[mt], streamIdx[mt], pkt) : AVERROR_INVALIDDATA;
}


inline void FFmpegMedia2::loadVideo(std::wstring filepath) {
	streamIdx[VIDEO] = loadMedia(filepath, fmtCtx[VIDEO], AVMEDIA_TYPE_VIDEO);
}
inline void FFmpegMedia2::loadAudio(std::wstring filepath) {
	streamIdx[AUDIO] = loadMedia(filepath, fmtCtx[AUDIO], AVMEDIA_TYPE_AUDIO);
}
inline void FFmpegMedia2::loadMask(std::wstring filepath) {
	streamIdx[MASK] = loadMedia(filepath,fmtCtx[MASK], AVMEDIA_TYPE_VIDEO);
}



inline int FFmpegMedia2::loadMedia(std::wstring filepath, AVFormatContext*& dest, enum AVMediaType mediaType) {

	if (filepath.length() < 1)return -1;
	int ret = 0;

	/* open file */
	ret = avformat_open_input(
		&dest,					//get format context
        WStr2StrA(filepath).c_str(), //filepath(cast from wstring to char)
		NULL,						//nothing to specify about input format
		NULL);						//no options

	if (ret != 0)return-1;	//return when failed to open

							/* set stream info */
	ret = avformat_find_stream_info(dest, NULL);

	if (ret < 0)return-1;		//return when no stream found

							/* find most appropriate video stream */
	AVCodec *codec = NULL;
	int strIdx = -1;

	strIdx = av_find_best_stream(
		dest,					//file info
		mediaType,		//media type we want
		-1,						//no stream specified
		-1,						//no related stream spiecified
		&codec,				//get codec(decoder) info
		0);						//(no flags available)


	if (strIdx >= 0 && codec != NULL) { // has video stream
												 /* init codec */
		if (avcodec_open2(dest->streams[strIdx]->codec, codec, NULL) >= 0) {
			
			return strIdx;
		}
		else {
			return -1;
		}

	}
	
	return -1;
}

inline bool FFmpegMedia2::openFile(std::wstring filepath, AVFormatContext** dest) {
	if (filepath.length() < 1)return false;
	return avformat_open_input(
		dest,
        WStr2StrA(filepath).c_str(),
		NULL,
		NULL) == 0;
}

inline bool FFmpegMedia2::setStreamInfo(AVFormatContext* fmt) {
	return avformat_find_stream_info(fmt, NULL) >= 0;
}

inline int FFmpegMedia2::getBestStreamIndex(AVFormatContext* fmt, AVCodec** codec, enum AVMediaType mediaType) {
	return av_find_best_stream(fmt, mediaType, -1, -1, codec, 0);
}

inline bool FFmpegMedia2::openCodec(AVFormatContext* fmt, int streamidx,AVCodec* codec) {
	AVCodecContext* ccc = fmt->streams[streamidx]->codec;
	return avcodec_open2(ccc, codec, NULL) >= 0;
}

int FFmpegMedia2 :: readFrame(AVFormatContext* fmt, int streamidx, AVPacket* pkt) {
	int end=0;
	while ((end=av_read_frame(fmt, pkt))>=0) {
		if (end < 0)return end;
		if (pkt->stream_index == streamidx) {
			return end;
		}
		else {
			av_free_packet(pkt);
		}
	}
	return end;
}

void FFmpegMedia2::getAlignedSize(AVCodecContext* videoCodec, int* alW, int* alH) {
	int _w, _h,al[8];
	_w = videoCodec->width;
	_h = videoCodec->height;
	avcodec_align_dimensions2(videoCodec, &_w, &_h, al);
	*alW = _w;
	*alH = _h;
}
