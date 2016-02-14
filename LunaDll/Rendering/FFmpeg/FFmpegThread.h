#ifndef A_FFMPEGTHHDR
#define A_FFMPEGTHHDR

#include "FFmpeg.h"
#include <functional>
#include <thread>
#include <vector>
#include <atomic>

struct FFmpegThreadFuncController {
	bool quit;
	bool pause;
	void* userdata;
};

struct FFmpegThreadFunc {
	std::function<void(FFmpegThreadFuncController*)> func;
	FFmpegThreadFuncController ctrlPtr;
};

class FFmpegThread {
public:
	std::thread* th;
	std::atomic<bool> workHasStarted;
	void createThread();
	void check_exit();
	void worker();
	void addWork(FFmpegThreadFunc* work);
	std::vector<FFmpegThreadFunc*> workList;
	void kill();
	std::atomic<bool> pendingKill;
	FFmpegThread();
};

#endif