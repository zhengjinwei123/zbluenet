#ifndef ZBLEUNET_CLIENT_TCP_CLIENT_H
#define ZBLEUNET_CLIENT_TCP_CLIENT_H

#include <string>
#include <stdint.h>
#include <memory>
#include <unordered_map>

#include <zbluenet/client/tcp_client_service.h>
#include <zbluenet/net/message_queue.h>
#include <zbluenet/exchange/base_struct.h>
#include <zbluenet/log.h>
#include <zbluenet/server/game_server.h>



namespace zbluenet {

	namespace exchange {
		class BaseStruct;
	}

	using exchange::BaseStruct;

	namespace net {
		class NetId;
	}

	using net::NetId;

	namespace protocol {
		class NetCommand;
	}
	using protocol::NetCommand;
	using net::MessageQueue;

	namespace client {

		class TcpClient  {
		public:
			using NetCommandQueue = MessageQueue<NetCommand *>;

			TcpClient(const std::string &client_name, const std::string &host, uint16_t port, int reconnect_interval_ms = 5000);
			~TcpClient();

			void onNetCommand(std::unique_ptr<NetCommand> &cmd);

			bool start(zbluenet::server::GameServer *game_server);

			template <typename AnyType>
			void registerMessageHandlerFunc(int id, const std::function<void(const zbluenet::net::NetId &, const AnyType *)> &message_handler)
			{
				registerMessageCreateFunc(id, &AnyType::create);
				setMessageHandler<AnyType>(id, message_handler);
			}

			void sendMessage(int message_id, std::unique_ptr<zbluenet::exchange::BaseStruct> &message);

		private:
			bool registerMessageCreateFunc(int id, zbluenet::exchange::BaseStruct::CreateFunc create_func);
			zbluenet::exchange::BaseStruct *createMessageMap(int id);

			template <typename AnyType>
			void setMessageHandler(int message_id, const std::function<void(const NetId &, const AnyType *)> &message_handler)
			{
				static MessageHandlerWrapper<AnyType> wrapper;
				if (wrapper.f) {
					LOG_ERROR("TcpClient::setMessageHandler(%d) handler is already set", message_id);
					return;
				}
				wrapper.f = message_handler;

				pservice_->setMessageHandler(message_id, std::bind(
					&MessageHandlerWrapper<AnyType>::call, &wrapper, std::placeholders::_1, std::placeholders::_2));
			}

			void onClientNetCommandQueueRead(NetCommandQueue *queue);

		private:
			template <typename AnyType>
			struct MessageHandlerWrapper {
				void call(const NetId &net_id, const BaseStruct *message)
				{
					this->f(net_id, (AnyType *)message);
				}

				std::function<void(const NetId &, const AnyType *)> f;
			};

		private:
			std::unique_ptr<TcpClientService> pservice_;


			NetCommandQueue client_net_cmd_queue_;;
			std::unordered_map<int, zbluenet::exchange::BaseStruct::CreateFunc> message_create_func_map_;
		};

	} // namespace net
} // namespace zbluenet

#endif
