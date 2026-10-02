#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

std::queue<int> jobs;
std::mutex m;
std::condition_variable cv;
std::condition_variable doneCv;

int doneCount = 0;

void Worker(std::stop_token st, int id)
{
    while (!st.stop_requested())
    {
        int job = 0;
        {
            std::unique_lock<std::mutex> lock(m);

            cv.wait(lock, [&st]()
                    { return st.stop_requested() || !jobs.empty(); });

            if (st.stop_requested())
            {
                break;
            }
            job = jobs.front();
            jobs.pop();
        }

        // 重い処理
        std::cout << "id " << id << " job = " << job << "\n";

        {
            std::lock_guard<std::mutex> lock(m);
            ++doneCount;
        }

        doneCv.notify_one();
    }
    std::cout << "Worker " << id << " stopped\n";
}

int main()
{
    std::cout << "task_04 start\n";

    std::jthread t1(Worker, 1);
    std::jthread t2(Worker, 2);
    std::jthread t3(Worker, 3);
    {
        std::lock_guard<std::mutex> lock(m);

        jobs.push(100);
        jobs.push(200);
        jobs.push(300);
        jobs.push(400);
        jobs.push(500);
        jobs.push(600);
        jobs.push(700);
    }
    cv.notify_all();

    // ここで少し待つ
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // while (doneCount < 3)
    // {
    //     //
    // }

    std::unique_lock<std::mutex> lock(m);

    doneCv.wait(lock, []()
                { return doneCount >= 7; });

    t1.request_stop();
    t2.request_stop();
    t3.request_stop();
    cv.notify_all();
    return 0;
}
