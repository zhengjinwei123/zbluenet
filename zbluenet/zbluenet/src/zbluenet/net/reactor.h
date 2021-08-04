#ifndef ZBLUENET_NET_REACTOR_H
#define ZBLUENET_NET_REACTOR_H

#include <zbluenet/net/tcp_connection.h>
#include <zbluenet/net/tcp_socket.h>
#include <zbluenet/net/io_device.h>
#include <zbluenet/net/io_service.h>
#include <zbluenet/protocol/net_command.h>

#include <stdint.h>
#include <memory>
#include <functional>
#include <unordered_map>

namespace zbluenet {

	namespace net {

		class SocketAddress;

		class Reactor : public IOService {
		public:
			using SocketId = TcpSocket::SocketId;
			using NewNetCommandCallback = std::function<void(std::unique_ptr<protocol::NetCommand> &)>;

			using RecvMessageCallback = std::function<void (Reactor *, SocketId, DynamicBuffer *, const NewNetCommandCallback &new_net_cmd_cb)>;
			using PeerCloseCallback = std::function<void (Reactor *, SocketId)>;
			using ErrorCallback = std::function<void (Reactor *, SocketId, int)>;
			using SendCompleteCallback = std::function<void(Reactor *, SocketId)>;
			using WriteMessageCallback = std::function<void()>;

		public:
			using TcpSocketMap = std::unordered_map<SocketId, TcpSocket *>;
			using TcpConnectionMap = std::unordered_map<SocketId, TcpConnection *>;
			using SocketId_TimerId_Map = std::unordered_map<SocketId, TimerId>;
			using TimerId_SocketId_Map = std::unordered_map<TimerId, SocketId>;

		public:
			Reactor(int max_connection_num);
			virtual ~Reactor();
			void setRecvBufferInitSize(size_t size);
			void setRecvBufferExpandSize(size_t size);
			void setRecvBufferMaxSize(size_t size);
			////
			void setSendBufferInitSize(size_t size);
			void setSendBufferExpandSize(size_t size);
			void setSendBufferMaxSize(size_t size);

			const size_t getConnectionNum() const { return connections_.size();  }

			void init(uint16_t id);

			void start();

			void loop() {}

		public:
			void setRecvMessageCallback(const RecvMessageCallback &recv_message_cb);
			void setPeerCloseCallback(const PeerCloseCallback &peer_close_cb);
			void setErrorCallback(const ErrorCallback &error_cb);
			void setWriteMessageCallback(const WriteMessageCallback &write_message_cb);
			void setNewNetCommandCallback(const NewNetCommandCallback &new_net_cmd_cb);

			bool isConnected(SocketId socket_id);
			bool getLocalAddress(SocketId socket_id, SocketAddress *addr) const;
			bool getPeerAddress(SocketId socket_id, SocketAddress *addr) const;

			bool sendMessage(SocketId socket_id, const char *buffer, size_t size, const SendCompleteCallback &send_complete_cb = nullptr);
			bool sendMessageThenClose(SocketId socket_id, const char *buffer, size_t size);
			void broadcastMessage(const char *buffer, size_t size);
			// 解绑
			virtual void closeSocket(SocketId socket_id);
			// 绑定socket
			virtual bool attachSocket(std::unique_ptr<TcpSocket> &peer_socket);

			bool checkExists(SocketId socket_id);

			int onSocketRead(IODevice *io_device);// 消息到来
			void onSocketWrite(IODevice *io_device); // 消息发送
			void onSocketError(IODevice *io_device); // 出错处理

		private:
			bool sendMessage(TcpConnection *connection, const char *buffer, size_t size, const SendCompleteCallback &send_complete_cb);
	
			void addSocketTimer(SocketId socket_id, int timeout_ms,
				TimerCallback timer_cb);
			void removeSocketTimer(SocketId socket_id);

			void onSendMessageError(TimerId timer_id);
			void sendCompleteCloseCallback(Reactor *reactor, SocketId socket_id);

		protected:
			bool quit_;
			uint16_t id_;
			int max_connection_num_;

			size_t conn_read_buffer_init_size_;
			size_t conn_read_buffer_expand_size_;
			size_t conn_read_buffer_max_size_;

			size_t conn_write_buffer_init_size_;
			size_t conn_write_buffer_expand_size_;
			size_t conn_write_buffer_max_size_;

			TcpSocketMap sockets_;
			TcpConnectionMap connections_;
			SocketId_TimerId_Map socket_to_timer_map_;
			TimerId_SocketId_Map timer_to_socket_map_;

			RecvMessageCallback recv_message_cb_;
			PeerCloseCallback peer_close_cb_;
			ErrorCallback error_cb_;
			WriteMessageCallback write_message_cb_;
			NewNetCommandCallback new_net_command_cb_;
		};
	} // namespace net 
} // namespace zbluenet
#endif // ZBLUENET_NET_REACTOR_H
