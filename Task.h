
#include <functional>

class Task {
public:
	Task(function<void(void)> fn);
	void exec();

private:
	function<void(void)> handler;

};

Task::Task(function<void(void)> fn) {
	this->handler = fn;
};

void Task::exec() {
	this->handler();
}

