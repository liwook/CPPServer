#pragma once
#include <arpa/inet.h>


namespace sockets
{
	void setNonblock(int sockfd);
	
	void shutdownWrite(int sockfd);

	int getSocketError(int sockfd);

	struct sockaddr_in getLocalAddr(int sockfd);
	struct sockaddr_in getPeerAddr(int sockfd);
}  // namespace sockets


void perror_if(bool condtion, const char* errorMessage);

