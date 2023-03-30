#include"Socket.h"
#include"util/util.h"
#include"InetAddr.h"
#include<fcntl.h>
#include<unistd.h>
Socket::Socket()
	:sockfd_(socket(AF_INET,SOCK_STREAM,0))
{
	perror_if(sockfd_ == -1, "socket");
}
Socket::Socket(int fd)
	:sockfd_(fd)
{
	perror_if(sockfd_ == -1, "socket(int fd)");
}
Socket::~Socket()
{
	if (sockfd_ != -1) {
		close(sockfd_);
		sockfd_ = -1;
	}
}
void Socket::bind(const InetAddr& serv_addr)
{
	int ret = ::bind(sockfd_, (struct sockaddr*)serv_addr.getSockAddr(), sizeof(sockaddr_in));
	perror_if(ret == -1, "bind");
}
int Socket::accept(InetAddr* addr)
{
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);
	//int cfd = ::accept(sockfd_, (struct sockaddr*)&cliaddr, &len);
	int cfd = ::accept4(sockfd_, (sockaddr*)&cliaddr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);	//这个是可以直接就设置非阻塞的
	perror_if(cfd == -1, "accept");
	addr->setAddr(cliaddr);

	return cfd;
}

void Socket::listen()
{
	int ret = ::listen(sockfd_, 128);
	perror_if(ret == -1, "listen");
}

void Socket::setNonblock()
{
	int flag = fcntl(sockfd_, F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(sockfd_, F_SETFL, flag);
}