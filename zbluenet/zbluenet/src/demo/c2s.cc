#include "c2s.h"
// WARNING:  this code generate by prototransfer tool, do not edit it.


namespace protocol {

	//**************************C2SLoginReqMessage*********************
	C2SLoginReqMessage::C2SLoginReqMessage()
	{

	}

	C2SLoginReqMessage::~C2SLoginReqMessage()
	{

	}

	void C2SLoginReqMessage::swap(C2SLoginReqMessage &other)
	{
		this->data.Swap(&other.data);
	}

	int C2SLoginReqMessage::encode(char *buffer, size_t size) const
	{
		if (!data.SerializeToArray(buffer, size)) {
			return -1;
		}

		return data.ByteSizeLong();
	}

	int C2SLoginReqMessage::decode(const char *buffer, size_t size)
	{
		std::string str(buffer, size);
		if (false == data.ParseFromString(str)) {
			return 0;
		}

		return size;
	}

	std::string C2SLoginReqMessage::dump() const
	{
		return this->data.DebugString();
	}


	//**************************S2CLoginRespMessage*********************
	S2CLoginRespMessage::S2CLoginRespMessage()
	{

	}

	S2CLoginRespMessage::~S2CLoginRespMessage()
	{

	}

	void S2CLoginRespMessage::swap(S2CLoginRespMessage &other)
	{
		this->data.Swap(&other.data);
	}

	int S2CLoginRespMessage::encode(char *buffer, size_t size) const
	{
		if (!data.SerializeToArray(buffer, size)) {
			return -1;
		}

		return data.ByteSizeLong();
	}

	int S2CLoginRespMessage::decode(const char *buffer, size_t size)
	{
		std::string str(buffer, size);
		if (false == data.ParseFromString(str)) {
			return 0;
		}

		return size;
	}

	std::string S2CLoginRespMessage::dump() const
	{
		return this->data.DebugString();
	}


	//**************************C2SLogoutReqMessage*********************
	C2SLogoutReqMessage::C2SLogoutReqMessage()
	{

	}

	C2SLogoutReqMessage::~C2SLogoutReqMessage()
	{

	}

	void C2SLogoutReqMessage::swap(C2SLogoutReqMessage &other)
	{
		this->data.Swap(&other.data);
	}

	int C2SLogoutReqMessage::encode(char *buffer, size_t size) const
	{
		if (!data.SerializeToArray(buffer, size)) {
			return -1;
		}

		return data.ByteSizeLong();
	}

	int C2SLogoutReqMessage::decode(const char *buffer, size_t size)
	{
		std::string str(buffer, size);
		if (false == data.ParseFromString(str)) {
			return 0;
		}

		return size;
	}

	std::string C2SLogoutReqMessage::dump() const
	{
		return this->data.DebugString();
	}


	//**************************S2CLogoutRespMessage*********************
	S2CLogoutRespMessage::S2CLogoutRespMessage()
	{

	}

	S2CLogoutRespMessage::~S2CLogoutRespMessage()
	{

	}

	void S2CLogoutRespMessage::swap(S2CLogoutRespMessage &other)
	{
		this->data.Swap(&other.data);
	}

	int S2CLogoutRespMessage::encode(char *buffer, size_t size) const
	{
		if (!data.SerializeToArray(buffer, size)) {
			return -1;
		}

		return data.ByteSizeLong();
	}

	int S2CLogoutRespMessage::decode(const char *buffer, size_t size)
	{
		std::string str(buffer, size);
		if (false == data.ParseFromString(str)) {
			return 0;
		}

		return size;
	}

	std::string S2CLogoutRespMessage::dump() const
	{
		return this->data.DebugString();
	}


} // namespace protocol
