#include <zbluenet/client/tcp_client_service.h>

namespace zbluenet {
	namespace client {
		TcpClientService::TcpClientService(const std::string &host, uint16_t port, const CreateMessageFunc &create_message_func, int reconnect_interval_ms):
			connected_(false),
			socket_id_(-1),
			reconnect_timer_(-1),
			remote_addr_(host, port),
			reconnect_interval_ms_(reconnect_interval_ms),
			net_thread_(40960, 40960, create_message_func),
			net_protocol_(40960, create_message_func)
		{

		}

		TcpClientService::~TcpClientService()
		{

		}

		bool TcpClientService::createClient()
		{
		
		}

		bool TcpClientService::init(const CreateMessageFunc &create_message_func,
												  const NewNetCommandCallback &new_net_cmd_cb)
		{
			if (false == net_thread_.init(0, 0, 81920, 409600, new_net_cmd_cb,
				std::bind(&TcpClientService::onRecvMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)))
			{
				return false;
			}

			new_net_cmd_cb_ = new_net_cmd_cb;

			return true;
		}

		void TcpClientService::start()
		{
			TcpClientService *that = this;
	
			net_thread_.start([that](Thread *p) -> void {
				that->connect();
			}, [that](Thread *p) -> void {
				that->stop();
			});

			net_thread_.setPeerCloseCallback([that](TcpSocket::SocketId socket_id) -> void {
				that->disconnect();
				that->startConnectTimer();
			});

			net_thread_.setErrorCallback([that](TcpSocket::SocketId socket_id) -> void {
				that->disconnect();
				that->startConnectTimer();
			});
		}

		void TcpClientService::stop()
		{
			disconnect();
			stopConnectTimer();
		}

		void TcpClientService::sendMessage(const NetId &net_id, int message_id, std::unique_ptr<zbluenet::exchange::BaseStruct> &message)
		{

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

		void TcpClientService::processNetCommand(const NetCommand *cmd)
		{

		}

		void TcpClientService::onNetCommand(NetCommandQueue *queue)
		{

		}

		bool TcpClientService::connect()
		{
			if (false == tcp_socket_.activeOpenNonblock(remote_addr_)) {
				startConnectTimer();
				return false;
			}
			socket_id_ = tcp_socket_.getId();
			connected_ = true;

			std::unique_ptr<TcpSocket> socket(new TcpSocket());
			net_thread_.attach(socket);
			return true;
		}

		void TcpClientService::disconnect()
		{
			if (connected_) {
				net_thread_.closeSocket(socket_id_);
				connected_ = false;
				socket_id_ = -1;
			}
		}

		void TcpClientService::startConnectTimer()
		{
			stopConnectTimer();
			reconnect_timer_ = net_thread_.startTimer(reconnect_interval_ms_, std::bind(&TcpClientService::onTimer, this, std::placeholders::_1), 1);
		}
		void TcpClientService::stopConnectTimer()
		{
			if (reconnect_timer_ != -1) {
				net_thread_.stopTimer(reconnect_timer_);
			}
		}

		void TcpClientService::onTimer(int64_t timer_id)
		{
			if (timer_id == reconnect_timer_) {
				reconnect_timer_ = -1;
				connect();
			}
		}

		void TcpClientService::onRecvMessage(NetThread *pthread,
			TcpSocket::SocketId socket_id,
			DynamicBuffer *buffer,
			const NewNetCommandCallback &new_net_cmd_cb)
		{
			for (;;) {
				int message_id = 0;
				std::unique_ptr<zbluenet::exchange::BaseStruct> message;
				NetProtocol::RetCode::type ret =
					net_protocol_.recvMessage(buffer, &message_id, message);
				if (NetProtocol::RetCode::WAITING_MORE_DATA == ret) {
					return;
				}
				else if (NetProtocol::RetCode::ERR == ret) {
					disconnect();
					startConnectTimer();
					return;
				}
				else if (NetProtocol::RetCode::MESSAGE_READY == ret) {
					std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::MESSAGE));
					cmd->id.reactor_id = pthread->getId();
					cmd->id.socket_id = socket_id;
					cmd->message_id = message_id;
					cmd->message = message.release();
					new_net_cmd_cb_(cmd);
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