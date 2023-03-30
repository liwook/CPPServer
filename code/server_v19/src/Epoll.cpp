#include"Epoll.h"
#include"util/util.h"
#include<string.h>
#include"Channel.h"
const int kSize = 1024;

Epoll::Epoll()
	:epfd_(epoll_create(1))
	,events_(new epoll_event[kSize])
{
	perror_if(epfd_ == -1, "epoll_create");
	memset(events_,0, sizeof(kSize*sizeof(epoll_event)));
}
Epoll::~Epoll()
{
	if (epfd_ != -1) {
		epfd_ = -1;
	}
	delete[] events_;
}

//v13版本有改动
void Epoll::updateChannel(Channel* ch)
{	
	int fd = ch->Fd();
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	//ev.data.fd = sockfd;	不使用这个
	ev.data.ptr = ch;	//关键是这里
	ev.events = ch->Event();	//从ch中获取需要监听的事件

	if (ch->isInEpoll()) {//表示已在epoll上
		//新添加的 判断是否是不监听任何事件
		if (ch->isNoneEvent()) {	//表明该channel已不监听任何事件，就可以进行EPOLL_CTL_DEL
			del(ch);
		}
		else {
			int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev);
			perror_if(ret == -1, "epoll_ctl mod");
		}
	}
	else {
		int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
		perror_if(ret == -1, "epoll_ctl add");
		ch->setInEpoll(true);
	}
}

void Epoll::del(Channel* ch)
{
	//之前的写法不太安全
	//在v13版本中，这里添加了这个判断，先判断该channel是否已被epoll监听，是已被监听才进行删除
	if (ch->isInEpoll()) {
		int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, ch->Fd(), nullptr);
		perror_if(ret == -1, "epoll_ctl DEL");
		ch->setInEpoll(false);
	}

	//int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, ch->Fd(), nullptr);
	//perror_if(ret == -1, "epoll_ctl DEL");
	//ch->setInEpoll(false);
}


void Epoll::Epoll_wait(vector<Channel*>& active, int timeout)
{
	int nums = epoll_wait(epfd_, events_, kSize, timeout);
	perror_if(nums == -1, "epoll_wait");
	for (int i = 0; i < nums; ++i) {
		Channel* ch = static_cast<Channel*>(events_[i].data.ptr);
		ch->setRevents(events_[i].events);	//设置从epoll中返回的事件
		active.emplace_back(ch);
	}
}