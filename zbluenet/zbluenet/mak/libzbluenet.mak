TARGET = build/libzbluenet

SRCS = \
src/zbluenet/timestamp.cc \
src/zbluenet/semaphore.cc \
src/zbluenet/thread.cc \
src/zbluenet/timer_heap.cc \
src/zbluenet/dynamic_buffer.cc \
src/zbluenet/log.cc \
src/zbluenet/logger.cc \
src/zbluenet/logger_async_sink.cc \
src/zbluenet/logger_base.cc \
src/zbluenet/logger_file_sink.cc \
src/zbluenet/logger_mgr.cc \
src/zbluenet/logger_stderr_sink.cc \
src/zbluenet/exchange/base_struct.cc \
src/zbluenet/net/acceptor.cc \
src/zbluenet/net/epoll_reactor.cc \
src/zbluenet/net/epoller.cc \
src/zbluenet/net/epoll_acceptor.cc \
src/zbluenet/net/fd_set.cc \
src/zbluenet/net/io_device.cc \
src/zbluenet/net/io_service.cc \
src/zbluenet/net/net_id.cc \
src/zbluenet/net/net_thread.cc \
src/zbluenet/net/network.cc \
src/zbluenet/net/reactor.cc \
src/zbluenet/net/select_acceptor.cc \
src/zbluenet/net/select_reactor.cc \
src/zbluenet/net/socket_address.cc \
src/zbluenet/net/tcp_connection.cc \
src/zbluenet/net/tcp_socket.cc \
src/zbluenet/protocol/net_command.cc \
src/zbluenet/protocol/net_protocol.cc \
src/zbluenet/server/game_server.cc \
src/zbluenet/server/game_service.cc \
src/zbluenet/server/tcp_service.cc \
src/zbluenet/net/self_pipe.cc \
src/zbluenet/net/epoll_service.cc \
src/zbluenet/client/net_client_thread.cc \
src/zbluenet/client/tcp_client_service.cc \
src/zbluenet/client/tcp_client.cc \


LINK_TYPE = static
INCLUDE = -Isrc
CPP_FLAG = -DZBLUENET_BUILD_ENABLE_BASE_LOG
BUILD_DIR = build


include mak/main.mak