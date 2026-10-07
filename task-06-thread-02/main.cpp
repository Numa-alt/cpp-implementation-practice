#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <chrono>
#include <condition_variable>
#include <mutex>

struct Task
{
    int mId;
    int mMillisecond;
};

class JobManager
{
private:
    std::queue<Task> mTasks;
    std::vector<std::thread> mWorkers;

    std::mutex mMutex;
    std::mutex mCoutMutex;

    bool mStopped = false;    // スレッドに停止を通知するためのフラグ
    int mActiveTaskCount = 0; // 処理中タスクの数

    std::condition_variable mCv;             // Workerが仕事待ち
    std::condition_variable mCvWaitComplete; // mainが全Task完了待ち

    void WorkerLoop(int workerId)
    {
        while (1)
        {
            std::unique_lock<std::mutex> lock(mMutex);

            mCv.wait(lock, [this]()
                     { return mStopped || !mTasks.empty(); });

            // 停止要求 → waitを抜ける -> return
            if (mStopped)
            {
                return;
            }

            // キューの先頭をコピー
            Task task = mTasks.front();

            // キューの先頭を消す
            mTasks.pop();

            ++mActiveTaskCount;

            lock.unlock();

            // タスクを実行
            {
                std::lock_guard<std::mutex> lock(mCoutMutex);

                std::cout << "worker id " << workerId << " start task id " << task.mId << " time " << task.mMillisecond << "ms" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(task.mMillisecond));

            {
                std::lock_guard<std::mutex> lock(mCoutMutex);

                std::cout << "Worker id " << workerId << " finish task id " << task.mId << std::endl;
            }

            lock.lock();

            --mActiveTaskCount;

            if ((mActiveTaskCount == 0) && mTasks.empty())
            {
                mCvWaitComplete.notify_all();
            }
        }
    }

public:
    JobManager()
    {
        mWorkers.emplace_back(&JobManager::WorkerLoop, this, 1);
        mWorkers.emplace_back(&JobManager::WorkerLoop, this, 2);
    }
    ~JobManager()
    {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mStopped = true;
        }

        mCv.notify_all();

        for (auto &worker : mWorkers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }
    void AddTask(const Task &task)
    {
        {

            std::lock_guard<std::mutex> lock(mMutex);
            mTasks.push(task);
        }
        mCv.notify_one();
    }
    void WaitTaskComplete()
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mCvWaitComplete.wait(lock, [this]()
                             { return mTasks.empty() && (mActiveTaskCount == 0); });
    }
};

int main()
{
    std::cout << "task06 thread02" << std::endl;

    JobManager jobManager;

    jobManager.AddTask({1, 300});
    jobManager.AddTask({2, 100});
    jobManager.AddTask({3, 200});
    jobManager.AddTask({4, 150});
    jobManager.WaitTaskComplete();

    std::cout << "task06 thread02 end" << std::endl;

    return 0;
}
