#ifndef ZBLUENET_NET_NETWORK_H
#define ZBLUENET_NET_NETWORK_H

#include <zbluenet/net/platform.h>
#include <zbluenet/class_util.h>
#include <zbluenet/logger_base.h>


namespace zbluenet {
	namespace net {
		class NetWork : public Noncopyable {
		public:
			NetWork();
			~NetWork();

		public:
			static void Init();
			static int makeNonblock(SOCKET fd);
			static int makeReuseAddr(SOCKET fd);
			static int makeTcpNoDelay(SOCKET fd);
			static bool setCloseOnExec(SOCKET fd);
			static int destroySocket(SOCKET fd);
		};
	} // namespace net
} // namespace zbluenet

#endif //ZBLUENET_NET_NETWORK_H 
