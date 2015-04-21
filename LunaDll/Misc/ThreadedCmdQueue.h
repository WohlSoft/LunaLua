#ifndef ThreadedCmdQueue__hhhh
#define ThreadedCmdQueue__hhhh

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class ThreadedCmdQueue {
private:
    std::queue<T> mQueue;
    std::mutex mMutex;
    std::condition_variable mCond;
public:
    inline void push(const T& mCmd) {
        std::unique_lock<std::mutex> lck(mMutex);

        mQueue.push(mCmd);
        mCond.notify_all();
    }

    inline T pop() {
        T cmd;
        std::unique_lock<std::mutex> lck(mMutex);

        while (mQueue.empty()) mCond.wait(lck);
        cmd = mQueue.front();
        mQueue.pop();
        mCond.notify_all();
        return cmd;
    }

    inline void waitTillEmpty() {
        std::unique_lock<std::mutex> lck(mMutex);

        while (!mQueue.empty()) mCond.wait(lck);
    }
};

#endif