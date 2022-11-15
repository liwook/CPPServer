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

	void quit();	//�˳��¼�ѭ��
private:
	std::atomic_bool quit_;	//��־�˳�loopѭ��

	std::unique_ptr<Epoll> ep_;
	channelList activeChannels_;	//���浱ǰ��Ծ�¼���Channel�б�
};