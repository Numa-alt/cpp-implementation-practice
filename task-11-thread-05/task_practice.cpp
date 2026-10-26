#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <condition_variable>

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
    std::vector<std::thread> mThreads;

    bool mStopping = false;
    std::condition_variable mCv;
    std::condition_variable mCvWaitTaskComplete;

    std::mutex mMutex;

    int mActiveTaskCount = 0;

    std::queue<Task> mTasks;

    void Worker()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mMutex);

            mCv.wait(lock, [&]()
                     { return mStopping || !mTasks.empty(); });

            if (mStopping && mTasks.empty())
            {
                break;
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
        mThreads.push_back(std::thread(&TaskManager::Worker, this));
        mThreads.push_back(std::thread(&TaskManager::Worker, this));
    }
    ~TaskManager()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        mStopping = true;
        lock.unlock();

        mCv.notify_all();

        for (int i = 0; i < mThreads.size(); i++)
        {
            if (mThreads[i].joinable())
            {
                mThreads[i].join();
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
    tm.AddTask(1, 100);
    tm.AddTask(1, 100);
    tm.AddTask(1, 100);

    tm.WaitTaskComplete();
}