#include <zbluenet/net/tcp_socket.h>

#include <zbluenet/net/network.h>
#include <zbluenet/log.h>

#ifdef _WIN32

#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <cerrno>
#endif

namespace zbluenet {
	namespace net {
		TcpSocket::TcpSocket()
		{
		}

		TcpSocket::~TcpSocket()
		{
			close();
		}

		bool TcpSocket::open(SocketAddress::Protocol::type protocol)
		{
			if (fd_ != INVALID_SOCKET) {
				close();
			}

			if (SocketAddress::Protocol::IP_V4 == protocol) {
				fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			} else if (SocketAddress::Protocol::IP_V6 == protocol) {
				fd_ = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
			} else {
				errno = EAFNOSUPPORT;
				return false;
			}

			if (fd_ == INVALID_SOCKET) {
				return false;
			}
			if (NetWork::setCloseOnExec(fd_) == false) {
				close();
				LOG_MESSAGE_ERROR("1111123232323");
				return false;
			}
			return true;
		}

		void TcpSocket::close()
		{
			if (fd_ != INVALID_SOCKET) {
				NetWork::destroySocket(fd_);
				fd_ = INVALID_SOCKET;
			}
		}

		bool TcpSocket::connect(const SocketAddress &addr)
		{
			if (addr.getNativeAddress() == nullptr) {
				errno = EAFNOSUPPORT;
				return false;
			}

			if (::connect(fd_, (const struct sockaddr *)addr.getNativeAddress(),
				(int)addr.getNativeAddressSize())  == SOCKET_ERROR ) {
				return false;
			}

			return true;
		}

		bool TcpSocket::bind(const SocketAddress &addr)
		{
			if (addr.getNativeAddress() == nullptr) {
				errno = EAFNOSUPPORT;
				return false;
			}

			if (::bind(fd_, (const struct sockaddr *)addr.getNativeAddress(), 
				(int)addr.getNativeAddressSize()) == SOCKET_ERROR) {
				return false;
			}
			return true;
		}

		bool TcpSocket::listen(int backlog)
		{
			if (::listen(fd_, backlog) == SOCKET_ERROR) {
				return false;
			}
			return true;
		}

		bool TcpSocket::accept(TcpSocket *peer_socket)
		{
			SOCKET sock_fd = ::accept(fd_, nullptr, nullptr);
			if (INVALID_SOCKET == sock_fd) {
				return false;
			}

			peer_socket->setFD(sock_fd);
			if (NetWork::setCloseOnExec(sock_fd) == false) {
				peer_socket->close();
				return false;
			}

			return true;
		}

		bool TcpSocket::getLocalAddress(SocketAddress *addr) const
		{
			struct sockaddr_storage sock_addr;
			socklen_t addr_len = sizeof(sock_addr);
			if (::getsockname(fd_, (struct sockaddr *)&sock_addr, &addr_len) != 0) {
				return false;
			}
			return addr->setNativeAddress(&sock_addr);
		}

		bool TcpSocket::getPeerAddress(SocketAddress *addr) const
		{
			struct sockaddr_storage sock_addr;
			socklen_t addr_len = sizeof(sock_addr);
			if (::getpeername(fd_, (struct sockaddr *)&sock_addr, &addr_len) != 0) {
				return false;
			}
			return addr->setNativeAddress(&sock_addr);
		}

		int TcpSocket::readableBytes() const
		{
#ifdef _WIN32
			u_long readable_bytes = 0;
			if (::ioctlsocket(fd_, FIONREAD, &readable_bytes) == -1) {
				return -1;
			}

			//char data[1024];
			//int n = ::recv(fd_, data, 1, MSG_PEEK);

			return static_cast<int>(readable_bytes);
#else // LINUX
			int readable_bytes = 0;
			if (::ioctl(fd_, FIONREAD, &readable_bytes) == -1) {
				return -1;
			}
			return readable_bytes;
#endif // WIN32
		}

		int TcpSocket::recv(char *buffer, size_t size)
		{
			return ::recv(fd_, buffer, (int)size, 0);
		}

		// MSG_NOSIGNAL
		// linux下当服务器连接断开，客户端还发数据的时候，因为连接失败发送出错，
		//不仅send()的返回值会有反映，而且还会像系统发送一个异常消息，如果不作处理，系统会出 BrokePipe，程序会退出。
		int TcpSocket::send(const char *buffer, size_t size)
		{
#ifdef _WIN32
			return ::send(fd_, buffer, (int)size, 0);
#else // LINUX
			return ::send(fd_, buffer, size, MSG_NOSIGNAL);
#endif // WIN32
		}

		bool TcpSocket::shutdownRead()
		{
#ifdef _WIN32
			if (::shutdown(fd_, SD_RECEIVE) != 0) {
				return false;
			}
#else
			if (::shutdown(fd_, SHUT_RD) != 0) {
				return false;
			}
#endif
			return true;
		}

		bool TcpSocket::shutdownWrite()
		{
#ifdef _WIN32
			if (::shutdown(fd_, SD_SEND) != 0) {
				return false;
			}
#else
			if (::shutdown(fd_, SHUT_WR) != 0) {
				return false;
			}
#endif
			return true;
		}

		bool TcpSocket::shutdownBoth()
		{
#ifdef _WIN32
			if (::shutdown(fd_, SD_BOTH) != 0) {
				return false;
			}
#else
			if (::shutdown(fd_, SHUT_RDWR) != 0) {
				return false;
			}
#endif
			return true;
		}

		int TcpSocket::getSocketError()
		{
			int opt = 0;
			socklen_t opt_len = sizeof(opt);
#ifdef _WIN32
			if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, (char *)&opt, &opt_len) == SOCKET_ERROR) {
				return errno;
			}
#else
			if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR,  &opt, &opt_len) == SOCKET_ERROR) {
				return errno;
		}
#endif
			return opt;
		}


		bool TcpSocket::setReuseAddr()
		{
			if (SOCKET_ERROR == NetWork::makeReuseAddr(fd_)) {
				return false;
			}
			return true;
		}

		bool TcpSocket::setTcpNoDelay()
		{
			if (SOCKET_ERROR == NetWork::makeTcpNoDelay(fd_)) {
				return false;
			}
			return true;
		}


		// 客户端发起连接
		/************************************************************************/
		/* open setReuseAaddr setTcpNodealy connect                                                                     */
		/************************************************************************/
		bool TcpSocket::activeOpen(const SocketAddress &remote_addr)
		{
			if (open(remote_addr.getProtocol()) == false) {
				return false;
			}
			if (setReuseAddr() == false ||
				setTcpNoDelay() == false ||
				connect(remote_addr) == false) {
				close();
				return false;
			}
			return true;
		}

		/************************************************************************/
		/* activeOpen setNonblock                                                                     */
		/************************************************************************/
		bool TcpSocket::activeOpenNonblock(const SocketAddress &remote_addr)
		{
			if (activeOpen(remote_addr) == false) {
				return false;
			}
			if (setNonblock() == false) {
				close();
				return false;
			}
			return true;
		}

		// 创建服务
		/************************************************************************/
		/* open setReuseAddr setTcpNodelay bind listen(128)                                                                     */
		/************************************************************************/
		bool TcpSocket::passiveOpen(const SocketAddress &local_addr)
		{
			if (open(local_addr.getProtocol()) == false) {
				LOG_MESSAGE_ERROR("11111");
				return false;
			}
			if (setReuseAddr() == false ||
				setTcpNoDelay() == false ||
				bind(local_addr) == false ||
				listen(128) == false) {
				close();
				LOG_MESSAGE_ERROR("1111122222");
				return false;
			} 
			return true;
		}

		/************************************************************************/
		/* passiveOpen setNonblock                                                                     */
		/************************************************************************/
		bool TcpSocket::passiveOpenNonblock(const SocketAddress &local_addr)
		{
			if (passiveOpen(local_addr) == false) {
				return false;
			}
			if (setNonblock() == false) {
				close();
				return false;
			}
			return true;
		}

		bool TcpSocket::setNonblock()
		{
			if (-1 == NetWork::makeNonblock(fd_)) {
				return false;
			}
			return true;
		}
		/************************************************************************/
		/* accept setTcpNodelay setNonblock                                                                     */
		/************************************************************************/
		bool TcpSocket::acceptNonblock(TcpSocket *peer)
		{
			if (accept(peer) == false) {
				return false;
			}
			if (peer->setTcpNoDelay() == false ||
				peer->setNonblock() == false) {
				return false;
			}
			return true;
		}
	} // namespace net
}// namespace zbluenet
