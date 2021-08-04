#ifndef ZBLUENET_NET_SELECT_ACCEPTOR_H
#define ZBLUENET_NET_SELECT_ACCEPTOR_H

/************************************************************************/
/* 基于 select 接收器模式， 用于接收客户端连接请求                                                                     */
/************************************************************************/

#include <zbluenet/net/acceptor.h>
#include <zbluenet/net/tcp_socket.h>
#include <zbluenet/net/fd_set.h>

#include <stdint.h>

namespace zbluenet {

	class Thread;

	namespace net {

		class SelectAcceptor : public Acceptor {
		public:
			SelectAcceptor(TcpSocket *listen_socket, uint16_t max_socket_num = 1024);
			virtual ~SelectAcceptor();

		protected:
			virtual void loop();

		private:
			FDSet fd_read_;
		};
	} // namespace net
} // namespace zbluenet

#endif // ZBLUENET_NET_SELECT_ACCEPTOR_H
