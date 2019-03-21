#include <iostream>

#include <thread>
#include <vector>
#include <future>
#include <fstream>


std::mutex counter;


inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D &d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

void find_solution_interval(int const from, int const to, int const n, int *res) {
    int solution = 0;
    for (int x = from + 1; x <= to; ++x) {
        for (int y = 1; y <= n * n * 20; ++y) {
            if (((x * y) / (x + y)) > n) {
                break;
            }
            if ((x * y) % (x + y) == 0) {
                if ((x * y) / (x + y) == n) {
//                    std::cout << x << " RES " << y << std::endl;
                    solution++;
                }
            }
        }


    }

    counter.lock();
    *res += solution;
    counter.unlock();
}


int main(int argc, char *argv[]) {
    using std::cout;
    using std::endl;
    using std::thread;
    using std::vector;
    using std::mutex;
    vector<std::thread> vecOfThreads;


    int result = 0;
    // default config
    long threads = 1;
    long n = 100;

    if (argc >= 2) {

        threads = strtol(argv[1], nullptr, 10);
    }

    if (argc == 3) {

        n = strtol(argv[2], nullptr, 10);
    }
//    cout << n << "N" << endl;
//    cout << threads << "threads" << endl;

    auto stage1_start_time = get_current_time_fenced();
    //creating threads
    for (int i = 0; i < threads; ++i) {
        vecOfThreads.emplace_back(&find_solution_interval, (n * n * 20 / threads) * i,
                                  ((n * n * 2 / threads) * i) + n * n * 20 / threads, n,
                                  &result);
    }
    for (auto &vecOfThread : vecOfThreads) {
        vecOfThread.join();
    }
    auto finish_time = get_current_time_fenced();
    auto total_time = finish_time - stage1_start_time;
    std::ofstream out_file("result.txt", std::ofstream::app);
    out_file << "N: " << n << "\tRES: " << result << "\tTIME: " << to_us(total_time) << "\tTHREADS: " << threads << endl;

    //    output time and result for python script
    cout << to_us(total_time) << "\t" << result << endl;
    return 0;
}

