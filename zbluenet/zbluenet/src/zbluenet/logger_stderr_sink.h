#ifndef ZBLUENET_LOGGER_STDERR_SINK_H
#define ZBLUENET_LOGGER_STDERR_SINK_H

#include <cstddef>

#include <zbluenet/class_util.h>
#include <zbluenet/logger_sink.h>

namespace zbluenet {

	class LoggerStderrSink : public LoggerSink {
	public:
		LoggerStderrSink();
		virtual ~LoggerStderrSink();

		virtual void log(const char *buffer, size_t size);
	};

} // namespace zbluenet

#endif
