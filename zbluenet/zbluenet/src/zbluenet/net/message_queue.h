#ifndef ZBLUENET_NET_MESSAGE_QUEUE_H
#define ZBLUENET_NET_MESSAGE_QUEUE_H

#include <zbluenet/class_util.h>
#include <zbluenet/net/self_pipe.h>
#include <zbluenet/net/io_service.h>
#include <zbluenet/concurrent_queue.h>
#include <zbluenet/log.h>

#include <functional>
#include <cstddef>

namespace zbluenet {
	namespace net {

		template <typename T>
		class MessageQueue : public Noncopyable {
		public:
			using RecvMessageCallback = std::function<void(MessageQueue *)>;

			MessageQueue(size_t max_size = 0) :
				 queue_(max_size)
			{
				
			}

			~MessageQueue()
			{

			}

			bool attach(IOService &io_service)
			{

#ifdef _WIN32
				return true;
#else
				io_service_ = &io_service;
				if (pipe_.open() == false ||
					pipe_.setNonblock() == false ||
					pipe_.setCloseOnExec() == false) {
					LOG_ERROR("MessageQueue::attach failed");
					return false;
				}

				pipe_.setReadCallback(std::bind(&MessageQueue::pipeReadCallback, this, std::placeholders::_1));
				return pipe_.attachIOService(*io_service_);
#endif
			}

			IOService *getIOService() const
			{
#ifndef _WIN32
				return io_service_;
#else
				return nullptr;
#endif
			}

			void setRecvMessageCallback(const RecvMessageCallback &recv_message_cb)
			{
#ifndef _WIN32
				recv_message_cb_ = recv_message_cb;
#endif
			}

			size_t size()
			{
				return queue_.size();
			}

			void push(const T &item)
			{
				queue_.push(item);

#ifndef _WIN32
				pipe_.write("1", 1);
#endif
			}

			bool pop(T &item)
			{
				return queue_.popIfNotEmpty(item);
			}

			bool peek(T &item)
			{
				return queue_.peek(item);
			}

			void yield()
			{
#ifndef _WIN32
				pipe_.write("1", 1);
#endif
			}

		private:
			int pipeReadCallback(IODevice *io_device)
			{
#ifndef _WIN32
				char buffer[1024];
				while (pipe_.read(buffer, sizeof(buffer)) > 0);
				if (recv_message_cb_) {
					recv_message_cb_(this);
				}
#endif

				return 0;
			}

		private:
			ConcurrentQueue<T> queue_;
#ifndef _WIN32
			IOService *io_service_;
			SelfPipe pipe_;
			RecvMessageCallback recv_message_cb_;
#endif
		};

	} // namespace net
} // namespace zbluenet

#endif // ZBLUENET_NET_MESSAGE_QUEUE_H
