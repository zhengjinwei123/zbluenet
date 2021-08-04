#ifndef ZBLUENET_NET_TCP_SOCKET_H
#define ZBLUENET_NET_TCP_SOCKET_H

#include <cstddef>
#include <zbluenet/class_util.h>
#include <zbluenet/net/platform.h>
#include <zbluenet/net/socket_address.h>
#include <zbluenet/net/network.h>
#include <zbluenet/net/io_device.h>

namespace zbluenet {
	namespace net {

		class TcpSocket  : public IODevice {
		public:
			using SocketId = int64_t;

			TcpSocket();
			virtual ~TcpSocket();

			bool open(SocketAddress::Protocol::type protocol);
			void close();
			bool connect(const SocketAddress &addr);
			bool bind(const SocketAddress &addr);
			bool listen(int backlog);
			bool accept(TcpSocket *peer_socket);

			bool getLocalAddress(SocketAddress *addr) const;
			bool getPeerAddress(SocketAddress *addr) const;

			int readableBytes() const;
			int recv(char *buffer, size_t size);
			int send(const char *buffer, size_t size);

			bool shutdownRead();
			bool shutdownWrite();
			bool shutdownBoth();

			int getSocketError();
			bool setReuseAddr();
			bool setTcpNoDelay();

			// 客户端发起连接
			/************************************************************************/
			/* open setReuseAaddr setTcpNodealy connect                                                                     */
			/************************************************************************/
			bool activeOpen(const SocketAddress &remote_addr);
			
			/************************************************************************/
			/* activeOpen setNonblock                                                                     */
			/************************************************************************/
			bool activeOpenNonblock(const SocketAddress &remote_addr);

			// 创建服务
			/************************************************************************/
			/* open setReuseAddr setTcpNodelay bind listen(128)                                                                     */
			/************************************************************************/
			bool passiveOpen(const SocketAddress &local_addr);
			
			/************************************************************************/
			/* passiveOpen setNonblock                                                                     */
			/************************************************************************/
			bool passiveOpenNonblock(const SocketAddress &local_addr);

			/************************************************************************/
			/* accept setTcpNodelay setNonblock                                                                     */
			/************************************************************************/
			bool acceptNonblock(TcpSocket *peer);

			virtual bool setNonblock();
		};

	} // namespace net
} // namespace zbluenet

#endif // ZBLUENET_NET_TCP_SOCKET_H
