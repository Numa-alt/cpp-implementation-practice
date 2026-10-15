#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <algorithm>

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

    std::mutex mMutex;

    std::thread mThread;

    bool mStopping = false;

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

            lock.unlock();
        }
    }

public:
    ThreadTaskQueue()
    {

        mThread = std::thread(&ThreadTaskQueue::WorkerLoop, this);
    }

    ~ThreadTaskQueue()
    {
        //
    }

    // タスクを追加
    bool AddTask(int taskId, unsigned long processTimeMs)
    {
        mTasks.emplace(taskId, processTimeMs);
        return true;
    }

    void WaitComplete()
    {
        // キューが空で処理中のタスクがなくなるまでまつ
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
            Taskが空、かつ処理中のタスクがなければ終了
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