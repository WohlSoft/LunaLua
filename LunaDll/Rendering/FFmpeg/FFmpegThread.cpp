#include "FFmpegThread.h"
#include <chrono>
#include "../../Defines.h"
#include <algorithm>

FFmpegThreadFunc::FFmpegThreadFunc() {
	memset(&ctrl, 0, sizeof(FFmpegThreadFuncController));

}

FFmpegThreadFunc::FFmpegThreadFunc(std::function<void(FFmpegThreadFuncController*)>& _func):FFmpegThreadFunc() {
	func = _func;
}

void FFmpegThread::worker() {
	while (true) {
		//do not use for(auto& x:workList) 
		//GM_STAR_COUNT = workList.size();
		mtx1.lock();
		for (int i = 0; i < workList.size(); ++i) {
			workList[i]->func(&workList[i]->ctrl);
		}
		mtx1.unlock();
		check_exit();

		
		//NOTE:Sleep(1) limits the interval of this loop to 15ms at least. 
		//(due to windows' timeslice)
		if(!boost)Sleep(1);
	}
}
bool FFmpegThread::delWork(FFmpegThreadFunc* work) {
	int i = 0; bool found = false;
	if (work == NULL)return false;
	std::lock_guard<std::mutex>lock(mtx1);
	if (workList.empty())return false;
		
	for (; i < workList.size(); ++i) {
		if (workList[i] == work) {
			found = true;
			break;
		}
	}
	if(found)workList.erase(workList.begin() + i);
	return found;
}
FFmpegThread::FFmpegThread():pendingKill(false),workHasStarted(false),boost(false),th(NULL) {
}

void FFmpegThread::addWork(FFmpegThreadFunc* work) {
	std::lock_guard<std::mutex>lock(mtx1);
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
	th = new std::thread([this] {this->worker(); });
	th->detach();

}

void FFmpegThread::check_exit() {
	if (pendingKill) {
		workHasStarted = false;
		pendingKill = false;
		std::lock_guard<std::mutex>lock(mtx1);
		workList.clear(); //danger?
		
		throw "eusyo";
	}
}