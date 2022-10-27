#pragma once
#include<memory>
#include"Callbacks.h"
#include"Buffer.h"
#include"Socket.h"
class EventLoop;
class Channel;


class Connection:public std::enable_shared_from_this<Connection>
{
public:
	enum class StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
public:
	Connection(EventLoop* loop,int sockfd);
	EventLoop* getLoop()const { return loop_; }
	void setMessageCallback(const MessageCallback& cb)
	{
		messageCallback_ = cb;
	}
	void setCloseCallback(const CloseCallback& cb)
	{
		closeCallback_ = cb;
	}

	bool connected() const { return state_ == StateE::kConnected; }
	bool disconnected() const { return state_ == StateE::kDisconnected; }
	void setState(StateE state) { state_ = state; }
	void send(Buffer* message);
	void send(const void* message, int len);
	void send(const std::string& messgage);

	void connectEstablished();

	void connectDestroyed();

	// Advanced interface
	Buffer* inputBuffer()
	{
		return &inputBuffer_;
	}

	Buffer* outputBuffer()
	{
		return &outputBuffer_;
	}

	int fd()const { return socket_->fd(); }
private:

	void handleRead();
	void handleWrite();
	void handleClose();
private:
	EventLoop* loop_;

	StateE state_;  // FIXME: use atomic variable

	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;

	MessageCallback messageCallback_;
	CloseCallback closeCallback_;

	Buffer inputBuffer_;
	Buffer outputBuffer_;


};

using ConnectionPtr = std::shared_ptr<Connection>;