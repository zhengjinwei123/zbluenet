#ifndef ZBLUENET_LOGGER_MGR_H
#define ZBLUENET_LOGGER_MGR_H

#include <cstddef>
#include <cstdarg>
#include <memory>
#include <zbluenet/logger.h>
#include <zbluenet/class_util.h>

namespace zbluenet {

	class LoggerSink;

	class LoggerMgr {
	public:
		using LogFormatter = Logger::LogFormatter;

		void setMaxLoggerCount(int count = 1);
		void setMaxLogSize(int size = 4096);
		bool registerLogger(int logger_id, LogFormatter formatter = nullptr, int level = -1);
		void removeLogger(int logger_id);

		bool addSink(int logger_id, LoggerSink *sink, LogFormatter formatter = nullptr, int level = -1);

		void log(int logger_id, int level, const char *filename, int line, const char *function, const char *fmt, ...);
		void log(int logger_id, int level, const char *filename, int line, const char *function, const char *fmt, va_list args);

		void plainLog(int logger_id, int level, const char *format, ...);
		void plainLog(int logger_id, int level, const char *format, va_list args);

		void setLevelFilter(int logger_id, int level);

	private:
		ZBLEUNET_PRECREATED_SINGLETON(LoggerMgr);

		class Impl;
		std::unique_ptr<Impl> pimpl_;
	};
} // namespace zbluenet

#endif // ZBLUENET_LOGGER_MGR_H
