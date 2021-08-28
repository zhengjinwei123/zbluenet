#ifndef ZBLEUNET_CLIENT_NET_CLIENT_THREAD_H
#define ZBLEUNET_CLIENT_NET_CLIENT_THREAD_H

#include <functional>

#include <zbluenet/net/reactor.h>
#include <zbluenet/net/net_thread.h>
#include <zbluenet/net/tcp_socket.h>
#include <zbluenet/net/message_queue.h>
#include <zbluenet/protocol/net_command.h>

namespace zbluenet {

	using net::NetThread;
	using net::Reactor;
	using net::TcpSocket;
	using protocol::NetCommand;
	using net::MessageQueue;

	namespace exchange {
		class BaseStruct;
	}
	using exchange::BaseStruct;

	namespace client {

		class  NetClientThread : public NetThread {
		public:
			using SocketEventCallback = std::function<void (TcpSocket::SocketId)>;
			using CreateMessageFunc = std::function<BaseStruct * (int)>; // 创建消息实体的接口
			using NetCommandQueue = MessageQueue<NetCommand *>; // 发送消息的队列

			NetClientThread(int max_recv_packet_lenth, int max_send_packet_length, const CreateMessageFunc &create_message_func);
			~NetClientThread();

			void setSocketId(TcpSocket::SocketId socket_id) { remote_socket_id_ = socket_id; }


			void setPeerCloseCallback(const SocketEventCallback &peer_close_cb);
			void setErrorCallback(const SocketEventCallback &error_cb);

			void push(NetCommand *cmd);
			 
			virtual void onNetCommand(NetCommandQueue *queue = nullptr);
			virtual void onPeerClose(Reactor *reactor, TcpSocket::SocketId socket_id);
			virtual void onError(Reactor *reactor, TcpSocket::SocketId socket_id, int error);

		private:
			TcpSocket::SocketId remote_socket_id_;
			SocketEventCallback peer_close_cb_;
			SocketEventCallback error_cb_;
		};

	} // namespace client
} // namespace zbluenet

#endif // ZBLEUNET_CLIENT_NET_CLIENT_THREAD_H
