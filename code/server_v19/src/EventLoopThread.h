#pragma once
#include<thread>
#include<mutex>
#include<condition_variable>

class EventLoop;
class EventLoopThread
{
public:
	EventLoopThread();
	~EventLoopThread();

	EventLoop* startLoop();

private:
	void threadFunc();

	EventLoop* loop_;

	std::thread thread_;
	std::mutex mutex_;
	std::condition_variable cond_;
};