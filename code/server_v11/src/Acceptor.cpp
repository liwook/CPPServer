#include"Acceptor.h"

Acceptor::Acceptor(const InetAddr& listenAddr, EventLoop* eventloop)
	:loop_(eventloop)
	,acceptSocket_(Socket())
	,acceptChannel_(loop_,acceptSocket_.fd())
	,listen_(false)
{
	acceptSocket_.bind(listenAddr);
	
	auto cb = [this]() {handleRead(); };
	acceptChannel_.SetReadCallback(cb);

	//这个开始监听放到Server::start()函数中
	//this->listen();
}

Acceptor::~Acceptor()
{
	acceptChannel_.disableAll();
	acceptChannel_.remove();
}
void Acceptor::listen()
{
	acceptSocket_.listen();
	acceptChannel_.enableReading();
	listen_ = true;
}

void Acceptor::handleRead()
{
	InetAddr peerAddr;
	int connfd = acceptSocket_.accept(&peerAddr);
	if (connfd >= 0) {
		if (newConnectionCallback_) {
			newConnectionCallback_(connfd,peerAddr);
		}
	}
	else {
		printf("accpet error\n");
	}
}
