#ifndef ZBLUENET_SEMAPHORE_H
#define ZBLUENET_SEMAPHORE_H

#include <mutex>
#include <condition_variable>

namespace zbluenet {
	class Semaphore {
	public:
		Semaphore();
		~Semaphore();

		void wait();
		void wakeup();

	private:
		std::mutex mutex_;
		std::condition_variable condition_var_;

		int wait_count_;
		int wakeup_count_;
	};

} // namespace zbluenet
#endif // ZBLUENET_SEMAPHORE_H
