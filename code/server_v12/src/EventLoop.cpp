#include"EventLoop.h"
#include"Channel.h"
#include"Epoll.h"

// ����wakeupfd������notify���Ѵ���ص��������
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
	// ����wakeupfd�ķ����¼���Ļص�����
	wakeupChannel_->SetReadCallback([this]() {handleRead(); });
	//����wakeupchannel_��EPOLLIN���¼���
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

		//ִ�е�ǰEventLoop�¼�ѭ����Ҫ����Ļص��������
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


// ��������loop�����߳�  ��wakeupfd_дһ�����ݣ�wakeupChannel�ͷ������¼�����ǰloop�߳̾ͻᱻ����
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

//ִ������ص�����
void EventLoop::doPendingFunctors()	
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;
	// ��functorsת�Ƶ��ֲ���functors��������ִ�лص�ʱ���ü�������Ӱ��mainloopע��ص�
	{
		std::unique_lock<std::mutex> lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for (const auto& functor : functors) {
		printf("Execute the callback function functor() in EventLoop:: doPendingFunctors()\n");
		functor();//ִ�е�ǰloop��Ҫִ�еĻص�����
	}
	callingPendingFunctors_ = false;
}


