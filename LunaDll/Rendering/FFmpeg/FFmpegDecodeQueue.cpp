#include "FFmpegDecodeQueue.h"
#include "../../Defines.h"
FFmpegThread* FFmpegDecodeQueue::queueThread = new FFmpegThread();

FFmpegDecodeQueue::FFmpegDecodeQueue() : size_(0) {
	InitializeCriticalSectionEx(&crSect,4000, CRITICAL_SECTION_NO_DEBUG_INFO);
}

FFmpegDecodeQueue::FFmpegDecodeQueue(int msize) :FFmpegDecodeQueue(){
	MAX_SIZE = msize > 0 ? msize : 0;
}

FFmpegDecodeQueue::FFmpegDecodeQueue(int msize,int mcount) : FFmpegDecodeQueue(msize) {
	MAX_COUNT = mcount > 0 ? mcount : 10;
}

void FFmpegDecodeQueue::push(CustomAVPacket& packet) {
	EnterCriticalSection(&crSect);
	rawPush(packet);
	LeaveCriticalSection(&crSect);
}

void FFmpegDecodeQueue::rawPush(CustomAVPacket& packet) {
	//CustomAVPacket p;
	av_dup_packet(&packet);
	//av_copy_packet(&p, &packet);
	packets_.push_back(packet);
	size_ += packet.size;
}

bool FFmpegDecodeQueue::pop(CustomAVPacket& packet) {
	EnterCriticalSection(&crSect);
	bool __tmp = rawPop(packet);
	LeaveCriticalSection(&crSect);
	return __tmp;
}

bool FFmpegDecodeQueue::rawPop(CustomAVPacket& packet) {
	if (packets_.empty() || !packets_.front().decodeReady) return false;
	packet = packets_.front();
	packets_.pop_front();
	size_ -= packet.size;
	return true;
}

CustomAVPacket& FFmpegDecodeQueue::lastPacket() {
	return packets_.back();
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
bool FFmpegDecodeQueue::queueable() {
	return dataSize() < MAX_SIZE && dataCount() < MAX_COUNT;
}
FFmpegDecodeQueue::~FFmpegDecodeQueue() {
	clear();
	DeleteCriticalSection(&crSect);
}