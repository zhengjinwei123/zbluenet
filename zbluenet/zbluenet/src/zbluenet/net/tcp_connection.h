#ifndef ZBLUENET_NET_TCP_CONNECTION_H
#define ZBLUENET_NET_TCP_CONNECTION_H

#include <zbluenet/net/tcp_socket.h>
#include <zbluenet/class_util.h>
#include <zbluenet/dynamic_buffer.h>

#include <cstddef>
#include <stdint.h>

namespace zbluenet {
	namespace net {

		class Reactor;

		class TcpConnection  : public Noncopyable {
		public:
			struct Status {
				enum type {
					NONE,
					CONNECTING,
					CONNECTED,
					PEER_CLOSED,
					PENDING_ERROR,
				};
			};

			using SendCompleteCallback = std::function<void(Reactor *, TcpSocket::SocketId)>;

			TcpConnection(TcpSocket *socket, size_t read_buffer_init_size,
				size_t read_buffer_expand_size,
				size_t write_buffer_init_size,
				size_t write_buffer_expand_size);
			~TcpConnection() {}

			TcpSocket *getSocket() { return socket_; }
			Status::type getStatus() const { return status_;  }
			int getErrorCode() const { return error_code_; }
			DynamicBuffer &getReadBuffer() { return read_buffer_; }
			DynamicBuffer &getWriteBuffer() { return write_buffer_; }

			void setStatus(Status::type status) { status_ = status; }
			void setError(int error_code);

			void setReactorId(uint16_t reactor_id) { reactor_id_ = reactor_id;  }
			const uint16_t getReactorId() const { return reactor_id_; }

			const SendCompleteCallback &getSendCompleteCallback() const { return send_complete_cb_;  }
			void setSendCompleteCallback(const SendCompleteCallback send_complete_cb) { send_complete_cb_ = send_complete_cb;  }

		private:
			TcpSocket *socket_;
			Status::type status_;
			int error_code_;
			DynamicBuffer read_buffer_;
			DynamicBuffer write_buffer_;

			uint16_t reactor_id_;

			SendCompleteCallback send_complete_cb_;
		};

	} // namespace net
} // namespace zbluenet

#endif // ZBLUENET_NET_TCP_CONNECTION_H
