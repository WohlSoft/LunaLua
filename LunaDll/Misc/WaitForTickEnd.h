#pragma once

#include <cstdint>
#include <atomic>
#include <mutex>
#include <condition_variable>

// Allows things running in another thread to wait to synchronously occur
// during the ending of a tick
class WaitForTickEnd
{
private:
    static volatile uint64_t refCount;
    static volatile bool inTickEnd;
    static std::mutex runningMutex;
    static std::condition_variable notifyWaiters;
    static std::condition_variable notifyWaiterDone;

    std::unique_lock<std::mutex> waiterLock;
public:
    WaitForTickEnd();
    ~WaitForTickEnd();

public:
    static void RunQueued();
};
