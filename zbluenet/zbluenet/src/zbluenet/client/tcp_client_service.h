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
#include <zbluenet/net/socket_address.h>

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
			using MessageHandler = std::function<void(const zbluenet::exchange::BaseStruct *)>;
			using MessageHandlerMap = std::unordered_map<int, MessageHandler>;


			TcpClientService(const std::string &client_name, const std::string &host, uint16_t port,
				const CreateMessageFunc &create_message_func, int reconnect_interval_ms = 5000);
			~TcpClientService();

			bool createClient(const NewNetCommandCallback &new_net_cmd_cb);
			void sendMessage(int message_id, std::unique_ptr<zbluenet::exchange::BaseStruct> &message);
			void setMessageHandler(int message_id, const MessageHandler &message_handler);
			void processNetCommand(const NetCommand *cmd);

			const std::string& getClientName() const { return client_name_;  }


		private:
			MessageHandler getMessageHandler(int message_id) const;
			void push(std::unique_ptr<NetCommand> &cmd);
			void onConnect();
			void onDisconnect();//66063558
			
		private:
			SocketAddress remote_addr_;
			std::string client_name_;

			NetClientThread net_thread_;
			CreateMessageFunc create_message_func_;
			MessageHandlerMap message_handlers_;
		};
	} // namespace client
} // namespace zbluenet

#endif // ZBLEUNET_CLIENT_TCP_CLINET_SERVICE_H
