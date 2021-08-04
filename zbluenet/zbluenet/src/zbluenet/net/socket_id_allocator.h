#ifndef ZBLUENET_NET_SOCKET_ID_ALLOCATOR_H
#define ZBLUENET_NET_SOCKET_ID_ALLOCATOR_H

#include <stdint.h>
#include <zbluenet/net/platform.h>

namespace zbluenet {
	namespace net {
		class SocketIdAllocator {
		public:
			SocketIdAllocator():
				value_(0)
			{

			}

			~SocketIdAllocator()
			{

			}

			int64_t nextId(SOCKET fd = 0)
			{
				if (++value_ < 0) {
					value_ = 0;
				}
				return ((uint64_t)(value_) << 32) + (uint64_t)fd;
			}

		private:
			int32_t value_;
		};

	} // namespace net
} // namespace zbluenet

#endif // ZBLUENET_NET_SOCKET_ID_ALLOCATOR_H
