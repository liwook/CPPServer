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
#include"Connection.h"
#include<map>
class Server
{
public:
	using connectionMap = std::map<int, ConnectionPtr>;
public:
	Server(const InetAddr& serverAddr, EventLoop* eventloop);
	~Server();

	void setMessageCallback(const MessageCallback& cb)
	{
		messageCallback_ = cb;
	}

private:
	void newConnection(int sockfd);
	void removeConnection(const ConnectionPtr& conn);

private:
	EventLoop* loop_;

	std::unique_ptr<Acceptor> acceptor_;
	connectionMap connections_;

	MessageCallback messageCallback_;
};