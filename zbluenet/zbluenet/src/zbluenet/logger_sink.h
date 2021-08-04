#ifndef ZBLUENET_LOG_SINK_H
#define ZBLUENET_LOG_SINK_H

#include <cstddef>

#include <zbluenet/class_util.h>

namespace zbluenet {

	class LoggerSink : public Noncopyable {
	public:
		LoggerSink() {}
		virtual ~LoggerSink() {}

		virtual void log(const char *buffer, size_t size) = 0;
	};

} // namespace zbluenet

#endif

