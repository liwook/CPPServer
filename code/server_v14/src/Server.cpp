#include"Server.h"
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

Server::Server(const InetAddr& listenAddr, EventLoop* eventloop)
	:loop_(eventloop)
	,ipPort_(listenAddr.toIpPort())
	,acceptor_(std::make_unique<Acceptor>(listenAddr,loop_))
	,loop_threadpool_(std::make_unique<EventLoopThreadPool>(loop_))
	,compute_threadpool_(std::make_unique<ThreadPool>())
	,started_(0)
{
	acceptor_->setNewconnectionCallback([this](int sockfd, const InetAddr& peerAddr) {newConnection(sockfd, peerAddr); });
}

Server::~Server()
{
	//不一定会进入到下面的for循环中的，因为可能connections_中已没有元素
	for (auto& item : connections_) {
		ConnectionPtr conn(item.second);
		item.second.reset();
		//printf("~Server() connections_.reset() later  user_count= %ld\n", item.second.use_count());

		conn->connectDestroyed();
	}
}


void Server::start(int IOThreadNum, int compute_threadNum)
{
	if (started_++ == 0) //防止一个TcpServer对象被启动多次
	{
		loop_threadpool_->setThreadNum(IOThreadNum);
		loop_threadpool_->start();
		compute_threadpool_->start(compute_threadNum);	//添加这句，开启计算线程池，若compute_threadNum是0，那就是没有开启咯
		acceptor_->listen();
	}
}

void Server::newConnection(int sockfd, const InetAddr& peerAddr)
{
	//不使用sockets::setNonblock，是因为在Socket::accept函数中使用了accept4()函数，这个是可以直接就设置非阻塞的
	//sockets::setNonblock(sockfd);

	//轮训，选择一个subLoop
	EventLoop* ioLoop = loop_threadpool_->getNextLoop();
	InetAddr localAddr(sockets::getLocalAddr(sockfd));

	//auto conn = std::make_shared<Connection>(loop_, sockfd, localAddr,peerAddr);
	auto conn = std::make_shared<Connection>(ioLoop, sockfd, localAddr,peerAddr);	//把loop_改成 ioLoop
	connections_[sockfd] = conn;

	conn->setMessageCallback(messageCallback_);
	conn->setCloseCallback([this](const ConnectionPtr& connection) {removeConnection(connection); });
	conn->setConnectionCallback(connectionCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);

	//conn->connectEstablished();	//建立连接
	//改成下面的
	ioLoop->runInLoop([conn]() { conn->connectEstablished();});//转移到该ioLoop线程来调用connectEstablished()
}

void Server::removeConnection(const ConnectionPtr& conn)
{
	//这是之前的
	//auto n = connections_.erase(conn->fd());	//在Server类中
	//conn->connectDestroyed();

	//现在的
	loop_->runInLoop([this, conn]() { removeConnectionInLoop(conn); });

}

// 在对应的loop中移除，不会发生多线程的错误
void Server::removeConnectionInLoop(const ConnectionPtr& conn)
{
	connections_.erase(conn->fd());	

	auto ioLoop = conn->getLoop();
	ioLoop->queueInLoop([conn]() {conn->connectDestroyed(); });
}