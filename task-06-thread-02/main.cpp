#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <chrono>
#include <condition_variable>


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

    bool mStopped = false; //スレッドに停止を通知するためのフラグ
    int mActiveTaskCount = 0;//処理中タスクの数

    std::condition_variable mCv;//Workerが仕事待ち
    std::condition_variable mCvWaitComplete;//mainが全Task完了待ち

    void WorkerLoop(int workerId)
    {

    }
public:
    void AddTask(const Task& task)
    {

    }
    void WaitTaskComplete()
    {

    }
};


int main()
{
    std::cout << "task06 thread02" << std::endl;

    JobManager jobManager;

    jobManager.AddTask({1,100});
    jobManager.AddTask({2,300});
    jobManager.WaitTaskComplete();

    std::cout<<"task06 thread02 end"<<std::endl;

    return 0;
}


