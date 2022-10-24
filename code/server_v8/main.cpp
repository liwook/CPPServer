#include"src/Server.h"
const int MAXSIZE = 1024;

int main()
{
	InetAddr servAddr(10000);
	EventLoop loop;
	Server server(servAddr, &loop);
	loop.loop();

	return 0;
}

