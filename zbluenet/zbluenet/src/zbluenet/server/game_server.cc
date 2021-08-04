#include <zbluenet/server/game_server.h>

#include <zbluenet/exchange/base_struct.h>
#include <zbluenet/protocol/net_protocol.h>
#include <zbluenet/net/net_thread.h>
#include <zbluenet/net/net_id.h>
#include <zbluenet/log.h>

#include <algorithm>

namespace zbluenet {

	using protocol::NetProtocol;
	using net::NetId;

	namespace server {

		GameServer::GameServer(const std::string &host, uint16_t port, uint8_t reactor_num) :
			main_log_inited_(false),
			net_log_inited_(false),
			net_protocol_(40960, std::bind(&GameServer::createMessageMap, this, std::placeholders::_1)),
			pGameService_(new GameService(host, port, reactor_num))
		{
			
		}

		GameServer::~GameServer()
		{

		}

		bool GameServer::check()
		{
			return main_log_inited_ && net_log_inited_;
		}
		// 初始化主日志
		bool GameServer::initMainLogger(const std::string &main_log_filename,
			zbluenet::LogLevel::type log_level, bool open_console_log)
		{
			if (false == pGameService_->initLogger(main_log_filename,
				open_console_log, log_level, zbluenet::LogManager::LoggerType::MAIN)) {
				return false;
			}
			main_log_inited_ = true;
			return true;
		}

		// 初始化网络日志
		bool GameServer::initNetLogger(const std::string &net_log_filename,
			zbluenet::LogLevel::type log_level, bool open_console_log)
		{
			if (false == pGameService_->initLogger(net_log_filename,
				open_console_log, log_level, zbluenet::LogManager::LoggerType::NET)) {
				return false;
			}
			net_log_inited_ = true;
			return true;
		}

		// 获取消息，解码
		void GameServer::onRecvMessage(NetThread *pthread, TcpSocket::SocketId socket_id,
			DynamicBuffer *buffer, const NewNetCommandCallback &new_net_cmd)
		{
			for (;;) {
				int message_id = 0;
				std::unique_ptr<zbluenet::exchange::BaseStruct> message;
				NetProtocol::RetCode::type ret =
					net_protocol_.recvMessage(buffer, &message_id, message);
				if (NetProtocol::RetCode::WAITING_MORE_DATA == ret) {
					return;
				} else if (NetProtocol::RetCode::ERR == ret) {
					pthread->closeSocket(socket_id);
					return;
				} else if (NetProtocol::RetCode::MESSAGE_READY == ret) {
					std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::MESSAGE));
					cmd->id.reactor_id = pthread->getId();
					cmd->id.socket_id = socket_id;
					cmd->message_id = message_id;
					cmd->message = message.release();
					new_net_cmd(cmd);
				}
			}
		}

		bool GameServer::registerMessageCreateFunc(int id, zbluenet::exchange::BaseStruct::CreateFunc create_func)
		{
			if (message_create_func_map_.find(id) != message_create_func_map_.end()) {
				return false;
			}
			message_create_func_map_[id] = create_func;
			return true;
		}

		// 创建消息映射
		zbluenet::exchange::BaseStruct *GameServer::createMessageMap(int id)
		{
			auto iter = message_create_func_map_.find(id);
			if (iter == message_create_func_map_.end()) {
				return nullptr;
			}

			return (iter->second)();
		}


		bool GameServer::init(const ConnectEventCallback &on_conn_cb,
			const DisconectEventCallback &on_disconnect_cb,
			const ExceedRequestEventCallback &on_exceed_request_frequency_limit_cb, int max_conn_num) {
			if (this->check() == false) {
				LOG_ERROR("GameServer::start check fail, please init logger first");
				return false;
			}

			if (false == pGameService_->createService(max_conn_num)) {
				LOG_ERROR("createService fail");
				return false;
			}

			if (false == pGameService_->init(
				std::bind(&GameServer::createMessageMap, this, std::placeholders::_1),
				std::bind(&GameServer::onRecvMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4))) {
				LOG_ERROR("service init fail");
				return false;
			}

			pGameService_->setConnectCallback(on_conn_cb);
			pGameService_->setDisconnectCallback(on_disconnect_cb);
			pGameService_->setExceedRequestFrequencyLimitCallback(on_exceed_request_frequency_limit_cb);

			return true;
		}

		// 启动服务
		void GameServer::start()
		{
			pGameService_->start();
		}

	} // namespace server
} // namespace zbluenet