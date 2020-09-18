#include <queue>
#include <functional>
#include <vector>
#include <thread>
#include "BlockingQueue.h"

using namespace std;

class ThreadPool {
private:
	BlockingQueue<function<void(void)>> taskList;
	vector<thread*> threads;


public:
	ThreadPool(int theads);
	void execute(function<void(void)> fn);
	int taskCount();
};


ThreadPool::ThreadPool(int size) {
	for (int i = 0; i < size; i++) {
		thread *threadItem = new thread([this]() {
			while (true) {
				auto task = taskList.Take();
				task();
			}
			});
		threads.push_back(threadItem);
	}
}

void ThreadPool::execute(function<void(void)> task) {
	taskList.Put(task);
}

int ThreadPool::taskCount() {
	return taskList.Size();
}