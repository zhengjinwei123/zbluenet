#include <zbluenet/semaphore.h>

namespace zbluenet {
	Semaphore::Semaphore() :
		wait_count_(0), wakeup_count_(0)
	{
	}

	Semaphore::~Semaphore()
	{

	}

	void Semaphore::wait()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if (--wait_count_ < 0) {
			condition_var_.wait(lock, [this]() -> bool {
				return wakeup_count_ > 0;
			});
			--wakeup_count_;
		}
	}

	void Semaphore::wakeup()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (++wait_count_ <= 0) {
			++wakeup_count_;
			condition_var_.notify_one();
		}
	}

} // namespace zbluenet