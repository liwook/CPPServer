#include"EventLoop.h"
#include"Channel.h"
#include"Epoll.h"

EventLoop::EventLoop()
	:ep_(std::make_unique<Epoll>())
{

}

EventLoop::~EventLoop()
{
}

void EventLoop::loop()
{
	quit_ = false;
	while (!quit_) {
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

void EventLoop::quit() {
	quit_ = true;
}
