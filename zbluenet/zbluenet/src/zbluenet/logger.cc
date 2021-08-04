#include <zbluenet/logger.h>

#include <algorithm>

namespace zbluenet {
	Logger::Logger(LogFormatter formatter, int level, int max_log_size):
		formatter_(formatter), level_(level), max_log_size_(max_log_size)
	{

	}

	Logger::~Logger()
	{
		for (size_t i = 0, size = sinks_.size(); i < size; ++i) {
			delete sinks_[i];
		}
	}

	bool Logger::addSink(LoggerSink *sink, LogFormatter formatter, int level)
	{
		sinks_.reserve(sinks_.size() + 1);
		formatters_.reserve(formatters_.size() + 1);
		levels_.reserve(levels_.size() + 1);

		sinks_.emplace_back(sink);
		formatters_.emplace_back(formatter);
		levels_.emplace_back(level);

		return true;
	}

	void Logger::log(int level, const char *filename, int line, const char* function, const char *fmt, va_list args)
	{
		if (level < level_) {
			return;
		}

		std::unique_ptr<char[]> buffer(new char[max_log_size_]);
		size_t count = 0;
		bool buffer_ready = false;

		for (size_t i = 0; i < sinks_.size(); ++i) {
			if (level < levels_[i]) {
				continue;
			}

			if (false == buffer_ready) {
				LogFormatter formatter = nullptr;

				if (formatters_[i] != nullptr) {
					formatter = formatters_[i];
				} else if (formatter_ != nullptr) {
					formatter = formatter_;
				}

				if (nullptr == formatter) {
					count = ::vsnprintf(buffer.get(), max_log_size_, fmt, args);
				} else {
					count = formatter(buffer.get(), max_log_size_, level, filename, line, function, fmt, args);
				}

				count = std::min(count, (size_t)max_log_size_);
				buffer_ready = true;
			}

			sinks_[i]->log(buffer.get(), count);
		}
	}

	void Logger::plainLog(int level, const char *fmt, va_list args)
	{
		if (level < level_) {
			return;
		}

		std::unique_ptr<char[]> buffer(new char[max_log_size_]);
		size_t count = 0;
		bool buffer_ready = false;

		for (size_t i = 0, size = sinks_.size(); i < size; ++i) {
			if (level < levels_[i]) {
				continue;
			}

			if (false == buffer_ready) {
				count = ::vsnprintf(buffer.get(), max_log_size_, fmt, args);

				count += ::snprintf(buffer.get() + count, max_log_size_ - count, "\n");

				count = std::min(count, (size_t)max_log_size_);
				buffer_ready = true;
			}

			sinks_[i]->log(buffer.get(), count);
		}
	}
}// namespace zbluenet