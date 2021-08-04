#ifndef ZBLUENET_PROTOCOL_NET_PROTOCOL_H
#define ZBLUENET_PROTOCOL_NET_PROTOCOL_H

#include <zbluenet/class_util.h>
#include <functional>
#include <memory>

namespace zbluenet {

	class DynamicBuffer;

	namespace exchange {
		class BaseStruct;
	}

	namespace protocol {

		class NetProtocol : public Noncopyable {
		public:
			struct RetCode
			{
				enum type {
					ERR = -1,
					WAITING_MORE_DATA = 0,
					MESSAGE_READY = 1,
				};
			};

			using CreateMessageFunc = std::function<zbluenet::exchange::BaseStruct * (int)>;

			NetProtocol(int max_packet_length, const CreateMessageFunc &create_message_func);
			~NetProtocol();

			RetCode::type recvMessage(DynamicBuffer *buffer, int *ret_message_id, std::unique_ptr<zbluenet::exchange::BaseStruct> &ret_message);
			bool writeMessage(int message_id, const zbluenet::exchange::BaseStruct *message, DynamicBuffer *buffer);


		private:
			int max_packet_length_;
			CreateMessageFunc create_message_func_;
		};

	} // namespace protocol
} // namespace zbluenet

#endif // ZBLUENET_PROTOCOL_NET_PROTOCOL_H
