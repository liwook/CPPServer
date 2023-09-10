#include"Server.h"
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

Server::Server(const InetAddr& listenAddr, EventLoop* eventloop)
	:loop_(eventloop)
	,ipPort_(listenAddr.toIpPort())
	,acceptor_(std::make_unique<Acceptor>(listenAddr,loop_))
	,loop_threadpool_(std::make_unique<EventLoopThreadPool>(loop_))
	,compute_threadpool_(std::make_unique<ThreadPool>())
	,started_(0)
{
	acceptor_->setNewconnectionCallback([this](int sockfd, const InetAddr& peerAddr){newConnection(sockfd, peerAddr); });
}

Server::~Server()
{
	//��һ������뵽�����forѭ���еģ���Ϊ����connections_����û��Ԫ��
	for (auto& item : connections_) {
		ConnectionPtr conn(item.second);
		item.second.reset();

		conn->connectDestroyed();
	}
}


void Server::start(int IOThreadNum, int compute_threadNum)
{
	if (started_++ == 0) //��ֹһ��TcpServer�����������
	{
		loop_threadpool_->setThreadNum(IOThreadNum);
		loop_threadpool_->start();
		compute_threadpool_->start(compute_threadNum);				//�����䣬���������̳߳أ���compute_threadNum��0���Ǿ���û�п�����

		acceptor_->listen();
	}
}

void Server::newConnection(int sockfd, const InetAddr& peerAddr)
{
	//printf("Server::newConnection() sockfd=%d\n", sockfd);
	//ѡ��һ��subLoop
	EventLoop* ioLoop = loop_threadpool_->getNextLoop();
	InetAddr localAddr(sockets::getLocalAddr(sockfd));

	auto conn = std::make_shared<Connection>(ioLoop, sockfd, localAddr,peerAddr);
	connections_[sockfd] = conn;

	conn->setMessageCallback(messageCallback_);
	conn->setCloseCallback([this](const ConnectionPtr& connection) 
		{removeConnection(connection); } );
	conn->setConnectionCallback(connectionCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);

	ioLoop->runInLoop([conn]() { conn->connectEstablished();});
}

void Server::removeConnection(const ConnectionPtr& conn)
{
	//����֮ǰ��
	//auto n = connections_.erase(conn->fd());	//��Server����
	//conn->connectDestroyed();

	//���ڵ�
	loop_->runInLoop([this, conn]() { removeConnectionInLoop(conn); });

}

// �ڶ�Ӧ��loop���Ƴ������ᷢ�����̵߳Ĵ���
void Server::removeConnectionInLoop(const ConnectionPtr& conn)
{
	connections_.erase(conn->fd());	

	auto ioLoop = conn->getLoop();
	ioLoop->queueInLoop([conn]() {conn->connectDestroyed(); });
}