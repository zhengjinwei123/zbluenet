#include "c2s_login_message_handler.h"

#include <zbluenet/log.h>

#include <functional>
#include <memory>

#include "c2s_message.h"
#include "server_app.h"

void C2SLoginMessageHandler::bind()
{
	ServerApp::getInstance()->getServer()->registerMessageHandlerFunc<C2SLoginRequest>(1001, 
		std::bind(&C2SLoginMessageHandler::onC2SLoginRequest, std::placeholders::_1, std::placeholders::_2));
}

void C2SLoginMessageHandler::onC2SLoginRequest(const zbluenet::net::NetId &net_id, const C2SLoginRequest *request)
{
	LOG_DEBUG("zjw called %s", request->dump().c_str());

	std::unique_ptr<C2SLoginRequest> message(new C2SLoginRequest());
	message->data.set_account("jiangxi");
	message->data.set_age(99);
	message->data.add_friends(900);
	message->data.add_friends(901);

	ServerApp::getInstance()->getServer()->sendMessage<C2SLoginRequest>(net_id, 1001, message);
}
