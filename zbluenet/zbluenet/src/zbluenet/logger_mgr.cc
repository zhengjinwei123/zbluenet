#include <zbluenet/logger_mgr.h>
#include <zbluenet/logger_sink.h>

#include <vector>

namespace zbluenet {
	class LoggerMgr::Impl {
	public:
		using LogFormatter = LoggerMgr::LogFormatter;
		using LoggerVec = std::vector<Logger *>;

		Impl();
		~Impl();

		void setMaxLoggerCount(int count);
		void setMaxLogSize(int size /* = 4096 */);

		bool registerLogger(int logger_id, LogFormatter formatter /* = nullptr */, int level /* = -1 */);
		void removeLogger(int logger_id);

		bool addSink(int logger_id, LoggerSink *sink, LogFormatter formatter /* = nullptr */, int level /* = -1 */);
		void log(int logger_id, int level, const char *filename, int line, const char *function, const char *fmt, va_list args);
		void plainLog(int logger_id, int level, const char *format, va_list args);

		void setLevelFilter(int logger_id, int level);

	private:
		LoggerVec loggers_;
		int max_log_size_;
	};

	//////////////////////////////////////////////////////////////////////////
	ZBLUENET_PRECREATED_SINGLETON_IMPL(LoggerMgr)

	LoggerMgr::Impl::Impl():
		max_log_size_(0)
	{

	}

	LoggerMgr::Impl::~Impl()
	{
		for (size_t i = 0, size = loggers_.size(); i < size; ++i) {
			delete loggers_[i];
		}
	}

	void LoggerMgr::Impl::setMaxLoggerCount(int count)
	{
		if (count < 0) {
			return;
		}

		if (count < (int)loggers_.size()) {
			for (size_t i = count, size = loggers_.size(); i < size; ++i) {
				delete loggers_[i];
			}
		}
		loggers_.resize(count, nullptr);
	}

	void LoggerMgr::Impl::setMaxLogSize(int size /* = 4096 */)
	{
		if (size <= 0) {
			return;
		}
		max_log_size_ = size;
	}

	bool LoggerMgr::Impl::registerLogger(int logger_id, LogFormatter formatter /* = nullptr */, int level /* = -1 */)
	{
		if (logger_id < 0 || logger_id >= (int)loggers_.size()) {
			return false;
		}
		if (loggers_[logger_id] != nullptr) {
			return false;
		}
		loggers_[logger_id] = new Logger(formatter, level, max_log_size_);

		return true;
	}

	void LoggerMgr::Impl::removeLogger(int logger_id)
	{
		if (logger_id < 0 || logger_id >= (int)loggers_.size()) {
			return;
		}
		if (nullptr == loggers_[logger_id]) {
			return;
		}

		delete loggers_[logger_id];
		loggers_[logger_id] = nullptr;
	}

	bool LoggerMgr::Impl::addSink(int logger_id, LoggerSink *sink, LogFormatter formatter /* = nullptr */, int level /* = -1 */)
	{
		if (logger_id < 0 || logger_id >= (int)loggers_.size()) {
			return false;
		}
		if (nullptr == loggers_[logger_id]) {
			return false;
		}

		return loggers_[logger_id]->addSink(sink, formatter, level);
	}

	void LoggerMgr::Impl::log(int logger_id, int level, const char *filename, int line, const char *function, const char *fmt, va_list args)
	{
		if (logger_id < 0 || logger_id >= (int)loggers_.size()) {
			return;
		}
		if (nullptr == loggers_[logger_id]) {
			return;
		}

		loggers_[logger_id]->log(level, filename, line, function, fmt, args);
	}

	void LoggerMgr::Impl::plainLog(int logger_id, int level, const char *format, va_list args)
	{
		if (logger_id < 0 || logger_id >= (int)loggers_.size()) {
			return;
		}
		if (nullptr == loggers_[logger_id]) {
			return;
		}
		loggers_[logger_id]->plainLog(level, format, args);
	}

	void LoggerMgr::Impl::setLevelFilter(int logger_id, int level)
	{
		if (logger_id < 0 || logger_id >= (int)loggers_.size()) {
			return;
		}
		if (nullptr == loggers_[logger_id]) {
			return;
		}
		loggers_[logger_id]->setLevelFilter(level);
	}
	//////////////////////////////////////////////////////////////////////////
	LoggerMgr::LoggerMgr() :
		pimpl_(new Impl())
	{
		setMaxLoggerCount();
		setMaxLogSize();
	}

	LoggerMgr::~LoggerMgr()
	{

	}

	void LoggerMgr::setMaxLoggerCount(int count)
	{
		pimpl_->setMaxLoggerCount(count);
	}

	void LoggerMgr::setMaxLogSize(int size)
	{
		pimpl_->setMaxLogSize(size);
	}

	bool LoggerMgr::registerLogger(int logger_id, LogFormatter formatter, int level)
	{
		return pimpl_->registerLogger(logger_id, formatter, level);
	}

	void LoggerMgr::removeLogger(int logger_id)
	{
		pimpl_->removeLogger(logger_id);
	}

	bool LoggerMgr::addSink(int logger_id, LoggerSink *sink, LogFormatter formatter, int level)
	{
		return pimpl_->addSink(logger_id, sink, formatter, level);
	}

	void LoggerMgr::log(int logger_id, int level, const char *filename, int line, const char *function, const char *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		pimpl_->log(logger_id, level, filename, line, function, fmt, args);
		va_end(args);
	}

	void LoggerMgr::log(int logger_id, int level, const char *filename, int line, const char *function, const char *fmt, va_list args)
	{
		pimpl_->log(logger_id, level, filename, line, function, fmt, args);
	}

	void LoggerMgr::plainLog(int logger_id, int level, const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		pimpl_->plainLog(logger_id, level, format, args);
		va_end(args);
	}

	void LoggerMgr::plainLog(int logger_id, int level, const char *format, va_list args)
	{
		pimpl_->plainLog(logger_id, level, format, args);
	}

	void LoggerMgr::setLevelFilter(int logger_id, int level)
	{
		pimpl_->setLevelFilter(logger_id, level);
	}

} // namespace zbluenet
