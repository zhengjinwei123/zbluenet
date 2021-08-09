#include "c2s_login_message_handler.h"

#include <zbluenet/log.h>
#include <functional>
#include <memory>

#include "server_app.h"

void C2SLoginMessageHandler::bind()
{
	sServerApp->registerMessageHandler<protocol::C2SLoginReqMessage>(
		std::bind(&C2SLoginMessageHandler::onC2SLoginRequest, std::placeholders::_1, std::placeholders::_2));
}

void C2SLoginMessageHandler::onC2SLoginRequest(const zbluenet::net::NetId &net_id, const protocol::C2SLoginReqMessage *request)
{
	LOG_DEBUG("zjw called %s", request->dump().c_str());

	std::unique_ptr<protocol::S2CLoginRespMessage> message(new protocol::S2CLoginRespMessage());
	message->data.set_ret(9999);
	sServerApp->sendMessage(net_id, message);
}
