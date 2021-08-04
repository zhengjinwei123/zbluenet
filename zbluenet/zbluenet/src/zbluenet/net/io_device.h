#ifndef ZBLUENET_NET_IO_DEVICE_H
#define ZBLUENET_NET_IO_DEVICE_H

#include <zbluenet/class_util.h>
#include <zbluenet/net/platform.h>
#include <zbluenet/net/io_service.h>
#include <stdint.h>
#include <functional>

namespace zbluenet {
	namespace net {

		class IODevice : public Noncopyable {
		public:
			using EventCallback = std::function< void (IODevice *)>;
			using ReadCallback = std::function< int (IODevice *)>;

		public:
			IODevice();

			virtual ~IODevice();
			void setId(int64_t id);
			const int64_t getId() const;

			void setFD(SOCKET fd);
			SOCKET getFD() const;

			bool attachIOService(IOService &io_service);
			void detachIOService();

			void setReadCallback(const ReadCallback &read_cb);
			void setWriteCallback(const EventCallback &write_cb);
			void setErrorCallback(const EventCallback &error_cb);

			const ReadCallback &getReadCallback() const { return read_cb_;  }
			const EventCallback &getWriteCallback() const { return write_cb_;  }
			const EventCallback &getErrorCallback() const { return error_cb_;  }


			virtual int read(char *buffer, size_t size);
			virtual int write(const char *buffer, size_t size);
			virtual bool setNonblock();
			virtual bool setCloseOnExec();

		protected:
			int64_t id_;
			SOCKET fd_;
			ReadCallback read_cb_;
			EventCallback write_cb_;
			EventCallback error_cb_;
			IOService *io_service_;
		};

	} // namespace net
} // namespace zbluenet

#endif // ZBLUENET_NET_IO_DEVICE_H
