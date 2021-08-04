#include <zbluenet/net/io_service.h>

#include <zbluenet/net/io_device.h>
#include <zbluenet/timestamp.h>

namespace zbluenet {
	namespace net {

		IOService::IOService() : quit_(false)
		{

		}
		IOService::~IOService()
		{

		}

		void IOService::quit()
		{
			quit_ = true;
		}

		bool IOService::addIODevice(IODevice *io_device)
		{
			return true;
		}

		bool IOService::removeIODevice(IODevice *io_device)
		{
			return true;
		}

		IOService::TimerId IOService::startTimer(int64_t timeout_ms, const TimerCallback &timer_cb, int call_times)
		{
			Timestamp now;
			return timer_heap_.addTimer(now, timeout_ms, timer_cb, call_times);
		}

		void IOService::stopTimer(TimerId timer_id)
		{
			timer_heap_.removeTimer(timer_id);
		}

		void IOService::checkTimeout(const Timestamp &now)
		{
			timer_heap_.checkTimeout(now);
		}
	} // namespace net
} // namespace zbluenet