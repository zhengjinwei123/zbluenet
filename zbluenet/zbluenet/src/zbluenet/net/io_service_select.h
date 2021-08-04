#ifndef ZBLUENET_NET_IO_SERVICE_SELECT_H
#define ZBLUENET_NET_IO_SERVICE_SELECT_H

#include <zbluenet/net/io_service.h>
#include <zbluenet/net/fd_set.h>
#include <zbluenet/net/platform.h>

#include <unordered_map>
#include <vector>
#include <mutex>

namespace zbluenet {
	namespace net {

		class IOServiceSelect : public IOService {
		public:
			using IODeviceMap = std::unordered_map<IODevice::DescriptorID, IODevice *>;
			using IODeviceVector = std::vector<IODevice *>;

			IOServiceSelect();
			virtual ~IOServiceSelect();

			void pushIODevice(IODevice *io_device);
			virtual void loop();
	
		private:
			bool doNetEvents();
			virtual bool addIODevice(IODevice *io_device);
			virtual bool removeIODevice(IODevice *io_device);
			virtual bool updateIODevice(IODevice *io_device);

			void processReadEvent();
			void processWriteEvent();

		private:
			FDSet fd_read_;
			FDSet fd_read_back_;
			FDSet fd_write_;
			IODeviceMap io_device_map_; // 正式的设备列表
			IODeviceVector io_device_list_; // 缓冲的设备列表

			bool devices_changed_;
			std::mutex io_device_list_mutex_;

			int max_sock_;
		};
	} // namespace net
} // namespace zbluenet
#endif // ZBLUENET_NET_IO_SERVICE_SELECT_H
