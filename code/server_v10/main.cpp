#include"src/Server.h"
#include<stdio.h>
#include<functional>
#include<thread>
#include<memory>
#include<iostream>
using namespace std;
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
	server.setConnectionCallback([](const ConnectionPtr& conn) {
		if (conn->connected()) {
			printf("new client fd %d ip:port: %s  connected..\n", conn->fd(), conn->peerAddress().toIpPort().c_str());
			//conn->send("hello\n");
		}
		});
	//std::thread t([&loop]() {
	//	std::this_thread::sleep_for(std::chrono::seconds(20)); 
	//	loop.quit();
	//	});
	//t.detach();
	loop.loop();

	return 0;
}

