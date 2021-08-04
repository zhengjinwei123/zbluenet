#include <zbluenet/net/acceptor.h>
#include <zbluenet/log.h>

#include <memory>

namespace zbluenet {
	namespace net {
		Acceptor::Acceptor(TcpSocket *listen_socket, uint16_t max_socket_num) : listen_socket_(listen_socket), quit_(true),
			new_conn_cb_(nullptr),
			max_socket_num_(max_socket_num)
		{

		}

		Acceptor::~Acceptor() {
			quit_ = true;
			if (thread_.isRun()) {
				thread_.close();
			}
		}

		void Acceptor::setNewConnCallback(NewConnectionCallback new_conn_cb)
		{
			new_conn_cb_ = new_conn_cb;
		}

		bool Acceptor::start()
		{
			quit_ = false;

			Acceptor *that = this;
			thread_.start(nullptr, [that](Thread *pthread) -> void {
				that->loop();
			});

			return true;
		}

		void Acceptor::stop()
		{
			quit_ = true;
			thread_.close();
		}

		void Acceptor::loop() 
		{

		}

		bool Acceptor::accept()
		{
			std::unique_ptr<TcpSocket> peer_socket(new TcpSocket());
			if (false == listen_socket_->acceptNonblock(peer_socket.get())) {
				LOG_MESSAGE_ERROR("TcpServer::createServer passiveOpenNonblock failed");
				return false;
			}

			// ·ÖÅäsocketid
			TcpSocket::SocketId socket_id = socket_id_allocator_.nextId(peer_socket->getFD());
			peer_socket->setId(socket_id);

			if (new_conn_cb_)
				new_conn_cb_(peer_socket);

			return true;
		}

	} // namespace net
} // namespace zbluenet