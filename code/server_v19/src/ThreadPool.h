#pragma once
#include<queue>
#include<vector>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<memory>
#include<thread>
class ThreadPool
{
public:
	using Task = std::function<void()>;
	
	explicit ThreadPool();
	~ThreadPool();

	void start(int numThreads);
	void stop();

	void add(Task f);
private:
	void runInThread();

	std::mutex mutex_;
	std::condition_variable cond_;
	std::vector <std::unique_ptr<std::thread>> threads_;
	std::queue<Task> tasks_;

	bool running_;
};
