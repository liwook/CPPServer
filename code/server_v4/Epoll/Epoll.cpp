#include"Epoll.h"
#include"../util/util.h"
#include<string.h>
const int SIZE = 1024;

Epoll::Epoll()
	:epfd_(epoll_create(1))
	,events_(new epoll_event[SIZE])
{
	perror_if(epfd_ == -1, "epoll_create");
	memset(events_,0, sizeof(SIZE*sizeof(epoll_event)));
}
Epoll::~Epoll()
{
	if (epfd_ != -1) {
		epfd_ = -1;
	}
	delete[] events_;
}
void Epoll::update(int sockfd, int events,int op)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.data.fd = sockfd;
	ev.events = events;
	int ret = epoll_ctl(epfd_, op, sockfd, &ev);
	perror_if(ret == -1, "epoll_ctl");
}
void Epoll::Epoll_wait(vector<epoll_event>& active, int timeout)
{
	int nums = epoll_wait(epfd_, events_, SIZE, timeout);
	perror_if(nums == -1, "epoll_wait");
	for (int i = 0; i < nums; ++i) {
		active.emplace_back(events_[i]);
	}
}