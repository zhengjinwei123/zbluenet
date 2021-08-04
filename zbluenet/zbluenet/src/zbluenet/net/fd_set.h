#ifndef ZBLUENET_NET_FD_SET_H
#define ZBLUENET_NET_FD_SET_H

#include <zbluenet/net/platform.h>

namespace zbluenet {
	namespace net {
		class FDSet {
		public:
			FDSet();
			~FDSet();

			void create(int max_fd_count);
			void destroy();
			void add(SOCKET fd);
			void del(SOCKET fd);
			void zero();
			bool has(SOCKET fd);
			fd_set * fdset();
			void copy(FDSet& other);
		private:
			fd_set *pfd_set_;
			size_t fd_size_;
			int max_sock_fd_;
		};
	} // namespace net
} // namespace zbluenet

#endif // ZBLUENET_NET_FD_SET_H
