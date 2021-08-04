#include <zbluenet/net/io_service_select.h>
#include <zbluenet/log.h>
#include <zbluenet/net/io_device.h>
#include <zbluenet/timestamp.h>


namespace zbluenet {
	namespace net {

		IOServiceSelect::IOServiceSelect() :
			IOService(),
			devices_changed_(false),
			max_sock_(0)
		{
			fd_read_.create(1024);
			fd_write_.create(1024);
			fd_read_back_.create(1024);
		}

		IOServiceSelect::~IOServiceSelect()
		{

		}

		// run in thread
		void IOServiceSelect::loop()
		{
			quit_ = false;

			Timestamp now;

			while (!quit_) {
				
				if (!io_device_list_.empty()) {
					// 从客户端缓冲区取出客户
					std::lock_guard<std::mutex> lock(io_device_list_mutex_);
					for (auto io_device : io_device_list_) {
						// 加入监听
						addIODevice(io_device);
					}
					io_device_list_.clear();
					devices_changed_ = true;
				}

				if (io_device_map_.empty()) {
					continue;
				}
				doNetEvents();
			}
		}

		bool IOServiceSelect::doNetEvents()
		{
			if (io_device_map_.empty()) {
				return true;
			}

			// 计算可读集合
			if (devices_changed_) {
				devices_changed_ = false;
				fd_read_.zero();

				// 计算 max_sock
				max_sock_ = io_device_map_.begin()->second->getDescriptorId();
				for (auto iter : io_device_map_) {
					if (max_sock_ < iter.second->getDescriptorId()) {
						max_sock_ = iter.second->getDescriptorId();
					}
				}
				fd_read_back_.copy(fd_read_);
			} else {
				fd_read_.copy(fd_read_back_);
			}

			// 计算可写集合
			bool need_write = false;
			fd_write_.zero();
			for (auto iter : io_device_map_) {
				if (iter.second->isWriteEventActive()) {
					need_write = true;
					fd_write_.add(iter.second->getDescriptorId());
				}
			}

			// select
			timeval t = { 0, 1 };
			int ret = 0;
			if (need_write) {
				ret = select(max_sock_ + 1, fd_read_.fdset(), fd_write_.fdset(), nullptr, &t);
			} else {
				ret = select(max_sock_ + 1, fd_read_.fdset(), nullptr, nullptr, &t);
			}

			if (ret < 0) {
				if (errno == EINTR) {
					return true;
				}
				return false;
			} else if (ret == 0) {
				return true;
			}

			this->processReadEvent();
			this->processWriteEvent();

			return true;
		}

		void IOServiceSelect::processReadEvent()
		{
			// 处理读事件
#ifdef _WIN32
			auto pfdset = fd_read_.fdset();
			for (int i = 0; i < pfdset->fd_count; i++) {
				auto iter = io_device_map_.find(pfdset->fd_array[i]);
				if (iter != io_device_map_.end()) {
					IODevice *io_device = iter->second;
					if (io_device->getReadCallback()) {
						int ret1 = (io_device->getReadCallback())(io_device);
						if (ret1 != 0) {
							removeIODevice(io_device);
						}
					}
				}
			}
#else 

			for (auto iter = io_device_map_.begin(); iter != io_device_map_.end();) {
				IODevice *io_device = iter->second;
				if (fd_read_.has(io_device->getDescriptorId())) {
					if (io_device->getReadCallback()) {
						int ret1 = (io_device->getReadCallback())(io_device);
						if (ret1 != 0) {
							removeIODevice(io_device);
							auto iterOld = iter;
							iter++;
							io_device_map_.erase(iterOld);
							continue;
						}
					}
				}
				iter++;
			}

#endif // _WIN32
		}

		void IOServiceSelect::processWriteEvent()
		{
#ifdef _WIN32
			auto pfdset = fd_write_.fdset();
			for (int i = 0; i < pfdset->fd_count; i++) {
				auto iter = io_device_map_.find(pfdset->fd_array[i]);
				if (iter != io_device_map_.end()) {
					IODevice *io_device = iter->second;
					if (io_device->getWriteCallback()) {
						int ret1 = (io_device->getWriteCallback())(io_device);
						if (ret1 != 0) {
							removeIODevice(io_device);
						}
					}
				}
		}
#else
			for (auto iter = io_device_map_.begin(); iter != io_device_map_.end();) {
				IODevice *io_device = iter->second;
				if (fd_read_.has(io_device->getDescriptorId())) {
					if (io_device->getReadCallback()) {
						int ret1 = (io_device->getReadCallback())(io_device);
						if (ret1 != 0) {
							removeIODevice(io_device);
							auto iterOld = iter;
							iter++;
							io_device_map_.erase(iterOld);
							continue;
						}
					}
				}
				iter++;
			}
#endif // _WIN32
		}

		void IOServiceSelect::pushIODevice(IODevice *io_device)
		{
			std::lock_guard<std::mutex> lock(io_device_list_mutex_);
			io_device_list_.emplace_back(io_device);
		}

		bool IOServiceSelect::addIODevice(IODevice *io_device)
		{
			if (io_device == nullptr) {
				return false;
			}

			if (io_device_map_.find(io_device->getDescriptorId()) != io_device_map_.end()) {
				return false;
			}

			if (io_device->getReadCallback()) {
				if (fd_read_.has(io_device->getDescriptorId())) {
					fd_read_.del(io_device->getDescriptorId());
				}

				fd_read_.add(io_device->getDescriptorId());
			}

			if (io_device->getWriteCallback()) {
				if (fd_write_.has(io_device->getDescriptorId())) {
					fd_write_.del(io_device->getDescriptorId());
				}

				fd_write_.add(io_device->getDescriptorId());
			}

			io_device_map_.insert(std::make_pair(io_device->getDescriptorId(), io_device));

			return true;
		}

		bool IOServiceSelect::removeIODevice(IODevice *io_device)
		{
			if (io_device == nullptr) {
				return false;
			}

			if (io_device_map_.find(io_device->getDescriptorId()) == io_device_map_.end()) {
				return true;
			}

			if (io_device->getReadCallback()) {
				if (fd_read_.has(io_device->getDescriptorId())) {
					fd_read_.del(io_device->getDescriptorId());
				}
			}
			if (io_device->getWriteCallback()) {
				if (fd_write_.has(io_device->getDescriptorId())) {
					fd_write_.del(io_device->getDescriptorId());
				}
			}

			io_device_map_.erase(io_device->getDescriptorId());

			return true;
		}

		bool IOServiceSelect::updateIODevice(IODevice *io_device)
		{
			return true;
		}
	} // namespace net
}// namespace zbluenet