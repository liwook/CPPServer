#include"EventLoop.h"
#include"Channel.h"
#include"Epoll.h"

// 创建wakeupfd，用来notify唤醒处理回调任务操作
int createEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		printf("eventfd error:%d\n", errno);
	}
	return evtfd;
}

EventLoop::EventLoop()
	:threadId_(std::this_thread::get_id())
	,quit_(false)
	, callingPendingFunctors_(false)
	,ep_(std::make_unique<Epoll>())
	,wakeupFd_(createEventfd())
	,wakeupChannel_(std::make_unique<Channel>(this,wakeupFd_))
{
	// 设置wakeupfd的发生事件后的回调操作
	wakeupChannel_->SetReadCallback([this]() {handleRead(); });
	//监听wakeupchannel_的EPOLLIN读事件了
	wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	::close(wakeupFd_);
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

		//执行当前EventLoop事件循环需要处理的回调任务操作
		doPendingFunctors();	
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


void EventLoop::runInLoop(Functor cb)
{
	printf("Call EventLoop::runInLoop() when not in the IO thread\n");
	if (isInLoopThread()) {
		printf(" EventLoop::runInLoop() In this IO thread, directly call the callback function\n");
		cb();
	}
	else {
		queueInLoop(std::move(cb));
	}
}

void EventLoop::queueInLoop(Functor cb)
{
	printf("Call EventLoop::queueInLoop() when not in the IO thread\n");
	{
		std::unique_lock<std::mutex> lock(mutex_);
		pendingFunctors_.emplace_back(std::move(cb));
	}
	if (!isInLoopThread() || callingPendingFunctors_) {
		wakeup();
	}
}


// 用来唤醒loop所在线程  向wakeupfd_写一个数据，wakeupChannel就发生读事件，当前loop线程就会被唤醒
void EventLoop::wakeup()
{
	printf("need to wake up the IO thread, call EventLoop:: wakeup()\n");
	uint64_t one = 1;
	ssize_t n = write(wakeupFd_, &one, sizeof(one));
	if (n != sizeof(one))
	{
		printf("EventLoop wakeup write %lu bytes instead of 8 \n", n);
	}
}

void EventLoop::handleRead()
{
	printf("Respond to wake-up of wakeup() by using EventLoop::handleRead()\n");
	uint64_t one = 1;
	auto n = ::read(wakeupFd_, &one, sizeof(one));
	if (n != sizeof(one)){
		printf("EventLoop::handleRead() reads %lu bytes \n", n);
	}
}

//执行任务回调函数
void EventLoop::doPendingFunctors()	
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;
	// 把functors转移到局部的functors，这样在执行回调时不用加锁。不影响mainloop注册回调
	{
		std::unique_lock<std::mutex> lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for (const auto& functor : functors) {
		printf("Execute the callback function functor() in EventLoop:: doPendingFunctors()\n");
		functor();//执行当前loop需要执行的回调操作
	}
	callingPendingFunctors_ = false;
}


