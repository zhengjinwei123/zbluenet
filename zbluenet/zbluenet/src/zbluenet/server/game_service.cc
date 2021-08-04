#include <zbluenet/server/game_service.h>
#include <zbluenet/logger_base.h>
#include <zbluenet/server/tcp_service.h>

#include <functional>

namespace zbluenet {
	namespace server {

		GameService::GameService(const std::string &host, uint16_t port, uint8_t reactor_num) :
			TcpService(host, port, reactor_num)
		{

		}

		GameService::~GameService()
		{
		
		}

		bool GameService::initLogger(const std::string &file_path_name,
			bool open_console_log,
			zbluenet::LogLevel::type log_level,
			zbluenet::LogManager::LoggerType::type logger_type)
		{
			return zbluenet::LogManager::getInstance()->initLogger(file_path_name,
				open_console_log, log_level, logger_type);
		}

		void GameService::onConnect(const NetId &net_id)
		{
			TcpService::onConnect(net_id);
			if (on_connect_cb_) {
				on_connect_cb_(net_id);
			}
		}

		void GameService::onDisconnect(const NetId &net_id)
		{
			TcpService::onDisconnect(net_id);
			if (on_disconnect_cb_) {
				on_disconnect_cb_(net_id);
			}
		}

		void GameService::onExceedRequestFrequencyLimit(const NetId &net_id)
		{
			if (on_exceed_request_cb_) {
				on_exceed_request_cb_(net_id);
			}
		}

	} // namespace server
} // namespace zbluenet