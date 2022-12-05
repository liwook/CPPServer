#include"src/Server.h"
#include<stdio.h>
#include<functional>
#include<thread>
#include<memory>
#include<iostream>
#include"src/ThreadPool.h"
using namespace std;

void myprint(const char* msg)
{
	printf(" %s  now:%s\n",msg, Timestamp::now().toFormattedString().c_str());
}

int main()
{
	EventLoop loop;
	loop.runAfter(1, []() {myprint("once1"); });
	loop.runAfter(1.5, []() {myprint("once1.5"); });
	loop.runAfter(2.5, []() {myprint("once2.5"); });
	loop.runAfter(4.8, []() {myprint("once4.8"); });
	loop.runEvery(2, []() {myprint("every2"); });
	
	loop.loop();

	//InetAddr servAddr(10000);
	//EventLoop loop;
	//Server server(servAddr, &loop);

	//server.setConnectionCallback([](const ConnectionPtr& conn) {
	//	if (conn->connected()) {
	//		printf("Connection connected ip:port: %s  connected..\n", conn->peerAddress().toIpPort().c_str());
	//	}else {
	//		printf("Connection disconnected\n");
	//	}
	//});

	//server.setMessageCallback([](const ConnectionPtr& conn, Buffer* buf) {
	//		std::string msg(buf->retrieveAllAsString());
	//		printf("onMessage() %ld bytes reveived:%s\n", msg.size(), msg.c_str());
	//		conn->send(msg);  // 回送接受的数据 
	//		//conn->shutdown();	//关闭写端
	//});

	//server.start(2);	//开启的io线程数量，主线程不包括在内的
	//loop.loop();

	return 0;
}

