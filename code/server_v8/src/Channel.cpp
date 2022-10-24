#include"Channel.h"
#include"EventLoop.h"
#include"Epoll.h"

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

void Channel::enableReading()
{
	setEvents(EPOLLIN);

	loop_->updateChannel(this);

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
	if (readCallback_) {
		readCallback_();
	}
}

void Channel::remove()
{
	loop_->removeChannel(this);
}

