#pragma once
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include <condition_variable>

const int WAIT_TIME = 2; // 单位秒

class ThreadPool
{
private:
	static ThreadPool *_instance;			  // 单例
	static std::once_flag _of;				  // 用于单例创建
	std::mutex _mtx;						  // 互斥量
	std::condition_variable _cv;			  // 条件变量
	std::vector<std::thread> _threads;		  // 存放线程
	std::queue<std::function<void()>> _tasks; // 存放任务
	const int _maxThreadNum;				  // 最大线程数
	bool _isStop;							  // 是否退出

	ThreadPool(int mtn);
	~ThreadPool();

public:
	ThreadPool(const ThreadPool &) = delete;
	ThreadPool &operator=(const ThreadPool &) = delete;
	static ThreadPool *getInstance(int mtn);
	template <typename Func, typename... Args>
	void addTask(Func &&func, Args &&...args);
	void start();
	void stop();
};

template <typename Func, typename... Args>
void ThreadPool::addTask(Func &&func, Args &&...args)
{
	auto task = std::bind(std::forward<Func>(func), std::forward<Args>(args)...); // 连接函数和形参

	// 加入任务队列
	_mtx.lock();
	_tasks.emplace(std::move(task));
	_mtx.unlock();

	_cv.notify_one(); // 唤醒一个线程
}