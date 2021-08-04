#ifndef ZBLUENET_PROTOCOL_NET_COMMAND_H
#define ZBLUENET_PROTOCOL_NET_COMMAND_H

#include <zbluenet/net/net_id.h>
#include <zbluenet/class_util.h>

namespace zbluenet {

	namespace exchange {
		class BaseStruct;
	}

	using net::NetId;

	namespace protocol {
		
		class NetCommand : public Noncopyable {
		public:
			struct Type {
				enum type {
					NEW,
					MESSAGE,
					ENABLE_BROADCAST,
					DISABLE_BROADCAST,
					BROADCAST,
					CLOSE,
				};
			};

			NetCommand(Type::type typ);
			~NetCommand();

		public:
			Type::type type;
			NetId id;
			int message_id;
			zbluenet::exchange::BaseStruct *message;
		};
	} // namespace net
} // namespace zbluenet

#endif // ZBLUENET_PROTOCOL_NET_COMMAND_H
