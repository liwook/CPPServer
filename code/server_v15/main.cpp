#include"src/Server.h"
#include<stdio.h>
#include<functional>
#include<thread>
#include<memory>
#include<iostream>
#include"src/ThreadPool.h"
using namespace std;


int main()
{
	InetAddr servAddr(10000);
	EventLoop loop;
	Server server(servAddr, &loop);

	server.setConnectionCallback([](const ConnectionPtr& conn) {
		if (conn->connected()) {
			printf("Connection connected ip:port: %s  connected..\n", conn->peerAddress().toIpPort().c_str());
		}else {
			printf("Connection disconnected\n");
		}
	});

	server.setMessageCallback([](const ConnectionPtr& conn, Buffer* buf) {
			std::string msg(buf->retrieveAllAsString());
			printf("onMessage() %ld bytes reveived:%s\n", msg.size(), msg.c_str());
			conn->send(msg);  // 回送接受的数据 
			//conn->shutdown();	//关闭写端
	});

	server.start(2);	//开启的io线程数量，主线程不包括在内的
	loop.loop();

	return 0;
}


int main()
{
	InetAddr servAddr(10000);
	EventLoop loop;
	Server server(servAddr, &loop);

	server.setConnectionCallback([](const ConnectionPtr& conn) {
		if (conn->connected()) {
			printf("Connection connected ip:port: %s  connected..\n", conn->peerAddress().toIpPort().c_str());
		}
		else {
			printf("Connection disconnected\n");
		}
	});

	server.setMessageCallback([](const ConnectionPtr& conn, Buffer* buf) {
		std::string msg(buf->retrieveAllAsString());
		conn->send(msg);  // 回送接受的数据 
	});

	server.start(2);//开启的io线程数量，主线程不包括在内的
	loop.loop();

	return 0;
}

