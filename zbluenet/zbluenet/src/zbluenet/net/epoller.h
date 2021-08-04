#ifndef ZBLUENET_NET_EPOLLER_H
#define ZBLUENET_NET_EPOLLER_H

#include <zbluenet/net/platform.h>

#include <stdint.h>
#include <vector>
#include <unordered_set>

#ifndef _WIN32
#include <sys/epoll.h>

namespace zbluenet {
	namespace net {

		class IODevice;

		class Epoller {
		public:
			using EventVector = std::vector<struct epoll_event>;
			using IODeviceSet = std::unordered_set<intptr_t>;

			Epoller();
			~Epoller();

			int create(int max_event_num = 30000);
			void destroy();
			bool add(IODevice *io_device);
			bool del(IODevice *io_device);
			bool update(IODevice *io_device);

			int doEvent(int timeout);

		private:
			bool checkIODeviceExist(IODevice *io_device) const;

		private:
			int epoll_fd_;
			EventVector events_;
			IODeviceSet removed_io_devices_;
		};
	} // namespace net
} // namespace zbluenet


#endif

#endif // ZBLUENET_NET_EPOLLER_H
