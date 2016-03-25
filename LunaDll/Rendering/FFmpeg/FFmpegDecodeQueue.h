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
	FFmpegDecodeQueue(int max_s,int max_c);
	~FFmpegDecodeQueue();

	void push(CustomAVPacket& packet);
	void rawPush(CustomAVPacket& packet);

	bool pop(CustomAVPacket& packet);
	bool rawPop(CustomAVPacket& packet);
	CustomAVPacket& lastPacket();

	void rawClear();
	void clear();

	int dataSize() const { return size_; }
	int dataCount() const { return packets_.size(); }
	bool queueable();
	
	int MAX_SIZE;
	int MAX_COUNT;

	static FFmpegThread* queueThread;
	CRITICAL_SECTION crSect;
	//std::mutex mtx1, mtx2;
private:
	
	std::deque<CustomAVPacket> packets_;
	int size_;
};
#endif