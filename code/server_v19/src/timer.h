#pragma once
#include"timestamp.h"
#include"Callbacks.h"
#include<functional>
#include<atomic>
class Timer
{
public:
	Timer(TimerCallback cb, Timestamp when, double interval)
		:callback_(std::move(cb))
		,expiration_(when)
		,interval_(interval)
		,repeat_(interval>0.0)
		,sequence_(num_created_++)
	{}

	void run()const { callback_(); }

	Timestamp expiration()const { return expiration_; }
	bool repeat()const { return repeat_; }
	int64_t sequence()const { return sequence_; }
	void restart(Timestamp now);
	static int64_t numCreated() { return num_created_; }

private:
	const TimerCallback callback_;	//定时器执行的事件
	Timestamp expiration_;			//定时器的执行时间
	const double interval_;			//重复定时器执行事件的间隔时间，若是一次性定时器，该值为 0.0
	const bool repeat_;				//用于判断定时器是否是重复循环的
	const int64_t sequence_;		//用来辨别定时器的唯一标识

	static std::atomic_int64_t num_created_;	//用来sequence_
};