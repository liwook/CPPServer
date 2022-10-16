#include"InetAddr.h"
#include<string.h>

InetAddr::InetAddr()
{
	memset(&addr_, 0, sizeof(addr_));
}
InetAddr::InetAddr(unsigned short port, const char* ip)
{
	memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	if (ip == nullptr)
		addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		inet_pton(AF_INET, ip, &addr_.sin_addr.s_addr);
}

std::string InetAddr::toIp()const
{
	char ip[64] = { 0 };
	inet_ntop(AF_INET, &addr_.sin_addr.s_addr, ip, sizeof(ip));
	return ip;
}
std::string InetAddr::toIpPort()const
{
	char buf[64] = { 0 };
	sprintf(buf, "%s:%d", toIp().c_str(), toPort());
	return buf;
}
unsigned short InetAddr::toPort()const
{
	return ntohs(addr_.sin_port);
}