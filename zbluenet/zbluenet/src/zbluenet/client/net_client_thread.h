#ifndef ZBLEUNET_CLIENT_NET_CLIENT_THREAD_H
#define ZBLEUNET_CLIENT_NET_CLIENT_THREAD_H

#include <functional>
#include <memory>
#include <string>
#include <stdint.h>

#include <zbluenet/net/reactor.h>
#include <zbluenet/net/net_thread.h>
#include <zbluenet/net/tcp_socket.h>
#include <zbluenet/net/message_queue.h>
#include <zbluenet/protocol/net_command.h>
#include <zbluenet/net/socket_address.h>

namespace zbluenet {

	using net::NetThread;
	using net::Reactor;
	using net::TcpSocket;
	using net::SocketAddress;
	using protocol::NetCommand;
	using net::MessageQueue;

	namespace exchange {
		class BaseStruct;
	}
	using exchange::BaseStruct;

	namespace client {

		class  NetClientThread : public NetThread {
		public:		
			NetClientThread(int max_recv_packet_lenth,
				int max_send_packet_length,
				const CreateMessageFunc &create_message_func,
				int reconnect_interval_ms, std::string ip, uint16_t port);
			~NetClientThread();

			void startRun();
			virtual  void stop();

			void push(NetCommand *cmd);

			void onRecvMessage(NetThread *net_thread,
				TcpSocket::SocketId socket_id,
				DynamicBuffer *buffer,
				const NewNetCommandCallback &new_net_cmd_cb);

		private:
			bool connect();
			void disconnect();
			void startConnectTimer();
			void stopConnectTimer();
			void onTimer(int64_t timer_id);

			void onClose();
			 
		protected:
			virtual void onNetCommand(NetCommandQueue *queue = nullptr);
			virtual void onPeerClose(Reactor *reactor, TcpSocket::SocketId socket_id);
			virtual void onError(Reactor *reactor, TcpSocket::SocketId socket_id, int error);

		private:
			bool connected_;
			int reconnect_interval_ms_;
			TcpSocket::SocketId socket_id_;
			SocketAddress remote_addr_;
			TcpSocket tcp_socket_;
			int64_t reconnect_timer_;

			NetProtocol net_protocol_;
			NewNetCommandCallback new_net_cmd_cb_;
		};

	} // namespace client
} // namespace zbluenet

#endif // ZBLEUNET_CLIENT_NET_CLIENT_THREAD_H
