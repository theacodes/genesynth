#ifndef THEA_TASK_H
#define THEA_TASK_H

namespace thea {

typedef void (*TaskCallback)(void);

class Task {
public:
	Task(const char* name, TaskCallback callback, unsigned long interval) :
		name(name), callback(callback), interval(interval) {}

	const char* name;
	TaskCallback callback;
	unsigned long interval;
	unsigned long last_execution = 0;
	// The exponential moving average of this task's execution time.
	float average_execution_time = 0.f;
	 // Alpha value for the exponential moving average.
	float average_execution_time_alpha = 2.f / (1000.f + 1.f);
	unsigned long last_execution_time = 0;
	unsigned long max_execution_time = 0;
};

class TaskManager {
public:
	TaskManager() {}

	inline void add(Task* task) {
		for(int i = 0; i < 32; i++) {
			if(tasks[i] == nullptr) {
				tasks[i] = task;
				break;
			}
		}
	}

	inline void run() {
		for(int i = 0; i < 32; i++) {
			auto task = tasks[i];
			if(task == nullptr) continue;
			run_task(task);
		}
	}

	Task* tasks[32];

private:

	inline void run_task(Task* task) {
		auto now = micros();
		auto next = task->last_execution + task->interval;

		if (now < next) return;

		task->callback();

		task->last_execution = now;

		auto execution_time = micros() - now;
		task->last_execution_time = execution_time;
		task->average_execution_time = (task->average_execution_time_alpha * execution_time) + (1.f - task->average_execution_time_alpha) * task->average_execution_time;
		if(task->max_execution_time < execution_time) task->max_execution_time = execution_time;
	}
};

} // namespace thea

#endif
