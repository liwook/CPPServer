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
#include"EventLoopThreadPool.h"
#include"ThreadPool.h"
#include"timerqueue.h"
#include"./log/logger.h"
#include<map>
class Server
{
public:
	using connectionMap = std::map<int, ConnectionPtr>;
public:
	Server(const InetAddr& serverAddr, EventLoop* eventloop);
	~Server();

	void start(int IOThreadNum=0,int threadNum=0);
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

	// 在对应的loop中移除，不会发生多线程的错误
	void removeConnectionInLoop(const ConnectionPtr& conn);
private:
	EventLoop* loop_;

	const std::string ipPort_;

	std::unique_ptr<Acceptor> acceptor_;
	connectionMap connections_;

	std::unique_ptr<EventLoopThreadPool> loop_threadpool_;
	std::unique_ptr<ThreadPool> compute_threadpool_;	//先添加的，计算线程池
	std::atomic_int32_t started_;

	WriteCompleteCallback writeCompleteCallback_;
	MessageCallback messageCallback_;
	ConnectionCallback connectionCallback_;
};