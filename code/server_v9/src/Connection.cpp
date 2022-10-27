#include"Connection.h"
#include"EventLoop.h"
#include"Channel.h"
#include"Socket.h"

Connection::Connection(EventLoop* loop, int sockfd)
	:loop_(loop)
	,state_(StateE::kConnecting)
	,socket_(std::make_unique<Socket>(sockfd))
	,channel_(std::make_unique<Channel>(loop,sockfd))
{
	//设置好 读，写，关闭连接 的回调函数
	channel_->SetReadCallback([this]() {handleRead(); });
	channel_->setWriteCallback([this]() {handleWrite(); });
	//channel_->setCloseCallback([this]() {handleClose(); });
}

void Connection::send(Buffer* message)
{

}
void Connection::send(const void* message, int len)
{

}
void Connection::send(const std::string& message)
{
	if (state_ == StateE::kDisconnected)
		return;

	bool faultError = false;
	ssize_t nwrote = 0;
	size_t reamining = message.size();
	//如果当前channel没有写事件发生，并且发送缓冲区无待发送的数据，那就可以直接发送
	if (!channel_->isWrite() && outputBuffer_.readableBytes() == 0) {
		nwrote = ::write(fd(), message.data(), message.size());
		if (nwrote >= 0) {
			reamining = message.size() - nwrote;
			if (reamining == 0) {
				//表示数据已完全发送出去，没有操作，之后可以设置数据发送完毕，可以通知用户的事件
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

	assert(reamining <= message.size());
	if (!faultError && reamining > 0) {
		outputBuffer_.append(message.data() + nwrote, reamining);
		if (!channel_->isWrite()) {
			channel_->enableWriting();
		}
	}
}

void Connection::connectEstablished()
{
	assert(state_ == StateE::kConnecting);
	setState(StateE::kConnected);
	channel_->enableReading();
}

void Connection::connectDestroyed()
{
	if (state_ == StateE::kConnected) {
		setState(StateE::kDisconnected);
		channel_->disableAll();
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
	printf("Connection::handleClose() guardThis(shared_from_this())后 user_count= %ld\n", guardThis.use_count());
	closeCallback_(guardThis);	//closeCallback_就是Server::removeConnection()函数
}