#include"util.h"
#include<stdio.h>
#include<stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include<string.h>

void sockets::setNonblock(int sockfd)
{
	int flag = fcntl(sockfd, F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(sockfd, F_SETFL, flag);
}

void sockets::shutdownWrite(int sockfd)
{
	if (::shutdown(sockfd, SHUT_WR) < 0){
		printf( "sockets::shutdownWrite error\n");
	}
}



int sockets::getSocketError(int sockfd)
{
	int optval;
	socklen_t optlen = static_cast<socklen_t>(sizeof optval);

	if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
	{
		return errno;
	}
	else
	{
		return optval;
	}
}


struct sockaddr_in sockets::getLocalAddr(int sockfd)
{
	struct sockaddr_in localaddr;
	memset(&localaddr, 0,sizeof(localaddr));
	socklen_t addrlen = static_cast<socklen_t>(sizeof (localaddr));
	if (::getsockname(sockfd, (struct sockaddr*)&localaddr, &addrlen) < 0)
	{
		printf("sockets::getLocalAddr\n");
	}
	return localaddr;
}

struct sockaddr_in sockets::getPeerAddr(int sockfd)
{
	struct sockaddr_in peeraddr;
	memset(&peeraddr, 0, sizeof(peeraddr));
	socklen_t addrlen = static_cast<socklen_t>(sizeof(peeraddr));
	if (::getpeername(sockfd, (struct sockaddr*)&peeraddr, &addrlen) < 0)
	{
		printf("sockets::getPeerAddr\n");
	}
	return peeraddr;
}




void perror_if(bool condtion, const char* errorMessage)
{
	if (condtion) {
		perror(errorMessage);
		exit(1);
	}
}