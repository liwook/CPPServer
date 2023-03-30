#pragma once
#include<vector>
#include<memory>
#include<functional>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
	EventLoopThreadPool(EventLoop* base);
	~EventLoopThreadPool();

	void setThreadNum(int numThreads) { numThreads_ = numThreads; }
	void start();

	EventLoop* getNextLoop();

	bool started()const { return started_; }
private:
	EventLoop* baseLoop_;	// 与Acceptor所属EventLoop相同
	bool started_;
	int numThreads_;
	int next_;		//新连接到来，所选择的EventLoopThread下标
	std::vector<std::unique_ptr<EventLoopThread>> threads_;// IO线程列表
	std::vector<EventLoop*> loops_;		//EventLoop列表, 指向的是EventLoopThread线程函数创建的EventLoop对象
};