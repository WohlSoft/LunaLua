#include "WaitForTickEnd.h"

volatile uint64_t WaitForTickEnd::refCount = 0;
volatile bool WaitForTickEnd::inTickEnd = false;
std::mutex WaitForTickEnd::runningMutex;
std::condition_variable WaitForTickEnd::notifyWaiters;
std::condition_variable WaitForTickEnd::notifyWaiterDone;

WaitForTickEnd::WaitForTickEnd() :
    waiterLock(runningMutex)
{
    refCount++;
    while (!inTickEnd) {
        notifyWaiters.wait(waiterLock);
    }
}

WaitForTickEnd::~WaitForTickEnd()
{
    refCount--;
    notifyWaiterDone.notify_all();
}


void WaitForTickEnd::RunQueued()
{
    if (refCount != 0)
    {
        std::unique_lock<std::mutex> lock(runningMutex);
        inTickEnd = true;
        while (refCount > 0)
        {
            notifyWaiters.notify_one();
            notifyWaiterDone.wait(lock);
        }
        inTickEnd = false;
    }
}
