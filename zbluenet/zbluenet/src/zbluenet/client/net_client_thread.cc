#include <zbluenet/client/net_client_thread.h>
#include <zbluenet/log.h>
#include <zbluenet/exchange/base_struct.h>


namespace zbluenet {
	namespace client {

		NetClientThread::NetClientThread(int max_recv_packet_lenth, int max_send_packet_length,
			const NetClientThread::CreateMessageFunc &create_message_func, int reconnect_interval_ms, std::string ip, uint16_t port) :
			NetThread(max_recv_packet_lenth, max_send_packet_length, create_message_func),
			connected_(false),
			reconnect_interval_ms_(reconnect_interval_ms),
			socket_id_(-1),
			remote_addr_(ip, port),
			reconnect_timer_(-1),
			net_protocol_(40960, create_message_func)
		{

		}

		NetClientThread::~NetClientThread()
		{
		}
		
		void NetClientThread::startRun()
		{
			NetClientThread *that = this;
			this->start([that](Thread *pthread) -> void {
				that->connect();
			}, [that](Thread *pthread) -> void {
				that->disconnect();
			});
		}

		void NetClientThread::stop()
		{
			this->stopConnectTimer();
			this->disconnect();
			NetThread::stop();
		}

		void NetClientThread::push(NetCommand *cmd)
		{
			command_queue_.push(cmd);
		}

		bool NetClientThread::connect()
		{
			std::unique_ptr<TcpSocket> tcp_socket(new TcpSocket());
			if (false == tcp_socket->activeOpenNonblock(remote_addr_)) {
				LOG_INFO("tcp client connect failed (%s:%d), try reconnect after %d millisecond", remote_addr_.getIp().c_str(), remote_addr_.getPort(), reconnect_interval_ms_);
				startConnectTimer();
				return false;
			}

			// ·ÖÅäsocketid
			socket_id_ = socket_id_allocator_.nextId(tcp_socket->getFD());
			tcp_socket->setId(socket_id_);
			connected_ = true;

			this->attach(tcp_socket);

		/*	LOG_INFO("tcp client connect success (%s:%d)", remote_addr_.getIp().c_str(), remote_addr_.getPort());*/

			return true;
		}

		void NetClientThread::disconnect()
		{
			if (connected_) {
				this->closeSocket(socket_id_);
				connected_ = false;
				socket_id_ = -1;
			}
		}

		void NetClientThread::startConnectTimer()
		{
			stopConnectTimer();
			reconnect_timer_ = this->startTimer(reconnect_interval_ms_, std::bind(&NetClientThread::onTimer, this, std::placeholders::_1), 1);
		}

		void NetClientThread::stopConnectTimer()
		{
			if (reconnect_timer_  != -1) {
				this->stopTimer(reconnect_timer_);
			}
		}

		void NetClientThread::onTimer(int64_t timer_id)
		{
			if (timer_id == reconnect_timer_) {
				reconnect_timer_ = -1;
				LOG_INFO("tcp client reconnect to (%s:%d)", remote_addr_.getIp().c_str(), remote_addr_.getPort());
				connect();
			}
		}

		void NetClientThread::onRecvMessage(NetThread *net_thread,
			TcpSocket::SocketId socket_id,
			DynamicBuffer *buffer,
			const NewNetCommandCallback &new_net_cmd_cb)
		{
			for (;;) {
				int message_id = 0;
				std::unique_ptr<zbluenet::exchange::BaseStruct> message;
				NetProtocol::RetCode::type ret = net_protocol_.recvMessage(buffer, &message_id, message);
				if (NetProtocol::RetCode::WAITING_MORE_DATA == ret) {
					return;
				} else if (NetProtocol::RetCode::ERR == ret) {
					disconnect();
					startConnectTimer();
					return;
				} else if (NetProtocol::RetCode::MESSAGE_READY == ret) {
					std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::MESSAGE));
					cmd->id.reactor_id = this->getId();
					cmd->id.socket_id = socket_id_;
					cmd->message_id = message_id;
					cmd->message = message.release();
					new_net_cmd_cb_(cmd);
				}
			}
		}

		void NetClientThread::onClose()
		{
			this->disconnect();
			this->startConnectTimer();
		}

		void NetClientThread::onPeerClose(Reactor *reactor, TcpSocket::SocketId socket_id)
		{
			this->onClose();
		}

		void NetClientThread::onError(Reactor *reactor, TcpSocket::SocketId socket_id, int error)
		{
			this->onClose();
		}

		void NetClientThread::onNetCommand(NetCommandQueue *queue)
		{
			NetCommand *cmd_raw = nullptr;
#ifdef _WIN32
			while (command_queue_.pop(cmd_raw)) {
#else
			while (queue->pop(cmd_raw)) {
#endif
				if (nullptr == cmd_raw) {
					quit();
					return;
				}
				std::unique_ptr<NetCommand> cmd(cmd_raw);
				if (NetCommand::Type::MESSAGE == cmd->type) {
					if (net_protocol_.writeMessage(cmd->message_id, cmd->message, &encode_buffer_) == false) {
						LOG_MESSAGE_ERROR("encode message(%d) failed on net_id(%d:%lx)", cmd->message_id, id_, cmd->id.socket_id);
						
						this->onClose();
						return;
					}
					
					if (false == reactor_->sendMessage(socket_id_, encode_buffer_.readBegin(), encode_buffer_.readableBytes())) {
						this->onClose();
						return;
					}
				}
			}
		}
	} // namespace zbluenet
} // namespace zbluenet