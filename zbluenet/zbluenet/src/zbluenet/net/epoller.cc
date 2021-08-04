#include <zbluenet/net/epoller.h>
#include <zbluenet/net/io_device.h>
#include <zbluenet/log.h>

#include <fcntl.h>
#define MAX_EPOLL_TIMEOUT_MSEC (30 * 60 * 1000)

#ifndef _WIN32
#include <cerrno>

namespace zbluenet {
	namespace net {
		Epoller::Epoller() : epoll_fd_(-1),
			events_(32)
		{

		}

		Epoller::~Epoller()
		{
			destroy();
		}

		int Epoller::create(int max_event_num)
		{
			if (epoll_fd_ > 0) {
				destroy();
			}

			epoll_fd_ = epoll_create(max_event_num);
			if (epoll_fd_ == -1) {
				return -1;
			}
			int flags = ::fcntl(epoll_fd_, F_GETFD, 0);
			if (::fcntl(epoll_fd_, F_SETFD, flags | FD_CLOEXEC) != 0) {
				return -1;
			}
			return 0;
		}

		void Epoller::destroy()
		{
			if (epoll_fd_ != -1) {
				::close(epoll_fd_);
				epoll_fd_ = -1;
			} 
		}

		bool Epoller::add(IODevice *io_device)
		{
			struct epoll_event event;
			::memset(&event, 0, sizeof(event));
			event.events = 0;
			event.data.ptr = io_device;

			if (io_device->getReadCallback()) {
				event.events |= EPOLLIN | EPOLLPRI;
			}
			if (io_device->getWriteCallback()) {
				event.events |= EPOLLOUT;
			}
			
			if (::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, io_device->getFD(), &event) != 0) {
				LOG_DEBUG("Epoller::add failed %d", io_device->getFD());
				return false;
			}
			return true;
		}

		bool Epoller::del(IODevice *io_device)
		{
			struct epoll_event event;
			::memset(&event, 0, sizeof(event));

			if (::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, io_device->getFD(), &event) != 0) {
				return false;
			}

			removed_io_devices_.insert((intptr_t)io_device);

			return true;
		}

		bool Epoller::update(IODevice *io_device)
		{
			struct epoll_event event;
			::memset(&event, 0, sizeof(event));
			event.events = 0;
			event.data.ptr = io_device;

			if (io_device->getReadCallback()) {
				event.events |= EPOLLIN | EPOLLPRI;
			}
			if (io_device->getWriteCallback()) {
				event.events |= EPOLLOUT;
			}

			if (::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, io_device->getFD(), &event) != 0) {
				return false;
			}
			return true;
		}

		bool Epoller::checkIODeviceExist(IODevice *io_device) const
		{
			return removed_io_devices_.find((intptr_t)io_device) == removed_io_devices_.end();
		}

		int Epoller::doEvent(int timeout)
		{
			int event_count = ::epoll_wait(epoll_fd_, &events_[0], events_.size(), std::min(timeout, MAX_EPOLL_TIMEOUT_MSEC));
			if (-1 == event_count) {
				if (EINTR == errno) {
					return 0;
				}
				return -1;
			}

			for (int i = 0; i < event_count; ++i) {
				struct epoll_event *event = &events_[i];
				IODevice *io_device = (IODevice *)event->data.ptr;

				if (event->events & EPOLLOUT) {
					if (checkIODeviceExist(io_device) == false) {
						continue;
					}
					(io_device->getWriteCallback())(io_device);
				}
#ifdef EPOLLRDHUP
				if (event->events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
#else
				if (event->events & (EPOLLIN | EPOLLPRI)) {
#endif
					if (checkIODeviceExist(io_device) == false) {
						continue;
					}
					(io_device->getReadCallback())(io_device);
				}

				if (event->events & (EPOLLERR | EPOLLHUP)) {
					if (checkIODeviceExist(io_device) == false) {
						continue;
					}
					(io_device->getErrorCallback())(io_device);
				}
			}

			removed_io_devices_.clear();

			if (event_count >= (int)events_.size()) {
				events_.resize(events_.size() * 2);
			}

			return 0;
		}
	} // namespace net
} // namespace zbluenet
#endif