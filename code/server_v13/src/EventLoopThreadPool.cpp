#include"EventLoopThreadPool.h"
#include"EventLoop.h"
#include"EventLoopThread.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
	:baseLoop_(baseLoop)
	,started_(false)
	,numThreads_(0)
	,next_(0)
{

}
EventLoopThreadPool::~EventLoopThreadPool()
{
	// Don't delete loop, it's stack variable 是栈上的变量
}

void EventLoopThreadPool::start()
{
	started_ = true;
	for (int i = 0; i < numThreads_; ++i) {
		//auto t = std::make_unique<EventLoopThread>();
		//loops_.push_back(t->startLoop());	// 启动IO线程, 并将线程函数创建的EventLoop对象地址 插入loops_数组
		//threads_.push_back(std::move(t));	// 将EventLoopThread对象指针 插入threads_数组

		auto t = new EventLoopThread;
		threads_.push_back(std::unique_ptr<EventLoopThread>(t));
		// 底层创建线程，绑定一个新的EventLoop， 并返回该loop的地址
		loops_.push_back(t->startLoop());
	}
}

//简单的循环取用
EventLoop* EventLoopThreadPool::getNextLoop()
{
	EventLoop* loop = baseLoop_;
	if (!loops_.empty()) {
		loop = loops_[next_];
		next_ = (next_ + 1) % numThreads_;
	}
	return loop;
}