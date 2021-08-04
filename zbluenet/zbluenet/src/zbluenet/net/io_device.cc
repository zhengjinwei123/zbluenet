#include <zbluenet/net/io_device.h>
#include <zbluenet/log.h>
#include <zbluenet/net/network.h>

namespace zbluenet {
	namespace net {
		IODevice::IODevice() :
			id_(0),
			fd_(-1),
			io_service_(nullptr)
		{

		}

		IODevice::~IODevice()
		{
			detachIOService();
		}

		void IODevice::setId(int64_t id)
		{
			id_ = id; 
		}

		const int64_t IODevice::getId() const
		{ 
			return id_;
		}

		void IODevice::setFD(SOCKET fd) 
		{ 
			fd_ = fd;
		}

		SOCKET IODevice::getFD() const
		{
			return fd_; 
		}

		bool IODevice::attachIOService(IOService &io_service)
		{
			if (io_service_ != nullptr) {
				detachIOService();
			}
			if (io_service.addIODevice(this) == false) {
				return false;
			}
			io_service_ = &io_service;
			return true;
		}

		void IODevice::detachIOService()
		{
			if (nullptr == io_service_) {
				return;
			}

			io_service_->removeIODevice(this);
			io_service_ = nullptr;
		}

		void IODevice::setReadCallback(const ReadCallback &read_cb)
		{
			read_cb_ = read_cb;
		}

		void IODevice::setWriteCallback(const EventCallback &write_cb)
		{
			write_cb_ = write_cb;
		}

		void IODevice::setErrorCallback(const EventCallback &error_cb)
		{
			error_cb_ = error_cb;
		}

		int IODevice::read(char *buffer, size_t size)
		{
#ifdef _WIN32
			return 0;
#else
			return ::read(fd_, buffer, size);
#endif
		}

		int IODevice::write(const char *buffer, size_t size)
		{
#ifdef _WIN32
			return 0;
#else
			return ::write(fd_, buffer, size);
#endif
		}

		bool IODevice::setNonblock()
		{
			return NetWork::makeNonblock(fd_) == 0;
		}

		bool IODevice::setCloseOnExec()
		{
			return NetWork::setCloseOnExec(fd_) == 0;
		}

	} // namespace net
} // namespace zbluenet