#ifndef ZBLUENET_TASK_ASYNC_TASK_H
#define ZBLUENET_TASK_ASYNC_TASK_H


#include <zbluenet/class_util.h>
#include <functional>

namespace zbluenet {
	namespace task {
		class AsyncTask : public Noncopyable {
			using TaskCallback = std::function<void(AsyncTask *)>;

			AsyncTask();
			virtual ~AsyncTask();

			void doCallback();
			const TaskCallback& getTaskCallback() const { return task_cb_;  }
			void setTaskCallback(const TaskCallback &task_cb);
		private:
			TaskCallback task_cb_;
		};
	} // namespace task
} // namespace zbluenet

#endif // ZBLUENET_TASK_ASYNC_TASK_H
