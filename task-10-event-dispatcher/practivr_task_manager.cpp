#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <vector>

struct Task
{
    int mId;
    unsigned long mProcessTimeMs;
    Task(int id, unsigned long processTimeMs) : mId(id), mProcessTimeMs(processTimeMs)
    {
    }
};

class TaskManager
{
private:
    std::queue<Task> mTasks;
    bool mStopping = false;
    int mActiveTaskCount = 0;
    std::mutex mMutex;
    std::condition_variable mCv;
    std::condition_variable mCvWaitTaskComplete;
    std::vector<std::thread> mThreads;

    void Worker()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mMutex);

            mCv.wait(lock, [&]()
                     { return mStopping || !mTasks.empty(); });

            if (mStopping && mTasks.empty())
            {
                return;
            }

            Task task = mTasks.front();
            mTasks.pop();
            mActiveTaskCount++;

            lock.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(task.mProcessTimeMs));

            lock.lock();

            mActiveTaskCount--;

            bool complete = (mActiveTaskCount == 0) && mTasks.empty();

            lock.unlock();

            if (complete)
            {
                mCvWaitTaskComplete.notify_all();
            }
        }
    }

public:
    TaskManager()
    {
        mThreads.emplace_back(&TaskManager::Worker, this);
        mThreads.emplace_back(&TaskManager::Worker, this);
    }

    ~TaskManager()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mStopping = true;
        lock.unlock();
        mCv.notify_all();

        for (auto &t : mThreads)
        {
            if (t.joinable())
            {
                t.join();
            }
        }
    }

    bool AddTask(int id, unsigned long processTimeMs)
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);
            if (mStopping)
            {
                return false;
            }
            mTasks.emplace(id, processTimeMs);
        }
        mCv.notify_one();
        return true;
    }

    void WaitTaskComplete()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        mCvWaitTaskComplete.wait(lock, [&]()
                                 { return (mActiveTaskCount == 0) && mTasks.empty(); });
    }
};

int main()
{
    TaskManager tm;
    tm.AddTask(1, 100);
    tm.AddTask(2, 100);
    tm.AddTask(3, 100);
    tm.AddTask(4, 100);
    tm.WaitTaskComplete();
}
