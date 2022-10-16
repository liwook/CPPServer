#pragma once
#include<vector>
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

private:
	Epoll* ep_;
	channelList activeChannels_;	//保存当前活跃事件的Channel列表
};