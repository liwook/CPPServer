#include"Epoll.h"
#include"util/util.h"
#include<string.h>
#include"Channel.h"	//新添加
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
//void Epoll::update(int sockfd, int events,int op)
//{
//	struct epoll_event ev;
//	memset(&ev, 0, sizeof(ev));
//	ev.data.fd = sockfd;
//	ev.events = events;
//	int ret = epoll_ctl(epfd_, op, sockfd, &ev);
//	perror_if(ret == -1, "epoll_ctl");
//}
void Epoll::updateChannel(Channel* ch)
{
	int fd = ch->Fd();
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	//ev.data.fd = sockfd;	不使用这个
	ev.data.ptr = ch;	//关键是这里
	ev.events = ch->Event();	//从ch中获取需要监听的事件

	if (ch->isInEpoll()) {
		//表示已在epoll上
		int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev);
		perror_if(ret == -1, "epoll_ctl mod");
	}
	else {
		int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
		perror_if(ret == -1, "epoll_ctl add");
		ch->setInEpoll(true);
	}
	
}

void Epoll::del(Channel* ch)
{
	int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, ch->Fd(), nullptr);
	perror_if(ret == -1, "epoll_ctl DEL");
	ch->setInEpoll(false);
}


void Epoll::Epoll_wait(vector<Channel*>& active, int timeout)
{
	int nums = epoll_wait(epfd_, events_, SIZE, timeout);
	perror_if(nums == -1, "epoll_wait");
	for (int i = 0; i < nums; ++i) {
		Channel* ch = static_cast<Channel*>(events_[i].data.ptr);
		ch->setRevents(events_[i].events);	//设置从epoll中返回的事件
		active.emplace_back(ch);
	}
}
//void Epoll::Epoll_wait(vector<epoll_event>& active, int timeout)
//{
//	int nums = epoll_wait(epfd_, events_, SIZE, timeout);
//	perror_if(nums == -1, "epoll_wait");
//	for (int i = 0; i < nums; ++i) {
//		active.emplace_back(events_[i]);
//	}
//}