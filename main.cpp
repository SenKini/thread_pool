#include "thread_pool.hpp"
#include <iostream>
#include <future>

void task1() {
	std::cout << "hello" << std::endl;
}

void task2() {
	std::cout << "h" << std::endl;
}

void task3() {
	std::cout << "world" << std::endl;
}

void task4() {
	std::cout << "w" << std::endl;
}

int main() {
	ThreadPool t(5);

	t.addTask(task1);
	t.addTask(task2);
	t.addTask(task3);
	t.addTask(task4);

	// std::packaged_task<decltype(task)> pt(task);
	// pt();

	return 0;
}