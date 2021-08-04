#include <zbluenet/logger_stderr_sink.h>

#include <cstdio>

namespace zbluenet {

	LoggerStderrSink::LoggerStderrSink()
	{
	}

	LoggerStderrSink::~LoggerStderrSink()
	{
	}

	void LoggerStderrSink::log(const char *buffer, size_t size)
	{
		::fwrite(buffer, size, 1, stderr);
	}

} // namespace zbluenet
