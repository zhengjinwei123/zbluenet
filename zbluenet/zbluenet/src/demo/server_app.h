#ifndef SERVER_APP_H1
#define  SERVER_APP_H1
#include <zbluenet/net/net_id.h>
#include <zbluenet/server/game_server.h>
#include <zbluenet/class_util.h>


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

private:
	zbluenet::server::GameServer *pserver_;
	static ServerApp *instance_;

};

#endif // SERVER_APP_H1
