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
	const TimerCallback callback_;	//��ʱ��ִ�е��¼�
	Timestamp expiration_;			//��ʱ����ִ��ʱ��
	const double interval_;			//�ظ���ʱ��ִ���¼��ļ��ʱ�䣬����һ���Զ�ʱ������ֵΪ 0.0
	const bool repeat_;				//�����ж϶�ʱ���Ƿ����ظ�ѭ����
	const int64_t sequence_;		//�������ʱ����Ψһ��ʶ

	static std::atomic_int64_t num_created_;	//����sequence_
};