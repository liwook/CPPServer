#include"Channel.h"
#include"EventLoop.h"


Channel::Channel(EventLoop* loop, int fd)
	:loop_(loop)
	, fd_(fd)
	, events_(0)
	, revents_(0)
	, isInEpoll_(0)
{

}

void Channel::setEvents(int events)
{
	events_ = events;
}
int Channel::Event()const
{
	return events_;
}
void Channel::setRevents(int revents)
{
	revents_ = revents;
}
int Channel::Revent()const
{
	return revents_;
}

bool Channel::isInEpoll()
{
	return isInEpoll_ == true;
}

void Channel::setInEpoll(bool in)
{
	isInEpoll_ = in;
}

int Channel::Fd()const
{
	return fd_;
}


void Channel::handleEvent()
{
	//if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {//当事件为挂起并没有可读事件时
	//	if (closeCallback_)
	//		//printf("closeCallback_()............\n");
	//		closeCallback_();
	//}
	if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) { //关于读的事件 
		if (readCallback_)
			readCallback_();
	}
	if (revents_ & EPOLLOUT) {   //关于写的事件
		if (writeCallback_)
			writeCallback_();
	}
	
}

void Channel::remove()
{
	loop_->removeChannel(this);
}

void Channel::update()
{
	loop_->updateChannel(this);
}

