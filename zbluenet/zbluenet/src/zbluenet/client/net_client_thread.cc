#include <zbluenet/client/net_client_thread.h>
#include <zbluenet/log.h>

namespace zbluenet {
	namespace client {

		NetClientThread::NetClientThread(int max_recv_packet_lenth, int max_send_packet_length, const NetClientThread::CreateMessageFunc &create_message_func) :\
			NetThread(max_recv_packet_lenth, max_send_packet_length, create_message_func),
			remote_socket_id_(-1),
			peer_close_cb_(nullptr),
			error_cb_(nullptr)
		{

		}

		NetClientThread::~NetClientThread()
		{
			
		}

		void NetClientThread::push(NetCommand *cmd)
		{
			command_queue_.push(cmd);
		}

		void NetClientThread::onPeerClose(Reactor *reactor, TcpSocket::SocketId socket_id)
		{
			peer_close_cb_(socket_id);
		}

		void NetClientThread::onError(Reactor *reactor, TcpSocket::SocketId socket_id, int error)
		{
			error_cb_(socket_id);
		}

		void NetClientThread::setPeerCloseCallback(const SocketEventCallback &peer_close_cb)
		{
			peer_close_cb_ = peer_close_cb;
		}

		void NetClientThread::setErrorCallback(const SocketEventCallback &error_cb)
		{
			error_cb_ = error_cb;
		}

		void NetClientThread::onNetCommand(NetCommandQueue *queue)
		{
			NetCommand *cmd_raw = nullptr;
#ifdef _WIN32
			while (command_queue_.pop(cmd_raw)) {
#else
			while (queue->pop(cmd_raw)) {
#endif
				if (nullptr == cmd_raw) {
					quit();
					return;
				}
				std::unique_ptr<NetCommand> cmd(cmd_raw);
				if (NetCommand::Type::MESSAGE == cmd->type) {
					if (net_protocol_.writeMessage(cmd->message_id, cmd->message, &encode_buffer_) == false) {
						LOG_MESSAGE_ERROR("encode message(%d) failed on net_id(%d:%lx)", cmd->message_id, id_, cmd->id.socket_id);
						
						peer_close_cb_(0);
						return;
					}
					
					if (false == reactor_->sendMessage(cmd->id.socket_id, encode_buffer_.readBegin(), encode_buffer_.readableBytes())) {
						peer_close_cb_(0);
						return;
					}
				}
			}
		}
	} // namespace zbluenet
} // namespace zbluenet