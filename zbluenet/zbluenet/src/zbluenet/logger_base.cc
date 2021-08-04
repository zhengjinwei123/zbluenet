#include <zbluenet/logger_base.h>

#include <cstdio>

namespace zbluenet {

	ZBLUENET_PRECREATED_SINGLETON_IMPL(LoggerBase);

	static void defaultLogFunc(int level, const char *fmt, va_list args)
	{
		if (level < LoggerBase::LogLevel::MIN || level >= LoggerBase::LogLevel::MAX)
		{
			return;
		}

		static const char *log_level_string[] = {
			"DEBUG",
			"INFO",
			"WARNING",
			"ERROR"
		};

		::fprintf(stderr, "[%s]", log_level_string[level]);
		::vfprintf(stderr, fmt, args);

		::fprintf(stderr, "\n");
	}

	LoggerBase::LoggerBase() :
		log_func_(defaultLogFunc)
	{
	}

	LoggerBase::~LoggerBase()
	{
	}

	void LoggerBase::setLogFunc(LogFunc log_func)
	{
		log_func_ = log_func;
	}

	void LoggerBase::log(int level, const char *fmt, ...)
	{
#ifdef ZBLUENET_BUILD_ENABLE_BASE_LOG
		va_list args;
		va_start(args, fmt);
		log_func_(level, fmt, args);
		va_end(args);
#endif
	}
} // namespace zbluenet