#ifndef ZBLUENET_NET_SELF_PIPE_H
#define ZBLUENET_NET_SELF_PIPE_H

#include <zbluenet/class_util.h>
#include<zbluenet/net/io_device.h>
#include <cstddef>

#ifndef _WIN32

namespace zbluenet {
	namespace net {
		class SelfPipe : public IODevice {
		public:
			SelfPipe();
			virtual ~SelfPipe();

			bool open();
			void close();
			virtual int read(char *buffer, size_t size);
			virtual int write(const char*buffer, size_t size);
			virtual bool setNonblock();
			virtual bool setCloseOnExec();

		private:
			int fd1_;
		};

	} // namespace net
} // namespace zbluenet
#endif // ZBLUENET_NET_SELF_PIPE_H
#endif 
