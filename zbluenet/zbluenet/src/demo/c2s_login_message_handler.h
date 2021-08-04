#ifndef C2S_LOGIN_MESSAGE_HANDLER_H
#define C2S_LOGIN_MESSAGE_HANDLER_H

#include <zbluenet/net/net_id.h>
#include "c2s_message.h"

class C2SLoginMessageHandler {
public:
	static void bind();

	static void onC2SLoginRequest(const zbluenet::net::NetId &net_id, const C2SLoginRequest *request);
};

#endif //C2S_LOGIN_MESSAGE_HANDLER_H
