#include"Channel.h"
#include"EventLoop.h"


Channel::Channel(EventLoop* loop, int fd)
	:loop_(loop)
	, fd_(fd)
	, events_(0)
	, revents_(0)
	, isInEpoll_(false)
	,tied_(false)
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
	if (tied_) {
		std::shared_ptr<void> guard = tie_.lock();
		if (guard) {
			handleEventWithGuard();
		}
	}
	else {															//这个else里面是用来建立连接的，因为开始建立连接的时候tied_是false,是连接建立后开始通信tied_才为true
		handleEventWithGuard();
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

void Channel::tie(const std::shared_ptr<void>& obj)
{
	tie_ = obj;
	tied_ = true;
}

void Channel::handleEventWithGuard()
{
	//当事件为挂起并没有可读事件时
	if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
		if (closeCallback_) {
			printf("channel closeCallback\n");
			closeCallback_();
		}
	}
	if (revents_ & EPOLLERR) {
		if (errorCallback_)
			errorCallback_();
	}
	if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) { //关于读的事件 
		if (readCallback_)
			readCallback_();
	}
	if (revents_ & EPOLLOUT) {   //关于写的事件
		if (writeCallback_)
			writeCallback_();
	}
}