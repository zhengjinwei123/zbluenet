#ifndef ZBLUENET_LOGGER_H
#define ZBLUENET_LOGGER_H


#include <memory>
#include <functional>
#include <cstdarg>
#include <cstddef>
#include <vector>

#include <zbluenet/class_util.h>
#include <zbluenet/logger_sink.h>

namespace zbluenet {

	class Logger {
	public:
		using LogFormatter = std::function < size_t(char *buffer, size_t size, int level, const char *filename, int line, const char *func, const char *format, va_list args)>;
		using LoggerSinkVec = std::vector<LoggerSink *>;
		using LoggerLevelVec = std::vector<int>;
		using LoggerFormatterVec = std::vector<LogFormatter>;

		explicit Logger(LogFormatter formatter, int level, int max_log_size);
		~Logger();

		bool addSink(LoggerSink *sink, LogFormatter formatter, int level);
		void log(int level, const char *filename, int line, const char* function, const char *fmt, va_list args);
		void plainLog(int level, const char *fmt, va_list args);
		void setLevelFilter(int level) { level_ = level; }

	private:
		LogFormatter formatter_;
		int level_;
		int max_log_size_;

		LoggerSinkVec sinks_;
		LoggerFormatterVec formatters_;
		LoggerLevelVec levels_;
	};

} // namespace zbluenet

#endif // ZBLUENET_LOGGER_H
