#pragma once
#include<vector>
#include<memory>
#include<atomic>
#include<thread>
#include<mutex>
#include <sys/eventfd.h>
#include<functional>
class Channel;
class Epoll;

class EventLoop
{
public:
	using Functor = std::function<void()>;
	using channelList=std::vector<Channel*>;
public:
	EventLoop();
	~EventLoop();

	void loop();
	void updateChannel(Channel* channel);
	void removeChannel(Channel* channel);

	bool isInLoopThread()const { return threadId_ == std::this_thread::get_id(); }
	void runInLoop(Functor cb);
	void queueInLoop(Functor cb);
		// 用来唤醒loop所在线程  向wakeupfd_写一个数据，wakeupChannel就发生读事件，当前loop线程就会被唤醒
	void wakeup();

	std::thread::id getThreadId(){ return threadId_; }

	void quit();	//退出事件循环

	void assertInLoopThread()
	{
		if (!isInLoopThread()) {
			printf("not in this loopThread\n");
			//会出错，现在还没有处理
		}
	}
private:
	void doPendingFunctors();	//执行任务回调函数
	void handleRead();	//用于wake up



	std::thread::id threadId_;
	std::atomic_bool quit_;	//标志退出loop循环

	std::atomic_bool callingPendingFunctors_;   //标识当前loop是否有需要执行的回调操作

	std::unique_ptr<Epoll> ep_;
	channelList activeChannels_;	//保存当前活跃事件的Channel列表

	int wakeupFd_;
	std::unique_ptr<Channel> wakeupChannel_;

	std::vector<Functor> pendingFunctors_;  //存储loop需要执行的所有回调操作
	std::mutex mutex_;  // 互斥锁，用来保护上面vector容器的线程安全操作
};