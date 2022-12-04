#pragma once
#include"timer.h"
#include<set>
#include"Channel.h"
#include<unordered_map>
#include<atomic>
class Timer;
class EventLoop;

class TimerQueue
{
public:
	explicit TimerQueue(EventLoop* loop);
	~TimerQueue();

	int64_t addTimer(Timer::TimerCallback cb, Timestamp when, double interval);
	void cancel(int64_t timerId);

private:
	using Entry = std::pair<Timestamp, Timer*>;
	using TimerList = std::set<Entry>;

	void addTimerInLoop(Timer* timer);
	void cancelInLoop(int64_t timerId);

	void handleRead();	//����������ڲ�ִ�ж�ʱ��������
	std::vector<Entry> getExpired(Timestamp now);		//�õ����еĳ�ʱ��ʱ��
	void reset(const std::vector<Entry>& expired, Timestamp now);
	bool insert(Timer* timer);	//����ö�ʱ��

	EventLoop* loop_;
	const int timerfd_;
	Channel timerfd_channel_;
	TimerList timers_;		//����Ϊ�� ������ҳ�ʱ��ʱ�� ������
	std::unordered_map<int64_t, Timer*> active_timers_;		//����Ϊ�˷������ɾ�����������Զ�ʱ��Ψһ�ı�ʶ��sequeueΪkey
	std::unordered_map<int64_t, Timer*> cancelingTimers_;	//Ϊ�˽����ʱ������ִ�У�ȴ��ɾ���ö�ʱ���������
	std::atomic_bool callingExpiredTimers_;		//��ʶ��ʱ���Ƿ���ִ��

};