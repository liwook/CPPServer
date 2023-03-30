#include"Connection.h"
#include"EventLoop.h"
#include"Channel.h"
#include"Socket.h"
#include"util/util.h"
#include<thread>
#include<iostream>
#include"../src/log/logger.h"

Connection::Connection(EventLoop* loop, int sockfd, const InetAddr& loaclAddr, const InetAddr& peerAddr)
	:loop_(loop)
	, state_(StateE::kConnecting)
	, socket_(std::make_unique<Socket>(sockfd))
	, channel_(std::make_unique<Channel>(loop, sockfd))
	,localAddr_(loaclAddr)
	,peerAddr_(peerAddr)
{
	//设置好 读，写，关闭连接,错误处理 的回调函数
	channel_->SetReadCallback([this]() {handleRead(); });
	channel_->setWriteCallback([this]() {handleWrite(); });
	channel_->setCloseCallback([this]() {handleClose(); });
	channel_->setErrorCallback([this]() {handleError(); });
}

Connection::~Connection()
{
	LOG_DEBUG << "Connection::dtor at  fd= " << channel_->Fd() << "%d  state= " << static_cast<int>(state_);
}
void Connection::send(Buffer* message)
{
	send(message->peek(), message->readableBytes());
	message->retrieveAll();
}
void Connection::send(const char* message, size_t len)
{
	if (state_ == StateE::kConnected) {
		if (loop_->isInLoopThread()) {
			sendInLoop(message, len);
		}
		else {
			loop_->runInLoop([this,message,len]() {sendInLoop(message, len); });
		}
	}
}
void Connection::send(const std::string& message)
{
	send(message.data(), message.size());
}

void Connection::shutdown()
{
	if (state_ == StateE::kConnected) {
		setState(StateE::kDisconnecting);
		//以前是直接调用的，现在使用runInLoop()函数去调用
		loop_->runInLoop([this]() { shutdownInLoop(); });
	}
}
void Connection::shutdownInLoop()
{
	if (!channel_->isWrite())  // 说明当前outputBuffer中的数据已经全部发送完成
	{
		
		sockets::shutdownWrite(fd());   // 关闭写端 ,能触发EPOLLHUP,也会触发EPOLLIN
	}
}
void Connection::forceClose()
{
	if (state_ == StateE::kConnected || state_ == StateE::kDisconnecting)
	{
		setState(StateE::kDisconnecting);
		//handleClose();
		//使用queueInLoop函数，就一定是放在任务队列中，即是在EventLoop::doPendingFunctors()中执行forceCloseInLoop()，需要使用shared_from_this()
		loop_->queueInLoop([this]() { shared_from_this()->forceCloseInLoop(); });
	}
}

void Connection::forceCloseInLoop()
{
	loop_->assertInLoopThread();
	if (state_ == StateE::kConnected || state_ == StateE::kDisconnecting)
	{
		setState(StateE::kDisconnecting);
		handleClose();
	}
}
void Connection::connectEstablished()
{
	assert(state_ == StateE::kConnecting);
	setState(StateE::kConnected);
	channel_->tie(shared_from_this());
	channel_->enableReading();
	connectionCallback_(shared_from_this());	//调用用户设置的连接成功或断开的回调函数
}

// 连接销毁(关闭连接的最后一步)
void Connection::connectDestroyed()
{
	if (state_ == StateE::kConnected) {	//一般不会进入这个if
		setState(StateE::kDisconnected);
		channel_->disableAll();

		connectionCallback_(shared_from_this());//调用用户设置的连接成功或断开的回调函数
	}
	
	channel_->remove();
	//Conntion的生命周期在这个函数中才会结束(如果用户不持有TcpConnection对象)
}


void Connection::handleRead()
{
	int savedErrno = 0;
	auto n = inputBuffer_.readFd(fd(), &savedErrno);
	if (n > 0) {
		//这个是用户设置好的函数
		messageCallback_(shared_from_this(), &inputBuffer_);
	}
	else if (n == 0) {
		//表示客户端关闭了连接
		handleClose();
	}
	else {
		handleError();
	}
}
void Connection::handleWrite()
{
	if (!channel_->isWrite()) {
		LOG_INFO << "Connection fd = " << channel_->Fd() << "  is down, no more writing";
		return;
	}

	auto n = ::write(fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
	if (n > 0) {
		//更新readerIndex
		outputBuffer_.retrieve(n);
		if (outputBuffer_.readableBytes() == 0) {//表明要发送的数据已全部发送完毕，所以取消写事件
			channel_->disableWriting();

		/*	if (state_ == StateE::kDisconnecting) {
				shutdownInLoop();
			}*/
		}
		else {
			LOG_INFO << "read to write more data";
		}
	}
	else {
		LOG_ERROR << "handleWrite error";
	}
}
void Connection::handleClose()
{
	assert(state_ == StateE::kConnected || state_ == StateE::kDisconnecting);
	setState(StateE::kDisconnected);
	channel_->disableAll();

	ConnectionPtr guardThis(shared_from_this());
	connectionCallback_(guardThis);		//调用用户定义的connectionCallback_函数

	closeCallback_(guardThis);	//closeCallback_就是Server::removeConnection()函数
}

void Connection::handleError()
{
	int err = sockets::getSocketError(channel_->Fd());
	LOG_DEBUG << "Connection::handleError() error=" << err;
}

void Connection::sendInLoop(const char* message, size_t len)
{
	if (state_ == StateE::kDisconnected) {
		LOG_DEBUG << "disconnected, give up writing";
		return;
	}

	bool faultError = false;
	ssize_t nwrote = 0;
	size_t reamining = len;
	//如果当前channel没有写事件发生，并且发送缓冲区无待发送的数据，那就可以直接发送
	if (!channel_->isWrite() && outputBuffer_.readableBytes() == 0) {
		nwrote = ::write(fd(), message, len);
		if (nwrote >= 0) {
			reamining = len - nwrote;
			if (reamining == 0) {
				//表示数据已完全发送出去，通知用户写已完成
				if (writeCompleteCallback_) {
					writeCompleteCallback_(shared_from_this());
				}
			}
		}
		else {	//nwrote<0
			nwrote = 0;
			if (errno != EWOULDBLOCK) {
				if (errno == EPIPE || errno == ECONNRESET) {
					faultError = true;
				}
			}
		}
	}

	if (!faultError && reamining > 0) {
		outputBuffer_.append(static_cast<const char*>(message) + nwrote, reamining);
		if (!channel_->isWrite()) {
			channel_->enableWriting();
		}
	}
}