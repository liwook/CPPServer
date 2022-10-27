#include"Server.h"
#include<string.h>
#include<unistd.h>
#include<fcntl.h>


void setNonblock(int sockfd)
{
	int flag = fcntl(sockfd, F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(sockfd, F_SETFL, flag);
}

Server::Server(const InetAddr& listenAddr, EventLoop* eventloop)
	:loop_(eventloop)

	,acceptor_(std::make_unique<Acceptor>(listenAddr,loop_))
{
	auto cb = [this](int sockfd){newConnection(sockfd); };
	acceptor_->setNewconnectionCallback(cb);
}

Server::~Server()
{
	//printf("~Server() start()\n");
	//不一定会进入到下面的for循环中的，因为可能connections_中已没有元素
	for (auto& item : connections_) {
		ConnectionPtr conn(item.second);
		item.second.reset();
		//printf("~Server() connections_.reset() later  user_count= %ld\n", item.second.use_count());

		conn->connectDestroyed();
	}
}

void Server::newConnection(int sockfd)
{
	setNonblock(sockfd);

	auto conn = std::make_shared<Connection>(loop_, sockfd);
	printf("newconnection make_shared 后 user_count= %ld\n", conn.use_count());
	connections_[sockfd] = conn;
	printf("connections_[sockfd] = conn 后 user_count= %ld\n", conn.use_count());

	conn->setMessageCallback(messageCallback_);
	conn->setCloseCallback([this](const ConnectionPtr& connection) {removeConnection(connection); });
	conn->connectEstablished();	//建立连接


	//Channel* channel = new Channel(loop_, sockfd);	//这版本没有delete,会内存泄漏，之后版本会修改
	//auto cb = [this,channel]() {handleEvent(channel); };

	//channel->SetReadCallback(cb);	//设置回调
	//channel->enableReading();
}

void Server::removeConnection(const ConnectionPtr& conn)
{
	auto n = connections_.erase(conn->fd());	//在Server类中
	//printf(" connections_.erase() later user_count= %ld\n", conn.use_count());//打开这句，可以查看智能指针的引用计数

	assert(n == 1);
	conn->connectDestroyed();
}


