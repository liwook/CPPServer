#pragma once
#include"timer.h"
#include<set>
#include"Channel.h"
#include<unordered_map>
#include<atomic>
#include"Callbacks.h"
class Timer;
class EventLoop;

class TimerQueue
{
public:
	explicit TimerQueue(EventLoop* loop);
	~TimerQueue();

	int64_t addTimer(TimerCallback cb, Timestamp when, double interval);
	void cancel(int64_t timerId);

private:
	using Entry = std::pair<Timestamp, Timer*>;
	using TimerList = std::set<Entry>;

	void addTimerInLoop(Timer* timer);
	void cancelInLoop(int64_t timerId);

	void handleRead();	//在这个函数内部执行定时器的任务
	std::vector<Entry> getExpired(Timestamp now);		//得到所有的超时定时器
	void reset(const std::vector<Entry>& expired, Timestamp now);
	bool insert(Timer* timer);	//插入该定时器

	EventLoop* loop_;
	const int timerfd_;
	Channel timerfd_channel_;
	TimerList timers_;		//这是为了 方便查找超时定时器 的容器
	std::unordered_map<int64_t, Timer*> active_timers_;		//这是为了方便进行删除的容器，以定时器唯一的标识符sequeue为key
	std::unordered_map<int64_t, Timer*> cancelingTimers_;	//为了解决定时器正在执行，却想删除该定时器的问题的
	std::atomic_bool callingExpiredTimers_;		//标识定时器是否在执行

};