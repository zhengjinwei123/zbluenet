#include <zbluenet/net/epoll_acceptor.h>
#include <zbluenet/timestamp.h>
#include <zbluenet/log.h>

#include <functional>

#ifndef _WIN32
namespace zbluenet {
	namespace net {

		EPollAcceptor::EPollAcceptor(TcpSocket *listen_socket, uint16_t max_socket_num) :
			Acceptor(listen_socket, max_socket_num)
		{
			if (-1 == epoller_.create(max_socket_num)) {
				LOG_ERROR("epoller_.create error %d", max_socket_num);
				return;
			}

			listen_socket->setReadCallback(std::bind(&EPollAcceptor::onListenSocketRead, this, std::placeholders::_1));

			epoller_.add(listen_socket);
		}

		EPollAcceptor::~EPollAcceptor()
		{
			epoller_.del(listen_socket_);
			epoller_.destroy();
		}

		int EPollAcceptor::onListenSocketRead(IODevice *io_device)
		{
			TcpSocket *listen_socket = static_cast<TcpSocket *>(io_device);
			for (;;) {
				std::unique_ptr<TcpSocket> peer_socket(new TcpSocket());
				if (listen_socket->acceptNonblock(peer_socket.get()) == false) {
					if (EAGAIN == errno) {
						break;
					}
					else if (ECONNABORTED == errno) {
						continue;
					}
					else if (EMFILE == errno || ENFILE == errno) {
						return -1;
					}
					else {
						LOG_MESSAGE_ERROR("socket(%lx) accept failed : %s", listen_socket->getId(), strerror(errno));
						return -1;
					}
				}
				// ·ÖÅäsocketid
				TcpSocket::SocketId socket_id = socket_id_allocator_.nextId(peer_socket->getFD());
				peer_socket->setId(socket_id);

				if (new_conn_cb_) {
					new_conn_cb_(peer_socket);
				}
			}

			return 0;
		}

       void EPollAcceptor::loop()
	   {
		   Timestamp now;
		   while (!quit_) {
			   now.setNow();
			   int timer_timeout = timer_heap_.getNextTimeoutMillisecond(now);

			   if (timer_timeout == -1) {
				   timer_timeout = 0;
			   }

			   int ret = epoller_.doEvent(timer_timeout);
			   if (ret == -1) {
				   quit_ = true;
				   break;
			   }

			   now.setNow();
			   checkTimeout(now);
		   }
	   }

	} // namespace net
} // namespace zbluenet

#endif