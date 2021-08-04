#ifndef ZBLUENET_TIMESTAMP_H
#define ZBLUENET_TIMESTAMP_H

#include <chrono>
#include <ctime>
#include <cstddef>

namespace zbluenet {
	/************************************************************************/
	/*  1second = 1000 millisecond = 1000 * 1000 microsecond = 1000 * 1000 * 1000 nanosecond                                                                     */
	/************************************************************************/

	class Timestamp {
	public:
		Timestamp();
		~Timestamp();

		// 获取当前时间戳 毫秒级别
		static time_t getNowInMillisecond();

		// 获取当前时间戳 秒级别
		static time_t getNow();

		void setNow();

		time_t getSecond() const;
		int64_t getMillisecond() const;
		int64_t getNanosecond() const;

		Timestamp &operator+=(int64_t millisecond);
		bool operator<(const Timestamp &other) const;
		Timestamp operator+(int64_t millisecond) const;

		bool millisecondLess(const Timestamp &other) const;
		bool millisecondEqual(const Timestamp &other) const;
		int64_t distanceSecond(const Timestamp &other) const;
		int64_t distanceMillisecond(const Timestamp &other) const;

		/************************************************************************/
		/* 	char filePath[1024];
		std::string file_path = "zjw-%Y%m%d %H:%M:%S.log";
		zblue::Timestamp::format(filePath, sizeof(filePath), file_path.c_str(), tnow.getSecond());
		printf("filepath: %s \n", filePath);                                                                     */
		/************************************************************************/
		static size_t format(char *buffer, size_t size, const char* format, time_t second);
	private:
		time_t second_;
		int64_t nanosecond_;
	};


} // namespace zbluenet

#endif // ZBLUENET_TIMESTAMP_H
