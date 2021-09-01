#include <zbluenet/client/tcp_client.h>
#include <zbluenet/timestamp.h>
#include <zbluenet/log.h>

namespace zbluenet {
	namespace client {

		TcpClient::TcpClient(const std::string &client_name, const std::string &host, uint16_t port, int reconnect_interval_ms)
		{
			pservice_.reset(new TcpClientService(client_name,
																		host,
																		port, 
																		std::bind(&TcpClient::createMessageMap, this, std::placeholders::_1),
																		reconnect_interval_ms));
		}

		TcpClient::~TcpClient()
		{

		}

		void TcpClient::onNetCommand(std::unique_ptr<NetCommand> &cmd)
		{
			client_net_cmd_queue_.push(cmd.get());
			cmd.release();
		}

		bool TcpClient::start(zbluenet::server::GameServer *game_server)
		{
			if (false == pservice_->createClient(std::bind(&TcpClient::onNetCommand, this, std::placeholders::_1))) {
				return false;
			}

#ifdef _WIN32 
			game_server->addEventFunc([=](zbluenet::Timestamp &now) -> void {
				this->onClientNetCommandQueueRead(nullptr);
			});
#else
			client_net_cmd_queue_.setRecvMessageCallback([=](NetCommandQueue *queue) -> void {
				this->onClientNetCommandQueueRead(queue);
			});
			game_server->attachNetCommandQueue(&client_net_cmd_queue_);
#endif

			return true;
		}

		void TcpClient::onClientNetCommandQueueRead(NetCommandQueue *queue)
		{
			NetCommand *cmd_raw = nullptr;

			size_t count = 0;

#ifdef _WIN32
			while (client_net_cmd_queue_.pop(cmd_raw)) {
#else
			while (queue->pop(cmd_raw)) {
#endif
				pservice_->processNetCommand(cmd_raw);
				if (++count >= 100) {
					return;
				}
			}
		}

		zbluenet::exchange::BaseStruct *TcpClient::createMessageMap(int id)
		{
			auto iter = message_create_func_map_.find(id);
			if (iter == message_create_func_map_.end()) {
				return nullptr;
			}

			return (iter->second)();
		}

		bool TcpClient::registerMessageCreateFunc(int id, zbluenet::exchange::BaseStruct::CreateFunc create_func)
		{
			if (message_create_func_map_.find(id) != message_create_func_map_.end()) {
				return false;
			}
			message_create_func_map_[id] = create_func;
			return true;
		}

		void TcpClient::sendMessage(int message_id, std::unique_ptr<zbluenet::exchange::BaseStruct> &message)
		{
			pservice_->sendMessage(message_id, message);
		}

	} // namespace client
} // namespace zbluenet