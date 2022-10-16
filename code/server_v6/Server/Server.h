#pragma once

#include"../Channel/Channel.h"
#include"../Epoll/Epoll.h"
#include"../Socket/Socket.h"
#include"../util/util.h"
#include"../EventLoop/EventLoop.h"
#include"../InetAddr/InetAddr.h"

class Server
{
public:
	Server(const InetAddr& serverAddr, EventLoop* eventloop);
	~Server();
	void handleEvent(Channel* ch);
	void newConnection(Socket* serv_sock);

private:
	EventLoop* loop_;

	Socket* serv_socket_;
	Channel* serv_channel_;
};