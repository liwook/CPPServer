#pragma once

class Epoll;
class Channel
{
public:
	Channel(Epoll* ep, int fd);

	void setEvents(int events);
	int Event()const;
	void setRevents(int events);
	int Revent()const;

	void enableReading();
	bool isInEpoll();
	void setInEpoll(bool in);
	int Fd()const;
private:
	Epoll* ep_;
	int fd_;
	int events_;
	int revents_;
	bool isInEpoll_;
};