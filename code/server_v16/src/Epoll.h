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
	void updateChannel(Channel* ch);
	void del(Channel* ch);
	int GetEpollfd()const { return epfd_; }
	void Epoll_wait(vector<Channel*>& active, int timeout = 10);
private:
	int epfd_;
	struct epoll_event* events_;
};