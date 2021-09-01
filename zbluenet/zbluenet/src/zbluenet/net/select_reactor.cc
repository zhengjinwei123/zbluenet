#include <zbluenet/net/select_reactor.h>
#include <zbluenet/log.h>
#include <zbluenet/thread.h>
#include <zbluenet/timestamp.h>

namespace zbluenet {
	namespace net {

		bool SelectReactor::attachSocket(std::unique_ptr<TcpSocket> &peer_socket)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			new_sockets_.emplace_back(peer_socket.get());
			peer_socket.release();

			return true;
		}
	

		void SelectReactor::closeSocket(SocketId socket_id)
		{
			auto iter = sockets_.find(socket_id);
			if (iter != sockets_.end()) {
				fd_to_socketId_map_.erase(iter->second->getFD());
			}
			Reactor::closeSocket(socket_id);
		}

		void SelectReactor::loop()
		{
			quit_ = false;
			Timestamp now;
			while (!quit_) {
				now.setNow();

				if (!new_sockets_.empty()) {
					std::lock_guard<std::mutex> lock(mutex_);
					for (auto newSock : new_sockets_) {
						std::unique_ptr<TcpSocket> peer_socket(newSock);
						fd_to_socketId_map_.insert(std::make_pair(peer_socket->getFD(), peer_socket->getId()));

						Reactor::attachSocket(peer_socket);
					}

					new_sockets_.clear();
					client_change_ = true;
				}

				// 没有连接
				if (connections_.empty()) {
					now.setNow();
					checkTimeout(now);

					continue;
				}

				// 玩家心跳检测
				heartCheck();

				// 网络事件处理
				if (false == doNetEvent()) {
					quit_ = true;
				}

				this->doWriteEvent();

				now.setNow();
				checkTimeout(now);
			}

			LOG_MESSAGE_DEBUG("SelectReactor::loop:  thread[%d] exit", Thread::getId());
		}

		void SelectReactor::heartCheck()
		{

		}

		bool SelectReactor::doNetEvent()
		{
			if (client_change_) {
				client_change_ = false;
				fd_read_.zero();
				// 将描述符 加入集合
				max_sock_ = sockets_.begin()->second->getFD();
				for (auto iter  : sockets_) {
					fd_read_.add(iter.second->getFD());

					if (max_sock_ < iter.second->getFD()) {
						max_sock_ = iter.second->getFD();
					}
				}
				fd_read_back_.copy(fd_read_);
			} else {
				fd_read_.copy(fd_read_back_);
			}

			timeval dt = { 0, 1 };
			int ret = 0;

			int maxSock = 0;
#ifndef _WIN32
			maxSock = max_sock_ + 1;
#endif

			ret = select(maxSock, fd_read_.fdset(), nullptr, nullptr, &dt);

			if (ret <0) {
				if (errno == EINTR) {
					return true;
				}
				LOG_MESSAGE_ERROR("SelectReactor::doNetEvent error");
				return false;
			}
			else if (ret == 0) {
				return true;
			}

			this->doReadEvent();

			return true;
		}

		void SelectReactor::doReadEvent()
		{
#ifdef _WIN32
			auto pfdset = fd_read_.fdset();
			for (size_t i = 0; i < pfdset->fd_count; ++i) {
				auto iter = fd_to_socketId_map_.find(pfdset->fd_array[i]);
				if (iter == fd_to_socketId_map_.end()) {
					continue;
				}
				auto iter_socket = sockets_.find(iter->second);
				if (iter_socket == sockets_.end()) {
					continue;
				}
				if (iter_socket->second->getReadCallback()) {
					int ret = iter_socket->second->getReadCallback()(iter_socket->second);
					if (ret  == -1) {
						fd_read_back_.del(pfdset->fd_array[i]);
					}
				}
			}
#else
			for (auto iter = sockets_.begin(); iter != sockets_.end();++iter) {
				if (fd_read_.has(iter->second->getFD())) {
					if (iter->second->getReadCallback()) {
						iter->second->getReadCallback()(iter->second);
					}
				}
			}
#endif
		}

		void SelectReactor::doWriteEvent()
		{
			if (write_message_cb_) {
				write_message_cb_();
			}
		}

	} // namespace net
} // namespace zbluenet