#include"EventLoop.h"
#include"../Channel/Channel.h"
#include"../Epoll/Epoll.h"

EventLoop::EventLoop()
	:ep_(new Epoll)
{

}

EventLoop::~EventLoop()
{
	if (ep_) {
		delete ep_;
	}
}

void EventLoop::loop()
{
	while (1) {
		activeChannels_.clear();
		ep_->Epoll_wait(activeChannels_);
		for (auto& active : activeChannels_) {
			active->handleEvent();
		}
	}
}
void EventLoop::updateChannel(Channel* channel)
{
	ep_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
	ep_->del(channel);
}
