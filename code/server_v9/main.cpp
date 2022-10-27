#include"src/Server.h"
#include<stdio.h>
#include<functional>
#include<thread>

void onMessage(const ConnectionPtr& conn, Buffer* buf) {
	std::string msg(buf->retrieveAllAsString());
	printf("onMessage() %ld bytes reveived:%s\n", msg.size(), msg.c_str());

	conn->send(msg);  // 回送接受的数据
}


int main()
{
	InetAddr servAddr(10000);
	EventLoop loop;
	Server server(servAddr, &loop);
	server.setMessageCallback([=](const ConnectionPtr& conn, Buffer* buf) {onMessage(conn, buf); });
	//std::thread t([&loop]() {
	//	std::this_thread::sleep_for(std::chrono::seconds(20)); 
	//	loop.quit();
	//	});
	//t.detach();
	loop.loop();

	return 0;
}

