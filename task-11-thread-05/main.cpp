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
    Task(int taskId, unsigned long processTimeMs) : mTaskId(taskId), mProcessTimeMs(processTimeMs)
    {
    }
    int mTaskId;
    unsigned long mProcessTimeMs;
};

class ThreadTaskQueue
{
private:
    std::queue<Task> mTasks;

    std::condition_variable mCv;
    std::condition_variable mCompleteCv;

    std::mutex mMutex;

    std::thread mThread;

    bool mStopping = false;

    bool mExecuting = false;

    void WorkerLoop()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mMutex);

            // 条件が成立するまでミューテックスを解放して待つ
            mCv.wait(lock, [&]()
                     { return mStopping || !mTasks.empty(); });

            // 終了要求かつタスクのキューが空なら終了
            if (mStopping && mTasks.empty())
            {
                break;
            }

            // 先頭から一つ取り出す
            Task task = mTasks.front();
            mTasks.pop();

            mExecuting = true;

            lock.unlock();

            // タスクを実行する
            std::this_thread::sleep_for(std::chrono::milliseconds(task.mProcessTimeMs));

            lock.lock();
            mExecuting = false;

            bool complete = mTasks.empty();

            // スコープの終わりでlockは自動的にlock.unlock()される
            // しかし、notify_all()を呼ぶためにここでロック解除する
            lock.unlock();

            if (complete)
            {
                mCompleteCv.notify_all();
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
        // 共有変数をロックして終了要求を立てる
        std::unique_lock<std::mutex> lock(mMutex);
        mStopping = true;

        // wait中のWorkerを起こす
        mCv.notify_all();

        lock.unlock();

        // そのstd::threadが今join()またはdetach()できる状態かを確認
        if (mThread.joinable())
        {
            // Worker終了を待つ
            mThread.join();
        }
    }

    // タスクを追加
    bool AddTask(int taskId, unsigned long processTimeMs)
    {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            if (mStopping)
            {
                return false;
            }

            // タスクをキューに追加
            mTasks.emplace(taskId, processTimeMs);

            // mutexを解除
        }

        // 待機中のワーカーを起こす
        mCv.notify_one();

        return true;
    }

    void WaitComplete()
    {
        // キューが空で処理中のタスクがなくなるまでまつ
        std::unique_lock<std::mutex> lock(mMutex);

        mCompleteCv.wait(lock, [&]()
                         { return !mExecuting && mTasks.empty(); });
    }
};

int main()
{
    std::cout << "task-11-thread-05 start\n";

    ThreadTaskQueue threadTaskQueue;

    threadTaskQueue.AddTask(1, 300);
    threadTaskQueue.AddTask(2, 150);
    threadTaskQueue.AddTask(3, 100);
    threadTaskQueue.AddTask(4, 250);
    threadTaskQueue.AddTask(5, 200);

    threadTaskQueue.WaitComplete();

    std::cout << "task-11-thread-05 end\n";
    return 0;
}

/*
    スレッドのメモ

        スレッドでWorkerLoopを実行

        WorkerLoop()
        while(1){
            TaskQueueを確認。
            終了用雨旧があり、Taskが空、かつ処理中のタスクがなければ終了
            TaskQueueから１つ取り出す
            タスクを実行
            タスクが終了
        }


        std::thread を宣言。
        コンストラクタに、
            実行するメンバ関数(ThreadTaskQueue::WorkerLoop)
            どのインスタンスのWorkerLoopを呼ぶのか指定するためthis
        を指定
*/

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
    std::mutex mCoutMutex;
    std::condition_variable mCv;
    std::condition_variable mCvWaitComplete;
    std::condition_variable mCoutCv;
    std::queue<Task> mTasks;

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
                break;
            }

            Task task = mTasks.front();
            mTasks.pop();

            mActiveTaskCount++;

            lock.unlock();

            {
                std::unique_lock<std::mutex> lock(mCoutMutex);
                std::cout << "start task " << task.mId << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(task.mProcessTimeMs));
            {
                std::unique_lock<std::mutex> lock(mCoutMutex);
                std::cout << "end task " << task.mId << "\n";
            }

            lock.lock();

            mActiveTaskCount--;

            bool complete = (mActiveTaskCount == 0) && mTasks.empty();

            lock.unlock();

            {
                std::unique_lock<std::mutex> lock(mCoutMutex);
                std::cout << "active task count " << mActiveTaskCount << " complete " << complete << "\n";
            }

            if (complete)
            {

                {
                    std::unique_lock<std::mutex> lock(mCoutMutex);
                    std::cout << "wait complete notify all" << "\n";
                }
                mCvWaitComplete.notify_all();
            }
        }
    }

public:
    TaskManager()
    {
        mThreads.emplace_back(std::thread(&TaskManager::Worker, this));
        mThreads.emplace_back(std::thread(&TaskManager::Worker, this));
    }
    ~TaskManager()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        std::cout << "TaskManager destrocutor\n";

        mStopping = true;

        mCv.notify_all();

        lock.unlock();

        for (int i = 0; i < mThreads.size(); i++)
        {
            std::cout << "thread " << i << " joinable " << mThreads[i].joinable() << std::endl;
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

    void WaitComplete()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        mCvWaitComplete.wait(lock, [&]()
                             { return (mActiveTaskCount == 0) && mTasks.empty(); });
    }
};

int main()
{
    std::cout << "start task11\n";
    TaskManager tm;

    tm.AddTask(1, 200);
    tm.AddTask(2, 150);
    tm.AddTask(3, 400);
    tm.AddTask(4, 200);

    std::cout << "wait complete\n";
    tm.WaitComplete();

    std::cout << "end task11\n";
    return 0;
}