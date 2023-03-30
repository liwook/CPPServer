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
	// Don't delete loop, it's stack variable ��ջ�ϵı���
}

void EventLoopThreadPool::start()
{
	started_ = true;
	for (int i = 0; i < numThreads_; ++i) {
		//auto t = std::make_unique<EventLoopThread>();
		//loops_.push_back(t->startLoop());	// ����IO�߳�, �����̺߳���������EventLoop�����ַ ����loops_����
		//threads_.push_back(std::move(t));	// ��EventLoopThread����ָ�� ����threads_����

		auto t = new EventLoopThread;
		threads_.push_back(std::unique_ptr<EventLoopThread>(t));
		// �ײ㴴���̣߳���һ���µ�EventLoop�� �����ظ�loop�ĵ�ַ
		loops_.push_back(t->startLoop());
	}
}

//�򵥵�ѭ��ȡ��
EventLoop* EventLoopThreadPool::getNextLoop()
{
	EventLoop* loop = baseLoop_;
	if (!loops_.empty()) {
		loop = loops_[next_];
		next_ = (next_ + 1) % numThreads_;
	}
	return loop;
}