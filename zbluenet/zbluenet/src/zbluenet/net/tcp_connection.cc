#include <zbluenet/net/tcp_connection.h>

namespace zbluenet {
	namespace net {

		TcpConnection::TcpConnection(TcpSocket *socket, size_t read_buffer_init_size,
			size_t read_buffer_expand_size,
			size_t write_buffer_init_size,
			size_t write_buffer_expand_size) :
			socket_(socket),
			status_(Status::NONE),
			error_code_(0),
			read_buffer_(read_buffer_init_size, read_buffer_expand_size),
			write_buffer_(write_buffer_init_size, write_buffer_expand_size)
		{

		}

		void TcpConnection::setError(int error_code)
		{
			status_ = Status::PENDING_ERROR;
			error_code_ = error_code;
		}

	} // namespace net

} // namespace zbluenet