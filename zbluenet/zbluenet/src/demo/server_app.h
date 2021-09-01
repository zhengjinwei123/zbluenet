#ifndef SERVER_APP_H1
#define  SERVER_APP_H1
#include <zbluenet/net/net_id.h>
#include <zbluenet/server/game_server.h>
#include <zbluenet/class_util.h>
#include <functional>
#include <zbluenet/client/tcp_client.h>

#include <memory>

#include "c2s.h"

class ServerApp {
private:
	ServerApp();

	ServerApp(ServerApp&) = delete;
	ServerApp& operator=(const ServerApp&) = delete;

public:
	~ServerApp()
	{
		if (instance_ != nullptr) {
			delete instance_;
		}
	}

	static ServerApp* getInstance() {
		if (instance_ == nullptr) {
			instance_ = new ServerApp();
		}
		return instance_;
	}

	void onConnect(const zbluenet::net::NetId &net_id);
	void onDisconnect(const zbluenet::net::NetId &net_id);
	zbluenet::server::GameServer* getServer();

	void start();

	template <typename AnyType>
	void registerMessageHandler(const std::function<void(const zbluenet::net::NetId &, const AnyType *)> &message_handler)
	{
		static const int message_id = protocol::C2SMessageType::id<AnyType>::value;
		pserver_->registerMessageHandlerFunc(message_id, message_handler);
	}

	template <typename AnyType>
	void sendMessage(const zbluenet::net::NetId &net_id, std::unique_ptr <AnyType> &messsage)
	{
		static const int message_id = protocol::C2SMessageType::id<AnyType>::value;
		pserver_->sendMessage(net_id, message_id, messsage);
	}

	template<typename AnyType>
	void broadcastMessage(std::unique_ptr <AnyType> &messsage)
	{
		static const int message_id = protocol::C2SMessageType::id<AnyType>::value;
		pserver_->broadcastMessage(message_id, messsage);
	}

private:
	zbluenet::server::GameServer *pserver_;
	static ServerApp *instance_;

	std::unique_ptr<zbluenet::client::TcpClient> battle_server_terminal_;
};

#define sServerApp ServerApp::getInstance()

#endif // SERVER_APP_H1
