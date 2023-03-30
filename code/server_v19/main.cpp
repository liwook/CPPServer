#include"src/Server.h"
#include<stdio.h>
#include<functional>
#include<thread>
#include<memory>
#include<iostream>

using namespace std;

void log_test()
{

	std::thread t1([]() {
		for (int i = 0; i < 900000; ++i)
			LOG_INFO << "1111woshisdfsd " << 23 << 34 << "buox";
		});
	t1.detach();
	std::thread t2([]() {
		for (int i = 0; i < 1010000; ++i)
			//LOG_DEBUG << "22woshisdfsd " << 23 << 34 << "buox";
			LOG_INFO << "wo��log_info��" << 555 << " test";
		});
	t2.detach();
	for (int i = 0; i < 1010000; ++i)
		LOG_INFO << 11 << "�й���" << 23;
	std::this_thread::sleep_for(std::chrono::seconds(3));
}

int main()
{
	
	//log_test();
	
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

