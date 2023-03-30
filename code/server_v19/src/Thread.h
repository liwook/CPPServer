#pragma once
#include<thread>
#include<functional>
#include<memory>
#include"CountDownLatch.h"

class Thread
{
public:
	using TheadFunc = std::function<void()>;

	explicit Thread(TheadFunc);
	~Thread();

	void start();
	void join();
	bool started()const { return started_; }
	pid_t tid()const { return tid_; }

private:
	bool		started_;
	bool		joined_;
	std::thread thread_;
	pid_t		tid_;
	TheadFunc	func_;
	CountDownLatch latch_;
};