# zbluenet
基于c++11 异步多线程跨平台(linux + windows)网络库，适用于网络游戏


##### 1. 跨平台
```
1. 支持windows 平台, 使用vs2017 编译
2. 支持 linux 平台， make 一键编译
```

#### 2. 异步多线程事件驱动
```
1. windows 上使用 select 模式， 最高支持 2048 个socket 事件
2. linux 上使用epoll
3. 一个accept 线程， 多个 reactor 线程,  reactor 线程由用户配置， acceptor 线程负责均衡各个 reactor 线程处理的socket 数
4. reactor 线程负责处理 收包，发包， 解包，封包
```

#### 3. 主逻辑单线程，不并发
```
1. 各个reactor 线程把解包的数据发送到主线程， 主线程有个消息队列， 排队处理消息
```

#### 4. c++11语法
```
1. 程序中广泛使用到了lambda 表达式， 仿函数， 智能指针
2. 只使用c++ 标准库, 没有使用任何第三方算法库
```

#### 5. 支持使用 google protobuffer 通讯


#### 6. example


启动

```c++
	zbluenet::LogManager::getInstance()->setMaxLoggerCount(2);

	pserver_ = new zbluenet::server::GameServer("127.0.0.1", 9091, 2);
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
	pserver_->startTimer(1000, [](int64_t timer_id)-> void {
		LOG_DEBUG("timer %lld called", timer_id);
	}, 10);

	pserver_->startTimer(1001, [](int64_t timer_id)-> void {
		LOG_DEBUG("timer %lld called", timer_id);
	});

	// 启动服务
	pserver_->start();
```

注册消息处理器
```c++
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

```

