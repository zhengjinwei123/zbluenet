#ifndef ZBLUENET_NET_ACCEPTOR_H
#define ZBLUENET_NET_ACCEPTOR_H

#include <zbluenet/class_util.h>
#include <zbluenet/thread.h>
#include <zbluenet/net/tcp_socket.h>

#include <zbluenet/net/socket_id_allocator.h>
#include <zbluenet/net/io_service.h>

#include <functional>
#include <cstddef>
#include <stdint.h>

namespace zbluenet {

	namespace net {

		class Acceptor  : public IOService {
		public:
			using NewConnectionCallback = std::function<void (std::unique_ptr<TcpSocket> &peer_socket)>;

		public:
			Acceptor(TcpSocket *listen_socket, uint16_t max_socket_num = 1024);
			virtual ~Acceptor();

			void setNewConnCallback(NewConnectionCallback new_conn_cb);
			virtual bool start();
			virtual void stop();
			virtual void loop();

		protected:
			bool accept();
			
		protected:
			SocketAddress socket_addr_;
			TcpSocket *listen_socket_;
			Thread thread_;
			bool quit_;
			SocketIdAllocator socket_id_allocator_;

			NewConnectionCallback new_conn_cb_;

			uint16_t max_socket_num_;
		};
	} // namespace net
} // namespace zbluenet


#endif // ZBLUENET_NET_ACCEPTOR_H
