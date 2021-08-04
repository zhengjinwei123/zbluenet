#ifndef ZBLUENET_DYNAMIC_BUFFER_H
#define ZBLUENET_DYNAMIC_BUFFER_H
#include <stdint.h>
#include <cstddef>
#include <vector>

namespace zbluenet {
	class DynamicBuffer {
	public:
		DynamicBuffer(size_t init_size = 1024,
			size_t expand_size = 1024);
		~DynamicBuffer();
		void swap(DynamicBuffer &other);

		size_t capacity() const;
		size_t discardableBytes() const;
		size_t readableBytes() const;
		size_t writableBytes() const;

		const char *readBegin() const;
		char *writeBegin();
		void read(size_t size);
		void write(size_t size);
		void reserveWritableBytes(size_t size);
		void clear();

		/* integer is big endian by default */
		bool peekInt8(uint8_t &v, size_t offset = 0);
		bool peekInt8(uint16_t &v, size_t offset = 0);
		bool peekInt8(uint32_t &v, size_t offset = 0);
		bool peekInt8(uint64_t &v, size_t offset = 0);
		bool peekInt16(uint16_t &v, size_t offset = 0, bool littleEndian = false);
		bool peekInt16(uint32_t &v, size_t offset = 0, bool littleEndian = false);
		bool peekInt16(uint64_t &v, size_t offset = 0, bool littleEndian = false);
		bool peekInt32(uint32_t &v, size_t offset = 0, bool littleEndian = false);
		bool peekInt32(uint64_t &v, size_t offset = 0, bool littleEndian = false);
		bool peekInt64(uint64_t &v, size_t offset = 0, bool littleEndian = false);
		bool readInt8(uint8_t &v);
		bool readInt8(uint16_t &v);
		bool readInt8(uint32_t &v);
		bool readInt8(uint64_t &v);
		bool readInt16(uint16_t &v, bool littleEndian = false);
		bool readInt16(uint32_t &v, bool littleEndian = false);
		bool readInt16(uint64_t &v, bool littleEndian = false);
		bool readInt32(uint32_t &v, bool littleEndian = false);
		bool readInt32(uint64_t &v, bool littleEndian = false);
		bool readInt64(uint64_t &v, bool littleEndian = false);
		void writeInt8(uint8_t v);
		void writeInt16(uint16_t v, bool littleEndian = false);
		void writeInt32(uint32_t v, bool littleEndian = false);
		void writeInt64(uint64_t v, bool littleEndian = false);
		size_t readBytes(char *buffer, size_t count);
		void writeBytes(const char *buffer, size_t count);

	private:
		std::vector<char> buffer_;
		size_t expand_size_;
		size_t read_index_;
		size_t write_index_;
	};
} // namespace zbluenet

#endif // ZBLUENET_DYNAMIC_BUFFER_H
