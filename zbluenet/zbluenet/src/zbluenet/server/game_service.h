#ifndef ZBLEUNET_SERVER_SERVICE_H
#define ZBLEUNET_SERVER_SERVICE_H

#include  <zbluenet/net/tcp_socket.h>
#include <zbluenet/log.h>
#include <zbluenet/class_util.h>
#include <zbluenet/server/tcp_service.h>

#include <memory>
#include <string>
#include <cstddef>


namespace zbluenet {

	class DynamicBuffer;

	namespace net {
		class NetThread;
		class TcpSocket;
	}

	namespace protocol {
		class NetCommand;
	}

	namespace exchange {
		class BaseStruct;
	}

	using net::NetThread;
	using net::TcpSocket;
	using protocol::NetCommand;

	namespace server {

		class GameService : public TcpService {
		public:
			using ConnectEventCallback = std::function<void(const NetId &)>;
			using DisconectEventCallback = std::function<void(const NetId &)>;
			using ExceedRequestEventCallback = std::function<void(const NetId &)>;

			using NewNetCommandCallback = std::function<void(std::unique_ptr<NetCommand> &)>;

			GameService(const std::string &host, uint16_t port, uint8_t reactor_num);
			virtual ~GameService();


			template <typename AnyType>
			void setMessageHandler(int message_id, const std::function<void (const NetId &, const AnyType *)> &message_handler)
			{
				static MessageHandlerWrapper<AnyType> wrapper;
				if (wrapper.f) {
					LOG_ERROR("c2s_message(%d) handler is already set", message_id);
					return;
				}
				wrapper.f = message_handler;
				TcpService::setMessageHandler(message_id, std::bind(
					&MessageHandlerWrapper<AnyType>::call, &wrapper, std::placeholders::_1, std::placeholders::_2));
			}

			template<typename AnyType>
			void sendMessage(int message_id, const zbluenet::net::NetId &net_id, std::unique_ptr <AnyType> &messsage)
			{
				std::unique_ptr<zbluenet::exchange::BaseStruct> wrap_message(messsage.release());
				TcpService::sendMessage(net_id, message_id, wrap_message);
			}

			template<typename AnyType>
			void broadcastMessage(int message_id, std::unique_ptr <AnyType> &messsage)
			{
				std::unique_ptr<zbluenet::exchange::BaseStruct> wrap_message(messsage.release());
				TcpService::broadcastMessage(message_id, wrap_message);
			}

			virtual void onConnect(const NetId &net_id);
			virtual void onDisconnect(const NetId &net_id);
			virtual void onExceedRequestFrequencyLimit(const NetId &net_id);

			void setConnectCallback(const ConnectEventCallback &cb) { on_connect_cb_ = cb; }
			void setDisconnectCallback(const DisconectEventCallback &cb) { on_disconnect_cb_ = cb; }
			void setExceedRequestFrequencyLimitCallback(const ExceedRequestEventCallback &cb) { on_exceed_request_cb_ = cb; }

			bool initLogger(const std::string &file_path_name,
				bool open_console_log,
				zbluenet::LogLevel::type log_level,
				zbluenet::LogManager::LoggerType::type logger_type);

		private:
			ConnectEventCallback on_connect_cb_;
			DisconectEventCallback on_disconnect_cb_;
			ExceedRequestEventCallback on_exceed_request_cb_;
		};

	} // namespace server
} // namespace zbluenet

#endif // ZBLEUNET_SERVER_SERVICE_H
