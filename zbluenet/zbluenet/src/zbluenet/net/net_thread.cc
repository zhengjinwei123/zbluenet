#include <zbluenet/net/net_thread.h>
#include <zbluenet/net/select_reactor.h>
#include <zbluenet/log.h>
#include <zbluenet/net/epoll_reactor.h>

#include <functional>
#include <memory>

namespace zbluenet {
	namespace net {

		NetThread::NetThread(int max_recv_packet_lenth, int max_send_packet_length, const CreateMessageFunc &create_message_func) :
			id_(-1),
			reactor_(nullptr),
			command_queue_(0),
			net_protocol_(max_recv_packet_lenth, create_message_func),
			encode_buffer_(max_send_packet_length),
			new_net_cmd_cb_(nullptr)
		{

		}

		NetThread::~NetThread()
		{
			if (reactor_ != nullptr) {
				delete reactor_;
			}
			thread_.close();
		}

		bool NetThread::init(int id,
			int max_client_num,
			int max_recv_buffer_size,
			int max_send_buffer_size,
			const NewNetCommandCallback &new_net_cmd_cb,
			const RecvMessageCallback &recv_message_cb)
		{
#ifdef _WIN32
			reactor_ = new SelectReactor(max_client_num);
#else
			reactor_ = new EpollReactor(max_client_num);
#endif

			reactor_->setRecvBufferMaxSize(max_recv_buffer_size);
			reactor_->setSendBufferMaxSize(max_send_buffer_size);
			reactor_->setRecvBufferExpandSize(1024);
			reactor_->setSendBufferExpandSize(1024);
			reactor_->setRecvBufferInitSize(1024);
			reactor_->setSendBufferInitSize(1024);

			// 解码前的消息回调
			reactor_->setRecvMessageCallback(std::bind(&NetThread::onRecvMessage, this, std::placeholders::_1,
				std::placeholders::_2, std::placeholders::_3));
			// 关闭回调
			reactor_->setPeerCloseCallback(std::bind(&NetThread::onPeerClose, this, std::placeholders::_1, std::placeholders::_2));
			// 出错回调
			reactor_->setErrorCallback(std::bind(&NetThread::onError, this, std::placeholders::_1,
				std::placeholders::_2, std::placeholders::_3));
			// 解码后的消息回调
			reactor_->setNewNetCommandCallback(new_net_cmd_cb);

#ifdef _WIN32
			// windows 下手动发送消息
			reactor_->setWriteMessageCallback([=]()-> void {
				this->onNetCommand();
			});
#else
			// linux 下 epoll 会监控写事件
			if (false == command_queue_.attach(*reactor_)) {
				return false;
			}

			command_queue_.setRecvMessageCallback([=](NetCommandQueue *queue) -> void {
				this->onNetCommand(queue);
			});
#endif

			// 线程id
			id_ = id;
			//  接收消息的回调函数, 主线程传递进来的
			new_net_cmd_cb_ = new_net_cmd_cb;

			recv_message_cb_ = recv_message_cb;

			return true;
		}

		void NetThread::start(const Thread::EventCallback &onBefore, const Thread::EventCallback &onEnd)
		{
			NetThread *that = this;
			thread_.start(onBefore, [that](Thread *pthread) -> void {
				if (that->reactor_ != nullptr) {
					that->reactor_->loop();
				}
			}, onEnd);

		

		}

		void NetThread::stop()
		{
			thread_.close();
		}

		void NetThread::push(NetCommand *cmd) // 接收来自主线程发送来的消息
		{
			command_queue_.push(cmd);
		}

		void NetThread::attach(std::unique_ptr<TcpSocket> &peer_socket) // 新连接到来
		{
			TcpSocket::SocketId socket_id = peer_socket->getId();
			reactor_->attachSocket(peer_socket);
			this->sendNetCommandNew(socket_id);
		}

		void NetThread::closeSocket(TcpSocket::SocketId socket_id)
		{
			sendNetCommandClose(socket_id);
			reactor_->closeSocket(socket_id);
		}

		void NetThread::sendNetCommandClose(TcpSocket::SocketId socket_id)
		{
			std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::CLOSE));
			cmd->id.reactor_id = id_;
			cmd->id.socket_id = socket_id;
			new_net_cmd_cb_(cmd);
		}

		void NetThread::sendNetCommandNew(TcpSocket::SocketId socket_id)
		{
			std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::NEW));
			cmd->id.reactor_id = id_;
			cmd->id.socket_id = socket_id;
			new_net_cmd_cb_(cmd);
		}

		// 消息到了
		void NetThread::onRecvMessage(Reactor *reactor, TcpSocket::SocketId socket_id, DynamicBuffer *buffer)
		{
			if (recv_message_cb_) {
				recv_message_cb_(this, socket_id, buffer, new_net_cmd_cb_);
			}
		}

		// 对方关闭
		void NetThread::onPeerClose(Reactor *reactor, TcpSocket::SocketId socket_id)
		{
			LOG_MESSAGE_DEBUG("net_thread close peer, net_id(%d: %lx)", id_, socket_id);
			closeSocket(socket_id);
		}

		// 出错了
		void NetThread::onError(Reactor *reactor, TcpSocket::SocketId socket_id, int error)
		{
			LOG_MESSAGE_ERROR("net_thread error  net_id (%d:%lx), error_code=(%d), error=(%s)", id_, socket_id, error, strerror(error));

			closeSocket(socket_id);
		}

		IOService::TimerId NetThread::startTimer(int64_t timeout_ms, const IOService::TimerCallback &timer_cb, int call_times)
		{
			return reactor_->startTimer(timeout_ms, timer_cb, call_times);
		}

		void NetThread::stopTimer(IOService::TimerId timer_id)
		{
			reactor_->stopTimer(timer_id);
		}

		void NetThread::quit()
		{
			reactor_->quit();
		}
		// 需要发送的消息， 在线程的循环中处理
		void NetThread::onNetCommand(NetCommandQueue *queue)
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
				// broadcast
				if (NetCommand::Type::BROADCAST == cmd->type) {
					if (net_protocol_.writeMessage(cmd->message_id, cmd->message, &encode_buffer_) == false) {
						LOG_MESSAGE_ERROR("NetThread::onNetCommand encode broadcast message failed on net_thread(%d)", id_);
						continue;
					}

					for (auto iter = broadcast_list_.begin(); iter != broadcast_list_.end(); ++iter) {
						reactor_->sendMessage(*iter, encode_buffer_.readBegin(), encode_buffer_.readableBytes());
					}

					continue;
				}

				if (reactor_->isConnected(cmd->id.socket_id) == false) {
					continue;
				}

				if (NetCommand::Type::CLOSE == cmd->type) {
					reactor_->closeSocket(cmd->id.socket_id);
					continue;
				}  else if (NetCommand::Type::MESSAGE == cmd->type) {
					if (net_protocol_.writeMessage(cmd->message_id, cmd->message, &encode_buffer_) == false) {
						LOG_MESSAGE_ERROR("encode message(%d) failed on net_id(%d:%lx)", cmd->message_id, id_, cmd->id.socket_id);
						closeSocket(cmd->id.socket_id);
						continue;
					}
					reactor_->sendMessage(cmd->id.socket_id, encode_buffer_.readBegin(), encode_buffer_.readableBytes());
				} else if (NetCommand::Type::ENABLE_BROADCAST == cmd->type) {
					broadcast_list_.insert(cmd->id.socket_id);
				} else if (NetCommand::Type::DISABLE_BROADCAST== cmd->type) {
					broadcast_list_.erase(cmd->id.socket_id);
				}
			}
		}

	} // namespace net
} // namespace zbluenet