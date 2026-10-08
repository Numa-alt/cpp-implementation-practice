#include <iostream>
#include <mutex>
#include <vector>
#include <chrono>
#include <condition_variable>
#include <thread>
#include <queue>

struct Task
{
    int mId;
    unsigned long mProcessTime;
    std::function<void()> mCallback;
};

class JobManager
{
private:
    std::queue<Task> mTasks;

    std::vector<std::thread> mWorker;

    std::mutex mMutex;
    std::mutex mCoutMutex; // std::coutが乱れないように

    bool mStopping = false;

    int mActiveTaskCount = 0;

    std::condition_variable mCv;             // workerが仕事または停止要求を待つため
    std::condition_variable mCvWaitComplete; // mainがスレッドの完了を待つため

    void Worker(int id)
    {

        while (1)
        {

            //
            std::unique_lock<std::mutex> lock(mMutex);
            mCv.wait(lock, [this]()
                     {
                //停止命令、またはタスクが空じゃなければ起きる
                return mStopping || !mTasks.empty(); });

            if (mStopping)
            {
                return;
            }

            mActiveTaskCount++;
            Task task = mTasks.front();
            mTasks.pop();

            lock.unlock();

            // タスク処理
            {
                std::lock_guard<std::mutex> lock(mCoutMutex);
                std::cout << "Worker id " << id << " task id" << task.mId << " time " << task.mProcessTime << std::endl;
            }

            // スレッドをtask.mProcessTimeだけウェイト処理
            std::this_thread::sleep_for(std::chrono::milliseconds(task.mProcessTime));

            if (task.mCallback)
            {
                task.mCallback();
            }

            lock.lock();

            mActiveTaskCount--;

            if (mActiveTaskCount == 0 && mTasks.empty())
            {
                mCvWaitComplete.notify_all();
            }

            lock.unlock();
        }
    }

public:
    JobManager()
    {
        // スレッドを2つ開始
        mWorker.emplace_back(&JobManager::Worker, this, 1);
        mWorker.emplace_back(&JobManager::Worker, this, 2);
    }
    ~JobManager()
    {
        // 全てのスレッドに終了を通知
        std::unique_lock<std::mutex> lock(mMutex);
        mStopping = true;
        mCv.notify_all();
        lock.unlock();

        // 全てのスレッドが終了するまで待つ
        for (auto &worker : mWorker)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    void AddTask(const Task &task)
    {
        // ミューテックスを取得し、タスクキューに追加
        {
            // lockをはずした後にnotifyする
            {
                std::unique_lock<std::mutex> lock(mMutex);
                mTasks.push(task);
            }

            // スレッドを一つ起こす必要がある
            mCv.notify_one();
        }
    }
    void WaitTaskComplete()
    {
        //
        std::unique_lock<std::mutex> lock(mMutex);

        // 処理中のタスクがなく、キューがからになるまでまつ
        mCvWaitComplete.wait(lock, [this]()
                             { return mTasks.empty() && mActiveTaskCount == 0; });
    }
};

int main()
{
    std::cout << "task-08-thread-04 start" << std::endl;

    std::atomic<int> completeCount = 0;

    JobManager jobManager;
    jobManager.AddTask({1, 200, [&]()
                        {
                            ++completeCount;
                            std::cout << "task 1 complete " << std::endl;
                        }});
    jobManager.AddTask({2, 50, [&]()
                        {
                            ++completeCount;
                            std::cout << "task 2 complete " << std::endl;
                        }});
    jobManager.AddTask({3, 100, [&]()
                        {
                            ++completeCount;
                            std::cout << "task 3 complete " << std::endl;
                        }});
    jobManager.AddTask({4, 150, [&]()
                        {
                            ++completeCount;
                            std::cout << "task 4 complete " << std::endl;
                        }});
    jobManager.AddTask({5, 300, [&]()
                        {
                            ++completeCount;
                            std::cout << "task 5 complete " << std::endl;
                        }});
    jobManager.AddTask({6, 100, [&]()
                        {
                            ++completeCount;
                            std::cout << "task 6 complete " << std::endl;
                        }});

    jobManager.WaitTaskComplete();

    std::cout << "completeCount " << completeCount.load() << std::endl;

    std::cout << "task-08-thread-04 end" << std::endl;
    return 0;
}
