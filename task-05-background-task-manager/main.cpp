/*


*/

#include <condition_variable>
#include <iostream>
#include <string>
#include <thread>
#include <queue>
#include <vector>
#include <chrono>
#include <mutex>

struct Task
{
    int id;
    unsigned int processTime;
};

class BackgroundTaskManager
{
private:
    std::queue<Task> mTasks; // 未処理の仕事

    std::mutex mMutex; // 共有データ保護

    std::mutex mCoutMutex; // std::coutが混ざらないように

    std::condition_variable mJobCv;  // Workerが仕事街
    std::condition_variable mDoneCv; // mainが全件完了待ち

    int mActiveWorkerCount = 0; // 今まさに仕事中のWorkerの数

    std::vector<std::thread> mWorkers; // 3本のWorkerスレッド

    bool mStopping = false;

    void WorkerLoop(int workerId)
    {
        while (true)
        {

            std::unique_lock<std::mutex> lock(mMutex);

            // 待っている間はmMutexを自動的にunlock
            mJobCv.wait(lock, [this]()
                        { return mStopping || !mTasks.empty(); });

            // 停止要求 ->wait を抜ける -> returnしてWorker終了
            if (mStopping)
            {
                return;
            }

            // キューの先頭をコピー
            Task task = mTasks.front();

            // Queurから削除
            mTasks.pop();

            // 処理中Worker数を+1
            ++mActiveWorkerCount;

            lock.unlock();

            {
                std::lock_guard<std::mutex> lock(mCoutMutex);

                std::cout << "Worker " << workerId << " start Task " << task.id << std::endl;
            }

            // 重い処理
            std::this_thread::sleep_for(
                std::chrono::milliseconds(task.processTime));

            {
                std::lock_guard<std::mutex> lock(mCoutMutex);
                std::cout << "Worker " << workerId << " finish Task " << task.id << std::endl;
            }

            lock.lock();

            --mActiveWorkerCount;

            if (mTasks.empty() && mActiveWorkerCount == 0)
            {
                mDoneCv.notify_all();
            }
        }
    }

public:
    BackgroundTaskManager()
    {
        for (int i = 0; i < 3; ++i)
        {
            mWorkers.emplace_back(
                &BackgroundTaskManager::WorkerLoop,
                this,
                i + 1

            );
        }
    }

    ~BackgroundTaskManager()
    {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mStopping = true;
        }

        mJobCv.notify_all();

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
        mJobCv.notify_one();
    }
    void WaitTaskComplete()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        mDoneCv.wait(lock, [this]()
                     {
                //タスクが空 かつ 処理中Workerが0 になるまで mainを待たせる
                return mTasks.empty() && mActiveWorkerCount == 0; });
    }
};

int main()
{
    std::cout << "task 05 background task manager\n";

    BackgroundTaskManager manager;

    manager.AddTask({1, 300});
    manager.AddTask({2, 100});
    manager.AddTask({3, 500});
    manager.AddTask({4, 200});
    manager.AddTask({5, 400});
    manager.AddTask({6, 150});
    manager.AddTask({7, 250});

    manager.WaitTaskComplete();

    std::cout << "All jobs finished" << std::endl;
    return 0;
}
