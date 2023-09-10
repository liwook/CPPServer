#include"src/Server.h"
#include<stdio.h>
#include<thread>
#include<iostream>
#include"src/log/logger.h"
#include"src/HTTP/HttpRequest.h"
#include"src/HTTP/HttpContext.h"
#include"src/websocket/websocketServer.h"

using namespace std;


void onRequest(const Buffer* input, Buffer* output)
{
		//½øÐÐecho»Ø¸´
	output->append(input->peek(),input->readableBytes());
}


int main(int argc, char* argv[])
{
	int numThreads = 0;
	if (argc > 1) {
		Logger::setLogLevel(Logger::LogLevel::WARN);
		numThreads = atoi(argv[1]);
	}

	EventLoop loop;
	websocketServer server(&loop, InetAddr(9999));
	server.setHttpCallback(onRequest);
	server.start(numThreads);
	loop.loop();

	return 0;
}







