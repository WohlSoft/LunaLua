#include "BMPBox.h"
#include <math.h>
#include "../Globals.h"
#include "RenderUtils.h"
#include "GL/GLEngine.h"
#include "../Misc/FreeImageUtils/FreeImageHelper.h"
#include <Windows.h>


void DebugMsgBox(LPCSTR pszFormat, ...)
{
	va_list	argp;
	char pszBuf[256];
	va_start(argp, pszFormat);
	vsprintf(pszBuf, pszFormat, argp);
	va_end(argp);
	MessageBoxA(NULL, pszBuf, "debug info", MB_OK);
}

std::unordered_map<std::wstring, std::shared_ptr<Mix_Chunk>> BMPBox::audList;

// CTOR
BMPBox::BMPBox() {
    Init();
}


BMPBox::~BMPBox() {
	// If the GL Engine is running, deallocate the associated texture and erase the BMPBox->Texture mapping.
	if (g_GLEngine.IsEnabled())
	{
		g_GLEngine.ClearLunaTexture(*this);
	}

	if (m_hbmp != NULL) {
		DeleteObject(m_hbmp); //vBuffer will also be freed
		m_hbmp = NULL;
	}
	if (m_hdc != NULL) {
		DeleteDC(m_hdc);
		m_hdc = NULL;
	}
	if (swsCont != NULL) {
		sws_freeContext(swsCont);
		swsCont = NULL;
	}

	if (vCdcCont != NULL) {
		avcodec_close(vCdcCont);
		vCdcCont = NULL;
	}
	if (aCdcCont != NULL) {
		avcodec_close(aCdcCont);
		aCdcCont = NULL;
	}
	if (vFmtCont != NULL) {
		avformat_close_input(&vFmtCont);
	}
	if (srcFrame != NULL) {
		av_free(srcFrame);
	}
	if (destFrame != NULL) {
		av_free(destFrame);
	}
	if (vBuffer != NULL) {
		av_free(vBuffer);
	}
	/*
	if (audioDataPtr != NULL) {
	//shared_ptr will automatically be destroyed
	}
	*/
	av_free_packet(&vPkt);
}

// CTOR - Load from a file path
BMPBox::BMPBox(std::wstring filename, HDC screen_dc) {
	Init();

	if (filename.length() < 1)
		return;

	AVCodec *codec = NULL;

	int numBytes = 0;
	auto it = audList.find(filename);
	if (avformat_open_input(&vFmtCont, WStr2Str(filename).c_str(), NULL, NULL) != 0)goto imgload;

	if (avformat_find_stream_info(vFmtCont, NULL) < 0) goto imgload;

	vStrIdx = -1; aStrIdx = -1;
	for (unsigned int i = 0; i < vFmtCont->nb_streams; i++) {
		switch (vFmtCont->streams[i]->codec->codec_type) {
		case AVMEDIA_TYPE_VIDEO:
			vStrIdx = i;
			break;
		case AVMEDIA_TYPE_AUDIO:
			aStrIdx = i;
			break;
		default:
			break;
		}
	}

	if (it != audList.end() && it->second && !(it->second._Expired())) {
		audioDataPtr = it->second;
		audioLoadByCache = true;
	}
	else if (aStrIdx >= 0) {
		aCdcCont = vFmtCont->streams[aStrIdx]->codec;
		AVCodec *acodec = NULL;
		if ((acodec = avcodec_find_decoder(aCdcCont->codec_id)) == NULL) goto vid;
		if (avcodec_open2(aCdcCont, acodec, NULL) < 0)goto vid;
		audList.erase(filename);
		audList[filename] = decodeAudioOnce();
		av_seek_frame(vFmtCont, vStrIdx, 0, AVSEEK_FLAG_BACKWARD);
	}
vid:
	if (vStrIdx >= 0) {
		/*
		If we cache vCdcCont data we can save about 2MB of memory use
		*/

		vCdcCont = vFmtCont->streams[vStrIdx]->codec;
		if ((codec = avcodec_find_decoder(vCdcCont->codec_id)) == NULL) goto imgload;
		if (avcodec_open2(vCdcCont, codec, NULL) < 0)goto imgload;
		if (!avFrameAlloc())goto imgload;
		rFPS = vFmtCont->streams[vStrIdx]->avg_frame_rate;
		FPS = av_q2d(rFPS);
		m_H = vCdcCont->height;
		m_W = vCdcCont->width;
		avcodec_align_dimensions2(vCdcCont, &m_W, &m_H, aL);
		swsCont = sws_getContext(vCdcCont->width, vCdcCont->height, vCdcCont->pix_fmt, m_W, m_H, pixFmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);
		numBytes = avpicture_get_size(pixFmt, m_W, m_H);
		vBuffer = (uint8_t*)av_malloc(numBytes*sizeof(uint8_t));
		avpicture_fill((AVPicture*)destFrame, vBuffer, pixFmt, m_W, m_H);
		m_hbmp = FreeImageHelper::CreateEmptyBitmap(m_W, m_H, 32, &bmpPtr);
		m_hdc = CreateCompatibleDC(screen_dc);
		m_Filename = filename;
		SelectObject(m_hdc, m_hbmp);
		updateVideoFrame(true);
		//updateVideoFrame();
	}
	else {
		rFPS = { 65,1 };
		FPS = 65;
	}


	return;

imgload:
	// Load any image, converted to pre-multiplied BGRA
	m_hbmp = LoadGfxAsBitmap(filename);

	//gLogger.Log(L"Requested handle for: " + filename, LOG_STD);
	//int lasterr = GetLastError();
	//gLogger.Log(L"Last error: " + to_wstring((long long)lasterr), LOG_STD);

	m_hdc = CreateCompatibleDC(screen_dc);
	SelectObject(m_hdc, m_hbmp);

	// Get dimensions
	BITMAP bm;
	memset(&bm, 0, sizeof(BITMAP));
	GetObject(m_hbmp, sizeof(BITMAP), &bm);
	m_H = bm.bmHeight;
	m_W = bm.bmWidth;
	m_Filename = filename;
}

void BMPBox::initMovieCache() {
	audList.clear();
}
void BMPBox::chunkDeleter(Mix_Chunk* m) {
	free(m->abuf);
	m->abuf = NULL;
	Mix_FreeChunk(m);
	m = NULL;
}

void BMPBox::playAudio() {
	if (!AudioLoaded() || musicPlayed) return;
	audioSDLChannel = Mix_PlayChannel(-1, audioDataPtr.get(), 0);
	musicPlayed = true;
	lastAudioPlayedTick = SDL_GetTicks();
}

void BMPBox::stopPlay(bool noFlush) {
	if (!AudioLoaded() || !musicPlayed || audioSDLChannel == -1) return;
	Mix_HaltChannel(audioSDLChannel);
	musicPlayed = false;
	av_seek_frame(vFmtCont, vStrIdx, 0, AVSEEK_FLAG_BACKWARD);
	if (!noFlush) {
		avcodec_flush_buffers(vCdcCont);
		decodedFrameCount = 0;
	}
	else {
		avcodec_flush_buffers(vCdcCont);
		decodedFrameCount = -1;
	}

	lfUpd = 0;
	fUpd = 0;
	fCount = 0;
	av_free_packet(&vPkt);
	updateVideoFrame(true);
	m_modified.store(true, std::memory_order_relaxed);
}

std::shared_ptr<Mix_Chunk> BMPBox::decodeAudioOnce() {
	AVPacket _aPkt;
	av_init_packet(&_aPkt);
	AVFrame* audF = NULL;

	int64_t destLayout = AV_CH_LAYOUT_STEREO;
	int destSRate = 44100;
	uint8_t **destData = NULL;
	int destChannels = 0;
	int destLSize;
	int destSamples, destMaxSamples;
	AVSampleFormat destSampleFmt = AV_SAMPLE_FMT_S16;
	int destBufSize;
	SwrContext *swrCont;

	int offset = 0;

	swrCont = swr_alloc();
	if (swrCont == NULL)return nullptr;
	av_opt_set_int(swrCont, "in_channel_layout", aCdcCont->channel_layout, 0);
	av_opt_set_int(swrCont, "in_sample_rate", aCdcCont->sample_rate, 0);
	av_opt_set_sample_fmt(swrCont, "in_sample_fmt", aCdcCont->sample_fmt, 0);

	av_opt_set_int(swrCont, "out_channel_layout", destLayout, 0);
	av_opt_set_int(swrCont, "out_sample_rate", destSRate, 0);
	av_opt_set_sample_fmt(swrCont, "out_sample_fmt", destSampleFmt, 0);

	if (swr_init(swrCont) < 0)return nullptr;
	destChannels = av_get_channel_layout_nb_channels(destLayout);

	uint32_t mSize = (uint32_t)ceil(vFmtCont->streams[aStrIdx]->duration*av_q2d(vFmtCont->streams[aStrIdx]->time_base)*destSRate * 2 * destChannels);
	uint8_t* audAll = (uint8_t*)malloc(mSize); //Mix_Chunk will contains this ptr
	int finished = 0; int result = 0; int ret = 0;
	while (av_read_frame(vFmtCont, &_aPkt) == 0) {
		if (_aPkt.stream_index != aStrIdx) {
			/*
			Do not forget to free the packet from a different stream to avoid memory leaking.
			*/
			av_free_packet(&_aPkt);
			continue;
		}
		while (_aPkt.size > 0) {
			audF = av_frame_alloc();
			result = avcodec_decode_audio4(aCdcCont, audF, &finished, &_aPkt);
			if (result >= 0 && finished) {

				_aPkt.size -= result;
				_aPkt.data += result;
				destMaxSamples = destSamples = av_rescale_rnd(audF->nb_samples, destSRate, aCdcCont->sample_rate, AV_ROUND_UP);
				ret = av_samples_alloc_array_and_samples(&destData, &destLSize, destChannels, destSamples, destSampleFmt, 0);
				if (ret < 0)break;
				destSamples = av_rescale_rnd(
					swr_get_delay(swrCont, aCdcCont->sample_rate) + audF->nb_samples,
					destSRate,
					aCdcCont->sample_rate,
					AV_ROUND_UP);
				if (destSamples > destMaxSamples) {
					av_freep(&destData[0]);
					av_freep(&destData);
					ret = av_samples_alloc(destData, &destLSize, destChannels, destSamples, destSampleFmt, 1);
					if (ret < 0)break;
					destMaxSamples = destSamples;
				}
				ret = swr_convert(swrCont, destData, destSamples, (const uint8_t**)audF->data, audF->nb_samples);
				if (ret < 0)break;

				destBufSize = av_samples_get_buffer_size(&destLSize, destChannels, ret, destSampleFmt, 1);
				if (destBufSize < 0)break;
				if (offset + destBufSize < mSize) {
					memcpy(&audAll[offset], destData[0], destBufSize);
					offset += destBufSize;

				}
				av_freep(&destData[0]);
				av_freep(&destData);
			}
			else {
				_aPkt.size = 0;
				_aPkt.data = NULL;
			}
			av_frame_unref(audF);
		}
		av_free_packet(&_aPkt);
	}
	if (destData) av_freep(&destData[0]);
	av_freep(&destData);
	swr_free(&swrCont);
	audioDataPtr.reset(Mix_QuickLoad_RAW(audAll, mSize), chunkDeleter);
	return audioDataPtr;
}



void BMPBox::updateVideoFrame(bool forceReset) {
	lastRenderRequestedGFrame = gFrames;
	fCount++;
	lfUpd = fUpd;
	fUpd = (int)floor(fCount*(65 - FPS) / 65);
	if (!VideoLoaded()) return;
	if (!musicPlayed && AudioLoaded() && !forceReset) {
		playAudio();
	}

	int cVidTime = (int)round((decodedFrameCount + vCdcCont->delay - 2) * 1000 / FPS);
	int cAudTime = AudioLoaded() && musicPlayed ? SDL_GetTicks() - lastAudioPlayedTick : cVidTime;

	int deleg = fUpd - lfUpd + round(FPS*(cVidTime - cAudTime) / 1000);
	if (forceReset) {
		deleg = 0;
	}
	while (deleg <= 0) {

		while ((vEndFlag = av_read_frame(vFmtCont, &vPkt)) >= 0) {
			if (vPkt.stream_index == vStrIdx) {
				break;
			}
			else {
				av_free_packet(&vPkt);
			}
		}
		if (vEndFlag < 0) {
			if (vEndFlag == AVERROR_EOF) {
				stopPlay(true);
			}
			return;
		}
		//buffer some frames,so memory usage tends to be large
		avcodec_decode_video2(vCdcCont, srcFrame, &frameFinished, &vPkt);
		if (frameFinished) {
			if (deleg >= 0) {
				sws_scale(swsCont, srcFrame->data, srcFrame->linesize, 0, srcFrame->height, destFrame->data, destFrame->linesize);
				for (int h = 0; h < m_H; h++) {
					memcpy((uint32_t*)bmpPtr + h*m_W, ((uint8_t*)destFrame->data[0]) + h*destFrame->linesize[0], destFrame->linesize[0]);
				}

				m_modified.store(true, std::memory_order_relaxed);
			}

			lastDecodedGFrame = gFrames;
			deleg++;
			decodedFrameCount++;
		}
		av_free_packet(&vPkt);
	}


}

BMPBox::BMPBox(HBITMAP bitmapData, HDC screen_dc)
{
    Init();

    // Load any image, converted to pre-multiplied BGRA
    m_hbmp = bitmapData;

    //gLogger.Log(L"Requested handle for: " + filename, LOG_STD);
    //int lasterr = GetLastError();
    //gLogger.Log(L"Last error: " + to_wstring((long long)lasterr), LOG_STD);

    m_hdc = CreateCompatibleDC(screen_dc);
    SelectObject(m_hdc, m_hbmp);

    // Get dimensions
    BITMAP bm;
    memset(&bm, 0, sizeof(BITMAP));
    GetObject(m_hbmp, sizeof(BITMAP), &bm);
    m_H = bm.bmHeight;
    m_W = bm.bmWidth;
}

// INIT
void BMPBox::Init() {
	audioLoadByCache = false;
	audioSDLChannel = -1;
	lastDecodedGFrame = -1;
	lastRenderRequestedGFrame = -1;
	decodedFrameCount = 0;
	SDL_zero(specD);
	SDL_zero(specH);
	musicPlayed = false;
	m_modified.store(false, std::memory_order_relaxed);
	audioDataPtr = NULL;
	vStrIdx = -1;
	aStrIdx = -1;
	aCdcCont = NULL;
	fCount = 0;
	rFPS = { 0,0 };
	fUpd = 0;
	lfUpd = 0;
	FPS = 0;
	m_H = 0;
	m_W = 0;
	m_hbmp = NULL;
	m_hdc = NULL;
	vFmtCont = NULL;
	vCdcCont = NULL;
	srcFrame = NULL;
	destFrame = NULL;
	vBuffer = NULL;
	bmpPtr = NULL;
	swsCont = NULL;
	av_init_packet(&vPkt);
}

bool BMPBox::avFrameAlloc() {

	srcFrame = avcodec_alloc_frame();
	destFrame = avcodec_alloc_frame();
	return !(srcFrame == NULL || destFrame == NULL);
}

// Makes a specified color transparent
void BMPBox::MakeColorTransparent(int rgb_color) {
    union {
        struct {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t a;
        } asStruct;
        uint32_t asInt;
    } bgra_color;

    // Convert RGB to BGRA
    bgra_color.asStruct.b = (rgb_color & 0x0000FF);
    bgra_color.asStruct.g = (rgb_color & 0x00FF00) >> 8;
    bgra_color.asStruct.r = (rgb_color & 0xFF0000) >> 16;
    bgra_color.asStruct.a = 255;

    if (m_hbmp) {
        BITMAP bm;
        memset(&bm, 0, sizeof(BITMAP));
        GetObject(m_hbmp, sizeof(BITMAP), &bm);
        uint32_t *pData = (uint32_t *)bm.bmBits;
        uint32_t dataLen = bm.bmHeight * bm.bmWidth;

        // Presuming we can get the bitmap data, replace all of the color with
        // transparency.
        if (pData) {
            for (uint32_t idx = 0; idx < dataLen; idx++) {
                if (pData[idx] == bgra_color.asInt) {
                    pData[idx] = 0;
                }
            }
        }
    }
}

// IMAGE LOADED - Returns true if this object loaded correctly / the bitmap handle isn't null
bool BMPBox::ImageLoaded() {
    if (m_hbmp == NULL)
        return false;
    return true;
}
bool BMPBox::VideoLoaded() {
	return vFmtCont != NULL && vCdcCont != NULL && srcFrame != NULL && destFrame != NULL && vBuffer != NULL;
}
bool BMPBox::AudioLoaded() {
	return audioLoadByCache || (aCdcCont != NULL && audioDataPtr != NULL);
}

void BMPBox::forEachPixelValue(std::function<void(BYTE)> forEachFunc)
{
    if (!forEachFunc)
        return;

    if (!m_hbmp)
        return;

    BITMAP bm;
    memset(&bm, 0, sizeof(BITMAP));
    GetObject(m_hbmp, sizeof(BITMAP), &bm);
    BYTE *pData = (BYTE *)bm.bmBits;
    uint32_t dataLen = bm.bmHeight * bm.bmWidth * 4;

    if (pData) {
        for (uint32_t i = 0; i < dataLen; i++) {
            forEachFunc(pData[i]);
        }
    }
    
}

void* BMPBox::getBits()
{
    BITMAP bm = { 0 };
    GetObject(m_hbmp, sizeof(bm), &bm);
    return bm.bmBits;
}

BMPBox* BMPBox::loadIfExist(const std::wstring& filename, HDC screen_dc)
{
    DWORD fAttrib = GetFileAttributesW(filename.c_str());
    if (fAttrib == INVALID_FILE_ATTRIBUTES || fAttrib & FILE_ATTRIBUTE_DIRECTORY)
        return nullptr;
    return new BMPBox(filename, screen_dc);
}
