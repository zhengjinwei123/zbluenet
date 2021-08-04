#include <zbluenet/net/reactor.h>
#include <zbluenet/log.h>

#include <algorithm>

namespace zbluenet {
	namespace net {

		Reactor::Reactor(int max_connection_num) : quit_(false),
			id_(0),
			max_connection_num_(max_connection_num),
			conn_read_buffer_init_size_(0),
			conn_read_buffer_expand_size_(0),
			conn_read_buffer_max_size_(0),
			conn_write_buffer_init_size_(0),
			conn_write_buffer_expand_size_(0),
			conn_write_buffer_max_size_(0)
		{

		}

		Reactor::~Reactor() {
			for (TcpConnectionMap::iterator iter = connections_.begin();
				iter != connections_.end(); ++iter) {
				delete iter->second;
			}

			for (TcpSocketMap::iterator iter = sockets_.begin();
				iter != sockets_.end(); ++iter) {
				delete iter->second;
			}


			quit();
		}

		void Reactor::setRecvBufferInitSize(size_t size)
		{
			conn_read_buffer_init_size_ = size;
		}

		void Reactor::setRecvBufferExpandSize(size_t size)
		{
			conn_read_buffer_expand_size_ = size;
		}

		void Reactor::setRecvBufferMaxSize(size_t size)
		{
			conn_read_buffer_max_size_ = size;
		}

		////
		void Reactor::setSendBufferInitSize(size_t size)
		{
			conn_write_buffer_init_size_ = size;
		}

		void Reactor::setSendBufferExpandSize(size_t size)
		{
			conn_write_buffer_expand_size_ = size;
		}

		void Reactor::setSendBufferMaxSize(size_t size)
		{
			conn_write_buffer_max_size_ = size;
		}

		void Reactor::init(uint16_t id)
		{
			id_ = id;
		}

		// 解绑
		void Reactor::closeSocket(SocketId socket_id)
		{
			{
				TcpConnectionMap::iterator iter = connections_.find(socket_id);
				if (iter != connections_.end()) {
					delete iter->second;
					connections_.erase(iter);
				}
			}

			{
				TcpSocketMap::iterator iter = sockets_.find(socket_id);
				if (iter != sockets_.end()) {
					delete iter->second;
					sockets_.erase(iter);
				}
			}

		}

		bool Reactor::checkExists(SocketId socket_id)
		{
			return sockets_.find(socket_id) != sockets_.end();
		}

		// 绑定socket
		bool Reactor::attachSocket(std::unique_ptr<TcpSocket> &peer_socket)
		{
			if (sockets_.find(peer_socket->getId()) != sockets_.end()) {
				return false;
			}

			peer_socket->setReadCallback(std::bind(&Reactor::onSocketRead, this, std::placeholders::_1));
			peer_socket->setErrorCallback(std::bind(&Reactor::onSocketError, this, std::placeholders::_1));

			// create connection
			std::unique_ptr<TcpConnection> connection(new TcpConnection(peer_socket.get(),
				conn_read_buffer_init_size_, conn_read_buffer_expand_size_,
				conn_write_buffer_init_size_, conn_write_buffer_expand_size_));
			connection->setStatus(TcpConnection::Status::CONNECTED);

			auto socket_id = peer_socket->getId();
			sockets_.insert(std::make_pair(peer_socket->getId(), peer_socket.get()));
			peer_socket.release();

			connections_.insert(std::make_pair(socket_id, connection.get()));
			connection.release();

			return true;
		}

		void Reactor::start()
		{
			quit_ = false;
		}

		 // 消息来了
		 int Reactor::onSocketRead(IODevice *io_device)
		 {
			 TcpSocket *socket = static_cast<TcpSocket *>(io_device);
			 SocketId socket_id = socket->getId();

			 TcpConnectionMap::iterator iter = connections_.find(socket_id);
			 if (connections_.end() == iter) {
				 return -1;
			 }

			 TcpConnection *connection = iter->second;
			 DynamicBuffer &read_buffer = connection->getReadBuffer();

			 bool data_arrive = false;
			 bool peer_close = false;

			 for (;;) {

				 int readable_bytes = socket->readableBytes();
				 if (data_arrive && readable_bytes == 0) {
					  break;
				 }

				 int bytes_to_read = (std::max)(1, readable_bytes);

				 // 检查缓冲是否溢出
				 if (conn_read_buffer_max_size_ > 0 &&
					 bytes_to_read + read_buffer.readableBytes() > conn_read_buffer_max_size_) {
					 peer_close = true;
					 break;
				 }

				 read_buffer.reserveWritableBytes(bytes_to_read);

				 int ret = socket->recv(read_buffer.writeBegin(), bytes_to_read);
				 if (ret > 0) {
					 read_buffer.write(ret);
					 data_arrive = true;
				 } else if (ret < 0) {
					 int errn = errno;
					 if (EAGAIN == errn) {
						break;
					 }

					 connection->setError(errno);
					 if (error_cb_) {
						 error_cb_(this, socket_id, connection->getErrorCode());
					 }
					 return -1;
				 }
				 else {
					 peer_close = true;
					 break;
				 }
			 }

			 // 消息来了, 回调函数处理
			 if (data_arrive) {
				 if (recv_message_cb_) {
					 recv_message_cb_(this, socket_id, &read_buffer, new_net_command_cb_);
				 }
			 }

			 // 连接断开了
			 if (peer_close) {
				 if (data_arrive) {
					 if (connections_.find(socket_id) == connections_.end()) {
						 return -1;
					 }
				 }

				 connection->setStatus(TcpConnection::Status::PEER_CLOSED);
				 if (peer_close_cb_) {
					 peer_close_cb_(this, socket_id);
				 }

				 return -1;
			 }

			 return 0;
		 }

		 void Reactor::setRecvMessageCallback(const RecvMessageCallback &recv_message_cb)
		 {
			 recv_message_cb_ = recv_message_cb;
		 }

		 void Reactor::setPeerCloseCallback(const PeerCloseCallback &peer_close_cb)
		 {
			 peer_close_cb_ = peer_close_cb;
		 }

		 void Reactor::setErrorCallback(const ErrorCallback &error_cb)
		 {
			 error_cb_ = error_cb;
		 }

		 void Reactor::setWriteMessageCallback(const WriteMessageCallback &write_message_cb)
		 {
			 write_message_cb_ = write_message_cb;
		 }

		 void Reactor::setNewNetCommandCallback(const NewNetCommandCallback &new_net_cmd_cb)
		 {
			 new_net_command_cb_ = new_net_cmd_cb;
		 }

		 // 出错了
		 void Reactor::onSocketError(IODevice *io_device)
		 {
			 TcpSocket *socket = static_cast<TcpSocket *>(io_device);

			 TcpConnectionMap::iterator iter = connections_.find(socket->getId());
			 if (connections_.end() == iter) {
				 return;
			 }

			 TcpConnection *connection = iter->second;

			 int socket_error = socket->getSocketError();
			 if (0 == socket_error) {
				 socket_error = errno;
			 }

			 connection->setError(socket_error);
			 if (error_cb_) {
				 error_cb_(this, socket->getId(), connection->getErrorCode());
			 }
		 }


		 // 消息需要发送
		 void Reactor::onSocketWrite(IODevice *io_device)
		 {
			 TcpSocket *socket = static_cast<TcpSocket *>(io_device);
			 TcpConnectionMap::iterator iter = connections_.find(socket->getId());
			 if (connections_.end() == iter) {
				 return;
			 }
			 TcpConnection *connection = iter->second;
			 DynamicBuffer &write_buffer = connection->getWriteBuffer();

			 int write_size = socket->send(write_buffer.readBegin(), write_buffer.readableBytes());

			 if (write_size < 0) {
				 if (errno != EAGAIN) {
					 connection->setError(errno);
					 if (error_cb_) {
						 error_cb_(this, socket->getId(), connection->getErrorCode());
					 }
					 return;
				 }
			 }
			 else {
				 write_buffer.read(write_size);
				 if (write_buffer.readableBytes() == 0) {
					 socket->setWriteCallback(nullptr);
					 SendCompleteCallback send_complete_cb = connection->getSendCompleteCallback();
					 connection->setSendCompleteCallback(nullptr);
					 if (send_complete_cb) {
						 send_complete_cb(this, socket->getId());
					 }
				 }

			 }
		 }

		 // 是否处于连接状态
		 bool Reactor::isConnected(SocketId socket_id)
		 {
			 TcpConnectionMap::const_iterator iter = connections_.find(socket_id);
			 if (connections_.end() == iter) {
				 return false;
			 }
			 TcpConnection *connection = iter->second;

			 return connection->getStatus() == TcpConnection::Status::CONNECTED;
		 }

		 bool  Reactor::getLocalAddress(SocketId socket_id, SocketAddress *addr) const
		 {
			 TcpSocketMap::const_iterator iter = sockets_.find(socket_id);
			 if (iter == sockets_.end()) {
				 return false;
			 }

			 TcpSocket *socket = iter->second;
			 return socket->getLocalAddress(addr);
		 }

		 bool Reactor::getPeerAddress(SocketId socket_id, SocketAddress *addr) const
		 {
			 TcpSocketMap::const_iterator iter = sockets_.find(socket_id);
			 if (iter == sockets_.end()) {
				 return false;
			 }

			 TcpSocket *socket = iter->second;
			 return socket->getPeerAddress(addr);
		 }

		 bool Reactor::sendMessage(SocketId socket_id, const char *buffer, size_t size, const SendCompleteCallback &send_complete_cb)
		 {
			 TcpConnectionMap::iterator iter = connections_.find(socket_id);
			 if (connections_.end() == iter) {
				 return false;
			 }
			 TcpConnection *connection = iter->second;
			 return sendMessage(connection, buffer, size, send_complete_cb);
		 }

		 bool Reactor::sendMessageThenClose(SocketId socket_id, const char *buffer, size_t size)
		 {

			 return sendMessage(socket_id, buffer, size, std::bind(&Reactor::sendCompleteCloseCallback, this, std::placeholders::_1, std::placeholders::_2));
		 }

		 bool Reactor::sendMessage(TcpConnection *connection, const char *buffer, size_t size, const SendCompleteCallback &send_complete_cb)
		 {
			 if (connection->getStatus() != TcpConnection::Status::CONNECTED) {
				 return false;
			 }
			 TcpSocket *socket = connection->getSocket();
			 DynamicBuffer &write_buffer = connection->getWriteBuffer();

			 size_t remain_size = size;

			 if (write_buffer.readableBytes() == 0) {
				 // 缓冲区没有消息需要发送， 直接发送
				 int write_size = socket->send(buffer, size);
				 if (write_size < 0) {
					 if (errno != EAGAIN) {
						 connection->setError(errno);
						 addSocketTimer(socket->getId(), 0, std::bind(&Reactor::onSendMessageError, this, std::placeholders::_1));
						 return false;
					 }
				 } else {
					 remain_size -= write_size;
				 }
			 }

			 // 塞到缓冲区吧
			 if (remain_size > 0) {
				// 检查缓冲区是否溢出
				 if (conn_write_buffer_max_size_ > 0 &&
					 (remain_size + write_buffer.readableBytes()) > conn_write_buffer_max_size_) {
					 connection->setError(ENOBUFS);
					 addSocketTimer(socket->getId(), 0, std::bind(&Reactor::onSendMessageError, this, std::placeholders::_1));
					 return false;
				 }

				 write_buffer.reserveWritableBytes(remain_size);
				 ::memcpy(write_buffer.writeBegin(), buffer, remain_size); // 写数据
				 write_buffer.write(remain_size); // 记录指针后移
				 connection->setSendCompleteCallback(send_complete_cb);
				 socket->setWriteCallback(std::bind(&Reactor::onSocketWrite, this, std::placeholders::_1));
			 }
			 else {
				 if (send_complete_cb) {
					 send_complete_cb(this, socket->getId());
				 }
			 }

			 return true;
		 }

		 void Reactor::addSocketTimer(SocketId socket_id, int timeout_ms,
			 TimerCallback timer_cb)
		 {
			 TimerId timer_id = startTimer(timeout_ms, timer_cb, 1);
			 socket_to_timer_map_[socket_id] = timer_id;
			 timer_to_socket_map_[timer_id] = socket_id;
		 }
		 void Reactor::removeSocketTimer(SocketId socket_id)
		 {
			 auto iter = socket_to_timer_map_.find(socket_id);
			 if (socket_to_timer_map_.end() == iter) {
				 return;
			 }

			 auto timer_id = iter->second;
			 socket_to_timer_map_.erase(iter);
			 timer_to_socket_map_.erase(timer_id);

			 stopTimer(timer_id);
		 }

		 // 发送消息出错回调
		 void Reactor::onSendMessageError(TimerId timer_id)
		 {
			 TimerId_SocketId_Map::iterator iter = timer_to_socket_map_.find(timer_id);
			 if (timer_to_socket_map_.end() == iter) {
				 return;
			 }

			 SocketId socket_id = iter->second;
			 timer_to_socket_map_.erase(iter);
			 socket_to_timer_map_.erase(socket_id);

			 TcpSocketMap::iterator iter2 = sockets_.find(socket_id);
			 if (sockets_.end() == iter2) {
				 return;
			 }

			 TcpConnectionMap::iterator iter3 = connections_.find(socket_id);
			 if (connections_.end() == iter3) {
				 return;
			 }

			 TcpSocket *socket = iter2->second;
			 TcpConnection *connection = iter3->second;
			 if (error_cb_) {
				 error_cb_(this, socket->getId(), connection->getErrorCode());
			 }
		 }

		 void Reactor::sendCompleteCloseCallback(Reactor *reactor, SocketId socket_id)
		 {
			 reactor->closeSocket(socket_id);
		 }

		 // 广播消息
		 void Reactor::broadcastMessage(const char *buffer, size_t size)
		 {
			 for (TcpConnectionMap::iterator iter = connections_.begin(); iter != connections_.end(); ++iter) {
				 TcpConnection *connection = iter->second;
				 sendMessage(connection, buffer, size, nullptr);
			 }
		 }

	} // namespace net
} // namespace zbluenet