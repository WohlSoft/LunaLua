#ifndef A_FFMPEGTHHDR
#define A_FFMPEGTHHDR

#include "FFmpeg.h"
#include <functional>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

struct FFmpegThreadFuncController {
	bool quit;
	bool pause;
	void* userdata;
};
/*
struct FFmpegThreadFunc {
	std::function<void(FFmpegThreadFuncController*)> func;
	FFmpegThreadFuncController ctrl;
};
*/

class FFmpegThreadFunc {
public:
	std::function<void(FFmpegThreadFuncController*)> func;
	FFmpegThreadFuncController ctrl;
	FFmpegThreadFunc();
	FFmpegThreadFunc(std::function<void(FFmpegThreadFuncController*)>& func);
};

class FFmpegThread {
public:
	std::thread* th;
	std::atomic<bool> workHasStarted;
	void createThread();
	void check_exit();
	void worker();
	void addWork(FFmpegThreadFunc* work);
	bool delWork(FFmpegThreadFunc* work);
	std::vector<FFmpegThreadFunc*> workList;
	void kill();
	std::atomic<bool> pendingKill;
	std::mutex mtx1;
	FFmpegThread();
};

#endif