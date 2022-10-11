#pragma once
#include<sys/epoll.h>
#include<vector>
using std::vector;

class Epoll
{
public:
	Epoll();
	~Epoll();
	void update(int sockfd, int events, int op);
	void Epoll_wait(vector<epoll_event>& active, int timeout = 10);
private:
	int epfd_;
	struct epoll_event* events_;
};