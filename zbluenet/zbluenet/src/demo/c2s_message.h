#ifndef CS2_MESSAGE_H
#define CS2_MESSAGE_H

#include <stdint.h>
#include <cstddef>
#include <string>
#include "proto/test.pb.h"

#include <zbluenet/exchange/base_struct.h>


class C2SLoginRequest : public zbluenet::exchange::BaseStruct {
public:
	C2SLoginRequest();
	~C2SLoginRequest();

	void swap(C2SLoginRequest &other);
	static zbluenet::exchange::BaseStruct *create() { return new C2SLoginRequest();  }
	virtual int encode(char *buffer, size_t size) const;
	virtual int decode(const char *buffer, size_t size);
	virtual C2SLoginRequest *clone() const { return new C2SLoginRequest(*this);  }

	virtual std::string dump() const;

public:
	Login data;
};


struct C2SMessageType {
	enum type {
		C2S_LOGIN_REQUEST = 1001,
		S2C_LOGIN_RESPONSE = 1002,
	};

	template <typename AnyType>
	struct id;

	static zbluenet::exchange::BaseStruct *create(int id);
};

template <>
struct C2SMessageType::id<C2SLoginRequest> {
	static const int value = C2S_LOGIN_REQUEST;
};


#endif
