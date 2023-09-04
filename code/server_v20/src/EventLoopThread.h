#pragma once
#include<thread>
#include<mutex>
#include<condition_variable>
#include"Thread.h"

class EventLoop;
class EventLoopThread
{
public:
	using ThreadInitCallback=std::function<void(EventLoop*)>;
	EventLoopThread();
	~EventLoopThread();

	EventLoop* startLoop();

private:
	void threadFunc();

	EventLoop* loop_;

	//std::thread thread_;
	Thread thread_;
	std::mutex mutex_;
	std::condition_variable cond_;

};