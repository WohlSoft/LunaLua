#ifndef PerfTracker_hhhh
#define PerfTracker_hhhh

#include <cstdint>
#include <string>

class PerfTracker {
// Types
public:
    enum PerfType {
        PERF_UNKNOWN = 0,
        PERF_DRAWING,
        PERF_LUA,
        PERF_MAX
    };
    static const char* const PerfTypeNames[PERF_MAX];

// Data members
private:
    bool m_Enabled;
    bool m_FrameStarted;
    bool m_SnapshotValid;
    double m_LastTime;
    PerfType m_CurrentType;
    double m_PerfTimes[PERF_MAX];
    double m_PerfTimesSnapshot[PERF_MAX];
    double m_FreqFactor;

private:
    double getTime() const;

    inline double endCurrentTag() {
        double newTime = getTime();
        m_PerfTimes[m_CurrentType] += newTime - m_LastTime;
        return newTime;
    }

public:
    PerfTracker();
    ~PerfTracker();

    inline void startFrame() {
        if (!m_Enabled) return;
        if (m_FrameStarted) return;
        m_FrameStarted = true;
        for (int i = 0; i < PERF_MAX; i++) {
            m_PerfTimes[i] = 0.0;
        }
        m_LastTime = getTime();
        m_CurrentType = PERF_UNKNOWN;
    }

    inline void endFrame() {
        if (!m_FrameStarted) {
            m_SnapshotValid = false;
            return;
        };
        m_FrameStarted = false;
        endCurrentTag();
        for (int i = 0; i < PERF_MAX; i++) {
            m_PerfTimesSnapshot[i] = m_PerfTimes[i];
        }
        m_SnapshotValid = true;
    }

    inline void tag(PerfType type) {
        if (!m_FrameStarted) return;
        m_LastTime = endCurrentTag();
        m_CurrentType = type;
    }

    inline PerfType getCurrentType() const {
        return m_CurrentType;
    }

    inline bool getPerfSnapshot(double out[PERF_MAX]) const {
        if (!m_SnapshotValid) return false;
        for (int i = 0; i < PERF_MAX; i++) {
            out[i] = m_PerfTimesSnapshot[i];
        }
        return true;
    }

    inline void disable() {
        m_Enabled = false;
        m_FrameStarted = false;
    }

    inline void enable() {
        m_Enabled = true;
    }

    void renderStats() const;
};

extern PerfTracker g_PerfTracker;

// Class with destructor for allocating a temporary perf tracker state
class PerfTrackerState {
private:
    PerfTracker& m_PerfTracker;
    const PerfTracker::PerfType m_LastType;
public:
    inline PerfTrackerState(PerfTracker::PerfType type, PerfTracker& perfTracker = g_PerfTracker) :
        m_PerfTracker(perfTracker),
        m_LastType(perfTracker.getCurrentType())
    {
        m_PerfTracker.tag(type);
    }

    inline ~PerfTrackerState() {
        m_PerfTracker.tag(m_LastType);
    }
};

#endif