#include <zbluenet/task/async_task.h>

namespace zbluenet {
	namespace task {
		AsyncTask::AsyncTask()
		{

		}

		AsyncTask::~AsyncTask()
		{

		}

		void AsyncTask::doCallback()
		{
			if (task_cb_) {
				task_cb_(this);
			}
		}

		void AsyncTask::setTaskCallback(const TaskCallback &task_cb)
		{
			task_cb_ = task_cb;
		}
	}
} // namespace zbluenet