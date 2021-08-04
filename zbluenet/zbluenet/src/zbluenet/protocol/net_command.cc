#include <zbluenet/protocol/net_command.h>

#include <cstddef>
#include <zbluenet/exchange/base_struct.h>

namespace zbluenet {
	namespace protocol {

		NetCommand::NetCommand(Type::type typ) :
			type(typ),
			message_id(0),
			message(nullptr)
		{

		}

		NetCommand::~NetCommand()
		{
			if (this->message != nullptr) {
				delete this->message;
			}
		}

	} //
} // namespace zbluenet