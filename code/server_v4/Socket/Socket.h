#pragma once

class InetAddr;

class Socket
{
public:
	Socket();
	Socket(int fd);
	~Socket();
	void bind(InetAddr* serv_addr);
	int accept(InetAddr* addr);
	void listen();
	void setNonblock();
	int fd()const { return sockfd_; }
private:
	int sockfd_;
};