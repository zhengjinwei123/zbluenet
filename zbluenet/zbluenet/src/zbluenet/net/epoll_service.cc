#include <zbluenet/net/epoll_service.h>
#include <zbluenet/timestamp.h>
#include <zbluenet/log.h>
#include <zbluenet/net/io_device.h>

#ifndef _WIN32
namespace zbluenet {
	namespace net {

		EpollService::EpollService(int max_client_num) :
			IOService()
		{
			epoller_.create(max_client_num);
		}

		EpollService::~EpollService()
		{
			epoller_.destroy();
		}

		bool EpollService::addIODevice(IODevice *io_device)
		{
			return epoller_.add(io_device);
		}

		bool EpollService::removeIODevice(IODevice *io_device)
		{
			return epoller_.del(io_device);
		}

		void EpollService::loop()
		{
			quit_ = false;
			Timestamp now;
			while (!quit_) {
				now.setNow();
				int timer_timeout = timer_heap_.getNextTimeoutMillisecond(now);

				if (timer_timeout == -1) {
					timer_timeout = 0;
				}

				int ret = epoller_.doEvent(timer_timeout);
				if (ret == -1) {
					LOG_DEBUG("EpollService::loop QUIT");
					quit_ = true;
					break;
				}

				now.setNow();
				checkTimeout(now);
		   }
		}
	} // namespace net
} // namespace zbluenet

#endif