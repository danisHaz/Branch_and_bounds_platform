#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <queue>
#include <functional>
#include <future>
#include <cmath>
#include <mutex>

#include "defines.hpp"

namespace babp {
namespace core {

    template < typename T >
    class BabpJob {
        std::function<T()> job;
        std::function<void(T&)> onDone;

        public:
        BabpJob(std::function<T()> job, std::function<void(T&)> onJobDone): job { std::move(job) }, onDone { std::move(onJobDone) } {}
    };

    class BabpThreadPool {

        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        int const maxThreadCount;
        std::mutex jobsMutex;
        std::condition_variable jobWaitingCondition;
        bool isStopped;

        std::atomic_int jobsCount { 0 };

        public:
        BabpThreadPool(int const maxThreadCount):
            maxThreadCount { std::min(maxThreadCount, static_cast<int>(std::thread::hardware_concurrency())) },
            isStopped { false } {
                threads.reserve(maxThreadCount);
            }
        
        void start() {
            for (std::size_t ind = 0; ind < maxThreadCount; ind++) {
                threads.emplace_back(std::thread { [this](){
                    while (true) {
                        std::function<void()> job;
                        {
                            std::unique_lock<std::mutex> lock(jobsMutex);
                            jobWaitingCondition.wait(lock, [this] {
                                return !tasks.empty() || isStopped;
                            });
                            if (isStopped) {
                                return;
                            }
                            PRINT("job is got");
                            job = tasks.front(); tasks.pop();
                        }
                        PRINT("job is launched");
                        job();
                        PRINT("job is done");
                    }
                } });
            }
        }

        void launchJob(std::function<void()> job) {
            {
                std::unique_lock<std::mutex> lock(jobsMutex);
                tasks.push([job, this]() {
                    job();
                    jobsCount--;
                });
            }
            jobsCount++;
            jobWaitingCondition.notify_one();
        }

        void stop() {
            while (jobsCount != 0) {} // wait for jobs to be done

            {
                std::unique_lock<std::mutex> lock(jobsMutex);
                isStopped = true;
            }
            jobWaitingCondition.notify_all();
            for (std::thread& thread : threads) {
                thread.join();
            }
            threads.clear();
        }
        
    };
} // namespace core
} // namespace babp