#pragma once
#include<functional>

class EventLoop;

class Channel
{
public:
	using ReadEventCallback = std::function<void()>;

public:
	//Channel(Epoll* ep, int fd);
	Channel(EventLoop* loop, int fd);


	void setEvents(int events);
	int Event()const;
	void setRevents(int events);
	int Revent()const;

	void enableReading();
	bool isInEpoll();
	void setInEpoll(bool in);
	int Fd()const;

	void remove();

	void SetReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }

	void handleEvent();
private:
	//Epoll* ep_;
	EventLoop* loop_;

	int fd_;
	int events_;
	int revents_;
	bool isInEpoll_;

	ReadEventCallback readCallback_;
};