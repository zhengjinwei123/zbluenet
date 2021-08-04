#ifndef ZBLUENET_NET_SELECT_REACTOR_H
#define ZBLUENET_NET_SELECT_REACTOR_H

/************************************************************************/
/* 基于 select 的反应器模式， 用于处理socket 读写事件                                                                     */
/************************************************************************/

#include <zbluenet/class_util.h>
#include <zbluenet/net/reactor.h>
#include <zbluenet/net/fd_set.h>
#include <zbluenet/net/tcp_socket.h>
#include <zbluenet/net/platform.h>

#include <memory>
#include <mutex>
#include <unordered_map>

namespace zbluenet {
	namespace net {

		class SelectReactor : public Reactor {
		public:
			using TcpSocketVector = std::vector<TcpSocket *>;
			using FdSocketIdMap = std::unordered_map<SOCKET, TcpSocket::SocketId>;

			SelectReactor(int max_connection_num) : Reactor(max_connection_num),
				client_change_(false)
			{
				fd_read_.create(max_connection_num);
				fd_write_.create(max_connection_num);
				fd_read_back_.create(max_connection_num);
			}

			virtual ~SelectReactor() {}

			virtual void loop();
			virtual void closeSocket(SocketId socket_id);
			virtual bool attachSocket(std::unique_ptr<TcpSocket> &peer_socket);

		private:
			void heartCheck();
			bool doNetEvent();

			void doReadEvent();
			void doWriteEvent();

		private:
			FDSet fd_read_;
			FDSet fd_write_;
			FDSet fd_read_back_;

			TcpSocketVector new_sockets_;
			FdSocketIdMap fd_to_socketId_map_;
			std::mutex mutex_;
			bool client_change_;
			SOCKET max_sock_;
		};
	} // namespace net 
} // namespace zbluenet
#endif // ZBLUENET_NET_SELECT_EVENT_LOOP_H
