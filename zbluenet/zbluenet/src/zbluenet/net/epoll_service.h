#ifndef ZBLUENET_NET_EPOLL_SERVICE_H
#define ZBLUENET_NET_EPOLL_SERVICE_H

#include <zbluenet/net/io_service.h>
#include <zbluenet/net/epoller.h>


#ifndef _WIN32
namespace zbluenet {
	namespace net {

		class IODevice;

		class EpollService : public IOService {
		public:
			EpollService(int max_client_num);
			virtual ~EpollService();

			virtual bool addIODevice(IODevice *io_device);
			virtual bool removeIODevice(IODevice *io_device);

			virtual void loop();

		private:
			Epoller epoller_;
		};
	} // namespace net
} // namespace zbluenet

#endif
#endif // ZBLUENET_NET_EPOLL_SERVICE_H
