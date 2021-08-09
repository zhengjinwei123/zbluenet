#ifndef PROTOCOL_C2S_MESSAGE_H
#define PROTOCOL_C2S_MESSAGE_H
// WARNING:  this code generate by prototransfer tool, do not edit it.

#include <stdint.h>
#include <cstddef>
#include <string>
#include <zbluenet/exchange/base_struct.h>


#include "./proto/c2s.pb.h"


namespace protocol {

	class C2SLoginReqMessage : public zbluenet::exchange::BaseStruct {
	public:
		C2SLoginReqMessage();
		~C2SLoginReqMessage();

		void swap(C2SLoginReqMessage&other);
		static zbluenet::exchange::BaseStruct *create() { return new C2SLoginReqMessage(); }
		virtual int encode(char *buffer, size_t size) const;
		virtual int decode(const char *buffer, size_t size);
		virtual C2SLoginReqMessage *clone() const { return new C2SLoginReqMessage(*this); }
		virtual std::string dump() const;

	public:
		protocol::C2SLoginReq data;
	};

	class S2CLoginRespMessage : public zbluenet::exchange::BaseStruct {
	public:
		S2CLoginRespMessage();
		~S2CLoginRespMessage();

		void swap(S2CLoginRespMessage&other);
		static zbluenet::exchange::BaseStruct *create() { return new S2CLoginRespMessage(); }
		virtual int encode(char *buffer, size_t size) const;
		virtual int decode(const char *buffer, size_t size);
		virtual S2CLoginRespMessage *clone() const { return new S2CLoginRespMessage(*this); }
		virtual std::string dump() const;

	public:
		protocol::S2CLoginResp data;
	};

	class C2SLogoutReqMessage : public zbluenet::exchange::BaseStruct {
	public:
		C2SLogoutReqMessage();
		~C2SLogoutReqMessage();

		void swap(C2SLogoutReqMessage&other);
		static zbluenet::exchange::BaseStruct *create() { return new C2SLogoutReqMessage(); }
		virtual int encode(char *buffer, size_t size) const;
		virtual int decode(const char *buffer, size_t size);
		virtual C2SLogoutReqMessage *clone() const { return new C2SLogoutReqMessage(*this); }
		virtual std::string dump() const;

	public:
		protocol::C2SLogoutReq data;
	};

	class S2CLogoutRespMessage : public zbluenet::exchange::BaseStruct {
	public:
		S2CLogoutRespMessage();
		~S2CLogoutRespMessage();

		void swap(S2CLogoutRespMessage&other);
		static zbluenet::exchange::BaseStruct *create() { return new S2CLogoutRespMessage(); }
		virtual int encode(char *buffer, size_t size) const;
		virtual int decode(const char *buffer, size_t size);
		virtual S2CLogoutRespMessage *clone() const { return new S2CLogoutRespMessage(*this); }
		virtual std::string dump() const;

	public:
		protocol::S2CLogoutResp data;
	};


	struct C2SMessageType {
		template <typename AnyType>
		struct id;

		zbluenet::exchange::BaseStruct *create(int id);
	};

	template<>
	struct C2SMessageType::id< C2SLoginReqMessage> {
		static const int value = protocol::C2S_LOGIN_REQ;
	};

	template<>
	struct C2SMessageType::id< S2CLoginRespMessage> {
		static const int value = protocol::S2C_LOGIN_RESP;
	};

	template<>
	struct C2SMessageType::id< C2SLogoutReqMessage> {
		static const int value = protocol::C2S_LOGOUT_REQ;
	};

	template<>
	struct C2SMessageType::id< S2CLogoutRespMessage> {
		static const int value = protocol::S2C_LOGOUT_RESP;
	};

} // namespace protocol
#endif