#pragma once
#include<string>
#include<arpa/inet.h>

class InetAddr
{
public:
	InetAddr();
	InetAddr(unsigned short port, const char* ip=nullptr);
	
	const sockaddr_in* getAddr()const { return &addr_; }

	std::string toIp();
	std::string toIpPort();
	unsigned short toPort();

private:
	struct sockaddr_in addr_;
};