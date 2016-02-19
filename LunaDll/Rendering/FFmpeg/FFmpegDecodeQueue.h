#ifndef A_FFMPEGDECQHDR
#define A_FFMPEGDECQHDR

#include "FFmpeg.h"
#include "FFmpegThread.h"
#include <deque>
#include <functional>
#include <Windows.h>
class FFmpegDecodeQueue {
public:
	FFmpegDecodeQueue();
	FFmpegDecodeQueue(int max_s);
	~FFmpegDecodeQueue();

	void push(AVPacket& packet);
	void rawPush(AVPacket& packet);

	bool pop(AVPacket& packet);
	bool rawPop(AVPacket& packet);

	void rawClear();
	void clear();

	int dataSize() const { return size_; }
	
	int MAX_SIZE;

	static FFmpegThread* queueThread;
	CRITICAL_SECTION crSect;
	//std::mutex mtx1, mtx2;
private:
	
	std::deque<AVPacket> packets_;
	int size_;
};
#endif