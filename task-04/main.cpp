// #include <iostream>
// #include <queue>
// #include <mutex>
// #include <condition_variable>
// #include <thread>

// std::queue<int> jobs;
// std::mutex m;
// std::condition_variable cv;
// std::condition_variable doneCv;

// int doneCount = 0;

// void Worker(std::stop_token st, int id)
// {
//     while (!st.stop_requested())
//     {
//         int job = 0;
//         {
//             std::unique_lock<std::mutex> lock(m);

//             cv.wait(lock, [&st]()
//                     { return st.stop_requested() || !jobs.empty(); });

//             if (st.stop_requested())
//             {
//                 break;
//             }
//             job = jobs.front();
//             jobs.pop();
//         }

//         // 重い処理
//         std::cout << "id " << id << " job = " << job << "\n";

//         {
//             std::lock_guard<std::mutex> lock(m);
//             ++doneCount;
//         }

//         doneCv.notify_one();
//     }
//     std::cout << "Worker " << id << " stopped\n";
// }

// #include <concepts>
// #include <string>
// #include <iostream>



int main()
 {
     return 0;
 }

