#ifndef ZBLEUNET_SERVER_GAME_SERVER_H
#define ZBLEUNET_SERVER_GAME_SERVER_H

#include <zbluenet/server/game_service.h>
#include <zbluenet/protocol/net_protocol.h>
#include <zbluenet/exchange/base_struct.h>
#include <zbluenet/server/tcp_service.h>
#include <zbluenet/net/net_id.h>
#include <zbluenet/log.h>
#include <zbluenet/net/io_service.h>

#include <string>
#include <cstddef>
#include <unordered_map>
#include <functional>
#include <memory>

namespace zbluenet {
	namespace server {

		class GameServer  {
		public:
			using ConnectEventCallback = GameService::ConnectEventCallback;
			using DisconectEventCallback = GameService::DisconectEventCallback;
			using ExceedRequestEventCallback = GameService::ExceedRequestEventCallback;

			using NewNetCommandCallback = GameService::NewNetCommandCallback;

			GameServer(const std::string &host, uint16_t port, uint8_t reactor_num);
			~GameServer();

			void onRecvMessage(NetThread *pthread,
				TcpSocket::SocketId socket_id,
				DynamicBuffer *buffer,
				const NewNetCommandCallback &new_net_cmd_cb);

		    zbluenet::exchange::BaseStruct *createMessageMap(int id);

			bool init(const ConnectEventCallback &on_conn_cb,
				const DisconectEventCallback &on_disconnect_cb,
				const ExceedRequestEventCallback &on_exceed_request_frequency_limit_cb, int max_conn_num = 30000);

			void start();

			bool initMainLogger(const std::string &main_log_filename,
				zbluenet::LogLevel::type log_loglevel, bool open_console_log = false);
			bool initNetLogger(const std::string &net_log_filename,
				zbluenet::LogLevel::type log_level, bool open_console_log = false);

			
			template <typename AnyType>
			void registerMessageHandlerFunc(int id, const std::function<void (const zbluenet::net::NetId &, const AnyType *)> &message_handler)
			{
				if (pGameService_ != nullptr) {
					registerMessageCreateFunc(id, &AnyType::create);
					pGameService_->setMessageHandler<AnyType>(id, message_handler);
				}
			}

			template <typename AnyType>
			void sendMessage(const zbluenet::net::NetId &net_id, int message_id, std::unique_ptr <AnyType> &messsage)
			{
				if (pGameService_ != nullptr) {
					pGameService_->sendMessage<AnyType>(message_id, net_id, messsage);
				}
			}

			template<typename AnyType>
			void broadcastMessage(int message_id, std::unique_ptr <AnyType> &messsage)
			{
				if (pGameService_ != nullptr) {
					pGameService_->broadcastMessage<AnyType>(message_id, messsage);
				}
			}

			zbluenet::net::IOService::TimerId startTimer(int64_t timeout_ms, const zbluenet::net::IOService::TimerCallback &timer_cb, int call_times = -1)
			{
				return pGameService_->startTimer(timeout_ms, timer_cb, call_times);
			}

			void stopTimer(zbluenet::net::IOService::TimerId timer_id)
			{
				pGameService_->stopTimer(timer_id);
			}

			void attachNetCommandQueue(zbluenet::server::TcpService::NetCommandQueue *queue)
			{
				pGameService_->attachNetCommandQueue(queue);
			}

			void addEventFunc(const zbluenet::server::TcpService::EventFunc& event_func)
			{
				pGameService_->addEventFunc(event_func);
			}

		private:
			bool check();
			bool registerMessageCreateFunc(int id, zbluenet::exchange::BaseStruct::CreateFunc create_func);

		private:
			bool main_log_inited_;
			bool net_log_inited_;
			protocol::NetProtocol net_protocol_;
			std::unique_ptr<GameService> pGameService_;

			std::unordered_map<int, zbluenet::exchange::BaseStruct::CreateFunc> message_create_func_map_;
		};
	} // namespace server
} // namespace zbluenet
#endif // ZBLEUNET_SERVER_GAME_SERVER_H
