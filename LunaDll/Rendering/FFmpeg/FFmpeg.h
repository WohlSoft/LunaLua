#ifndef A_FFMPEGHDR
#define A_FFMPEGHDR

extern "C" {
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}
struct CustomAVPacket :AVPacket {
	uint64_t userdata;
	bool decodeReady;
	CustomAVPacket() :AVPacket(), userdata(0),decodeReady(false) {}
};
#endif