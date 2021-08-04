#ifndef ZBLUENET_TIMER_HEAP_H
#define ZBLUENET_TIMER_HEAP_H

#include <zbluenet/type_define.h>
#include <zbluenet/class_util.h>
#include <stdint.h>
#include <functional>
#include <memory>

namespace zbluenet {

	class Timestamp;

	class TimerHeap : public Noncopyable {
	public:
		using TimerCallback = std::function<void(zbluenet_type::TimerId)>;

		TimerHeap();
		~TimerHeap();

		int64_t getNextTimeoutMillisecond(const Timestamp &now) const;
		zbluenet_type::TimerId addTimer(const Timestamp &now, int64_t timeout_ms, const TimerCallback &timer_cb, int call_times = -1);
		void removeTimer(zbluenet_type::TimerId timer_id);
		void checkTimeout(const Timestamp &now);

	private:
		class Impl;
		std::unique_ptr<Impl> pimpl_;
	};
} // namespace zbluenet

#endif // ZBLUENET_TIMER_HEAP_H
