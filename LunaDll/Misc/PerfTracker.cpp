#include <Windows.h>
#include <string>
#include <sstream>
#include "PerfTracker.h"
#include "../Rendering/Rendering.h"
#include "../Rendering/RenderOps/RenderStringOp.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"

// Global instance
PerfTracker g_PerfTracker;

const char* const PerfTracker::PerfTypeNames[PERF_MAX] = {
    "other",
    "draw",
    "lua"
};

PerfTracker::PerfTracker() :
    m_Enabled(false),
    m_FrameStarted(false),
    m_SnapshotValid(false),
    m_LastTime(0.0),
    m_CurrentType(PERF_UNKNOWN),
    m_PerfTimes(), m_PerfTimesSnapshot(),
    m_FreqFactor(0.0)
{
    LARGE_INTEGER freqStruct;
    QueryPerformanceFrequency(&freqStruct);
    m_FreqFactor = 1000.0 / freqStruct.QuadPart;
}

PerfTracker::~PerfTracker() {
}

double PerfTracker::getTime() const {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    return currentTime.QuadPart * m_FreqFactor;
}

void PerfTracker::renderStats() const {
    for (int i = 0; i < PerfTracker::PERF_MAX; i++) {
        std::ostringstream s;
        s << PerfTypeNames[i] << ": ";
        s << m_PerfTimesSnapshot[i];
        RenderStringOp* printTextOp = new RenderStringOp(Str2WStr(s.str()), 3, 10, 100.0f+(float)i*15.0f);
        gLunaRender.AddOp(printTextOp);
    }
}