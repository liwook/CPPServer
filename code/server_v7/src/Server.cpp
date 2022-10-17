#include"Server.h"
#include<string.h>
#include<unistd.h>
#include<fcntl.h>

const int READ_BUFFER = 1024;

void setNonblock(int sockfd)
{
	int flag = fcntl(sockfd, F_GETFL);
	flag |= O_NONBLOCK;
	fcntl(sockfd, F_SETFL, flag);
}

Server::Server(const InetAddr& listenAddr, EventLoop* eventloop)
	:loop_(eventloop)
	//,serv_socket_(new Socket)
	,acceptor_(std::make_unique<Acceptor>(listenAddr,loop_))
{
	auto cb = [this](int sockfd){newConnection(sockfd); };
	acceptor_->setNewconnectionCallback(cb);
	//serv_socket_->bind(listenAddr);
	//serv_socket_->listen();
	//serv_socket_->setNonblock();	//���÷�����

	//serv_channel_=new Channel(loop_, serv_socket_->fd());
	//auto cb = [this](){newConnection(serv_socket_); };
	//serv_channel_->SetCallback(cb);	//���ûص�
	//
	//serv_channel_->enableReading();	
}

Server::~Server()
{

}

void Server::handleEvent(Channel* ch)
{
	int fd = ch->Fd();
	char buf[READ_BUFFER];
	memset(buf, 0, sizeof(buf));

	ssize_t bytes_read = read(fd, buf, sizeof(buf));
	if (bytes_read > 0) {
		printf("client fd %d says: %s\n", fd, buf);
		write(fd, buf, bytes_read);
	}
	else if (bytes_read == -1) {
		perror("read");
	}
	else if (bytes_read == 0) {  //�ͻ��˶Ͽ�����
		printf("client fd %d disconnected\n", fd);
		loop_->removeChannel(ch);
		close(fd);
	}
}
void Server::newConnection(int sockfd)
{
	setNonblock(sockfd);

	Channel* channel = new Channel(loop_, sockfd);	//��汾û��delete,���ڴ�й©��֮��汾���޸�
	auto cb = [this,channel]() {handleEvent(channel); };
	//auto cb = [&]() {handleEvent(channel); };����ֶδ����ºˣ�

	channel->SetReadCallback(cb);	//���ûص�
	channel->enableReading();
}

