#include "FFmpegDecodeQueue.h"
FFmpegThread* FFmpegDecodeQueue::queueThread = new FFmpegThread();

FFmpegDecodeQueue::FFmpegDecodeQueue() : size_(0) {}

FFmpegDecodeQueue::FFmpegDecodeQueue(int msize) :FFmpegDecodeQueue(){
	MAX_SIZE = msize > 0 ? msize : 0;
}

void FFmpegDecodeQueue::push(AVPacket& packet) {
	av_dup_packet(&packet);
	packets_.push_back(packet);
	size_ += packet.size;
}

bool FFmpegDecodeQueue::pop(AVPacket& packet) {
	if (packets_.empty()) return false;
	packet = packets_.front();
	packets_.pop_front();
	size_ -= packet.size;
	return true;
}