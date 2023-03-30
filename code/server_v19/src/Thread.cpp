#include"Thread.h"
#include"currentthread.h"

Thread::Thread(TheadFunc func)
	:started_(false)
	,joined_(false)
	,func_(std::move(func))
	,latch_(1)
{
}
Thread::~Thread()
{
	if (started_ && !joined_)
		thread_.detach();
}

void Thread::start()
{
	started_ = true;

	thread_ = std::thread([this]() {
		tid_ = CurrentThread::tid();
		latch_.countDown();
		func_();	//Ïß³Ìº¯Êý
		});
	latch_.wait();
}

void Thread::join()
{
	if (thread_.joinable()) {
		thread_.join();
		joined_ = true;
	}
}