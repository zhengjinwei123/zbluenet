#ifndef ZBLUENET_LOGGER_ASYNC_H
#define ZBLUENET_LOGGER_ASYNC_H

#include <zbluenet/class_util.h>
#include <zbluenet/timestamp.h>
#include <zbluenet/logger_file_sink.h>
#include <zbluenet/thread.h>
#include <zbluenet/concurrent_queue.h>
#include <zbluenet/dynamic_buffer.h>
#include <zbluenet/object_pool.h>
#include <zbluenet/logger_sink.h>

#include <string>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <mutex>
#include <cstddef>
#include <memory>

namespace zbluenet  {
	class LoggerAsyncSink : public LoggerSink  {
	public:
		LoggerAsyncSink(LoggerSink *adapted_logger, size_t queue_size = 10240);
	   ~LoggerAsyncSink();
		void log(const char *buffer, size_t size);

	private:
		class Impl;
		std::unique_ptr<Impl> pimpl_;
	};
} // namespace zbluenet

#endif // ZBLUENET_LOGGER_ASYNC_H
