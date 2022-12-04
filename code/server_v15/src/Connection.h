#pragma once
#include<memory>
#include"Callbacks.h"
#include"Buffer.h"
#include"Socket.h"
#include"InetAddr.h"
class EventLoop;
class Channel;


class Connection:public std::enable_shared_from_this<Connection>
{
public:
	enum class StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
public:
	Connection(EventLoop* loop, int sockfd,const InetAddr& loaclAddr,const InetAddr& peerAddr);
	~Connection();
	void setMessageCallback(const MessageCallback& cb)
	{
		messageCallback_ = cb;
	}
	void setCloseCallback(const CloseCallback& cb)
	{
		closeCallback_ = cb;
	}
	void setConnectionCallback(const ConnectionCallback& cb)
	{
		connectionCallback_ = cb;
	}
	void setWriteCompleteCallback(const WriteCompleteCallback& cb)
	{
		writeCompleteCallback_ = cb;
	}

	const InetAddr& localAddress() const { return localAddr_; }
	const InetAddr& peerAddress() const { return peerAddr_; }

	bool connected() const { return state_ == StateE::kConnected; }
	bool disconnected() const { return state_ == StateE::kDisconnected; }
	void send(Buffer* message);
	void send(const char* message, size_t len);
	void send(const std::string& messgage);

	void shutdown();

	void forceClose();

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
	EventLoop* getLoop() const { return loop_; }

	int fd()const { return socket_->fd(); }
private:
	void handleRead();
	void handleWrite();
	void handleClose();
	void handleError();

	void shutdownInLoop();
	void forceCloseInLoop();
	void sendInLoop(const char* message, size_t len);

	void setState(StateE state) { state_ = state; }
private:
	EventLoop* loop_;

	StateE state_;  // FIXME: use atomic variable

	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;

	const InetAddr localAddr_;
	const InetAddr peerAddr_;

	CloseCallback closeCallback_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;

	Buffer inputBuffer_;
	Buffer outputBuffer_;
};

using ConnectionPtr = std::shared_ptr<Connection>;