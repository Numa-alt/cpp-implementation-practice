#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <algorithm>
#include <chrono>

#if 0
struct Task
{
    Task(int taskId, unsigned long processTImeMs) : mTaskId(taskId), mProcessTimeMs(processTimeMs)
    {
    }
    int mTaskId;
    unsigned long mProcessTimeMs;
};

class ThreadTaskQueue
{
private:
    bool Warker()
    {
    }

    std::vector<Task> mTasks;
    std::thread mThread;
    std::mutex mMutex;
    std::condition_variable mCv;
    std::condition_variable mCompleteCv;
    bool mStopping = false;
    bool mExecuting = false;

    void WorkLoop()
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

            Task task = mTask.front();
            mTask.pop();

            lock.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(task.mProcessTime));

            loc.lock();

            mExecuting = true;

            bool complete = mTasks.empt();

            lock.unlock();

            if (complete)
            {
                mCpleteCv.nofity_all();
            }
        }
    }

public:
    ThreadTaskQueue()
    {
        mThread = std::thread(&ThreadTaskQueue::WorkerLoop, this);
    }
    ~ThreadTaskQueue()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mStopping = true;

        mCv.notify_all();

        lock.unlock();
        if (mThread.joinable())
        {
            mThread.join();
        }
    }
    bool AddTask(int taskId, unsigned long processTimeMs)
    {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            if (mStopping)
            {
                return false;
            }

            mTasks.emplace(taskId, processTimeMs);
        }
        mCv.notify_one();

        return true;
    }
    void WaitTaskComplete()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        // タスクが終了するまで待つ
        mCompleteCv.wait(lock, [&]()
                         { return !mExecuting && mTasks.empty(); });
    }
};

int main()
{
    ThreadTaskQueue ttq;

    ttq.AddTask(1, 150);
    ttq.AddTask(2, 100);
    ttq.AddTask(3, 200);
    ttq.AddTask(4, 350);

    ttq.WaitTaskComplete();

    return true;
}

struct Task
{
    int mTaskId;
    unsigned int mProcessTimeMs;
    Task(int taskId, unsigned int processTimeMs) : mTaskId(taskId), mProcessTimeMs(processTimeMs)
    {
    }
};

class TaskManager
{
private:
    std::queue<Task> mTasks;
    std::thread mThread;
    std::mutex mMutex;

    std::condition_variable mCv;
    std::condition_variable mCvWaitComplete;

    // 停止
    bool mStopping = false;

    // 実行中
    bool mExecuting = false;

    // ループ
    //   ロック
    //   タスクがキューにはいるか停止フラグが立つまでwait
    void Worker()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mMutex);

            mCv.wait(lock, [&]()
                     { return mStopping || !mTasks.empty(); });

            // 停止命令かつキューが空ならループを抜ける
            if (mStopping && mTasks.empty())
            {
                break;
            }

            // タスクを取り出す
            Task task = mTasks.front();
            mTasks.pop();

            // 実行中フラグを立てる
            mExecuting = true;

            // ロック解除
            lock.unlock();

            // 指定時間待つ
            //  std::chrono::milliseconds(task.mProcessTimeMs);
            std::this_thrad::sleep_for(std::chrono::milliseconds(task.mProcessTimeMs));

            // ロック
            lock.lock();

            // 実行中フラグを解除
            mExecuting = false;

            // タスクリストが空なら終了を通知
            bool complete = mTasks.empty();

            lock.unlock();

            if (complete)
            {
                mCvWaitComplete.notify_all();
            }
        }
    }

public:
    // スレッド開始
    TaskManager()
    {
        mThread = std::thread(&TaskManager::Worker, this);
    }

    // ロックして停止フラグを立て、ロック解除。join可能ならjoinを待つ
    ~TaskManager()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        // 停止フラグ
        mStopping = true;

        // 通知
        mCv.nofity_all();

        lock.unlock();

        if (mThread.joinable())
        {
            mThread.join();
        }
    }

    // ロックし、停止フラグをチェック、タスクを追加。Workerを起こす
    bool AddTask(int taskId, unsigned long processTimeMs)
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);
            // 停止フラグが立てられているなら何もしない
            if (mStopping)
            {
                return false;
            }

            mTask.emplace(taskId, processTimeMs);
        }

        // Workerを起こす
        mCv.notify_one();

        return true;
    }

    void WaitComplete()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        mCvWaitComplete.wait(lock, [&]()
                             { return !mExecuting && mTask.empty(); });
    }
};

int main()
{
    TaskManager tm;

    tm.AddTask(1, 200);
    tm.AddTask(2, 150);
    tm.AddTask(3, 250);

    tm.WaitComplete();
}

struct Task
{
    int mTaskId;
    unsigned int mProcessTimeMs;
    Task(int taskId, unsigned int processTimeMs) : mTaskId(taskId), mProcessTimeMs(processTimeMs)
    {
    }
};

class TaskManager
{
private:
    std::thread mThread;
    bool mStopping = false;
    bool mExecuting = false;
    std::mutex mMutex;
    std::condition_variable mCv;
    std::condition_variable mCompleteCv;
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

            mExecuting = true;

            lock.unlock();

            // task
            std::this_thread::sleep_for(std::chrono::milliseconds(task.mProcessTimeMs));

            lock.lock();
            mExecuting = false;

            bool complete = mTasks.empty();

            lock.unlock();

            if (complete)
            {
                mCompleteCv.notify_all();
            }
        }
    }

public:
    TaskManager()
    {
        mThread = std::thread(&TaskManager::Worker, this);
    }
    ~TaskManager()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mStopping = true;
        mCv.notify_all();

        lock.unlock();

        if (mThread.joinable())
        {
            mThread.join();
        }
    }

    bool AddTask(int taskId, unsigned int processTimeMs)
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);
            if (mStopping)
            {
                return false;
            }
            mTasks.emplace(taskId, processTimeMs);
        }

        mCv.notify_one();

        return true;
    }

    void WaitComplete()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        mCompleteCv.wait(lock, [&]()
                         { return !mExecuting && mTasks.empty(); });
    }
};

int main()
{
    TaskManager tm;
    tm.AddTas(1, 100);
    tm.AddTas(2, 100);
    tm.AddTas(3, 100);
    tm.AddTas(4, 100);
    tm.WaitComplete();
    return 0;
}
#endif

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
    int mActiveTaskCount = 0;
    bool mStopping = false;
    std::thread mThreadA;
    std::thread mThreadB;
    std::mutex mMutex;
    std::condition_variable mCv;
    std::condition_variable mCvWaitComplete;
    std::condition_variable mCoutCv;
    std::queue<Task> mTasks;

    void Worker()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mMutex);

            if (mStopping && mTasks.empty())
            {
                break;
            }

            mCv.wait(lock, [&]()
                     { return mStopping || !mTasks.empty(); });

            if (mStopping && mTasks.empty())
            {
                break;
            }

            mActiveTaskCount++;

            Task task = mTasks.front();
            mTasks.pop();

            lock.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(task.mProcessTimeMs));

            lock.lock();

            mActiveTaskCount--;
            bool complete = (mActiveTaskCount == 0) && mTasks.empty();

            lock.unlock();

            if (complete)
            {
                mCvWaitComplete.notify_all();
            }
        }
    }

public:
    TaskManager()
    {
        mThreadA = std::thread(&TaskManager::Worker, this);
        mThreadB = std::thread(&TaskManager::Worker, this);
    }
    ~TaskManager()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        mStopping = true;

        mCv.wait(lock, [&]()
                 { return (mActiveTaskCount == 0) && mTasks.empty(); });

        lock.unlock();

        if (mThreadA.joinable())
        {
            mThreadA.join();
        }
        if (mThreadB.joinable())
        {
            mThreadB.join();
        }
    }
    bool AddTask(int id, unsigned long processTimeMs)
    {
        {
            std::unique_lock<std::mutex> lock(mMutex);

            mTasks.emplace(id, processTimeMs);
        }

        mCv.notify_one();
    }
    void WaitComplete()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        mCvWaitComplete.wait(lock, [&]()
                             { return (mActiveTaskCount == 0) && mTasks.empty(); });
    }
};

int main()
{
    TaskManager tm;
    tm.AddTask(1, 200);
    tm.AddTask(2, 150);
    tm.AddTask(3, 400);
    tm.AddTask(4, 200);
    tm.WaitComplete();
    return 0;
}