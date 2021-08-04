#include <zbluenet/net/epoll_reactor.h>
#include <zbluenet/timestamp.h>
#include <zbluenet/net/io_device.h>
#include <zbluenet/log.h>

#ifndef _WIN32

namespace zbluenet {
	namespace net {
		EpollReactor::EpollReactor(int max_socket_num) : 
			Reactor(max_socket_num)
		{
			epoller_.create(max_socket_num);
		}

		EpollReactor::~EpollReactor()
		{
			epoller_.destroy();
		}

		bool EpollReactor::addIODevice(IODevice *io_device)
		{
			return epoller_.add(io_device);
		}

		bool EpollReactor::removeIODevice(IODevice *io_device)
		{
			return epoller_.del(io_device);
		}

		void EpollReactor::closeSocket(SocketId socket_id)
		{
			auto iter = sockets_.find(socket_id);
			if (iter == sockets_.end()) {
				return;
			}
			iter->second->detachIOService();
			Reactor::closeSocket(socket_id);
		}

		bool EpollReactor::attachSocket(std::unique_ptr<TcpSocket> &peer_socket)
		{
			if (sockets_.find(peer_socket->getId()) != sockets_.end()) {
				return false;
			}

			peer_socket->setReadCallback(std::bind(&Reactor::onSocketRead, this, std::placeholders::_1));
			peer_socket->setErrorCallback(std::bind(&Reactor::onSocketError, this, std::placeholders::_1));

			if (false == peer_socket->attachIOService(*this)) {
				return false;
			}

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

		void EpollReactor::loop()
		{
			quit_ = false;
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

			LOG_ERROR("EpollReactor::loop quit");
		}

	} // namespace net
} // namespace zbluenet

#endif