#ifndef ZBLUENET_NET_EPOLL_REACTOR_H
#define ZBLUENET_NET_EPOLL_REACTOR_H

/************************************************************************/
/* 基于 epoll 的反应器模式， 用于处理socket 读写事件                                                                     */
/************************************************************************/

#include <zbluenet/net/reactor.h>
#include <zbluenet/net/epoller.h>
#include <zbluenet/net/tcp_socket.h>
#include <zbluenet/net/platform.h>

#ifndef _WIN32

namespace zbluenet {
	namespace net {

		class IODevice;

		class EpollReactor : public Reactor {
		public:
			EpollReactor(int max_socket_num);
			virtual ~EpollReactor();

			virtual bool addIODevice(IODevice *io_device);
			virtual bool removeIODevice(IODevice *io_device);

			virtual void loop();
			virtual void closeSocket(SocketId socket_id);
			virtual bool attachSocket(std::unique_ptr<TcpSocket> &peer_socket);

		private:
			Epoller epoller_;
		};


	} // namespace net
} // namespace zbluenet

#endif // _WIN32
#endif // ZBLUENET_NET_EPOLL_REACTOR_H
