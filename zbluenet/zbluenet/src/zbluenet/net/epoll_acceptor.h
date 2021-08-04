#ifndef ZBLUENET_NET_EPOLL_ACCEPTOR_H
#define ZBLUENET_NET_EPOLL_ACCEPTOR_H

#include <zbluenet/net/acceptor.h>
#include <zbluenet/net/tcp_socket.h>
#include <zbluenet/net/epoller.h>

#ifndef _WIN32
namespace zbluenet {
	namespace net {

		class IODevice;

		class EPollAcceptor : public Acceptor {
		public:
			EPollAcceptor(TcpSocket *listen_socket, uint16_t max_socket_num = 1024);
			virtual ~EPollAcceptor();

		protected:
			virtual void loop();

		private:
			int onListenSocketRead(IODevice *io_device);

		private:
			Epoller epoller_;
		};

	} // namespace net
} // namespace zbluenet

#endif
#endif // ZBLUENET_NET_EPOLL_ACCEPTOR_H
