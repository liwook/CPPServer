#include"Epoll/Epoll.h"
#include"Socket/Socket.h"
#include"util/util.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include"Server/Server.h"
const int MAXSIZE = 1024;

int main()
{
	InetAddr servAddr(10000);
	EventLoop loop;
	Server server(servAddr, &loop);
	loop.loop();

	return 0;
}

