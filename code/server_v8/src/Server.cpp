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

}

void Server::handleEvent(Channel* ch)
{
	int fd = ch->Fd();
	int saveErrno;
	auto len = inputBuffer_.readFd(fd, &saveErrno);
	if (len > 0) {
		auto msg = inputBuffer_.retrieveAllAsString().c_str();
		printf("client fd %d says: %s\n", fd, msg);
		write(fd, msg, len);
	}
	else if (len == -1) {
		perror("read");
	}
	else if (len == 0) {  //客户端断开连接
		printf("client fd %d disconnected\n", fd);
		loop_->removeChannel(ch);
		close(fd);
	}
}
void Server::newConnection(int sockfd)
{
	setNonblock(sockfd);

	Channel* channel = new Channel(loop_, sockfd);	//这版本没有delete,会内存泄漏，之后版本会修改
	auto cb = [this,channel]() {handleEvent(channel); };

	channel->SetReadCallback(cb);	//设置回调
	channel->enableReading();
}

