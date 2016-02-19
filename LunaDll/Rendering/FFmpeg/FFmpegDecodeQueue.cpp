#include "FFmpegDecodeQueue.h"
#include "../../Defines.h"
FFmpegThread* FFmpegDecodeQueue::queueThread = new FFmpegThread();

FFmpegDecodeQueue::FFmpegDecodeQueue() : size_(0) {
	InitializeCriticalSection(&crSect);
}

FFmpegDecodeQueue::FFmpegDecodeQueue(int msize) :FFmpegDecodeQueue(){
	MAX_SIZE = msize > 0 ? msize : 0;
}

void FFmpegDecodeQueue::push(AVPacket& packet) {
	EnterCriticalSection(&crSect);
	rawPush(packet);
	LeaveCriticalSection(&crSect);
}

void FFmpegDecodeQueue::rawPush(AVPacket& packet) {
	//AVPacket p;
	av_dup_packet(&packet);
	//av_copy_packet(&p, &packet);
	packets_.push_back(packet);
	size_ += packet.size;
}

bool FFmpegDecodeQueue::pop(AVPacket& packet) {
	EnterCriticalSection(&crSect);
	bool __tmp = rawPop(packet);
	LeaveCriticalSection(&crSect);
	return __tmp;
}

bool FFmpegDecodeQueue::rawPop(AVPacket& packet) {
	if (packets_.empty()) return false;
	packet = packets_.front();
	packets_.pop_front();
	size_ -= packet.size;
	return true;
}

void FFmpegDecodeQueue::rawClear() {
	//be careful of freeing in-use packet
	//std::lock_guard<std::mutex> lock(mtx1);
	
	for (int i = 0; i < packets_.size(); i++) {
		if(packets_[i].data)av_free_packet(&packets_[i]);
	}
	
	packets_.clear();
	size_ = 0;
}

void FFmpegDecodeQueue::clear() {
	EnterCriticalSection(&crSect);
	rawClear();
	LeaveCriticalSection(&crSect);
}

FFmpegDecodeQueue::~FFmpegDecodeQueue() {
	clear();
	DeleteCriticalSection(&crSect);
}