#include "thread_pool.h"

// 对静态变量初始化
ThreadPool *ThreadPool::_instance = nullptr;
std::once_flag ThreadPool::_of;

ThreadPool::ThreadPool(int mtn)
    : _maxThreadNum(mtn), _isStop(true) {}

ThreadPool::~ThreadPool() {
    stop();
    delete _instance;
}

ThreadPool *ThreadPool::getInstance(int mtn) {
    std::call_once(_of, [=] {
        _instance = new ThreadPool(mtn);
    });  // 确保实例只会被创建一次

    return _instance;
}

void ThreadPool::start() {
    // 防止连续调用 start() 的情况
    if (_isStop) {
        _isStop = false;

        for (int i = 0; i < _maxThreadNum; i++)
            _threads.emplace_back([this] {
                while (true) {
                    std::unique_lock ul(_mtx);
                    _cv.wait(ul, [this] {
                        return !_tasks.empty() || _isStop;
                    });  // 队列为空且没有停止则阻塞

                    if (_isStop)
                        return;

                    // 获取一个任务
                    auto task = std::move(_tasks.front());
                    _tasks.pop();

                    ul.unlock();  // 解锁

                    task();  // 执行任务
                }
            });
    }
}

void ThreadPool::stop() {
    // 防止连续调用 stop() 的情况
    if (!_isStop) {
        _mtx.lock();
        _isStop = true;
        _mtx.unlock();

        _cv.notify_all();
        for (auto &t : _threads)
            if (t.joinable())
                t.join();
    }
}