#pragma once

#include"Channel.h"
#include"Epoll.h"
#include"Socket.h"
#include"util/util.h"
#include"EventLoop.h"
#include"InetAddr.h"
#include<memory>
#include"Acceptor.h"
#include"Buffer.h"
class Server
{
public:
	Server(const InetAddr& serverAddr, EventLoop* eventloop);
	~Server();
	void handleEvent(Channel* ch);
	void newConnection(int sockfd);

private:
	EventLoop* loop_;

	std::unique_ptr<Acceptor> acceptor_;
	
	Buffer inputBuffer_;
};