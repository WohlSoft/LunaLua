#ifndef PerformanceTimer_hhhhhh
#define PerformanceTimer_hhhhhh

#include <Windows.h>

struct PerformanceTimer {
	PerformanceTimer();

	void StartSMBX();	// Get start time (now)
	void StopSMBX();	// Stop timer and accumulate into current time
	void ResetSMBX();
	LONGLONG GetSMBXPerformance();

	void StartLuna();	// Get start time (now)
	void StopLuna();	// Stop timer and accumulate into current time
	void ResetLuna();
	LONGLONG GetLunaPerformance();

	LARGE_INTEGER	SMBXStartTime;
	LARGE_INTEGER	SMBXEndTime;
	LARGE_INTEGER	SMBXCurTime;
	LARGE_INTEGER	SMBXDisplayTime;
	LARGE_INTEGER	LunaStartTime;
	LARGE_INTEGER	LunaEndTime;
	LARGE_INTEGER	LunaCurTime;
	LARGE_INTEGER	LunaDisplayTime;
};

#endif
