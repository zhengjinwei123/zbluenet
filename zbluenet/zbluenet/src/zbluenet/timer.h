#ifndef ZBLUENET_TIMER_H
#define ZBLUENET_TIMER_H

#include <zbluenet/base_heap_value.h>
#include <zbluenet/type_define.h>
#include <zbluenet/timestamp.h>

#include <functional>

namespace zbluenet {

	class Timer : public BaseHeapValue<Timestamp> {
	public:
		using TimerCallback = std::function<void(zbluenet_type::TimerId)>;

		Timer(zbluenet_type::TimerId timer_id, const Timestamp &timestamp, int64_t timeout_millisec, const TimerCallback &timer_cb, int32_t call_times):
			BaseHeapValue(timestamp), timer_id_(timer_id),
			timeout_millisec_(timeout_millisec),
			timer_cb_(timer_cb),
			call_times_(call_times)
		{

		}

		~Timer()
		{

		}

		zbluenet_type::TimerId getId() const { return timer_id_;  }

		int64_t getTimeoutMillisec() const { return timeout_millisec_;  }
		int32_t getCallTimes() const { return call_times_;  }
		void decCallTimes() { call_times_ -= 1;  }
		TimerCallback getCallbackFunc() const { return timer_cb_;  }

	private:
		zbluenet_type::TimerId timer_id_;
		int64_t timeout_millisec_;
		TimerCallback timer_cb_;
		int32_t call_times_;
	};

} // namespace zbluenet
#endif // ZBLUENET_TIMER_H
