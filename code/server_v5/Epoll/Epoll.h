#pragma once
#include<sys/epoll.h>
#include<vector>

using std::vector;

class Channel;

class Epoll
{
public:
	Epoll();
	~Epoll();
	//void update(int sockfd, int events, int op);
	void updateChannel(Channel* ch);
	void del(Channel* ch);


	//void Epoll_wait(vector<epoll_event>& active, int timeout = 10);
	void Epoll_wait(vector<Channel*>& active, int timeout = 10);
private:
	int epfd_;
	struct epoll_event* events_;
};