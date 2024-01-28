#include <mutex>
#include <vector>
#include <thread>
#include <queue>
#include <future>
#include <memory>
#include <functional>
#include <unordered_map>
#include <condition_variable>

#define WAIT_SECONDS 2

class ThreadPool {
private:
	std::mutex _mtx;											// 互斥量
	std::condition_variable _cv;								// 条件变量
	std::unordered_map<std::thread::id, std::thread> _threads;	// 存放线程，键值对的形式方便提前结束多余线程
	std::queue<std::function<void()>> _tasks;					// 存放任务
	int _maxThreadNum;											// 最大线程数
	int _curThreadNum;											// 正在运行的线程
	int _idleThreadNum;											// 空闲的线程
	bool _isQuit;												// 是否退出

	void worker();										// 执行 task
	void joinFinishedThread(std::thread::id threadId);	// 提前结束多余的线程

public:
	ThreadPool(int mtn);
	~ThreadPool();
	ThreadPool(const ThreadPool &) = delete;
	ThreadPool &operator=(const ThreadPool &) = delete;

	int getCurThreadNum();	// 获取正在运行的线程数

	template <typename Func, typename... Arg>
	auto addTask(Func &&func, Arg &&...args) {	// 添加任务
		using ReturnType = decltype(func(args...));

		auto taskFunc = std::bind(std::forward<Func>(func), std::forward<Arg>(args)...);
		auto taskPackge = new std::packaged_task<ReturnType()>(taskFunc);
		auto res = taskPackge->get_future();

		if (!_isQuit) {
			std::lock_guard lg(_mtx);

			_tasks.emplace([taskPackge] {
				(*taskPackge)();
			});

			if (_idleThreadNum > 0)	 // 如果有空闲线程则不必新建线程
				_cv.notify_one();
			else if (_curThreadNum < _maxThreadNum) {  // 保证线程数不会超过最大值
				std::thread t(&ThreadPool::worker, this);

				_threads[t.get_id()] = std::move(t);
				_curThreadNum++;
			}
		}

		return res;
	}
};