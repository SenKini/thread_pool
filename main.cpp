#include "thread_pool.h"
#include <iostream>
#include <chrono>

int main() {
    ThreadPool *tp = ThreadPool::getInstance(5);

    tp->addTask([] { std::cout << "hello1" << std::endl; });
    tp->addTask([] { std::cout << "hello2" << std::endl; });
    tp->start();
    tp->addTask([] { std::cout << "hello3" << std::endl; });
    tp->addTask([] { std::cout << "hello4" << std::endl; });

    std::this_thread::sleep_for(std::chrono::seconds(5));
    tp->stop();

    return 0;
}