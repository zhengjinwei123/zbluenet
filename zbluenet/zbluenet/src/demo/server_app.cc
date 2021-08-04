#include "server_app.h"

#include <zbluenet/log.h>
#include <memory>
#include <functional>
#include "c2s_login_message_handler.h"

ServerApp *ServerApp::instance_ = nullptr;


ServerApp::ServerApp():pserver_(nullptr)
{

}

void ServerApp::onConnect(const zbluenet::net::NetId &net_id)
{
	LOG_DEBUG("on connect : %d | %lld", net_id.reactor_id, net_id.socket_id);
}

void ServerApp::onDisconnect(const zbluenet::net::NetId &net_id)
{
	LOG_DEBUG("on disconnect : %d | %lld", net_id.reactor_id, net_id.socket_id);
}

zbluenet::server::GameServer* ServerApp::getServer() {
	return pserver_;
}

void ServerApp::start()
{
	zbluenet::LogManager::getInstance()->setMaxLoggerCount(2);

	pserver_ = new zbluenet::server::GameServer("10.235.200.249", 9091, 2);// 127.0.0.1
	std::string log_file_main = "./zbluenet.%Y%m%d.log";
	pserver_->initMainLogger(log_file_main, zbluenet::LogLevel::DEBUG, true);
	std::string log_file_net = "./net.%Y%m%d.log";
	pserver_->initNetLogger(log_file_net, zbluenet::LogLevel::DEBUG, true);

	// 服务初始化
	if (false == pserver_->init(
		std::bind(&ServerApp::onConnect, this, std::placeholders::_1),
		std::bind(&ServerApp::onDisconnect, this, std::placeholders::_1),
		nullptr, 30000)) {
		LOG_DEBUG("server initialized fail");
		return;
	}

	// 注册消息
	C2SLoginMessageHandler::bind();

	// 开启定时器
	/*pserver_->startTimer(1000, [](int64_t timer_id)-> void {
		LOG_DEBUG("timer %lld called", timer_id);
	}, 10);

	pserver_->startTimer(1001, [](int64_t timer_id)-> void {
		LOG_DEBUG("timer %lld called", timer_id);
	});*/

	// 启动服务
	pserver_->start();
}
