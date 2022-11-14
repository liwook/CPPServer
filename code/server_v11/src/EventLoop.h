#pragma once
#include<vector>
#include<memory>
#include<atomic>
class Channel;
class Epoll;

class EventLoop
{
public:
	using channelList=std::vector < Channel* >;
public:
	EventLoop();
	~EventLoop();

	void loop();
	void updateChannel(Channel* channel);
	void removeChannel(Channel* channel);

	void quit();	//退出事件循环
private:
	std::atomic_bool quit_;	//标志退出loop循环

	std::unique_ptr<Epoll> ep_;
	channelList activeChannels_;	//保存当前活跃事件的Channel列表
};