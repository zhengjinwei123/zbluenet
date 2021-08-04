#ifndef ZBLUENET_LOGGER_BASE_H
#define ZBLUENET_LOGGER_BASE_H

#include <cstdarg>
#include <functional>
#include <zbluenet/class_util.h>

namespace zbluenet {
	class LoggerBase {
	public:
		struct  LogLevel
		{
			enum type {
				MIN = 0,

				DEBUG = 0,
				INFO,
				WARNING,
				ERR,

				MAX
			};
		};


		using LogFunc = std::function<void(int level, const char *fmt, va_list args)>;

		void setLogFunc(LogFunc log_func);
		void log(int level, const char *fmt, ...);

	private:
		ZBLEUNET_PRECREATED_SINGLETON(LoggerBase);

		LogFunc log_func_;
	};
} // namespace zbluenet

#define BASE_DEBUG(fmt, ...) \
	zbluenet::LoggerBase::getInstance()->log(zbluenet::LoggerBase::LogLevel::DEBUG, fmt, ##__VA_ARGS__)

#define BASE_INFO(fmt, ...) \
	zbluenet::LoggerBase::getInstance()->log(zbluenet::LoggerBase::LogLevel::INFO, fmt, ##__VA_ARGS__)

#define BASE_WARNING(fmt, ...) \
	zbluenet::LoggerBase::getInstance()->log(zbluenet::LoggerBase::LogLevel::WARNING, fmt, ##__VA_ARGS__)

#define BASE_ERROR(fmt, ...) \
	zbluenet::LoggerBase::getInstance()->log(zbluenet::LoggerBase::LogLevel::ERR, fmt, ##__VA_ARGS__)

#endif // ZBLUENET_LOGGER_BASE_H
