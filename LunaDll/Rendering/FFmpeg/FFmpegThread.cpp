#include "FFmpegThread.h"
#include <chrono>

void FFmpegThread::worker() {
	while (true) {
		//std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		for (auto& ffth : workList) {
			ffth->func(&ffth->ctrlPtr);
		}
		check_exit();
	}
}

FFmpegThread::FFmpegThread():pendingKill(false),workHasStarted(false) {
}

void FFmpegThread::addWork(FFmpegThreadFunc* work) {
	workList.push_back(work);
	if (!workHasStarted) {
		createThread();
	}
	
}

void FFmpegThread::kill() {
	if (workHasStarted) {
		pendingKill = true;
	}
}

void FFmpegThread::createThread() {
	workHasStarted = true;
	pendingKill = false;
	th = new std::thread(&FFmpegThread::worker,this);
	th->detach();

}

void FFmpegThread::check_exit() {
	if (pendingKill) {
		workHasStarted = false;
		pendingKill = false;
		workList.clear(); //danger?
		throw "eusyo";
	}
}