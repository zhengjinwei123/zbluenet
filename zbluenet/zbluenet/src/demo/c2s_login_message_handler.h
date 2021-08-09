#ifndef C2S_LOGIN_MESSAGE_HANDLER_H
#define C2S_LOGIN_MESSAGE_HANDLER_H

#include <zbluenet/net/net_id.h>
#include "c2s.h"

class C2SLoginMessageHandler {
public:
	static void bind();

	static void onC2SLoginRequest(const zbluenet::net::NetId &net_id, const protocol::C2SLoginReqMessage *request);
};

#endif //C2S_LOGIN_MESSAGE_HANDLER_H
