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
			conn->send(msg);  // ���ͽ��ܵ����� 
			//conn->shutdown();	//�ر�д��
	});

	server.start(2);	//������io�߳����������̲߳��������ڵ�
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
		conn->send(msg);  // ���ͽ��ܵ����� 
	});

	server.start(2);//������io�߳����������̲߳��������ڵ�
	loop.loop();

	return 0;
}

