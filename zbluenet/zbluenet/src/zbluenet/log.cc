#include <zbluenet/log.h>
#include <zbluenet/logger_base.h>
#include <zbluenet/logger_async_sink.h>
#include <zbluenet/logger_file_sink.h>
#include <zbluenet/logger_stderr_sink.h>
#include <zbluenet/timestamp.h>
#include <zbluenet/logger_mgr.h>

#include <cstdarg>
#include <cstdio>
#include <memory>

namespace zbluenet {

	static size_t logFormatter(char *buffer, size_t buffer_size, int level, const char *filename, int line, const char *function, const char *fmt, va_list args)
	{
		if (level < LogLevel::MIN || level >= LogLevel::MAX) {
			return 0;
		}
		static const char *log_level_string[] = {
			"DEBUG", "INFO", "*WARN*", "**ERROR**"
		};

		Timestamp now;

		char timer_buffer[1024];
		Timestamp::format(timer_buffer, sizeof(timer_buffer), "%H:%M:%S", now.getSecond());

		size_t count = 0;

		do {
#ifdef _WIN32
			count += ::snprintf(buffer, buffer_size, "[%s][%s:%03lld][%s:%d](%s)", log_level_string[level], timer_buffer, now.getMillisecond(), filename, line, function);
#else
			count += ::snprintf(buffer, buffer_size, "[%s][%s:%03ld][%s:%d](%s)", log_level_string[level], timer_buffer, now.getMillisecond(), filename, line, function);
#endif

			if (count >= buffer_size) {
				break;
			}

			count += ::vsnprintf(buffer + count, buffer_size - count, fmt, args);
			if (count >= buffer_size) {
				break;
			}

			count += ::snprintf(buffer + count, buffer_size - count, "\n");
		} while (false);

		if (count >= buffer_size) {
			if (buffer_size >= 2) {
				buffer[buffer_size - 2] = '\n';
			}
			return buffer_size - 1;
		}

		return count;
	}

	static void baseLogFunc(int level, const char *fmt, va_list args)
	{
		static constexpr int log_level_map[] = {
			LogLevel::DEBUG,
			LogLevel::WARNING,
			LogLevel::ERR
		};
		
		zbluenet::LoggerMgr::getInstance()->log(0, log_level_map[level], "", 0, "base_log", fmt, args);
	}


	LogManager::LogManager() :
		logger_id_(0)
	{

	}
	LogManager::~LogManager()
	{

	}

	void LogManager::setMaxLoggerCount(int count)
	{
		LoggerMgr::getInstance()->setMaxLoggerCount(count);
	}

	bool LogManager::initLogger(const std::string &log_file_path, bool log_stderr, int level_filter, int logger_id)
	{
		if (logger_id != 0) {
			logger_id_ = logger_id;
		}

		if (LoggerMgr::getInstance()->registerLogger(logger_id_, logFormatter) == false) {
			return false;
		}

		LoggerMgr::getInstance()->setLevelFilter(logger_id_, level_filter);

		if (false == log_file_path.empty()) {
			std::unique_ptr<LoggerFileSink> file_sink(new LoggerFileSink(log_file_path));
			if (file_sink->openFile() == false) {
				return false;
			}

			std::unique_ptr<LoggerAsyncSink> async_sink(new LoggerAsyncSink(file_sink.get(), 1024));
			file_sink.release();

			if (LoggerMgr::getInstance()->addSink(logger_id_, async_sink.get()) == false) {
				return false;
			}
			async_sink.release();
		}

		if (log_stderr) {
			std::unique_ptr<LoggerStderrSink> stderr_sink(new LoggerStderrSink());
			if (LoggerMgr::getInstance()->addSink(logger_id_, stderr_sink.get()) == false) {
				return false;
			}
			stderr_sink.release();
		}

		LoggerBase::getInstance()->setLogFunc(baseLogFunc);

		if (logger_id == 0) {
			logger_id_++;
		}
	
		return true;
	}

	void logNetMessage(zbluenet::LogLevel::type log_level, const char *fmt, ...)
	{
		Timestamp now;
		char time_buffer[512];
		Timestamp::format(time_buffer, sizeof(time_buffer), "%H:%M:%S", now.getSecond());
		char wrap_format[1024];

#ifdef _WIN32
		snprintf(wrap_format, sizeof(wrap_format), "[Net][%s:%03lld] %s\n", time_buffer, now.getMillisecond(), fmt);
#else
		snprintf(wrap_format, sizeof(wrap_format), "[Net][%s:%03ld] %s\n", time_buffer, now.getMillisecond(), fmt);
#endif

		va_list args;
		va_start(args, fmt);
		zbluenet::LoggerMgr::getInstance()->plainLog(zbluenet::LogManager::LoggerType::NET, log_level, wrap_format, args);
		va_end(args);
	}
}