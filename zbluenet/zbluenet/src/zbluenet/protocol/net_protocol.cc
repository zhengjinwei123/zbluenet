#include <zbluenet/protocol/net_protocol.h>

#include <zbluenet/dynamic_buffer.h>
#include <zbluenet/exchange/base_struct.h>

namespace zbluenet {
	namespace protocol {

		NetProtocol::NetProtocol(int max_packet_length, const CreateMessageFunc &create_message_func):
			max_packet_length_(max_packet_length),
			create_message_func_(create_message_func)
		{

		}

		NetProtocol::~NetProtocol()
		{

		}

		NetProtocol::RetCode::type NetProtocol::recvMessage(DynamicBuffer *buffer, int *ret_message_id, std::unique_ptr<zbluenet::exchange::BaseStruct> &ret_message)
		{
			// 消息id
			uint16_t message_id = 0;
			if (buffer->peekInt16(message_id, 0) == false) {
				return RetCode::WAITING_MORE_DATA;
			}

			// 消息長度
			uint16_t message_length = 0;
			if (buffer->peekInt16(message_length, 2) == false) {
				return RetCode::WAITING_MORE_DATA;
			}

			// 檢查長度
			if ((int)message_length + 4 > max_packet_length_) {
				return RetCode::ERR;
			}

			if (buffer->readableBytes() < (size_t)(message_length + 4)) {
				return RetCode::WAITING_MORE_DATA;
			}

			// 創建消息
			std::unique_ptr<zbluenet::exchange::BaseStruct> message(create_message_func_(message_id));
			if (message.get() == nullptr) {
				return RetCode::ERR;
			}

			// 解包
			if  (message->decode(buffer->readBegin() + 4, message_length) < 0) {
				return RetCode::ERR;
			}

			buffer->read(message_length + 4);

			// 返回消息id 和消息

			*ret_message_id = message_id;
			ret_message.swap(message);

			return RetCode::MESSAGE_READY;
		}

		bool NetProtocol::writeMessage(int message_id, const zbluenet::exchange::BaseStruct *message, DynamicBuffer *buffer)
		{
			buffer->clear();

			// write message id
			buffer->writeInt16(message_id);

			// write message(preallocate message length)
			int encode_size = message->encode(
				buffer->writeBegin() + 2,
				buffer->writableBytes() - 2);
			if (encode_size < 0) {
				return false;
			}

			// write message length
			buffer->writeInt16(encode_size);
			buffer->write(encode_size);

			return true;
		}

	} // namespace protocol
} // namespace zbluenet