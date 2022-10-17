#pragma once

#include"Channel.h"
#include"Epoll.h"
#include"Socket.h"
#include"util/util.h"
#include"EventLoop.h"
#include"InetAddr.h"
#include<memory>
#include"Acceptor.h"

class Server
{
public:
	Server(const InetAddr& serverAddr, EventLoop* eventloop);
	~Server();
	void handleEvent(Channel* ch);
	//void newConnection(Socket* serv_sock);
	void newConnection(int sockfd);


private:
	EventLoop* loop_;

	std::unique_ptr<Acceptor> acceptor_;
	
	/*Socket* serv_socket_;
	Channel* serv_channel_;*/
};