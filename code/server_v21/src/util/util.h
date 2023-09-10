#pragma once
#include <arpa/inet.h>
#include<string>

namespace sockets
{
	void setReuseAddr(int sockfd);
	void setNonblock(int sockfd);
	void shutdownWrite(int sockfd);
	int getSocketError(int sockfd);

	struct sockaddr_in getLocalAddr(int sockfd);
	struct sockaddr_in getPeerAddr(int sockfd);
}  // namespace sockets

namespace ProcessInfo
{
	std::string hostname();

	pid_t pid();
}
void perror_if(bool condtion, const char* errorMessage);

