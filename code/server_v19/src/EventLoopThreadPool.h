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
	EventLoop* baseLoop_;	// ��Acceptor����EventLoop��ͬ
	bool started_;
	int numThreads_;
	int next_;		//�����ӵ�������ѡ���EventLoopThread�±�
	std::vector<std::unique_ptr<EventLoopThread>> threads_;// IO�߳��б�
	std::vector<EventLoop*> loops_;		//EventLoop�б�, ָ�����EventLoopThread�̺߳���������EventLoop����
};