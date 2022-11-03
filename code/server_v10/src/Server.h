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
	void setConnectionCallback(const ConnectionCallback& cb)
	{
		connectionCallback_ = cb;
	}
	void setWriteCompleteCallback(const WriteCompleteCallback& cb)
	{
		writeCompleteCallback_ = cb;
	}

private:
	//void newConnection(int sockfd);
	void newConnection(int sockfd,const InetAddr& peerAddr);
	void removeConnection(const ConnectionPtr& conn);

private:
	EventLoop* loop_;

	const std::string ipPort_;

	std::unique_ptr<Acceptor> acceptor_;
	connectionMap connections_;

	WriteCompleteCallback writeCompleteCallback_;
	MessageCallback messageCallback_;
	ConnectionCallback connectionCallback_;
};