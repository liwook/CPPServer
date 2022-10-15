#include"Socket.h"
#include"../util/util.h"
#include"../InetAddr/InetAddr.h"
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
void Socket::bind(InetAddr* serv_addr)
{
	int ret = ::bind(sockfd_, (sockaddr*)serv_addr->getAddr(), sizeof(sockaddr_in));
	perror_if(ret == -1, "bind");
}
int Socket::accept(InetAddr* addr)
{
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);
	int cfd = ::accept(sockfd_, (struct sockaddr*)&cliaddr, &len);
	perror_if(cfd == -1, "accept");
	addr->setAddr(cliaddr);
	printf("new client fd %d ip:%s ,port:%d connected..\n", cfd, addr->toIp().c_str(), addr->toPort());

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