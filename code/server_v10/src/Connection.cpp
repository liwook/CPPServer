#include"Connection.h"
#include"EventLoop.h"
#include"Channel.h"
#include"Socket.h"
#include"util/util.h"
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

	//这一节添加关闭连接,错误处理 的回调函数
	channel_->setCloseCallback([this]() {handleClose(); });
	channel_->setErrorCallback([this]() {handleError(); });
}


//Connection::Connection(EventLoop* loop, int sockfd)
//	:loop_(loop)
//	,state_(StateE::kConnecting)
//	,socket_(std::make_unique<Socket>(sockfd))
//	,channel_(std::make_unique<Channel>(loop,sockfd))
//{
//	//设置好 读，写 的回调函数
//	channel_->SetReadCallback([this]() {handleRead(); });
//	channel_->setWriteCallback([this]() {handleWrite(); });
//}

void Connection::send(Buffer* message)
{
	send(message->peek(), message->readableBytes());
	message->retrieveAll();
}
void Connection::send(const void* message, size_t len)
{
	if (state_ == StateE::kDisconnected)
		return;

	bool faultError = false;
	ssize_t nwrote = 0;
	size_t reamining = len;
	//如果当前channel没有写事件发生，并且发送缓冲区无待发送的数据，那就可以直接发送
	if (!channel_->isWrite() && outputBuffer_.readableBytes() == 0) {
		nwrote = ::write(fd(), message, len);
		if (nwrote >= 0) {
			reamining =len - nwrote;
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

	assert(static_cast<size_t>(reamining) <= len);
	if (!faultError && reamining > 0) {
		outputBuffer_.append(static_cast<const char*>(message) + nwrote, reamining);
		if (!channel_->isWrite()) {
			channel_->enableWriting();
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
		if (!channel_->isWrite()) {
			sockets::shutdownWrite(fd());
		}
	}
}
void Connection::forceClose()
{
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

void Connection::connectDestroyed()
{
	if (state_ == StateE::kConnected) {	//一般不会进入这个if
		setState(StateE::kDisconnected);
		channel_->disableAll();

		connectionCallback_(shared_from_this());//调用用户设置的连接成功或断开的回调函数
	}

	channel_->remove();
}


void Connection::handleRead()
{
	int savedErrno = 0;
	auto n = inputBuffer_.readFd(fd(), &savedErrno);
	if (n > 0) {
		messageCallback_(shared_from_this(), &inputBuffer_);	//这个是用户设置好的函数
	}
	else if (n == 0) {
		//表示客户端关闭了连接
		handleClose();
	}
	else {
		//表示出错了，现在不处理
		printf("readFd  error\n");
	}
}
void Connection::handleWrite()
{
	if (!channel_->isWrite()) {
		printf("Connection fd = % d is down, no more writing\n", channel_->Fd());
		return;
	}

	auto n = ::write(fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
	if (n > 0) {
		//更新readerIndex
		outputBuffer_.retrieve(n);
		if (outputBuffer_.readableBytes() == 0) {//表明要发送的数据已全部发送完毕，所以取消写事件
			channel_->disableWriting();
		}
		else {
			printf("read to write more data\n");
		}
	}
	else {
		printf("handleWrite error\n");
	}
}
void Connection::handleClose()
{
	assert(state_ == StateE::kConnected || state_ == StateE::kDisconnecting);
	setState(StateE::kDisconnected);
	channel_->disableAll();

	ConnectionPtr guardThis(shared_from_this());
	//printf("Connection::handleClose() guardThis(shared_from_this())后 user_count= %ld\n", guardThis.use_count());
	closeCallback_(guardThis);	//closeCallback_就是Server::removeConnection()函数
}

void Connection::handleError()
{
	int err = sockets::getSocketError(channel_->Fd());
	printf("Connection::handleError err %d", err);
}