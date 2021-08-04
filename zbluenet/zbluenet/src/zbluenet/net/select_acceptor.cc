#include <zbluenet/net/select_acceptor.h>
#include <zbluenet/net/platform.h>
#include <zbluenet/log.h>
#include <zbluenet/timestamp.h>
#include <algorithm>

namespace zbluenet {
	namespace net {

		SelectAcceptor::SelectAcceptor(TcpSocket *listen_socket, uint16_t max_socket_num) :
			Acceptor(listen_socket, max_socket_num)
		{
			fd_read_.create(max_socket_num);
		}

		SelectAcceptor::~SelectAcceptor()
		{

		}

		void SelectAcceptor::loop()
		{
			Timestamp now;
			while (!quit_) {
				now.setNow();

				fd_read_.zero();

				fd_read_.add(listen_socket_->getFD());

				int64_t timer_timeout = timer_heap_.getNextTimeoutMillisecond(now);
				int timeout = (std::min)(int(timer_timeout) * 1000, 30 * 60 * 1000 * 1000);
				timeval dt = { 0, timeout };
				int ret = select(int(listen_socket_->getFD()) + 1, fd_read_.fdset(), nullptr, nullptr, &dt);
				if (ret < 0) {
					if (errno == EINTR) {
						continue;
					}
					quit_ = true;
					break;
				}

				if (fd_read_.has(listen_socket_->getFD())) {
					this->accept();
				}
				// ¶¨Ê±Æ÷º¯Êý
				now.setNow();
				checkTimeout(now);
			}
		}
	} // namespace net
} // namespace zbluenet