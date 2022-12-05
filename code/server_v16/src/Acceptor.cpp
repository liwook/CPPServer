#include"Acceptor.h"
#include"./util/util.h"
Acceptor::Acceptor(const InetAddr& listenAddr, EventLoop* eventloop)
	:loop_(eventloop)
	,acceptSocket_(Socket())
	,acceptChannel_(loop_,acceptSocket_.fd())
	,listen_(false)
{
	sockets::setReuseAddr(acceptSocket_.fd());
	acceptSocket_.bind(listenAddr);
	acceptChannel_.SetReadCallback([this]() {handleRead(); });
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
	printf("Acceptor::listen()  listenfd= %d\n",acceptSocket_.fd());
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
