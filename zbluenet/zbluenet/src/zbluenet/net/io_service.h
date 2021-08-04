#ifndef ZBLUENET_NET_IO_SERVICE_H
#define ZBLUENET_NET_IO_SERVICE_H

#include <zbluenet/class_util.h>
#include <zbluenet/timer_heap.h>

#include <stdint.h>
#include <functional>

namespace zbluenet {
	class Timestamp;

	namespace net {

		class IODevice;

		class IOService: public Noncopyable {
		public:
			using TimerId = int64_t;
			using TimerCallback = std::function<void(TimerId)>;

		public:
			IOService();
			virtual ~IOService();

			virtual void loop() = 0;
			void quit();

			virtual bool addIODevice(IODevice *io_device);
			virtual bool removeIODevice(IODevice *io_device);

			TimerId startTimer(int64_t timeout_ms, const TimerCallback &timer_cb, int call_times = -1);
			void stopTimer(TimerId timer_id);

		protected:
			void checkTimeout(const Timestamp &now);

		protected:
			bool quit_;
			TimerHeap timer_heap_;
		};
	} // namespace net
} // namespace zbluenet
#endif // ZBLUENET_NET_IO_SERVICE_H
