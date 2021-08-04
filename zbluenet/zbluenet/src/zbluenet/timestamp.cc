#include <zbluenet/timestamp.h>

#ifdef _WIN32

#else
#include <sys/times.h>
#endif

namespace zbluenet {
	Timestamp::Timestamp():
		second_(0), nanosecond_(0)
	{
		setNow();
	}

	Timestamp::~Timestamp()
	{

	}

	time_t Timestamp::getNowInMillisecond()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	time_t Timestamp::getNow()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	void Timestamp::setNow()
	{
		int64_t nanosecond = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		second_ = nanosecond / 1000000000;
		nanosecond_ = nanosecond - (second_ * 1000000000);
	}

	time_t Timestamp::getSecond() const
	{
		return second_;
	}

	int64_t Timestamp::getMillisecond() const
	{
		return nanosecond_ / 1000000;
	}

	int64_t Timestamp::getNanosecond() const
	{
		return nanosecond_;
	}

	Timestamp &Timestamp::operator+=(int64_t millisecond)
	{
		second_ += millisecond / 1000;
		nanosecond_ += millisecond % 1000 * 1000000;
		if (nanosecond_ >= 1000000000) {
			second_ += 1;
			nanosecond_ -= 1000000000;
		}

		return *this;
	}

	bool Timestamp::operator<(const Timestamp &other) const
	{
		if (second_ != other.second_) {
			return second_ < other.second_;
		}
		return nanosecond_ < other.nanosecond_;
	}

	Timestamp Timestamp::operator+(int64_t millisecond) const
	{
		Timestamp t(*this);
		t += millisecond;

		return t;
	}

	bool Timestamp::millisecondLess(const Timestamp &other) const
	{
		if (second_ != other.second_) {
			return second_ < other.second_;
		}
		return getMillisecond() < other.getMillisecond();
	}

	bool Timestamp::millisecondEqual(const Timestamp &other) const
	{
		return (second_ == other.second_) && (getMillisecond() == other.getMillisecond());
	}

	int64_t Timestamp::distanceSecond(const Timestamp &other) const
	{
		if (second_ > other.second_) {
			return second_ - other.second_;
		}
		return other.second_ - second_;
	}


	int64_t Timestamp::distanceMillisecond(const Timestamp &other) const
	{
		const Timestamp *bigger = nullptr;
		const Timestamp *smaller = nullptr;
		if (*this < other) {
			bigger = &other;
			smaller = this;
		}
		else {
			bigger = this;
			smaller = &other;
		}

		return (bigger->second_ - smaller->second_) * 1000 + (bigger->getMillisecond() - smaller->getMillisecond());
	}

	size_t Timestamp::format(char *buffer, size_t size, const char* format, time_t second)
	{
		struct tm t;
#ifdef _WIN32
		::localtime_s(&t, &second);
#else
		::localtime_r(&second, &t);
#endif
		return strftime(buffer, size, format, &t);
	}


} // namespace zblue