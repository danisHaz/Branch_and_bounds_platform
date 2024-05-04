#include <iostream>
#include <chrono>
#include <thread>

#include "tasks.hpp"

int main() {

    auto beginTime = std::chrono::high_resolution_clock::now();
    task1(12, 12);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();


    return 0;
}