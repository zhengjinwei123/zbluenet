#include <zbluenet/logger_file_sink.h>
#include <zbluenet/timestamp.h>

#include <cstdio>

namespace zbluenet {

	class LoggerFileSink::Impl {
	public:
		explicit Impl(const std::string &file_path_name);
		~Impl();

		void log(const char *buffer, size_t size);
		bool openFile();

	private:
		std::string file_path_;
		std::string actual_file_path_;
		FILE *fp_;
		size_t file_size_;
	};
	//////////////////////////////////////////////////////////////////////////
	LoggerFileSink::Impl::Impl(const std::string &file_path_name):
		file_path_(file_path_name),
		actual_file_path_(""),
		fp_(nullptr)
	{

	}
	LoggerFileSink::Impl::~Impl()
	{
		if (fp_ != nullptr) {
			::fclose(fp_);
		}
	}

	void LoggerFileSink::Impl::log(const char *buffer, size_t size)
	{
		if (openFile() == false) {
			return;
		}
		::fwrite(buffer, size, 1, fp_);
	}

	bool LoggerFileSink::Impl::openFile()
	{
		char actual_file_path[1024];

		Timestamp now;
		Timestamp::format(actual_file_path, sizeof(actual_file_path), file_path_.c_str(), now.getSecond());

		if (actual_file_path_ == actual_file_path) {
			return true;
		}
		actual_file_path_ = actual_file_path;

		FILE *fp = ::fopen(actual_file_path_.c_str(), "a");
		if (nullptr == fp) {
			return false;
		}

		if (fp_ != nullptr) {
			::fclose(fp_);
		}
		::setbuf(fp, nullptr);

		fp_ = fp;

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	LoggerFileSink::LoggerFileSink(const std::string &file_path_name) :
		pimpl_(new Impl(file_path_name))
	{

	}

	LoggerFileSink::~LoggerFileSink()
	{

	}

	bool LoggerFileSink::openFile()
	{
		return pimpl_->openFile();
	}

	void LoggerFileSink::log(const char *buffer, size_t size)
	{
		pimpl_->log(buffer, size);
	}

} // namespace zbluenet