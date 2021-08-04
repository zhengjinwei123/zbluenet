#include <zbluenet/net/net_id.h>

namespace zbluenet {
	namespace net {
		size_t NetId::Hash::operator()(const NetId &net_id) const
		{
			return net_id.socket_id;
		}

		NetId::NetId() :
			reactor_id(-1),
			socket_id(-1)
		{

		}

		void NetId::reset()
		{
			this->socket_id = -1;
			this->reactor_id = -1;
		}

		NetId& NetId::operator=(const NetId &other)
		{
			if (this == &other) {
				return *this;
			}
			this->reactor_id = other.reactor_id;
			this->socket_id = other.socket_id;
			return *this;
		}

		bool NetId::operator==(const NetId &other) const
		{
			return (this->reactor_id == other.reactor_id) && (this->socket_id == other.socket_id);
		}

		bool NetId::isInvalid() const
		{
			return (this->reactor_id == -1) || (this->socket_id == -1);
		}
	} // namespace net
}// namespace zbluenet