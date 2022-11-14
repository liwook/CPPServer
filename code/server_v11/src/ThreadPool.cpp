#include"ThreadPool.h"
#include<assert.h>
ThreadPool::ThreadPool()
	:mutex_()
	, notEmpy_()
	, notFull_()
	, maxQueueSize_(0)
	, running_(false)
{

}
ThreadPool::~ThreadPool()
{
	if (running_) {
		stop();
	}
}


void ThreadPool::start(int numThreads)
{
	running_ = true;
	threads_.reserve(numThreads);

	for (int i = 0; i < numThreads; ++i) {
		threads_.emplace_back(std::make_unique<Thread>([this]() {
			runInThread();
			})
		);
		threads_[i]->start();	//这个才是线程的真正启动
	}
}
void ThreadPool::stop()
{
	{
		std::unique_lock<std::mutex> lock(mutex_);
		running_ = false;
		notEmpy_.notify_all();
		notFull_.notify_all();
	}
	for (auto& thr : threads_)
		thr->join();
}


void ThreadPool::run(Task task)
{
	if (threads_.empty()) {	//若没有线程，就直接执行任务
		task();
	}
	else {
		std::unique_lock<std::mutex> lock(mutex_);
		notFull_.wait(lock, [this]() {return !isFull() && running_; });

		if (!running_)
			return;

		queue_.emplace_back(std::move(task));
		notEmpy_.notify_one();
	}
}

bool ThreadPool::isFull()
{
	return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}
void ThreadPool::runInThread()
{
	while (running_) {
		Task task(take());
		if (task) {
			task();
		}
	}
}
ThreadPool::Task ThreadPool::take()
{
	std::unique_lock<std::mutex> lock(mutex_);
	notEmpy_.wait(lock, [this]() {return queue_.size() && running_; });

	Task task;
	if (!queue_.empty()) {
		task = queue_.front();
		queue_.pop_front();
		if (maxQueueSize_ > 0)
			notFull_.notify_one();
	}
	return task;
}