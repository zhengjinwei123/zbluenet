#ifndef ZBLUENET_LOGGER_FILE_SINK_H
#define ZBLUENET_LOGGER_FILE_SINK_H


#include <cstddef>
#include <string>
#include <memory>

#include <zbluenet/class_util.h>
#include <zbluenet/logger_sink.h>

namespace zbluenet {
	class LoggerFileSink : public LoggerSink {
	public:
		explicit LoggerFileSink(const std::string &file_path_name);
		~LoggerFileSink();

		bool openFile();

		void log(const char *buffer, size_t size);

	private:
		class Impl;
		std::unique_ptr<Impl> pimpl_;
	};

} // namespace zbluenet

#endif // ZBLUENET_LOGGER_FILE_H
