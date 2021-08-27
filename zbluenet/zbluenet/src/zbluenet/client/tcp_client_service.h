#ifndef ZBLEUNET_CLIENT_TCP_CLINET_SERVICE_H
#define ZBLEUNET_CLIENT_TCP_CLINET_SERVICE_H

#include <unordered_map>
#include <string>
#include <stdint.h>


#include <zbluenet/net/socket_address.h>
#include <zbluenet/net/tcp_socket.h>
#include <zbluenet/net/message_queue.h>
#include <zbluenet/net/epoll_service.h>
#include <zbluenet/client/net_client_thread.h>
#include <zbluenet/protocol/net_protocol.h>

namespace zbluenet {

	using net::TcpSocket;
	using net::SocketAddress;
	class DynamicBuffer;

	namespace exchange {
		class BaseStruct;
	}

	namespace net {
		class NetThread;
		class IOService;
		class NetId;
		class IODevice;
	}

	namespace protocol {
		class NetCommand;
		class NetProtocol;
	}

	using protocol::NetCommand;
	using net::IOService;
	using net::NetId;
	using net::IODevice;
	using net::MessageQueue;
	using net::NetThread;

	namespace client {

		class TcpClientService {
		public:
			using NewNetCommandCallback = std::function<void(std::unique_ptr<NetCommand> &)>;
			using CreateMessageFunc = std::function<zbluenet::exchange::BaseStruct *(int)>;
			using RecvMessageCallback = std::function< void(NetThread *, TcpSocket::SocketId, DynamicBuffer *, const NewNetCommandCallback &)>;

			using MessageHandler = std::function<void(const NetId &, const zbluenet::exchange::BaseStruct *)>;
			using MessageHandlerMap = std::unordered_map<int, MessageHandler>;
			using NetCommandQueue = MessageQueue<NetCommand *>; // 发送消息的队列

			TcpClientService(const std::string &host, uint16_t port,
				const CreateMessageFunc &create_message_func, int reconnect_interval_ms = 5000);
			~TcpClientService();

			bool createClient();
			bool init(const CreateMessageFunc &create_message_func, const NewNetCommandCallback &new_net_cmd_cb);
			void start();
			void stop();

			void sendMessage(const NetId &net_id, int message_id, std::unique_ptr<zbluenet::exchange::BaseStruct> &message);
			MessageHandler getMessageHandler(int message_id) const;
			void setMessageHandler(int message_id, const MessageHandler &message_handler);
			void processNetCommand(const NetCommand *cmd);

			void onNetCommand(NetCommandQueue *queue);

			bool connect();
			void disconnect();


			void startConnectTimer();
			void stopConnectTimer();
			void onTimer(int64_t timer_id);

			void onRecvMessage(NetThread *net_thread,
				TcpSocket::SocketId socket_id,
				DynamicBuffer *buffer,
				const NewNetCommandCallback &new_net_cmd_cb);

			void push(std::unique_ptr<NetCommand> &cmd);

		private:
			bool connected_;
			TcpSocket::SocketId socket_id_;
			int64_t reconnect_timer_;
			int reconnect_interval_ms_;
			SocketAddress remote_addr_;
			TcpSocket tcp_socket_;
			NetClientThread net_thread_;
			NetProtocol net_protocol_;

			CreateMessageFunc create_message_func_;
			MessageHandlerMap message_handlers_;
			NetCommandQueue client_net_command_queue_;
			NewNetCommandCallback new_net_cmd_cb_;
		};
	} // namespace client
} // namespace zbluenet

#endif // ZBLEUNET_CLIENT_TCP_CLINET_SERVICE_H
