#ifndef ZBLUENET_NET_NET_THREAD_H
#define ZBLUENET_NET_NET_THREAD_H

#include <functional>
#include <memory>
#include <set>

#include <zbluenet/concurrent_queue.h>
#include <zbluenet/net/tcp_socket.h>
#include <zbluenet/thread.h>
#include <zbluenet/net/reactor.h>
#include <zbluenet/protocol/net_protocol.h>
#include <zbluenet/protocol/net_command.h>
#include <zbluenet/net/message_queue.h>

namespace zbluenet {

	class DynamicBuffer;

	namespace exchange {
		class BaseStruct;
	}

	using protocol::NetProtocol;
	using protocol::NetCommand;

	namespace net {

		class NetId;
		class TcpSocket;

		class NetThread {
		public:
			using CreateMessageFunc = std::function<zbluenet::exchange::BaseStruct * (int)>; // ������Ϣʵ��Ľӿ�
			using NewNetCommandCallback = std::function<void (std::unique_ptr<NetCommand> &)>; // ������Ϣ�Ļص�����, ���̴߳��ݽ�����
			using NetCommandQueue = MessageQueue<NetCommand *>; // ������Ϣ�Ķ���
			using BroadcastList = std::set<TcpSocket::SocketId>;
			using RecvMessageCallback = std::function< void (NetThread *, TcpSocket::SocketId, DynamicBuffer *, const NewNetCommandCallback &)>;

			NetThread( int max_recv_packet_lenth, int max_send_packet_length, const CreateMessageFunc &create_message_func);
			~NetThread();

			bool init(int id,
				int max_client_num,
				int max_recv_buffer_size,
				int max_send_buffer_size,
				const NewNetCommandCallback &new_net_cmd_cb,
				const RecvMessageCallback &recv_message_cb);

			void start(const Thread::EventCallback &onBefore, const Thread::EventCallback &onEnd); // �����߳�
			void stop();
			void push(NetCommand *cmd); // �����������̷߳���������Ϣ
			void attach(std::unique_ptr<TcpSocket> &peer_socket); // ���̸߳��� �����ӵ���

			const size_t getConnectionNum() const { return reactor_->getConnectionNum(); }
			void closeSocket(TcpSocket::SocketId socket_id);

			const int getId() const { return id_;  }

			IOService::TimerId startTimer(int64_t timeout_ms, const IOService::TimerCallback &timer_cb, int call_times = -1);
			void stopTimer(IOService::TimerId timer_id);

		protected:
			
			void sendNetCommandClose(TcpSocket::SocketId socket_id);
			void sendNetCommandNew(TcpSocket::SocketId socket_id);

			void onRecvMessage(Reactor *reactor, TcpSocket::SocketId socket_id, DynamicBuffer *buffer);
			
			virtual void onNetCommand(NetCommandQueue *queue = nullptr);
			virtual void onPeerClose(Reactor *reactor, TcpSocket::SocketId socket_id);
			virtual void onError(Reactor *reactor, TcpSocket::SocketId socket_id, int error);

			void quit();

		protected:
			int id_;
			Thread thread_;
			Reactor *reactor_;
			NetCommandQueue command_queue_; // ������Ϣ�Ķ���
			NetProtocol net_protocol_; // ��Ϣ����
			DynamicBuffer encode_buffer_; // ��Ϣ����
			NewNetCommandCallback new_net_cmd_cb_; // ������Ϣ�ģ������̴��ݽ����� ��Ϣ���պ�ŵ������̵���Ϣ������
			BroadcastList broadcast_list_;
			RecvMessageCallback recv_message_cb_; // ��Ϣ���˵Ļص������� �ⲿ�Լ����
		};

	} // namespace net
} // namespace zbluenet

#endif // ZBLUENET_NET_NET_THREAD_H
