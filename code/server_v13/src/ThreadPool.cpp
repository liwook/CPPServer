#include"ThreadPool.h"
#include<assert.h>
ThreadPool::ThreadPool()
	 :running_(false)
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
		threads_.emplace_back(std::make_unique<std::thread>([this]() {
			//printf("thread start\n");
			runInThread();
			})
		);
	}
}
void ThreadPool::stop()
{
	{
		std::unique_lock<std::mutex> lock(mutex_);
		running_ = false;
		cond_.notify_all();
		//printf("stop  running_ is false\n");
	}
	for (auto& thr : threads_)
		thr->join();
}


void ThreadPool::add(Task task)
{
	if (threads_.empty()) {	//��û���̣߳���ֱ��ִ������
		task();
	}
	else {
		{
			std::unique_lock<std::mutex> lock(mutex_);

			if (!running_)
				return;
			tasks_.push(std::move(task));
		}
		
		cond_.notify_one();
	}
}

void ThreadPool::runInThread()
{
	//printf("runinthread start\n");
	while (running_) {
		Task task;
		{
			std::unique_lock<std::mutex> lock(mutex_);
			cond_.wait(lock, [this]() {return !running_ || !tasks_.empty(); });

			if (!tasks_.empty()) {
				task = std::move(tasks_.front());
				tasks_.pop();
			}
		}
		if (task) {
			task();
		}
	}
	//printf("thread exit\n");
}