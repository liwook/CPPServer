#pragma once
#include<deque>
#include<vector>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<memory>
#include"Thread.h"
class ThreadPool
{
public:
	using Task = std::function<void()>;
	
	explicit ThreadPool();
	~ThreadPool();

	void setMaxQueueSize(int maxsize) { maxQueueSize_ = maxsize; }
	void start(int numThreads);
	void stop();

	size_t queueSize()const { return maxQueueSize_; }

	void run(Task f);
private:
	bool isFull();
	void runInThread();
	Task take();
	std::mutex mutex_;
	std::condition_variable notEmpy_;
	std::condition_variable notFull_;
	std::vector <std::unique_ptr<Thread>> threads_;
	std::deque<Task> queue_;
	size_t maxQueueSize_;
	bool running_;
	
};