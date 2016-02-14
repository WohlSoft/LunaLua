#ifndef A_FFMPEGDECQHDR
#define A_FFMPEGDECQHDR

#include "FFmpeg.h"
#include "FFmpegThread.h"
#include <deque>
#include <functional>

class FFmpegDecodeQueue {
public:
	FFmpegDecodeQueue();
	FFmpegDecodeQueue(int max_s);

	void push(AVPacket& packet);

	bool pop(AVPacket& packet);

	int dataSize() const { return size_; }
	
	int MAX_SIZE;

	static FFmpegThread* queueThread;

private:
	std::deque<AVPacket> packets_;
	int size_;
};
#endif