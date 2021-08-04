#include <zbluenet/logger_async_sink.h>
#include <zbluenet/logger_sink.h>

#include <functional>

namespace zbluenet {

	class LoggerAsyncSink::Impl {
	public:
		explicit Impl(LoggerSink *adapted_logger, size_t queue_size);
		~Impl();
		void logThreadFunc();

		void log(const char  *buffer, size_t size);

	private:
		LoggerSink *adapted_logger_;

		Thread log_thread_;
		ConcurrentQueue<DynamicBuffer *> queue_;
		std::mutex pool_mutex_;
		ObjectPool<DynamicBuffer> pool_;
	};
	//////////////////////////////////////////////////////////////////////////
	LoggerAsyncSink::Impl::Impl(LoggerSink *adapted_logger, size_t queue_size):
		adapted_logger_(adapted_logger), queue_(queue_size)
	{
		log_thread_.start(nullptr, std::bind(&LoggerAsyncSink::Impl::logThreadFunc, this));
	}

	LoggerAsyncSink::Impl::~Impl()
	{
		queue_.push(NULL);
		log_thread_.close();

		if (adapted_logger_) {
			delete adapted_logger_;
		}
	}

	void LoggerAsyncSink::Impl::log(const char *buffer, size_t size)
	{
		std::unique_ptr<DynamicBuffer> queue_buffer;
		{
			std::lock_guard<std::mutex> lock(pool_mutex_);
			queue_buffer.reset(pool_.getObject());
		}

		queue_buffer->reserveWritableBytes(size);
		::memcpy(queue_buffer->writeBegin(), buffer, size);
		queue_buffer->write(size);

		queue_.push(queue_buffer.get());
		queue_buffer.release();
	}

	void LoggerAsyncSink::Impl::logThreadFunc()
	{
		for (;;) {
			DynamicBuffer *queue_buffer_raw = nullptr;
			queue_.pop(queue_buffer_raw);

			if (nullptr == queue_buffer_raw) {
				break;
			}

			std::unique_ptr<DynamicBuffer> queue_buffer(queue_buffer_raw);

			adapted_logger_->log(queue_buffer->readBegin(), queue_buffer->readableBytes());
			queue_buffer->read(queue_buffer->readableBytes());

			{
				std::lock_guard<std::mutex> lock(pool_mutex_);
				pool_.returnObject(queue_buffer.get());
				queue_buffer.release();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	LoggerAsyncSink::LoggerAsyncSink(LoggerSink *adapted_logger, size_t queue_size) :
		pimpl_(new Impl(adapted_logger, queue_size))
	{

	}

	LoggerAsyncSink::~LoggerAsyncSink()
	{

	}

	void LoggerAsyncSink::log(const char *buffer, size_t size)
	{
		pimpl_->log(buffer, size);
	}

} // namespace zbluenet