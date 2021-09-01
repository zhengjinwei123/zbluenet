#include <zbluenet/client/tcp_client_service.h>
#include <zbluenet/log.h>

namespace zbluenet {
	namespace client {
		TcpClientService::TcpClientService(const std::string &client_name, 
																const std::string &host,
																uint16_t port,
																const CreateMessageFunc &create_message_func,
																int reconnect_interval_ms) :
			remote_addr_(host, port),
			client_name_(client_name),
			net_thread_(40960, 40960, create_message_func, reconnect_interval_ms, host, port)
		{

		}

		TcpClientService::~TcpClientService()
		{
			net_thread_.stop();
		}

		bool TcpClientService::createClient(const NewNetCommandCallback &new_net_cmd_cb)
		{
			if (false == net_thread_.init(0, 0, 81920, 409600, new_net_cmd_cb,
				std::bind(&NetClientThread::onRecvMessage, &net_thread_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)))
			{
				LOG_ERROR("tcp client create failed (%s) (%s:%d)", client_name_.c_str(), remote_addr_.getIp().c_str(), remote_addr_.getPort());
				return false;
			}

			net_thread_.startRun();

			return true;
		}

		void TcpClientService::sendMessage(int message_id,
			std::unique_ptr<zbluenet::exchange::BaseStruct> &message)
		{
			std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::MESSAGE));
			cmd->message_id = message_id;
			cmd->message = message.release();
			push(cmd);
		}

		TcpClientService::MessageHandler TcpClientService::getMessageHandler(int message_id) const
		{
			MessageHandlerMap::const_iterator iter = message_handlers_.find(message_id);
			if (iter == message_handlers_.end()) {
				return nullptr;
			}
			return iter->second;
		}

		void TcpClientService::setMessageHandler(int message_id, const MessageHandler &message_handler)
		{
			message_handlers_[message_id] = message_handler;
		}

		void TcpClientService::onConnect()
		{
			LOG_DEBUG("tcp client connect success, client_name:(%s)  remote_addr:[%s:%d]", client_name_.c_str(), remote_addr_.getIp().c_str(), remote_addr_.getPort());
		}

		void TcpClientService::onDisconnect()
		{
			LOG_DEBUG("tcp client disconnect, client_name:(%s)  remote_addr:[%s:%d]", client_name_.c_str(), remote_addr_.getIp().c_str(), remote_addr_.getPort());
		}

		void TcpClientService::processNetCommand(const NetCommand *cmd)
		{
			if (NetCommand::Type::NEW == cmd->type) {
				onConnect();
			} else if (NetCommand::Type::CLOSE == cmd->type) {
				onDisconnect();
			} else if (NetCommand::Type::MESSAGE == cmd->type) {
				MessageHandler handlerFunc = getMessageHandler(cmd->message_id);
				if (nullptr != handlerFunc) {
					handlerFunc(cmd->message);
				} else {
					LOG_WARNING("TcpClientService::processNetCommand not found message_id(%d) handle",
						cmd->message_id);
				}
			}
		}

		void TcpClientService::push(std::unique_ptr<NetCommand> &cmd)
		{
			net_thread_.push(cmd.get());
			cmd.release();
		}

	} // namespace client

}